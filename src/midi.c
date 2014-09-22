

#define OSCCOUNT 32
#define TABLESIZE 4096


void do_note_on(char keynumber, char velocity) {
	char i;
	for(i=0; i<OSCCOUNT; i++) {
		if(Oscillators[i].keynumber == keynumber) {
			Oscillators[i].gain = 0;
			Oscillators[i].note = keynumber;
			Oscillators[i].phaseincr = midi_notes[keynumber];
			Oscillators[i].a_vel = velocity;
			Oscillators[i].state = ATTACK;
			return;
		}
	}
	nextOscillator = (nextOscillator + 1) % OSCCOUNT;
	Oscillators[nextOscillator].gain = 0;
	Oscillators[nextOscillator].note = keynumber;
	Oscillators[nextOscillator].phaseincr = midi_notes[keynumber];
	Oscillators[nextOscillator].a_vel = velocity;
	Oscillators[nextOscillator].state = ATTACK;
}

void do_note_off(char keynumber, char velocity) {
	for(char i=0; i<OSCCOUNT; i++) {
		if(Oscillators[i].keynumber == keynumber) {
			Oscillators[i].r_vel = velocity;
			Oscillators[i].state = RELEASE;
		}
	}
}

void do_aftertouch(char keynumber, char pressure) {
	for(char i=0; i<OSCCOUNT; i++) {
		if(Oscillators[i].keynumber == keynumber) {
			Oscillators[i].aftertouch = pressure;
		}
	}

}


int ControlTable[120] = 0;

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


struct MidiChannel {
	struct Program *program;
	char[127] controllerSettings;
	char programNumber;
	char currentAftertouch;
}




struct GlobalState {
	struct MidiChannel channel;
	struct Oscillator[Oscillator] Oscillators;
	char nextEmptyOscillator = 0;
}

struct GlobalState global_state = 0;


/* audio_rate_interrupt() 

Computes the next sample to be delivered to the audio DAC.
Intended to be called by the "FIFO empty" interrupt of the
audio peripheral.

Each Oscillator's phase is incremented and the Oscillator's value is
scaled to the gain of that Oscillator and added to the final output. 
*/




void audio_rate_interrupt( void ) {
	float newSample = 0;
	for(char i = 0; i<=OSCCOUNT; i++) {
		if(Oscillators[i].phaseaccum += (Oscillators[i].phaseincr + Oscillators[i].pitchbend) >= TABLESIZE) {
			Oscillators[i].phaseaccum -= TABLESIZE;
		}
		newSample += Oscillators[i].gain * Oscillators[i].program.wavetable[(int)Oscillators[i].phaseaccum] ;
	}
	DAC1R = newSample;
	DAC1L = newSample;
}



/* control_rate_interrupt()

the adsr values are computed for each Oscillator
*/


void control_rate_interrupt( void ) {
	for(char i = 0; i<=OSCCOUNT; i++) {
		switch(Oscillator.State) {

			case SILENT:
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

void process_Oscillators( void ) {
	for(char i = 0; i<CHANNELS; i++) {
		Oscillator* Oscillator = global_state.Oscillators[i]
		process_Oscillator_controllers(Oscillator);
	}
}







enum MidiLineState { IDLE,  NEED_2, NEED_1 } ;

struct MidiReceiverState {
	long midi_error_count;
	unsigned char rcvbuffer[3];
	char myChannel;
	enum MidiLineState lineState;
}


void process_midi_byte(struct MidiReceiverState *state, unsigned char inbyte) {
	switch(state->lineState) {

		case IDLE:
			if(inbyte && 0x80 == 0) {
				// received a data byte when it should have been a command byte
				state->midi_error_count++;
				break;
			}
			if(inbyte && 0x0F != state->myChannel) {
				// discard messages not bound for this instrument
				break;
			}
			switch(byte && 0xF0) {
				case 0x80: state->lineState = NEED_2; break;
				case 0x90: state->lineState = NEED_2; break;
				case 0xA0: state->lineState = NEED_2; break;
				case 0xB0: state->lineState = NEED_2; break;
				case 0xC0: state->lineState = NEED_1; break;
				case 0xD0: state->lineState = NEED_1; break;
				case 0xE0: state->lineState = NEED_2; break;
				case 0xF0: break; // ignore System messages for now.
			}

		case NEED_2:
			if(inbyte && 0x80) {
				state->midi_error_count++;
				state->lineState = IDLE;
				break;
			}
			state->rcvbuffer[1] = inbyte;
			state->lineState = NEED_1;
			break;

		case NEED_1:
			if(inbyte && 0x80) {
				state->midi_error_count++;
				state->lineState = IDLE;
				break;
			}
			state->rcvbuffer[2] = inbyte;
			switch(state->rcvbuffer[0] && 0xF0) {
				case 0x80: do_note_on(state->rcvbuffer[1], state->rcvbuffer[2]); break;
				case 0x90: do_note_off(state->rcvbuffer[1], state->rcvbuffer[2]); break;
				case 0xA0: do_aftertouch(state->rcvbuffer[1], state->rcvbuffer[2]); break;
				case 0xB0: do_control_change(state->rcvbuffer[1], state->rcvbuffer[2]); break;
				case 0xC0: do_patch_change(state->rcvbuffer[1]); break;
				case 0xD0: do_channel_pressure(state->rcvbuffer[1]); break;
				case 0xE0: do_pitch_bend((int)state->rcvbuffer[2] << 8 + state->rcvbuffer[1]); break;
				case 0xF0: do_system_message(state->rcvbuffer[1], state->rcvbuffer[2]);
			}
			state->lineState = IDLE;
			break;
	}
}