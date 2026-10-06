
// block 00494ad1
00494ad1  sub        esp, 0xc
00494ad4  fstp       xword ptr [esp]
00494ad7  fild       dword ptr [esp + 0x14]
00494adb  call       0x4948d9

// block 00494ae0
00494ae0  fld        xword ptr [esp]
00494ae3  fxch       st(1)
00494ae5  add        esp, 0xc
00494ae8  pop        eax
00494ae9  ret        4
