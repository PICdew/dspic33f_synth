

#include "main.h"
#include "voice.h"
#include "clock_init.h"

#include <p33FJ128GP802.h>	// Pin and register definitions.

Voice Voices[VOICECOUNT];

void init_voices() {
    int i;
    Voice defaultOsc = { 0, 0, 0, 0, SILENT, 0.0, 0.0, 0.0 };
    for(i=0; i<VOICECOUNT; i++) {
        Voices[i] = defaultOsc;
    }
}



void start_dac( void ) {
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


// DAC1RInterrupt called when Right channel FIFO needs data.
void __attribute__((__interrupt__, no_auto_psv ))  _DAC1RInterrupt( void ) {
	float newSample = 0;
    int i;
	for(i = 0; i<=VOICECOUNT; i++) {
		if(Voices[i].phaseaccum += (Voices[i].phaseincr + GlobalState.pitchbend) >= TABLESIZE) {
			Voices[i].phaseaccum -= TABLESIZE;
		}
		newSample += Voices[i].gain * GlobalState.program->wavetable[(int)Voices[i].phaseaccum] ;
	}
	newSample *= GlobalState.masterVolume;
	DAC1RDAT = (int)newSample;
	DAC1LDAT = (int)newSample;
	IFS4bits.DAC1RIF = 0; 					    // Clear Interrupt Flag
	}

// DAC1LInterrupt called when Left channel FIFO needs data.
void __attribute__((__interrupt__, no_auto_psv )) _DAC1LInterrupt( void ) {
	DAC1LDAT = 0; 	// Write next sample value
	IFS4bits.DAC1LIF = 0; 						// Clear Interrupt Flag
	}



void start_timer(void) {

}

void  __attribute__((__interrupt__, no_auto_psv )) _T1Interrupt( void ) {
    int i;
	for(i = 0; i<=VOICECOUNT; i++) {
		switch(Voices[i].state) {

			case SILENT:
				Voices[i].gain = 0;
			    break;

			case ATTACK:
				Voices[i].gain += GlobalState.program->A;
				if(Voices[i].gain > GlobalState.program->A_peak) Voices[i].state = DECAY;
				break;

			case DECAY:
				Voices[i].gain -= GlobalState.program->D;
				if(Voices[i].gain <= GlobalState.program->S_level) Voices[i].state = SUSTAIN;
				break;

			case SUSTAIN:
				break;

			case RELEASE:
				Voices[i].gain -= GlobalState.program->R;
				if(Voices[i].gain <= 0) Voices[i].state = SILENT;
				break;

			default:
				break;
		}
	}
}