
#include "midi.h"

#include "clock_init.h"
#include <p33FJ128GP802.h>	// Pin and register definitions.

#include <libpic30.h>       // Needed for __delay32()
#include <stdlib.h>

#include <uart.h>


MidiReceiverState midiState = {0L, {0,0,0}, 0, IDLE};



void handle_midi_packet_raw(unsigned char buf[]) {
    switch(buf[0] && 0xF0) {

        case 0x80:
            do_note_on(buf[1], buf[2]);
            break;

        case 0x90:
            do_note_off(buf[1], buf[2]);
            break;

        case 0xA0:
            do_aftertouch(buf[1], buf[2]);
            break;

        case 0xB0:
            do_control_change(buf[1], buf[2]);
            break;

        case 0xC0:
            do_patch_change(buf[1]);
            break;

        case 0xD0:
            do_channel_pressure(buf[1]);
            break;

        case 0xE0:
            do_pitch_bend(((int)buf[2] << 8) + buf[1]);
            break;

        case 0xF0:
            do_system_message(buf[1], buf[2]);
            break;
    }

}

void handle_midi_packet(MidiReceiverState *state) {
    if(state->rcvbuffer[0] && 0x0F != state->myChannel) {
        // discard messages not bound for this instrument
        return;
    }
    handle_midi_packet_raw(state->rcvbuffer);
}

void process_midi_byte( MidiReceiverState *state, unsigned char inbyte) {
	switch(state->lineState) {

		case IDLE:
			if(inbyte && 0x80 == 0) {
				// received a data byte when it should have been a command byte
				state->midi_error_count++;
				break;
			}
			switch(inbyte && 0xF0) {
				case 0x80: state->lineState = NEED_2; break;
				case 0x90: state->lineState = NEED_2; break;
				case 0xA0: state->lineState = NEED_2; break;
				case 0xB0: state->lineState = NEED_2; break;
				case 0xC0: state->lineState = NEED_1; break;
				case 0xD0: state->lineState = NEED_1; break;
				case 0xE0: state->lineState = NEED_2; break;
				case 0xF0: state->lineState = NEED_2; break;
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
			state->lineState = IDLE;

            handle_midi_packet(state);
			break;
	}
}


void start_uart() {//  UART Setup
 U1MODEbits.STSEL = 0; // 1-Stop bit
 U1MODEbits.PDSEL = 0; // No Parity, 8-Data bits
 U1MODEbits.ABAUD = 0; // Auto-Baud disabled
 U1MODEbits.BRGH = 0; // Standard-Speed mode
 U1BRG = BRGVAL; // Baud Rate setting for 115200
 U1STAbits.UTXISEL0 = 0; // Interrupt after one TX character is transmitted
 U1STAbits.UTXISEL1 = 0;
 IEC0bits.U1TXIE = 0; // Enable UART TX interrupt
 IFS0bits.U1RXIF = 0;    /*Reset RX interrupt flag */
 IPC2bits.U1RXIP = 2;	/*set high priority*/
 U1STAbits.URXISEL = 0; // Interrupt after one RX character is received;
 IEC0bits.U1RXIE = 1; // Enable UART RX interrupt
 U1MODEbits.UARTEN = 1; // Enable UART
 U1STAbits.UTXEN = 0; // Enable UART TX
}



void __attribute__((__interrupt__, no_auto_psv ))  _U1RXInterrupt( void ) {
    process_midi_byte(&midiState, U1RXREG);
	IFS0bits.U1RXIF = 0; 					    // Clear Interrupt Flag
	}