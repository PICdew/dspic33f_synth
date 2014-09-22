
#define OSCCOUNT 32
#define TABLESIZE 4096



enum ADSR_STATE { SILENT, ATTACK, DECAY, SUSTAIN, RELEASE } ;

struct Oscillator {
	char note;
	char a_vel;
	char r_vel;
	char aftertouch;
	enum ADSR_STATE state;
	float phaseincr;
	float gain;
	float phaseaccum;
}


struct InstrumentState {
	float pitchbend;
	float masterVolume;
	struct Program *program;
}