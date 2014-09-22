
#include <p33FJ128GP802.h>	// Pin and register definitions.


// Config flags:

_FOSCSEL(
		FNOSC_PRIPLL		// Primary osc with PLL
		//FNOSC_FRCPLL     	// Use internal FRC osc with PLL.
        &  IESO_OFF     	// Two-speed startup disabled.
		)

_FOSC(  
		FCKSM_CSDCMD  		// Clock switching and clock monitor disabled.
        &  IOL1WAY_OFF  	// Remappable IO off
        //&  OSCIOFNC_ON  	// OSC pin is digital IO (versus crystal driver)
		& OSCIOFNC_OFF		// Osc2 pin is OSC output
        //&  POSCMD_NONE  	// Primary Osc disabled.
		& POSCMD_XT         // XT osc
		)

_FWDT(  
		FWDTEN_OFF  		// Watchdog timer disabled. 
		&  WINDIS_OFF  		// Windowed WDT disabled.
		)

_FPOR(  
		ALTI2C_OFF 			// I2C on primary pins (SDA1/SCL1)
		&  FPWRT_PWR1 		// Power-on reset timer disabled.
		)

_FICD(  
		JTAGEN_OFF  		// JTAG disabled.  
		&  ICS_PGD1			// In-Circuit Debug pins set to PG{C/D}1
		)

_FBS(  
		RBS_NO_RAM  			// No boot segment Data Ram
		&  BSS_NO_BOOT_CODE   	// No boot Segment Program Memory
		&  BWRP_WRPROTECT_OFF  	// Boot Write protection disabled
		)

_FSS(  RSS_NO_RAM  				// No secure segment data ram
		&  SSS_NO_FLASH  		// No secure segment
		&  SWRP_WRPROTECT_OFF	// Write protect disabled
		)

_FGS(
		GSS_OFF 			// User program memory not code-protected
 		&  GCP_OFF  		// Code protect - in header..not in config window
		&  GWRP_OFF  		// General segment write protect disabled
		)

// UID registers.
//_FUID0( 0xFFFF )
//_FUID1( 0xFFFF )
//_FUID2( 0xFFFF )
//_FUID3( 0xFFFF )



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


// Desired clock config values
// These are for the internal FRC
//#define FRCFREQ         7372800UL
//#define PLL_N1			2
//#define PLL_N2			2
//#define PLL_M			40
//#define SAMPLE_RATE     48000
//#define ACLK_POST       1
//#define FCY				((PLL_M * FRCFREQ) / (PLL_N1 * PLL_N2 * 2))
//#define Fcy             FCY


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


// Pin definitions for this application.

#define STATUS_LED		PORTAbits.RA0
#define STATUS_LED_TRIS	TRISAbits.TRISA0


// Library includes.

#include <libpic30.h>       // Needed for __delay32()

#include "q12_20.c"

// Global variables.

volatile double tune;
volatile double phase_accum_left = 0;
volatile double phase_accum_right = 0;
const double notes[128];  // actual tables at end for readability.
const int wavetable[4096];		

volatile Q12_20 testvar = (Q12_20)1<<FRACBITS;

// Application functions

void blink_alive( void ) {
	int i;
    
	STATUS_LED_TRIS = 0;
    
	for( i = 0;  i < 3;  i++ ) {
        
		STATUS_LED = 0;
		__delay_ms(100);
        
		STATUS_LED = 1;
		__delay_ms(100);
        
		}	
	}




void setup_osc( void ) {
	CLKDIVbits.PLLPRE = PLLPRE_BITS;   // Configure the prescaler
	CLKDIVbits.PLLPOST = PLLPOST_BITS; // Configure the postscaler
	PLLFBDbits.PLLDIV = PLLDIV_BITS;   // Configure the PLL divisor.
	while (! OSCCONbits.LOCK) {		 	// Loop until the PLL achieves lock.
		Nop();  
		}
	}



void setup_pins( void ) {
	AD1PCFGL = 0b1111111111111111; // All digital
	TRISA = 0b1111111111111111;    // All inputs
	TRISB = 0b1111111111111111;    // All inputs
	LATA = 0b0000000000000000;     // All latches to 0
	LATB = 0b0000000000000000;	   // All latches to 0
	}

    
    
    
    
void setup_adc( void ) {
     AD1CON1bits.AD12B = 1;         // 12-bit ADC operation
     AD1CON2bits.VCFG = 4;          // select VDD/VSS as ref voltage
     AD1CON3bits.ADRC = 1;          // ADC Clock from internal RC source
     //AD1CON3bits.ADCS = 5;        // ADC Conversion Clock 
     AD1PCFGL = 0b1111111111111101; // AN1 enabled
     AD1CHS0bits.CH0SA = 1;         // MUXA +ve input selection (AIN1) for CH0
     AD1CHS0bits.CH0NA = 0;         // MUXA -ve input selection (Vref-) for CH0
     AD1CHS0bits.CH0SB = 1;         // MUXB +ve input selection (AIN1) for CH0
     AD1CHS0bits.CH0NB = 0;         // MUXB -ve input selection (Vref-) for CH0
     AD1CON1bits.SIMSAM = 0;        // Do not sample multiple channels
     AD1CON1bits.ASAM = 1;          // ADC Sample Control: Auto sampling start
     AD1CON1bits.SSRC = 7;          // Sample Clock Source: auto timer starts conversion
     AD1CON1bits.FORM = 2;          // Data Output Format: left aligned integer (signed frac)
     AD1CON2bits.BUFM = 0;          // DMA not used
     AD1CON2bits.CSCNA = 0;         // Do not scan inputs
     AD1CON2bits.CHPS = 0;          // Converts CH0
     AD1CON3bits.SAMC =  6;         // Auto sample time
     AD1CHS0  = 1;                  // select AN1
     IFS0bits.AD1IF = 0;            // clear int flag
     IEC0bits.AD1IE = 0;            // no interrupts
     AD1CON1bits.ADON = 1;          // enable ADC1
     AD1CON1bits.SAMP = 1;          // start first conversion
     // At this point the ADC is running on its own.
     // ADC1BUF0 will be holding the last sample.
    }


