/* 
 * File:   application.h
 * Author: joe
 *
 * Created on September 23, 2014, 11:15 AM
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "voice.h"
#define VOICECOUNT 32
#define TABLESIZE 4096

// Desired clock config values
// These are for an external 4.096MHz crystal
#define OSCFREQ         4096000UL
#define PLL_N1			2
#define PLL_N2			2
#define PLL_M			72
#define SAMPLE_RATE     48000
#define ACLK_POST       1
#define FCY				((PLL_M * OSCFREQ) / (PLL_N1 * PLL_N2 * 2))
#define Fcy             FCY


// Pin definitions for this application.
#define STATUS_LED		PORTAbits.RA0
#define STATUS_LED_TRIS	TRISAbits.TRISA0


extern Program DefaultProgram;
extern InstrumentState GlobalState;

void start_application();


#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

