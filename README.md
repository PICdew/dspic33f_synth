dspic33f_synth
==============

A wavetable synthesizer for the Microchip Corp dsPIC33FJ128GP802 16-bit micro

This is a Microchip MPLABX project in C.


This application uses the dsPIC33FJ128GP802 audio DAC to produce audio at 32Ksamples/s.

The instrument is free-running based on event handlers to the DAC FIFO and Timer interrupts.
The DAC-rate interrupt is reffered to as the "audio-rate" interrupt, while the 1khz timer
interrupt is referred to as the "control-rate" interrupt, as in csound. 

With each audio interrupt, a sample is computed by summing the contribution from an array of wavetable voices.
With each timer interrupt, the ADSR parameters are evaluated and the voice gain adjusted.

The instrument maintains a "program" that includes a pointer to the wavetable, and ADSR
parameters for keying events. 

When midi events are received (or synthesized by pressing the instrument's "key"), the "next" voice is
updated with the parameters of that "patch" and the voice plays out until the ADSR loop takes the envelope back
down to zero.

