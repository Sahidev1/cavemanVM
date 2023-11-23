## first iteration of idea: 

A virtual memory tape, where each adress/index on the tape contains a byte.
The computers also has access to a instruction tape where each instruction is 32-bits.
An extremely basic CPU with 5 registers, 2 32-bit input registers and 32 bit output register
, as well as a PC register which is a instruction tape pointer, and a  register for Memory tape pointer.
the CPU supports only 5 operations, addition, subtraction, bitwise AND, And an operation which add/subss an offset to the PC pointer,
Also another opearation which adds/subs an offset to mem pointer.
We also have a zero-flag register that is 1 if the previous result was 0.
