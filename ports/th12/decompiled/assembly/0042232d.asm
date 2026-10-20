
// block 0042232d
0042232d  sub        esp, 8
00422330  fstp       dword ptr [esp + 4]
00422334  fld        dword ptr [0x4a3ec0]
0042233a  fstp       dword ptr [esp]
0042233d  call       0x411b80

// block 00422342
00422342  mov        eax, dword ptr [0x4b0cb4]
00422347  cmp        eax, dword ptr [0x4b0cb0]
0042234d  je         0x42238b

// block 0042234f
0042234f  mov        eax, 0x4b0c40
00422354  call       0x4230a0

// block 00422359
00422359  or         dword ptr [0x4b0ce0], 8
00422360  jmp        0x42238b
