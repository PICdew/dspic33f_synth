
enum MidiLineState { IDLE,  NEED_2, NEED_1 } ;

struct MidiReceiverState {
	long midi_error_count;
	unsigned char rcvbuffer[3];
	char myChannel;
	enum MidiLineState lineState;
}

void process_midi_byte(struct MidiReceiverState *state, unsigned char inbyte);

