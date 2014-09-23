
#include "main.h"

// Clock status at Power up
//  FRCDIV<2:0> = 0b000        ->  No division
//  PLLPRE<4:0> = 0b0000       ->  N1 = divide by 2
//  PLLPOST<1:0> = 0b01        ->  N2 = divide by 4
//  PLLDIV<8:0> = 0b000110000  ->  M  = divide by 50
//  ACLKPSTSCLR = 0b000        ->  divide by 256
//  DACFDIV = 0b00000101       ->  divide by 6
//  Fin = 7.37MHz
//  Fosc = (M * Fin) / (N1 * N2) = 46.08MHz
//  Fcy = Fosc / 2               = 23.04MHz

// Compute the PLL divider settings
#define PLLPRE_BITS (PLL_N1 - 2)
#define PLLDIV_BITS (PLL_M - 2)

#if (PLL_N2 == 2)
#define PLLPOST_BITS 0b00
#endif
#if (PLL_N2 == 4)
#define PLLPOST_BITS 0b01
#endif
#if (PLL_N2 == 8)
#define PLLPOST_BITS 0b11
#endif


// Compute the DAC clock divisor settings.

// #define DACFDIV_VAL ((((FRCFREQ / PLL_N1) * PLL_M) / (256 * SAMPLE_RATE * ACLK_POST)) -1)
#define DACFDIV_VAL (((OSCFREQ * PLL_M)  / (256 * SAMPLE_RATE * ACLK_POST * PLL_N1)) -1)

#if ACLK_POST == 1
#define ACLKPOSTSCLR_VAL 0b111
#endif
#if ACLK_POST == 2
#define ACLKPOSTSCLR_VAL 0b110
#endif
#if ACLK_POST == 4
#define ACLKPOSTSCLR_VAL 0b101
#endif
#if ACLK_POST == 8
#define ACLKPOSTSCLR_VAL 0b100
#endif
#if ACLK_POST == 16
#define ACLKPOSTSCLR_VAL 0b011
#endif
#if ACLK_POST == 32
#define ACLKPOSTSCLR_VAL 0b010
#endif
#if ACLK_POST == 64
#define ACLKPOSTSCLR_VAL 0b001
#endif
#if ACLK_POST == 256
#define ACLKPOSTSCLR_VAL 0b000
#endif