void setup_dac( void ) {
	ACLKCONbits.SELACLK = 0; 			// use PLL VCO, rather than alternate OSC
	ACLKCONbits.AOSCMD = 0b00; 			// Disable Alternate Osc driver
	ACLKCONbits.APSTSCLR = ACLKPOSTSCLR_VAL; // Divide VCO to produce DAC clock.  
    ACLKCONbits.ASRCSEL = 1; 			// Use primary OSC not Alternate OSC.
	
	DAC1CONbits.DACSIDL = 0; 			// DAC remains on when idle.
	DAC1CONbits.AMPON = 1;  			// Amps remain on when idle.
	DAC1CONbits.FORM = 1; 				// Use signed integer format.
    DAC1CONbits.DACFDIV = DACFDIV_VAL; 	// Set the sample rate divisor from computed value.

	DAC1STATbits.LOEN = 1;  			// Turn on the left channel
	DAC1STATbits.LMVOEN = 0; 			// Disable midpoint voltage output.
	DAC1STATbits.LITYPE = 0; 			// Interrupt when FIFO "not full"

	DAC1STATbits.ROEN = 1;  			// Turn on the right channel
	DAC1STATbits.RMVOEN = 0;			// Disable midpoint voltage output
	DAC1STATbits.RITYPE = 0; 			// Interrupt when FIFO "not full"

	DAC1DFLT = (int)0;                 // Set the default value in case of FIFO underrun.

	IFS4bits.DAC1RIF = 0; 				// Clear Right Channel Interrupt Flag
	IFS4bits.DAC1LIF = 0; 				// Clear Left Channel Interrupt Flag

	IPC19bits.DAC1RIP = 0b111;			// Set interrupt priority for Right Channel
	IPC19bits.DAC1LIP = 0b111;			// Set interrupt priority for Left Channel

	IEC4bits.DAC1RIE = 1; 				// Right Channel Interrupt Enabled
	//IEC4bits.DAC1LIE = 1; 				// Left Channel Interrupt Enabled

    DAC1RDAT = (int)0;                 // Get the FIFOs primed.
    DAC1LDAT = (int)0;

	DAC1CONbits.DACEN = 1; 				// Turn on the DAC.
    }

   

void set_tune( double  hz ) {
	tune = ((double)4096 * (double)hz) / (double)SAMPLE_RATE;
	}	


	
void set_note(unsigned char n) {
	set_tune( notes[n&0x7F] );
	}
	
	
int main( void ) {
	unsigned char notenum = 69;
 	testvar = (Q12_20)1<<FRACBITS;
	SRbits.IPL = 0b000;                // Set CPU interrupt level to 0
	INTCON1bits.NSTDIS = 1;
	RCONbits.SWDTEN = 0;
	setup_pins(); 						// Set port ins and outs.
	setup_osc(); 						// Get system clock running
	blink_alive(); 						// Indicate to the user that we're alive.
    setup_adc();
    set_note(notenum);
	setup_dac();						// Setup dac control registers.
	while(1) {
        STATUS_LED = !STATUS_LED;       // Toggle LED pin so we can measure speed
        notenum = (ADC1BUF0>>9)&0x7f;  // Select note from position of knob.
        //if(notenum >= 127) {
		//	 notenum = 0;
		//	}
		//notenum ++;
		set_note(notenum);
		//__delay_ms(1000);
		}
	}


    
// ADC1Interrupt called when ADC conversion completes. 

void __attribute__((__interrupt__, no_auto_psv ))  _ADC1Interrupt( void ) {
	IFS0bits.AD1IF = 0; 					    // Clear Interrupt Flag 
    
    
	}    
    
    
                                    
// DAC1RInterrupt called when Right channel FIFO needs data. 

void __attribute__((__interrupt__, no_auto_psv ))  _DAC1RInterrupt( void ) {
	IFS4bits.DAC1RIF = 0; 					    // Clear Interrupt Flag 
	phase_accum_right += tune;				    // Increment phase angle
	if(phase_accum_right >= 4096) {				// Handle overflow
		phase_accum_right -= 4096;
		}	
	DAC1RDAT =  wavetable[(int)phase_accum_right]; 	// Write next sample value
	}


    
// DAC1LInterrupt called when Left channel FIFO needs data. 

void __attribute__((__interrupt__, no_auto_psv )) _DAC1LInterrupt( void ) {
	IFS4bits.DAC1LIF = 0; 						// Clear Interrupt Flag 
	phase_accum_left += tune;					// Increment phase angle
	if(phase_accum_left >= 4096) {				// Handle overflow
		phase_accum_left -= 4096;
		}
	DAC1LDAT = wavetable[(int)phase_accum_left]; 	// Write next sample value
	}

#include "midi_note_freqs.c"


// Full sine table.
// 4096 entries = 12 bit phase resolution
#include "wave_sine.c"