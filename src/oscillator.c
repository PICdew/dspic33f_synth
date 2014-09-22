
#include "oscillator.h"


struct Oscillator Oscillators[OSCCOUNT];
struct InstrumentState InstrumentState = { .pitcbend = 0.0, .masterVolume = 0.0} ;


void audio_rate_interrupt( void ) {
	float newSample = 0;
	for(char i = 0; i<=OSCCOUNT; i++) {
		if(Oscillators[i].phaseaccum += (Oscillators[i].phaseincr + InstrumentState.pitchbend) >= TABLESIZE) {
			Oscillators[i].phaseaccum -= TABLESIZE;
		}
		newSample += Oscillators[i].gain * InstrumentState.wavetable[(int)Oscillators[i].phaseaccum] ;
	}
	newSample *= InstrumentState.masterVolume;
	DAC1RDAT = (int)newSample;
	DAC1LDAT = (int)newSample;
}



void control_rate_interrupt( void ) {
	for(char i = 0; i<=OSCCOUNT; i++) {
		switch(Oscillator.State) {

			case SILENT:
				oscillator.gain = 0;
			    break;

			case ATTACK: 
				oscillator.gain += program.A;
				if(oscillator.gain > program.A_peak) oscillator.State = DECAY;
				break;

			case DECAY:
				oscillator.gain -= program.D;
				if(oscillator.gain <= program.S_level) oscillator.State = SUSTAIN;
				break;

			case SUSTAIN:
				break;

			case RELEASE:
				oscillator.gain -= program.R;
				if(oscillator.gain <= 0) oscillator.State = SILENT;
				break;

			default:
				break;
		}
	}
}
