
// block 004551b0
004551b0  fld        dword ptr [esp + 4]
004551b4  push       esi
004551b5  fld        st(0)
004551b7  mov        esi, eax
004551b9  call       0x4931e0

// block 004551be
004551be  add        eax, 0xffffd8f0
004551c3  cmp        eax, 0x19
004551c6  ja         0x45537a

// block 004551cc
004551cc  jmp        dword ptr [eax*4 + 0x455380]

// block 004551d3
004551d3  fstp       st(0)
004551d5  fild       dword ptr [esi + 0x3fc]
004551db  pop        esi
004551dc  ret        4

// block 004551df
004551df  fstp       st(0)
004551e1  fild       dword ptr [esi + 0x400]
004551e7  pop        esi
004551e8  ret        4

// block 004551eb
004551eb  fstp       st(0)
004551ed  fild       dword ptr [esi + 0x404]
004551f3  pop        esi
004551f4  ret        4

// block 004551f7
004551f7  fstp       st(0)
004551f9  fild       dword ptr [esi + 0x408]
004551ff  pop        esi
00455200  ret        4

// block 00455203
00455203  fstp       st(0)
00455205  fld        dword ptr [esi + 0x40c]
0045520b  pop        esi
0045520c  ret        4

// block 0045520f
0045520f  fstp       st(0)
00455211  fld        dword ptr [esi + 0x410]
00455217  pop        esi
00455218  ret        4

// block 0045521b
0045521b  fstp       st(0)
0045521d  fld        dword ptr [esi + 0x414]
00455223  pop        esi
00455224  ret        4

// block 00455227
00455227  fstp       st(0)
00455229  fld        dword ptr [esi + 0x418]
0045522f  pop        esi
00455230  ret        4

// block 00455233
00455233  fstp       st(0)
00455235  fild       dword ptr [esi + 0x41c]
0045523b  pop        esi
0045523c  ret        4

// block 0045523f
0045523f  fstp       st(0)
00455241  fild       dword ptr [esi + 0x420]
00455247  pop        esi
00455248  ret        4

// block 0045524b
0045524b  test       byte ptr [esi + 0x480], 1
00455252  fstp       st(0)
00455254  mov        esi, 0x4ce560
00455259  jne        0x455260

// block 0045525b
0045525b  mov        esi, 0x4ce568

// block 00455260
00455260  call       0x4645b0

// block 00455265
00455265  fstp       dword ptr [esp + 8]
00455269  fld        dword ptr [esp + 8]
0045526d  pop        esi
0045526e  ret        4

// block 00455271
00455271  test       byte ptr [esi + 0x480], 1
00455278  fstp       st(0)
0045527a  mov        esi, 0x4ce560
0045527f  jne        0x455286

// block 00455281
00455281  mov        esi, 0x4ce568

// block 00455286
00455286  call       0x4645e0

// block 0045528b
0045528b  fstp       dword ptr [esp + 8]
0045528f  fld        dword ptr [esp + 8]
00455293  pop        esi
00455294  ret        4

// block 00455297
00455297  test       byte ptr [esi + 0x480], 1
0045529e  fstp       st(0)
004552a0  fld        dword ptr [0x4a4098]
004552a6  push       ecx
004552a7  fstp       dword ptr [esp]
004552aa  mov        esi, 0x4ce560
004552af  jne        0x4552b6

// block 004552b1
004552b1  mov        esi, 0x4ce568

// block 004552b6
004552b6  call       0x410920

// block 004552bb
004552bb  fstp       dword ptr [esp + 8]
004552bf  fld        dword ptr [esp + 8]
004552c3  pop        esi
004552c4  ret        4

// block 004552c7
004552c7  fstp       st(0)
004552c9  fld        dword ptr [esi + 0x424]
004552cf  pop        esi
004552d0  ret        4

// block 004552d3
004552d3  fstp       st(0)
004552d5  fld        dword ptr [esi + 0x428]
004552db  pop        esi
004552dc  ret        4

// block 004552df
004552df  fstp       st(0)
004552e1  fld        dword ptr [esi + 0x42c]
004552e7  pop        esi
004552e8  ret        4

// block 004552eb
004552eb  fstp       st(0)
004552ed  fld        dword ptr [esi + 0x24]
004552f0  pop        esi
004552f1  ret        4

// block 004552f4
004552f4  fstp       st(0)
004552f6  fld        dword ptr [esi + 0x28]
004552f9  pop        esi
004552fa  ret        4

// block 004552fd
004552fd  fstp       st(0)
004552ff  fld        dword ptr [esi + 0x2c]
00455302  pop        esi
00455303  ret        4

// block 00455306
00455306  fstp       st(0)
00455308  pop        esi
00455309  fld        dword ptr [0x4ced1c]
0045530f  ret        4

// block 00455312
00455312  fstp       st(0)
00455314  pop        esi
00455315  fld        dword ptr [0x4ced20]
0045531b  ret        4

// block 0045531e
0045531e  fstp       st(0)
00455320  pop        esi
00455321  fld        dword ptr [0x4ced24]
00455327  ret        4

// block 0045532a
0045532a  fstp       st(0)
0045532c  pop        esi
0045532d  fld        dword ptr [0x4ced40]
00455333  ret        4

// block 00455336
00455336  fstp       st(0)
00455338  pop        esi
00455339  fld        dword ptr [0x4ced44]
0045533f  ret        4

// block 00455342
00455342  fstp       st(0)
00455344  pop        esi
00455345  fld        dword ptr [0x4ced48]
0045534b  ret        4

// block 0045534e
0045534e  test       byte ptr [esi + 0x480], 1
00455355  fstp       st(0)
00455357  mov        esi, 0x4ce560
0045535c  jne        0x455363

// block 0045535e
0045535e  mov        esi, 0x4ce568

// block 00455363
00455363  call       0x464440

// block 00455368
00455368  mov        dword ptr [esp + 8], eax
0045536c  fild       dword ptr [esp + 8]
00455370  test       eax, eax
00455372  jge        0x45537a

// block 00455374
00455374  fadd       dword ptr [0x4a3e10]

// block 0045537a
0045537a  pop        esi
0045537b  ret        4
