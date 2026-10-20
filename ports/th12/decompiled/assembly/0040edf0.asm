
// block 0040edf0
0040edf0  push       esi
0040edf1  push       0x790
0040edf6  call       0x46c9ea

// block 0040edfb
0040edfb  add        esp, 4
0040edfe  test       eax, eax
0040ee00  je         0x40ee0d

// block 0040ee02
0040ee02  mov        esi, eax
0040ee04  call       0x40ea30

// block 0040ee09
0040ee09  mov        esi, eax
0040ee0b  jmp        0x40ee0f

// block 0040ee0d
0040ee0d  xor        esi, esi

// block 0040ee0f
0040ee0f  push       esi
0040ee10  call       0x40ead0

// block 0040ee15
0040ee15  test       eax, eax
0040ee17  je         0x40ee30

// block 0040ee19
0040ee19  test       esi, esi
0040ee1b  je         0x40ee2c

// block 0040ee1d
0040ee1d  push       esi
0040ee1e  call       0x40eba0

// block 0040ee23
0040ee23  push       esi
0040ee24  call       0x46ca4f

// block 0040ee29
0040ee29  add        esp, 4

// block 0040ee2c
0040ee2c  xor        eax, eax
0040ee2e  pop        esi
0040ee2f  ret        

// block 0040ee30
0040ee30  mov        eax, esi
0040ee32  pop        esi
0040ee33  ret        
