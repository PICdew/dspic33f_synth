/* 
 * File:   midi_events.h
 * Author: joe
 *
 * Created on September 22, 2014, 3:57 PM
 */

#ifndef MIDI_H
#define	MIDI_H

#ifdef	__cplusplus
extern "C" {
#endif


#define MIDI_NOTE_COUNT 128
#define MIDI_BAUD 32500
#define BRGVAL ((FCY/MIDI_BAUD)/16)-1

extern const float midi_notes[MIDI_NOTE_COUNT] ;

enum MidiLineState { IDLE,  NEED_2, NEED_1 } ;

typedef struct _MidiReceiverState {
	long midi_error_count;
	unsigned char rcvbuffer[3];
	char myChannel;
	enum MidiLineState lineState;
} MidiReceiverState;

void process_midi_byte(MidiReceiverState *state, unsigned char inbyte);
void handle_midi_packet_raw(unsigned char buf[]);
void start_uart(void);

void do_note_on(char keynumber, char velocity);
void do_note_off(char keynumber, char velocity);
void do_aftertouch(char keynumber, char pressure);
void do_control_change(char ctrlnumber, char value);
void do_patch_change(char prognumber);
void do_channel_pressure(char pressure);
void do_pitch_bend(int pitchdelta);
void do_system_message(char byte1, char byte2);


#ifdef	__cplusplus
}
#endif

#endif	/* MIDI_H */

