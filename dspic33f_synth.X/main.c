/* 
 * File:   main.c
 * Author: joe
 *
 * Created on September 22, 2014, 12:16 PM
 */
#include "voice.h"
#include "main.h"
#include "clock_init.h"
#include "wave_sine.h"
#include "adc_input.h"
#include "midi.h"

#include <p33FJ128GP802.h>	// Pin and register definitions.
#include <libpic30.h>       // Needed for __delay32()


// Config flags:

_FOSCSEL( FNOSC_PRIPLL		// Primary osc with PLL
		//FNOSC_FRCPLL     	// Use internal FRC osc with PLL.
        &  IESO_OFF     	// Two-speed startup disabled.
		)

_FOSC(  FCKSM_CSDCMD  		// Clock switching and clock monitor disabled.
     &  IOL1WAY_OFF  	// Remappable IO off
   //&  OSCIOFNC_ON  	// OSC pin is digital IO (versus crystal driver)
     &  OSCIOFNC_OFF		// Osc2 pin is OSC output
   //&  POSCMD_NONE  	// Primary Osc disabled.
     &  POSCMD_HS         // HS osc
	 )

_FWDT(  FWDTEN_OFF  		// Watchdog timer disabled.
     &  WINDIS_OFF  		// Windowed WDT disabled.
     )

_FPOR(  ALTI2C_OFF 			// I2C on primary pins (SDA1/SCL1)
     &  FPWRT_PWR1 		// Power-on reset timer disabled.
     )

_FICD(  JTAGEN_OFF  		// JTAG disabled.
     &  ICS_PGD1			// In-Circuit Debug pins set to PG{C/D}1
     )

_FBS(  RBS_NO_RAM  			// No boot segment Data Ram
    &  BSS_NO_BOOT_CODE   	// No boot Segment Program Memory
    &  BWRP_WRPROTECT_OFF  	// Boot Write protection disabled
    )

_FSS(  RSS_NO_RAM  				// No secure segment data ram
    &  SSS_NO_FLASH  		// No secure segment
    &  SWRP_WRPROTECT_OFF	// Write protect disabled
    )

_FGS(  GSS_OFF 			// User program memory not code-protected
 		&  GCP_OFF  		// Code protect - in header..not in config window
		&  GWRP_OFF  		// General segment write protect disabled
		)

// UID registers.
//_FUID0( 0xFFFF )
//_FUID1( 0xFFFF )
//_FUID2( 0xFFFF )
//_FUID3( 0xFFFF )

Program DefaultProgram = {1.0, 1.0, 0.0, 1.0, 1.0, wave_sine};
InstrumentState GlobalState = {0.0, 0.0, &DefaultProgram};

void blink_alive( void ) {
	int i;
	for( i = 0;  i < 6;  i++ ) {
		STATUS_LED = ~STATUS_LED;
		__delay_ms(100);
		}
	}


void setup_pins( void ) {
	AD1PCFGL = 0b1111111111111111; // All digital
	TRISA = 0b1111111111111111;    // All inputs
	TRISB = 0b1111111111111111;    // All inputs
	STATUS_LED_TRIS = 0;
	LATA = 0b0000000000000000;     // All latches to 0
	LATB = 0b0000000000000000;	   // All latches to 0
	}


void start_pll( void ) {
	CLKDIVbits.PLLPRE = PLLPRE_BITS;   // Configure the prescaler
	CLKDIVbits.PLLPOST = PLLPOST_BITS; // Configure the postscaler
	PLLFBDbits.PLLDIV = PLLDIV_BITS;   // Configure the PLL divisor.
	while (!OSCCONbits.LOCK) { }
	}

void setup_interrupts() {
	SRbits.IPL = 0b000;                // Set CPU interrupt level to 0
	INTCON1bits.NSTDIS = 1;             // disable nested interrupts
}

void setup_watchdog() {
	RCONbits.SWDTEN = 0;                // turn off watchdog timer
}


void start_application(void) {
    init_voices();
    start_adc();
	start_dac();						// Setup dac control registers.
    start_uart();
	while(1) {
        STATUS_LED = ~STATUS_LED;       // Toggle LED pin so we can measure speed
		__delay_ms(500);
        // At this point the interrupt handlers should be running the instrument
		}
}

int main( void ) {
	setup_pins(); 						// Set port ins and outs.
    setup_interrupts();
    setup_watchdog();
	start_pll(); 						// Get system clock running
	blink_alive(); 						// Indicate to the user that we're alive.
    start_application();
    return 0;
}
