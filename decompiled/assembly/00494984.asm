
// block 00494984
00494984  push       eax
00494985  fnstsw     ax
00494987  and        eax, 0x3800
0049498c  je         0x49499b

// block 0049498e
0049498e  fild       word ptr [esp + 8]
00494992  call       0x4948c6

// block 00494997
00494997  pop        eax
00494998  ret        4

// block 0049499b
0049499b  fxch       st(1)
