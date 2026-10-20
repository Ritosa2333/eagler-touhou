
// block 00494a38
00494a38  push       eax
00494a39  mov        eax, dword ptr [esp + 0xc]
00494a3d  and        eax, 0x7ff00000
00494a42  cmp        eax, 0x7ff00000
00494a47  je         0x494a7c

// block 00494a49
00494a49  fnstsw     ax
00494a4b  and        eax, 0x3800
00494a50  je         0x494a5f

// block 00494a52
00494a52  fld        qword ptr [esp + 8]
00494a56  call       0x4948d9

// block 00494a5b
00494a5b  pop        eax
00494a5c  ret        8

// block 00494a5f
00494a5f  fxch       st(1)
00494a61  sub        esp, 0xc
00494a64  fstp       xword ptr [esp]
00494a67  fld        qword ptr [esp + 0x14]
00494a6b  call       0x4948d9

// block 00494a70
00494a70  fld        xword ptr [esp]
00494a73  fxch       st(1)
00494a75  add        esp, 0xc
00494a78  pop        eax
00494a79  ret        8

// block 00494a7c
00494a7c  fdivr      qword ptr [esp + 8]
00494a80  pop        eax
00494a81  ret        8
