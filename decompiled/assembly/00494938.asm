
// block 00494938
00494938  push       eax
00494939  mov        eax, dword ptr [esp + 0xc]
0049493d  and        eax, 0x7ff00000
00494942  cmp        eax, 0x7ff00000
00494947  je         0x49497c

// block 00494949
00494949  fnstsw     ax
0049494b  and        eax, 0x3800
00494950  je         0x49495f

// block 00494952
00494952  fld        qword ptr [esp + 8]
00494956  call       0x4948c6

// block 0049495b
0049495b  pop        eax
0049495c  ret        8

// block 0049495f
0049495f  fxch       st(1)
00494961  sub        esp, 0xc
00494964  fstp       xword ptr [esp]
00494967  fld        qword ptr [esp + 0x14]
0049496b  call       0x4948c6

// block 00494970
00494970  fld        xword ptr [esp]
00494973  fxch       st(1)
00494975  add        esp, 0xc
00494978  pop        eax
00494979  ret        8

// block 0049497c
0049497c  fdiv       qword ptr [esp + 8]
00494980  pop        eax
00494981  ret        8
