
#include "midi_receiver.h"

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