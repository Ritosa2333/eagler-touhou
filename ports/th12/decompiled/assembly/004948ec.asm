
// block 004948ec
004948ec  push       eax
004948ed  mov        eax, dword ptr [esp + 8]
004948f1  and        eax, 0x7f800000
004948f6  cmp        eax, 0x7f800000
004948fb  je         0x494930

// block 004948fd
004948fd  fnstsw     ax
004948ff  and        eax, 0x3800
00494904  je         0x494913

// block 00494906
00494906  fld        dword ptr [esp + 8]
0049490a  call       0x4948c6

// block 0049490f
0049490f  pop        eax
00494910  ret        4

// block 00494913
00494913  fxch       st(1)
00494915  sub        esp, 0xc
00494918  fstp       xword ptr [esp]
0049491b  fld        dword ptr [esp + 0x14]
0049491f  call       0x4948c6

// block 00494924
00494924  fld        xword ptr [esp]
00494927  fxch       st(1)
00494929  add        esp, 0xc
0049492c  pop        eax
0049492d  ret        4

// block 00494930
00494930  fdiv       dword ptr [esp + 8]
00494934  pop        eax
00494935  ret        4
