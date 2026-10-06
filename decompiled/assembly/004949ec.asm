
// block 004949ec
004949ec  push       eax
004949ed  mov        eax, dword ptr [esp + 8]
004949f1  and        eax, 0x7f800000
004949f6  cmp        eax, 0x7f800000
004949fb  je         0x494a30

// block 004949fd
004949fd  fnstsw     ax
004949ff  and        eax, 0x3800
00494a04  je         0x494a13

// block 00494a06
00494a06  fld        dword ptr [esp + 8]
00494a0a  call       0x4948d9

// block 00494a0f
00494a0f  pop        eax
00494a10  ret        4

// block 00494a13
00494a13  fxch       st(1)
00494a15  sub        esp, 0xc
00494a18  fstp       xword ptr [esp]
00494a1b  fld        dword ptr [esp + 0x14]
00494a1f  call       0x4948d9

// block 00494a24
00494a24  fld        xword ptr [esp]
00494a27  fxch       st(1)
00494a29  add        esp, 0xc
00494a2c  pop        eax
00494a2d  ret        4

// block 00494a30
00494a30  fdivr      dword ptr [esp + 8]
00494a34  pop        eax
00494a35  ret        4
