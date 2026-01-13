/*
** ADSP-21371 startup code generated on Jan 13, 2026 at 17:57:31.
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
** Custom additions can be inserted within the user-modifiable sections. These
** sections are bounded by comments that start with "$VDSG". Only changes
** placed within these sections are preserved when this file is re-generated.
**
** Product      : CrossCore Embedded Studio
** Tool Version : 6.2.5.2
*/

.FILE_ATTR libGroup="startup";
.FILE_ATTR libName="libc";

.FILE_ATTR libFunc="___lib_prog_term";
.FILE_ATTR FuncName="___lib_prog_term";
.FILE_ATTR libFunc="start";
.FILE_ATTR FuncName="start";
.FILE_ATTR prefersMem="any";
.FILE_ATTR prefersMemNum="50";

#include <sys/anomaly_macros_rtl.h> // defines silicon anomaly macros
#include <interrupt.h>              // defines interrupt support
#include <platform_include.h>       // defines MMR macros
#include <adi_osal.h>               // OSAL support
#include <sys/fatal_error_code.h>   // defines fatal error support

/* End marker for the list of static constructors.
** seg_ctdml has to be mapped right after seg_ctdm.
*/

.SECTION/DATA/DOUBLEANY seg_ctdml;
.RETAIN_NAME __ctor_NULL_marker;
.GLOBAL __ctor_NULL_marker;
.VAR __ctor_NULL_marker = 0;

/* The .gdt (global dispatch table) data is used by the C++ exception
** runtime library support to determine which area of code a particular
** exception belongs. These .gdt sections must be mapped contiguously
** into memory by the LDF starting with this one and then lastly .gdtl
** should be mapped.
*/

.SECTION/DATA/DOUBLEANY .gdt;
.VAR ___eh_gdt[1];
.GLOBAL ___eh_gdt;
.TYPE ___eh_gdt,STT_OBJECT;

/* Suppress the assemblers informational message regarding trampolines
** generated for Hardware Anomaly 09000011.
*/
.MESSAGE/SUPPRESS 2518,2519;


.GLOBAL start;

.SECTION/CODE/DOUBLEANY seg_pmco;

start:


/*$VDSG<insert-code-very-beginning>                             */
.start_of_user_code_very_beginning:
      /*
      ** Insert any additional code to be executed before any other
      ** startup code here.
      ** Code inserted here is preserved when this file is re-generated.
      ** Note that the C runtime environment hasn't been set up yet
      ** so you cannot call C or C++ functions.
      */
.end_of_user_code_very_beginning:
/*$VDSG<insert-code-very-beginning>                             */

      /*
      ** Initializes the processor, memory, C runtime and heaps.
      */
      .EXTERN ___lib_setup_c;
      CALL ___lib_setup_c;


/*$VDSG<insert-code-after-setup>                                */
.start_of_user_code_after_setup:
      /*
      ** Insert any additional code to be executed at the start after
      ** the C runtime and hardware has been initialized.
      ** Code inserted here is preserved when this file is re-generated.
      */
.end_of_user_code_after_setup:
/*$VDSG<insert-code-after-setup>                                */

      /*
      ** Call the OSAL init function.
      */
      .EXTERN _adi_osal_Init;    // ADI_OSAL_STATUS adi_osal_Init(void);
      CJUMP _adi_osal_Init (DB);
         DM(I7, M7) = R2;
         DM(I7, M7) = PC;

      R1 = E_ADI_OSAL_SUCCESS;
      COMPU(R0, R1);
      IF NE JUMP .osal_Init_failed;

      /*
      ** Call all the C++ static class instance constructors.
      */
      .EXTERN  __ctors;
      I0 = __ctors;              // load pointer to the list of constructors
                                 // into call preserved I0
      R5 = I6;                   // hold current I6 in preserved register R5
      R0 = DM(I0, M6);           // get the address of the first constructor
      R0 = PASS R0, I13 = R0;    // check if it's the NULL list terminator
.call_ctors:
      IF EQ JUMP .call_ctors_exit; // if it is NULL we have completed

      I6 = I7;
      JUMP (M13, I13) (DB);      // call the constructor
         DM(I7, M7) = R5;
         DM(I7, M7) = PC;
      JUMP (PC, .call_ctors) (DB);
         R0 = DM(I0, M6);        // get the address of the next constructor
         R0 = PASS R0, I13 = R0; // check if it's the NULL list terminator
.call_ctors_exit:

      R4 = R4 - R4,              // argc = 0
         R8 = M5;                // argv = NULL


/*$VDSG<insert-code-before-call-to-main>                        */
.start_of_user_code1:
      /*
      ** Insert any additional code to be executed before calling main() here.
      ** Code inserted here is preserved when this file is re-generated.
      ** Avoid clobbering the values of registers R4 and R8 as they hold
      ** the argc and argv parameters for main().
      */
.end_of_start_of_user_code1:
/*$VDSG<insert-code-before-call-to-main>                        */

      .EXTERN _main;
      JUMP _main (DB);           // call main()
         DM(I7, M7) = 0;         // NULL FP to terminate call stack unwinding
         DM(I7, M7) = ___lib_prog_term-1;

.start.end:

      .GLOBAL ___lib_prog_term;
___lib_prog_term:                // Automatic breakpoint location.
      NOP;                       // Placed prior to loop label so that we
                                 // break only once.
.lib_prog_term_loop:
      IDLE;
      JUMP .lib_prog_term_loop;  // Stay put.
.___lib_prog_term.end:

      /*
      ** The call to _adi_osal_Init returned an error so call _adi_fatal_error.
      */
      .EXTERN _adi_fatal_error;
.osal_Init_failed:
      R12 = R0;                  // pass adi_osal_Init result value
      JUMP _adi_fatal_error (DB); // doesn't return
         R4 = _AFE_G_LibraryError;
         R8 = _AFE_S_adi_osal_Init_failure;
.osal_Init_failed.end:

