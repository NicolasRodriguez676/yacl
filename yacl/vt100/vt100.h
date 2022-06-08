#ifndef _VT100_H_
#define _VT100_H_

enum VT_100_Control_Sequences {
	ESC = 0x1b,
	CSI = 0x5b
};

void vt100_rst_term();
void vt100_backspace();

#endif //_VT100_H_
