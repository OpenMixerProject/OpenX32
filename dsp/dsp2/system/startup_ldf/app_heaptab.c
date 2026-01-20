/*
** ADSP-21371 user heap source file generated on Jan 20, 2026 at 18:06:43.
*/
/*
** Copyright (C) 2000-2023 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically based upon the options selected
** in the System Configuration utility. Changes to the Heap configuration
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

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rule_1_1)
#pragma diag(suppress:misra_rule_6_3)
#pragma diag(suppress:misra_rule_8_10)
#pragma diag(suppress:misra_rule_10_1_a)
#pragma diag(suppress:misra_rule_11_3)
#endif /* _MISRA_RULES */

#pragma file_attr("libData=HeapTable")

#include <stdlib.h>
#include <limits.h>

extern "asm" unsigned long ldf_heap_space;
extern "asm" unsigned long ldf_heap_length;
extern "asm" int ExtHeap_space;
extern "asm" int ExtHeap_length;

const __heap_desc_t heap_table[3] =
{

  { &ldf_heap_space, (unsigned long) &ldf_heap_length, 0 },
  { &ExtHeap_space, (unsigned long) &ExtHeap_length, 1 },

  { (void *)0, (size_t)0, 0 } /* This terminates the table. */
};

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif /* _MISRA_RULES */

