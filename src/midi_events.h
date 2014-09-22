

void do_note_on(char keynumber, char velocity);
void do_note_off(char keynumber, char velocity);
void do_aftertouch(char keynumber, char pressure);
void do_control_change(char ctrlnumber, char value);
void do_patch_change(char prognumber);
void do_channel_pressure(char pressure);
void do_pitch_bend(int pitchdelta);

