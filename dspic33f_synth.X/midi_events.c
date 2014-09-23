

#include "midi.h"
#include "main.h"
#include "voice.h"

int nextVoice = 0;

void do_note_on(char keynumber, char velocity) {
	int i;
	for(i=0; i<VOICECOUNT; i++) {
		if(Voices[i].note == keynumber) {
			Voices[i].gain = 0;
			Voices[i].note = keynumber;
			Voices[i].phaseincr = midi_notes[(int)keynumber];
			Voices[i].a_vel = velocity;
			Voices[i].state = ATTACK;
			return;
		}
	}
	nextVoice = (nextVoice + 1) % VOICECOUNT;
	Voices[nextVoice].gain = 0;
	Voices[nextVoice].note = keynumber;
	Voices[nextVoice].phaseincr = midi_notes[(int)keynumber];
	Voices[nextVoice].a_vel = velocity;
	Voices[nextVoice].state = ATTACK;
}

void do_note_off(char keynumber, char velocity) {
    int i;
	for(i=0; i<VOICECOUNT; i++) {
		if(Voices[i].note == keynumber) {
			Voices[i].r_vel = velocity;
			Voices[i].state = RELEASE;
		}
	}
}

void do_aftertouch(char keynumber, char pressure) {
    int i;
	for(i=0; i<VOICECOUNT; i++) {
		if(Voices[i].note == keynumber) {
			Voices[i].aftertouch = pressure;
		}
	}

}



void do_control_change(char ctrlnumber, char value) {
	switch(ctrlnumber) {
		case 120: break; /* All Sound Off */
		case 121: break; /* Reset All Controllers */
		case 122: break; /* Local Control On/Off */
		case 123: break; /* All Notes Off */
		case 124: break; /* Omni Mode off */
		case 125: break; /* Omni mode on */
		case 126: break; /* Mono Mode on (Poly off) */
		case 127: break; /* Poly Mode On (Mono Off) */
		default: break;
	}
}



void do_patch_change(char prognumber) {

}

void do_channel_pressure(char pressure) {

}

void do_pitch_bend(int pitchdelta) {
    GlobalState.pitchbend = pitchdelta;

}

void do_system_message(char byte1, char byte2) {

}
