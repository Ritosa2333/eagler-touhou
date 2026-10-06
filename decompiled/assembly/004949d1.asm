
// block 004949d1
004949d1  sub        esp, 0xc
004949d4  fstp       xword ptr [esp]
004949d7  fild       dword ptr [esp + 0x14]
004949db  call       0x4948c6

// block 004949e0
004949e0  fld        xword ptr [esp]
004949e3  fxch       st(1)
004949e5  add        esp, 0xc
004949e8  pop        eax
004949e9  ret        4
