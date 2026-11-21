#pragma once
#include <stdint.h>
typedef uint32_t ADI_INT_STATUS;
typedef void (*ADI_INT_HANDLER_PTR)(uint32_t, void*);
