// ===========================================================================
// OpenX32 - OpenInferno Port B Integration Wrapper (1 ms, 48 ch @ 48 kHz)
//
// Combines unmodified upstream dreviLish/openinferno RTL modules:
//   * aoip_regs   (0x000..0x0FF)
//   * inferno_regs(0x100..0x1FF)  MAXLAT=1 (1 ms), MAX_CH=48, NF=64, NR=64, MAX_FPP=16
//   * cpu_pktq    (0x200..0x2FF)  TX 2 KB, RX 8 KB + tx_arb
//   * fs_phase    (0x300..0x3FF)  measures X32 desk fsync vs media NCO
//   * ptp_clock   (shared PTP + media clock + SFD timestamps)
//   * rmii_rx / rmii_tx / inferno_tx / inferno_rx
//
// Audio interface: 6x TDM8 (48 ch) with 1-BCLK FSYNC delay matching OpenX32.
// Host interface:  32-bit register window (ext_reg_* from spi_reg_bridge.vhd).
// ===========================================================================
`timescale 1ns/1ps
`include "clog2.vh"

module inferno_port_x32 #(
    parameter MAX_CH  = 48,
    parameter NTXF    = 16,
    parameter NRXF    = 16,
    parameter MAXFCH  = 32,
    parameter MAX_FPP = 16,   // 1 ms mode automatic fpp = 16
    parameter NR      = 64,   // TX ring frames for 1 ms (MAXLAT=1)
    parameter NF      = 64,   // RX jitter buffer frames for 1 ms (MAXLAT=1)
    parameter MAXLAT  = 1,    // 1 ms max latency (fits both LFE5U-25F & XC3S1400A)
    parameter TXW     = 512,  // 2 KB CPU TX queue
    parameter RXW     = 2048  // 8 KB CPU RX queue
) (
    input  wire        clk50_i,
    input  wire        rst_i,
    input  wire        bclk_i,          // 12.288 MHz TDM bit clock
    input  wire        fsync_i,         // 48 kHz desk frame sync

    // RMII PHY (IOB-registered from board wrapper)
    input  wire        rmii_crs_dv_i,
    input  wire [1:0]  rmii_rxd_i,
    output wire        rmii_tx_en_o,
    output wire [1:0]  rmii_txd_o,

    // 6x TDM8 audio interface to OpenX32 208-ch matrix (1-BCLK FSYNC delay)
    input  wire [5:0]  tdm_i,
    output reg  [5:0]  tdm_o,

    // 32-bit register window from spi_reg_bridge (0x1000..0x1FFF -> addr[11:0])
    input  wire [11:0] reg_addr_i,
    input  wire [31:0] reg_wdata_i,
    input  wire        reg_we_i,
    input  wire        reg_re_i,
    output reg  [31:0] reg_rdata_o
);

    // ----------------------------------------------------------------
    // Register block decode (addr[11:8])
    //   0x000: aoip_regs, 0x100: inferno_regs, 0x200: cpu_pktq, 0x300: fs_phase
    // ----------------------------------------------------------------
    wire sel_aoip = (reg_addr_i[11:8] == 4'h0);
    wire sel_inf  = (reg_addr_i[11:8] == 4'h1);
    wire sel_pktq = (reg_addr_i[11:8] == 4'h2);
    wire sel_fsp  = (reg_addr_i[11:8] == 4'h3);

    wire [31:0] rdata_aoip, rdata_inf, rdata_pktq, rdata_fsp;

    always @(*) begin
        case (reg_addr_i[11:8])
            4'h0: reg_rdata_o = rdata_aoip;
            4'h1: reg_rdata_o = rdata_inf;
            4'h2: reg_rdata_o = rdata_pktq;
            4'h3: reg_rdata_o = rdata_fsp;
            default: reg_rdata_o = 32'h0;
        endcase
    end

    // ----------------------------------------------------------------
    // Interconnect wires
    // ----------------------------------------------------------------
    wire [1:0]  mode_req;
    wire [47:0] src_mac;
    wire [31:0] src_ip, link_offset, ptp_incr, media_inc, set_media_ts;
    wire [47:0] set_sec, tod_sec;
    wire [29:0] set_ns, tod_ns;
    wire        set_tod, set_media;

    wire [31:0] media_ts, media_sec;
    wire [16:0] media_sub, media_fs, set_msub;
    wire [31:0] set_msec;
    wire        frame_strobe;
    wire [77:0] ts_tx, ts_rx;

    wire        tx_en, rx_en, lat_manual;
    wire [6:0]  n_out;
    wire [1:0]  lat_mode;
    wire [7:0]  ts_adv;
    wire [31:0] play_offset;
    wire [5:0]  fpp_auto;

    wire        tx_tbl_we, rx_tbl_we;
    wire [9:0]  tbl_addr;
    wire [31:0] tbl_wdata, tx_tbl_rdata, rx_tbl_rdata;
    wire [NTXF-1:0] tx_pending, tx_running;
    wire [31:0] tx_packets, tx_overruns, rx_ok, rx_bad, rx_late, rx_early, rx_underruns, rx_late_part, rx_ts_gaps;

    wire [7:0]  mac_rx_tdata, mac_tx_tdata, inf_tx_tdata, cpu_tx_tdata;
    wire        mac_rx_tvalid, mac_rx_tlast, mac_rx_tuser, sfd_rx, rmii_crc_err_p;
    wire        mac_tx_tvalid, mac_tx_tlast, mac_tx_tready, sfd_tx;
    wire        inf_tx_tvalid, inf_tx_tlast, inf_tx_tready;
    wire        cpu_tx_tvalid, cpu_tx_tlast, cpu_tx_tready, tx_mine_cpu;
    wire        rx_claim;

    reg  [31:0] mac_crc_err_cnt;
    always @(posedge clk50_i) begin
        if (rst_i) mac_crc_err_cnt <= 32'd0;
        else if (rmii_crc_err_p) mac_crc_err_cnt <= mac_crc_err_cnt + 32'd1;
    end

    // ----------------------------------------------------------------
    // 1. Register Blocks & Clocks
    // ----------------------------------------------------------------
    aoip_regs u_aoip_regs (
        .clk(clk50_i), .rst(rst_i),
        .addr(reg_addr_i[7:0]), .raddr(reg_addr_i[7:0]),
        .we(reg_we_i & sel_aoip), .wdata(reg_wdata_i),
        .re(reg_re_i & sel_aoip), .rd_hold(1'b0), .rdata(rdata_aoip),
        .mode_req(mode_req), .src_mac(src_mac), .src_ip(src_ip),
        .link_offset(link_offset), .ptp_incr(ptp_incr), .media_inc(media_inc),
        .set_sec(set_sec), .set_ns(set_ns), .set_tod(set_tod),
        .set_media_ts(set_media_ts), .set_media(set_media),
        .ace_tx_en(), .ace_rx_en(), .ace_ch(), .ace_offset(),
        .mode_active(2'd1), .switching(1'b0),
        .media_ts(media_ts), .tod_sec(tod_sec), .tod_ns(tod_ns),
        .ts_tx_ns(ts_tx[29:0]), .ts_rx_ns(ts_rx[29:0]),
        .mac_crc_err(mac_crc_err_cnt),
        .ace_status(32'd0), .ace_tx_frames(32'd0), .ace_tx_overruns(32'd0),
        .ace_rx_ok(32'd0), .ace_rx_bad(32'd0), .ace_rx_lost(32'd0),
        .ace_rx_sync_err(32'd0), .ace_rx_underruns(32'd0), .ace_rx_overruns(32'd0), .ace_rx_ctrl(32'd0)
    );

    inferno_regs #(
        .NTXF(NTXF), .NRXF(NRXF), .MAX_CH(MAX_CH), .MAXFCH(MAXFCH),
        .MAX_FPP(MAX_FPP), .NR(NR), .NF(NF), .MAXLAT(MAXLAT)
    ) u_inf_regs (
        .clk(clk50_i), .rst(rst_i),
        .addr(reg_addr_i[7:0]), .raddr(reg_addr_i[7:0]),
        .we(reg_we_i & sel_inf), .wdata(reg_wdata_i),
        .re(reg_re_i & sel_inf), .rdata(rdata_inf),
        .tx_en(tx_en), .rx_en(rx_en), .n_out(n_out), .media_fs(media_fs),
        .set_msec(set_msec), .set_msub(set_msub),
        .lat_mode(lat_mode), .lat_manual(lat_manual), .ts_adv(ts_adv),
        .fs_now(media_fs), .link_offset(link_offset),
        .play_offset(play_offset), .fpp_auto(fpp_auto),
        .tx_tbl_we(tx_tbl_we), .rx_tbl_we(rx_tbl_we),
        .tbl_addr(tbl_addr), .tbl_wdata(tbl_wdata),
        .tx_tbl_rdata(tx_tbl_rdata), .rx_tbl_rdata(rx_tbl_rdata),
        .tx_pending(tx_pending), .tx_running(tx_running),
        .media_sec(media_sec), .media_sub(media_sub),
        .tx_packets(tx_packets), .tx_overruns(tx_overruns),
        .rx_ok(rx_ok), .rx_bad(rx_bad), .rx_late(rx_late), .rx_early(rx_early),
        .rx_underruns(rx_underruns), .rx_ts_gaps(rx_ts_gaps), .rx_late_part(rx_late_part)
    );

    ptp_clock u_ptp_clock (
        .clk(clk50_i), .rst(rst_i),
        .incr(ptp_incr), .set_tod(set_tod), .set_sec(set_sec), .set_ns(set_ns),
        .sec(tod_sec), .ns(tod_ns),
        .media_inc(media_inc), .set_media(set_media), .set_media_ts(set_media_ts),
        .media_ts(media_ts), .frame_strobe(frame_strobe),
        .media_fs(media_fs), .set_media_sec(set_msec), .set_media_sub(set_msub),
        .media_sec(media_sec), .media_sub(media_sub),
        .sfd_tx(sfd_tx), .sfd_rx(sfd_rx), .ts_tx(ts_tx), .ts_rx(ts_rx)
    );

    fs_phase u_fs_phase (
        .clk(clk50_i), .rst(rst_i),
        .frame_strobe(frame_strobe), .media_ts(media_ts), .desk_fs(fsync_i),
        .reg_addr(reg_addr_i[7:0]), .bus_addr(reg_addr_i[7:0]),
        .reg_we(reg_we_i & sel_fsp), .reg_wdata(reg_wdata_i),
        .reg_re(reg_re_i & sel_fsp), .reg_rdata(rdata_fsp)
    );

    // ----------------------------------------------------------------
    // 2. RMII MAC + CPU Packet Queue + Arbiter
    // ----------------------------------------------------------------
    rmii_rx u_rmii_rx (
        .clk(clk50_i), .rst(rst_i), .crs_dv(rmii_crs_dv_i), .rxd(rmii_rxd_i),
        .m_tdata(mac_rx_tdata), .m_tvalid(mac_rx_tvalid), .m_tlast(mac_rx_tlast), .m_tuser(mac_rx_tuser),
        .sfd_strobe(sfd_rx), .rx_ok(), .rx_crc_err(rmii_crc_err_p), .rx_runt()
    );

    rmii_tx u_rmii_tx (
        .clk(clk50_i), .rst(rst_i),
        .s_tdata(mac_tx_tdata), .s_tvalid(mac_tx_tvalid), .s_tlast(mac_tx_tlast), .s_tready(mac_tx_tready),
        .txd(rmii_txd_o), .tx_en(rmii_tx_en_o), .sfd_strobe(sfd_tx), .tx_underrun(), .tx_done()
    );

    cpu_pktq #(.TXW(TXW), .RXW(RXW)) u_pktq (
        .clk(clk50_i), .rst(rst_i),
        .reg_addr(reg_addr_i[7:0]), .reg_raddr(reg_addr_i[7:0]),
        .reg_we(reg_we_i & sel_pktq), .reg_wdata(reg_wdata_i),
        .reg_re(reg_re_i & sel_pktq), .rd_hold(1'b0), .reg_rdata(rdata_pktq),
        .our_mac(src_mac), .tod_sec(tod_sec[31:0]), .tod_ns(tod_ns),
        .m_tdata(cpu_tx_tdata), .m_tvalid(cpu_tx_tvalid), .m_tlast(cpu_tx_tlast), .m_tready(cpu_tx_tready),
        .tx_mine(tx_mine_cpu), .sfd_tx(sfd_tx),
        .s_tdata(mac_rx_tdata), .s_tvalid(mac_rx_tvalid), .s_tlast(mac_rx_tlast), .s_tuser(mac_rx_tuser),
        .sfd_rx(sfd_rx), .claim(rx_claim)
    );

    tx_arb u_tx_arb (
        .clk(clk50_i), .rst(rst_i),
        .a_tdata(inf_tx_tdata), .a_tvalid(inf_tx_tvalid), .a_tlast(inf_tx_tlast), .a_tready(inf_tx_tready),
        .b_tdata(cpu_tx_tdata), .b_tvalid(cpu_tx_tvalid), .b_tlast(cpu_tx_tlast), .b_tready(cpu_tx_tready),
        .m_tdata(mac_tx_tdata), .m_tvalid(mac_tx_tvalid), .m_tlast(mac_tx_tlast), .m_tready(mac_tx_tready),
        .mine_b(tx_mine_cpu)
    );

    // ----------------------------------------------------------------
    // 3. Inferno TX & RX Media Engines (48 ch, 1 ms / MAXLAT=1)
    // ----------------------------------------------------------------
    reg        inf_in_we;
    reg  [5:0] inf_in_ch;
    reg [23:0] inf_in_sample;

    wire       inf_out_valid;
    wire [5:0] inf_out_ch;
    wire[23:0] inf_out_sample;

    inferno_tx #(
        .MAX_CH(MAX_CH), .NFLOW(NTXF), .MAXFCH(MAXFCH), .MAX_FPP(MAX_FPP), .NR(NR)
    ) u_inf_tx (
        .clk(clk50_i), .rst(rst_i), .enable(tx_en),
        .src_mac(src_mac), .src_ip(src_ip), .fpp_auto(fpp_auto), .ts_adv(ts_adv),
        .tbl_we(tx_tbl_we), .tbl_waddr(tbl_addr[`CLOG2(NTXF)+3:0]), .tbl_wdata(tbl_wdata),
        .tbl_raddr(tbl_addr[`CLOG2(NTXF)+3:0]), .tbl_rdata(tx_tbl_rdata),
        .in_we(inf_in_we), .in_ch(inf_in_ch), .in_sample(inf_in_sample),
        .frame_strobe(frame_strobe), .media_ts(media_ts), .media_sec(media_sec),
        .media_sub(media_sub), .media_fs(media_fs),
        .m_tdata(inf_tx_tdata), .m_tvalid(inf_tx_tvalid), .m_tlast(inf_tx_tlast), .m_tready(inf_tx_tready),
        .pending(tx_pending), .running(tx_running),
        .packets_sent(tx_packets), .overruns(tx_overruns)
    );

    inferno_rx #(
        .MAX_CH(MAX_CH), .NFLOW(NRXF), .MAXFCH(MAXFCH), .NF(NF)
    ) u_inf_rx (
        .clk(clk50_i), .rst(rst_i), .enable(rx_en),
        .n_out(n_out), .media_fs(media_fs), .media_ts(media_ts),
        .tbl_we(rx_tbl_we), .tbl_waddr(tbl_addr[`CLOG2(NRXF)+4:0]), .tbl_wdata(tbl_wdata),
        .tbl_raddr(tbl_addr[`CLOG2(NRXF)+4:0]), .tbl_rdata(rx_tbl_rdata),
        .s_tdata(mac_rx_tdata), .s_tvalid(mac_rx_tvalid), .s_tlast(mac_rx_tlast), .s_tuser(mac_rx_tuser),
        .claim(rx_claim),
        .frame_strobe(frame_strobe), .play_ts(media_ts - play_offset),
        .out_valid(inf_out_valid), .out_ch(inf_out_ch), .out_sample(inf_out_sample), .out_frame_ok(),
        .packets_ok(rx_ok), .packets_bad(rx_bad), .late(rx_late), .early(rx_early),
        .underruns(rx_underruns), .late_part(rx_late_part), .ts_gaps(rx_ts_gaps)
    );

    // ----------------------------------------------------------------
    // 4. 6x TDM8 <-> Inferno 48-Channel Dual-Clock Bridge (1-BCLK FSYNC delay)
    // ----------------------------------------------------------------
    // Force distributed RAM (64x24 = 1.5 kbit each) so XC3S1400A saves 2 RAMB16
    (* ram_style = "distributed" *) reg [23:0] tdm_tx_buf [0:63];
    (* ram_style = "distributed" *) reg [23:0] tdm_rx_buf [0:63];
    reg [23:0] tdm_tx_rd_q, tdm_rx_rd_q;

    reg        wclk_prev;
    reg [7:0]  bit_pos;
    reg [23:0] tdm_in_sh  [0:5];
    reg [23:0] tdm_out_sh [0:5];
    reg [5:0]  tdm_out_bit;
    integer    li;

    always @(negedge bclk_i) begin
        tdm_o <= tdm_out_bit;
    end

    reg [7:0] cap_pos, tx_pos;
    reg [4:0] slot_bit, tx_bit;
    reg [2:0] slot, next_slot;
    reg [2:0] line_idx;

    always @(posedge bclk_i) begin
        wclk_prev <= fsync_i;
        if (fsync_i && !wclk_prev) cap_pos = 8'd255;
        else                       cap_pos = bit_pos;
        tx_pos    = cap_pos + 8'd1;
        bit_pos  <= tx_pos;
        slot_bit  = cap_pos[4:0];
        tx_bit    = tx_pos[4:0];
        slot      = cap_pos[7:5];
        next_slot = slot + 3'd1;

        if (slot_bit <= 5'd23) begin
            for (li = 0; li < 6; li = li + 1)
                tdm_in_sh[li] <= {tdm_in_sh[li][22:0], tdm_i[li]};
        end
        if (slot_bit >= 5'd24 && slot_bit <= 5'd29) begin
            line_idx = slot_bit - 5'd24;
            tdm_tx_buf[{line_idx, slot}] <= tdm_in_sh[line_idx];
        end
        if (slot_bit >= 5'd22 && slot_bit <= 5'd27) begin
            line_idx = slot_bit - 5'd22;
            tdm_rx_rd_q <= tdm_rx_buf[{line_idx, next_slot}];
        end
        if (slot_bit >= 5'd24 && slot_bit <= 5'd29) begin
            line_idx = slot_bit - 5'd24;
            tdm_out_sh[line_idx] <= tdm_rx_rd_q;
        end
        // Shift out MSB first 1 BCLK ahead (negedge registers tdm_o for next posedge)
        if (tx_bit <= 5'd23) begin
            for (li = 0; li < 6; li = li + 1) begin
                tdm_out_bit[li] <= tdm_out_sh[li][23];
                tdm_out_sh[li]  <= {tdm_out_sh[li][22:0], 1'b0};
            end
        end else begin
            tdm_out_bit <= 6'd0;
        end
    end

    // Stream 48 TX channels into inferno_tx on each frame_strobe, capture RX into tdm_rx_buf
    reg       tx_stream_on;
    reg [5:0] tx_stream_ch;
    reg       tx_stream_d;

    always @(posedge clk50_i) begin
        if (inf_out_valid)
            tdm_rx_buf[inf_out_ch] <= inf_out_sample;

        inf_in_we   <= 1'b0;
        tdm_tx_rd_q <= tdm_tx_buf[tx_stream_ch];

        if (rst_i) begin
            tx_stream_on <= 1'b0;
            tx_stream_ch <= 6'd0;
            tx_stream_d  <= 1'b0;
        end else if (frame_strobe) begin
            tx_stream_on <= 1'b1;
            tx_stream_ch <= 6'd0;
            tx_stream_d  <= 1'b0;
        end else if (tx_stream_on) begin
            if (tx_stream_d) begin
                inf_in_we     <= 1'b1;
                inf_in_ch     <= tx_stream_ch - 6'd1;
                inf_in_sample <= tdm_tx_rd_q;
            end
            if (tx_stream_ch == MAX_CH) begin
                tx_stream_on <= 1'b0;
                tx_stream_d  <= 1'b0;
            end else begin
                tx_stream_ch <= tx_stream_ch + 6'd1;
                tx_stream_d  <= 1'b1;
            end
        end
    end

endmodule
