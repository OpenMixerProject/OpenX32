// ===========================================================================
// OpenX32 Testbench: inferno_port_x32 (1 ms, 48 ch @ 48 kHz)
// ===========================================================================
`timescale 1ns/1ps

module inferno_port_x32_tb;
    reg         clk50 = 0;
    reg         bclk  = 0;
    reg         fsync = 0;
    reg         rst   = 1;

    reg  [11:0] reg_addr  = 0;
    reg  [31:0] reg_wdata = 0;
    reg         reg_we    = 0;
    reg         reg_re    = 0;
    wire [31:0] reg_rdata;

    wire        tx_en;
    wire [1:0]  txd;
    wire [5:0]  tdm_o;

    always #10 clk50 = ~clk50;       // 50 MHz
    always #40.69 bclk = ~bclk;      // ~12.288 MHz

    inferno_port_x32 #(
        .MAX_CH(48), .MAX_FPP(16), .NR(64), .NF(64), .MAXLAT(1), .TXW(512), .RXW(2048)
    ) dut (
        .clk50_i(clk50), .rst_i(rst), .bclk_i(bclk), .fsync_i(fsync),
        .rmii_crs_dv_i(tx_en), .rmii_rxd_i(txd), .rmii_tx_en_o(tx_en), .rmii_txd_o(txd),
        .tdm_i(6'd0), .tdm_o(tdm_o),
        .reg_addr_i(reg_addr), .reg_wdata_i(reg_wdata),
        .reg_we_i(reg_we), .reg_re_i(reg_re), .reg_rdata_o(reg_rdata)
    );

    task rd(input [11:0] a, output [31:0] v);
        begin
            @(posedge clk50);
            reg_addr <= a;
            reg_re   <= 1'b1;
            @(posedge clk50);
            v = reg_rdata; // combinational reg_rdata sampled on reg_re edge (matches openx32_core)
            reg_re   <= 1'b0;
        end
    endtask

    task wr(input [11:0] a, input [31:0] v);
        begin
            @(posedge clk50);
            reg_addr  <= a;
            reg_wdata <= v;
            reg_we    <= 1'b1;
            @(posedge clk50);
            reg_we    <= 1'b0;
        end
    endtask

    reg [31:0] val;

    initial begin
        #200; rst = 0; #200;

        // 1. Verify all 4 block IDs
        rd(12'h000, val);
        if (val !== 32'h4F415032) begin $display("FAIL: aoip ID=%h", val); $fatal(1); end
        rd(12'h100, val);
        if (val !== 32'h4F494E32) begin $display("FAIL: inferno ID=%h", val); $fatal(1); end
        rd(12'h200, val);
        if (val !== 32'h4F435131) begin $display("FAIL: pktq ID=%h", val); $fatal(1); end
        rd(12'h300, val);
        if (val !== 32'h4F465031) begin $display("FAIL: fs_phase ID=%h", val); $fatal(1); end

        // 2. Verify CAPS (MAX_CH=48) & CAPS2 (MAXLAT=1, MAX_FPP=16, NR=64)
        rd(12'h138, val);
        if (val[7:0] !== 8'd48) begin $display("FAIL: MAX_CH=%d expected 48", val[7:0]); $fatal(1); end
        rd(12'h13C, val);
        if (val[7:0] !== 8'd16 || val[9:8] !== 2'd1 || val[31:16] !== 16'd64) begin
            $display("FAIL: CAPS2=%h", val); $fatal(1);
        end

        // 3. Select 1 ms latency mode (LATENCY=1) -> expect play_offset=48, fpp=16
        wr(12'h12C, 32'd1);
        @(posedge clk50); @(posedge clk50);
        rd(12'h130, val);
        if (val[10:0] !== 11'd48 || val[21:16] !== 6'd16) begin
            $display("FAIL: LAT_STATUS=%h (expected offset=48, fpp=16)", val); $fatal(1);
        end

        // 4. Verify 5 ms mode (LATENCY=3) is refused because MAXLAT=1
        wr(12'h12C, 32'd3);
        @(posedge clk50); @(posedge clk50);
        rd(12'h12C, val);
        if (val[1:0] !== 2'd1) begin
            $display("FAIL: LATENCY accepted mode > MAXLAT (%h)", val); $fatal(1);
        end

        $display("PASS inferno_port_x32_tb: OAP2/OIN2/OCQ1/OFP1 IDs, 48ch, 1ms (offset=48, fpp=16), MAXLAT=1 verified!");
        $finish;
    end
endmodule
