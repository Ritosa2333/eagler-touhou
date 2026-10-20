
// block 00494a84
00494a84  push       eax
00494a85  fnstsw     ax
00494a87  and        eax, 0x3800
00494a8c  je         0x494a9b

// block 00494a8e
00494a8e  fild       word ptr [esp + 8]
00494a92  call       0x4948d9

// block 00494a97
00494a97  pop        eax
00494a98  ret        4

// block 00494a9b
00494a9b  fxch       st(1)
