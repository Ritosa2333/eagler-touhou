
// block 004949b8
004949b8  push       eax
004949b9  fnstsw     ax
004949bb  and        eax, 0x3800
004949c0  je         0x4949cf

// block 004949c2
004949c2  fild       dword ptr [esp + 8]
004949c6  call       0x4948c6

// block 004949cb
004949cb  pop        eax
004949cc  ret        4

// block 004949cf
004949cf  fxch       st(1)
