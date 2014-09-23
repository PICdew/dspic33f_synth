/* 
 * File:   oscillator.h
 * Author: joe
 *
 * Created on September 22, 2014, 12:17 PM
 */

#ifndef VOICE_H
#define	VOICE_H

#ifdef	__cplusplus
extern "C" {
#endif


enum ADSR_STATE { SILENT, ATTACK, DECAY, SUSTAIN, RELEASE } ;

typedef struct _Voice {
	char note;
	char a_vel;
	char r_vel;
	char aftertouch;
	enum ADSR_STATE state;
	float phaseincr;
	float gain;
	float phaseaccum;
} Voice;


typedef struct _Wavetable {
    int samplerate;
    int tablesize;
    const int wavetable[];
} Wavetable ;

typedef struct _Program {
	float A;
	float A_peak;
	float D;
	float S_level;
	float R;
	const int *wavetable;
} Program ;


typedef struct _InstrumentState {
	float pitchbend;
	float masterVolume;
	Program *program;
} InstrumentState;



extern Voice Voices[];

void init_voices();
void start_dac( void );

#ifdef	__cplusplus
}
#endif

#endif	/* VOICE_H */

