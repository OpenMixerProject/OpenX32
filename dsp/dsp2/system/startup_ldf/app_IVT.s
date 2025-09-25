/*
** ADSP-21371 app_IVT.s generated on Sep 11, 2025 at 16:06:41
*/
/*
** Copyright (C) 2000-2023 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically based upon the options selected
** in the System Configuration utility. Changes to the Startup Code configuration
** should be made by modifying the appropriate options rather than editing
** this file. To access the System Configuration utility, double-click the
** system.svc file from a navigation view.
**
** If you need to modify this file, select "Permit alteration of Interrupt
** Table Vector" from "Startup Code/LDF: Advanced Options" in the System
** Configuration utility. This encloses the whole file within a user
** modifiable section and any modification made will be preserved.
*/

.FILE_ATTR libGroup="startup";
.FILE_ATTR libName="libc";

#include <sys/anomaly_macros_rtl.h> // defines silicon anomaly macros
#include <interrupt.h>              // defines interrupt support
#include <platform_include.h>       // defines MMR macros
#include <adi_osal.h>               // OSAL support
#include <sys/fatal_error_code.h>   // defines fatal error support

/* INT_IRQ macro - used for the default interrupt vector instructions sequence
**                 of interrupts where the status stack (STS) is pushed
**                 automatically by the hardware. STS is automatically pushed
**                 for IRQ, Timer and VIRPTL interrupts.
*/
#define INT_IRQ(irp)                       \
    .RETAIN_NAME ___int_irq_##irp;         \
    .EXTERN __dispatcher_##irp;            \
    ___int_irq_##irp:                      \
    JUMP __dispatcher_##irp (DB);          \
       ASTAT = ADI_CID_##irp;              \
       NOP;                                \
    NOP;                                   \
    .___int_irq_##irp##.end:

/* INT macro     - used for the default interrupt vector instructions sequence
**                 of interrupts where the status stack (STS) is not pushed
**                 automatically by the hardware. STS is automatically pushed
**                 for IRQ, Timer and VIRPTL interrupts.
*/
#define INT(irp)                           \
    .RETAIN_NAME ___int_##irp;             \
    .EXTERN __dispatcher_##irp;            \
    ___int_##irp:                          \
    JUMP __dispatcher_##irp (DB);          \
       PUSH STS;                           \
       ASTAT = ADI_CID_##irp;              \
    NOP;                                   \
    .___int_##irp##.end:

/* INT_JUMP      - instructions to jump to a target label without
**                 going through an interrupt dispatcher.
*/
#define INT_JUMP(irp, jump_tgt)            \
    .EXTERN jump_tgt;                      \
    .RETAIN_NAME ___int_jump_##irp;        \
    ___int_jump_##irp:                     \
    NOP;                                   \
    JUMP jump_tgt;                         \
    NOP;                                   \
    NOP;

/* UNUSED_INTERRUPT - The instructions jump to fatal_error for the case
**                    where an interrupt that was not meant to be used
**                    gets latched.
*/
#define UNUSED_INTERRUPT(irp)              \
    .EXTERN _adi_fatal_error;              \
    .RETAIN_NAME ___int_unused_##irp;      \
    ___int_unused_##irp:                   \
    R12 = ADI_CID_##irp;                   \
    JUMP (PC, _adi_fatal_error) (DB);      \
       R4 = _AFE_G_RunTimeError;           \
       R8 = _AFE_S_UnusedInterruptRaised;

/* RESERVED_INTERRUPT - 4 RTI instructions for reserved interrupts. */
#define RESERVED_INTERRUPT RTI;RTI;RTI;RTI;

/*
** ADSP-2136[789] and ADSP-2137[15] Interrupt Vector code
*/
.SECTION/DOUBLEANY/CODE iv_code;

___interrupt_table:

.RETAIN_NAME ___int_EMUI;            // 0x00 - Emulator Interrupt
___int_EMUI:
      NOP;
      NOP;
      NOP;
      NOP;
.___int_EMUI.end:

INT_JUMP(RSTI,start)                 // 0x04 - Reset Interrupt
INT(IICDI)                           // 0x08 - Illegal Input Condition Detected
INT(SOVFI)                           // 0x0c - Stack Overflow/Full Interrupt
INT_IRQ(TMZHI)                       // 0x10 - Timer Expired High Priority
INT(SPERRI)                          // 0x14 - SPORT Error Interrupt
INT(BKPI)                            // 0x18 - Hardware Breakpoint Interrupt
RESERVED_INTERRUPT                   // 0x1c
INT_IRQ(IRQ2I)                       // 0x20 - IRQ2I_ is asserted
INT_IRQ(IRQ1I)                       // 0x24 - IRQ1I_ is asserted
INT_IRQ(IRQ0I)                       // 0x28 - IRQ0I_ is asserted
INT(P0I)                             // 0x2c - Programmable Interrupt 0
INT(P1I)                             // 0x30 - Programmable Interrupt 1
INT(P2I)                             // 0x34 - Programmable Interrupt 2
INT(P3I)                             // 0x38 - Programmable Interrupt 3
INT(P4I)                             // 0x3c - Programmable Interrupt 4
INT(P5I)                             // 0x40 - Programmable Interrupt 5
INT(P6I)                             // 0x44 - Programmable Interrupt 6
INT(P7I)                             // 0x48 - Programmable Interrupt 7
INT(P8I)                             // 0x4c - Programmable Interrupt 8
INT(P9I)                             // 0x50 - Programmable Interrupt 9
INT(P10I)                            // 0x54 - Programmable Interrupt 10
INT(P11I)                            // 0x58 - Programmable Interrupt 11
INT(P12I)                            // 0x5c - Programmable Interrupt 12
INT(P13I)                            // 0x60 - Programmable Interrupt 13
INT(P14I)                            // 0x64 - Programmable Interrupt 14
INT(P15I)                            // 0x68 - Programmable Interrupt 15
INT(P16I)                            // 0x6c - Programmable Interrupt 16
INT(P17I)                            // 0x70 - Programmable Interrupt 17
INT(P18I)                            // 0x74 - Programmable Interrupt 18
INT(CB7I)                            // 0x78 - Circular Buffer 7 Overflow
INT(CB15I)                           // 0x7c - Circular Buffer 15 Overflow
INT_IRQ(TMZLI)                       // 0x80 - Timer=0(Low Priority Option)
INT(FIXI)                            // 0x84 - Fixed-point Overflow
INT(FLTOI)                           // 0x88 - Floating-point Overflow
INT(FLTUI)                           // 0x8c - Floating-point Underflow
INT(FLTII)                           // 0x90 - Floating-point invalid
INT(EMULI)                           // 0x94 - Emulator Low Priority Interrupt
INT(SFT0I)                           // 0x98 - User Software Interrupt 0
INT(SFT1I)                           // 0x9c - User Software Interrupt 1
INT(SFT2I)                           // 0xa0 - User Software Interrupt 2
INT(SFT3I)                           // 0xa4 - User Software Interrupt 3
.___interrupt_table.end:             // 0xa8

