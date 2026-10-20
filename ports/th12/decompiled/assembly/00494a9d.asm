
// block 00494a9d
00494a9d  sub        esp, 0xc
00494aa0  fstp       xword ptr [esp]
00494aa3  fild       word ptr [esp + 0x14]
00494aa7  call       0x4948d9

// block 00494aac
00494aac  fld        xword ptr [esp]
00494aaf  fxch       st(1)
00494ab1  add        esp, 0xc
00494ab4  pop        eax
00494ab5  ret        4
