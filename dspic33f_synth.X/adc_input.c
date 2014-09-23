
#include "midi.h"


#include <p33FJ128GP802.h>	// Pin and register definitions.

void start_adc( void ) {
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
     //AD1CON1bits.SAMP = 1;          // start first conversion
     }

// INT0 called when "the button" is pressed.
//  At that point, we trigger an ADC conversion, and let the ADC interrupt
//  finish sending the note on event.
void __attribute__((__interrupt__, no_auto_psv ))   _INT0Interrupt( void ) {
    AD1CON1bits.SAMP = 1;          // start conversion
	IFS0bits.INT0IF = 0;         // Clear Interrupt Flag
	}


// ADC1Interrupt called when ADC conversion completes.
//  The knob position has been sampled, so we issue the note on event.
void __attribute__((__interrupt__, no_auto_psv ))  _ADC1Interrupt( void ) {
    char note = ADC1BUF0 >> 8;
    unsigned char buf[] = {0x80, note, 127};
    handle_midi_packet_raw(buf);
	IFS0bits.AD1IF = 0; 					    // Clear Interrupt Flag
	}

