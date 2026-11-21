#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/platform.h>
#include <services/int/adi_int.h>
#include <cdef21371.h>
#include <sru.h>


// ==============================================================================
// Main
// ==============================================================================
int main(void)
{
	SRU(FLAG7_O, DPI_PB07_I); // connect output of Flag7 to input of DPI-PinBuffer 7 (LED on DPI7)
	SRU(HIGH, DPI_PBEN07_I); // set Pin-Buffer to output (HIGH=Output, LOW=Input)
	sysreg_bit_set(sysreg_FLAGS, FLG7O); // Flag 7 als Ausgang definieren

	while(true)
	{
		//i++;

		//if (i > 24000000000000.0)
		//{
			sysreg_bit_set(sysreg_FLAGS, FLG7);
			sysreg_bit_clr(sysreg_FLAGS, FLG7);
		//}
	}

    //return 0;
}
