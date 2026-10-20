
// block 0049499d
0049499d  sub        esp, 0xc
004949a0  fstp       xword ptr [esp]
004949a3  fild       word ptr [esp + 0x14]
004949a7  call       0x4948c6

// block 004949ac
004949ac  fld        xword ptr [esp]
004949af  fxch       st(1)
004949b1  add        esp, 0xc
004949b4  pop        eax
004949b5  ret        4
