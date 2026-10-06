
// block 00494ab8
00494ab8  push       eax
00494ab9  fnstsw     ax
00494abb  and        eax, 0x3800
00494ac0  je         0x494acf

// block 00494ac2
00494ac2  fild       dword ptr [esp + 8]
00494ac6  call       0x4948d9

// block 00494acb
00494acb  pop        eax
00494acc  ret        4

// block 00494acf
00494acf  fxch       st(1)
