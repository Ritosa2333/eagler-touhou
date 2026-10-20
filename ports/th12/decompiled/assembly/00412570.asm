
// block 00412570
00412570  fld        dword ptr [edi]
00412572  push       esi
00412573  fld        qword ptr [0x4a3d48]
00412579  mov        esi, dword ptr [0x4b4514]
0041257f  fmul       st(1), st(0)
00412581  fxch       st(1)
00412583  call       0x4931e0

// block 00412588
00412588  mov        dword ptr [esi + 0x988], eax
0041258e  fmul       dword ptr [edi + 4]
00412591  call       0x4931e0

// block 00412596
00412596  fild       dword ptr [esi + 0x988]
0041259c  fld        qword ptr [0x4a3d40]
004125a2  mov        dword ptr [esi + 0x98c], eax
004125a8  mov        eax, 1
004125ad  fmul       st(1), st(0)
004125af  fxch       st(1)
004125b1  fstp       dword ptr [esi + 0x97c]
004125b7  fimul      dword ptr [esi + 0x98c]
004125bd  fstp       dword ptr [esi + 0x980]
004125c3  mov        dword ptr [esi + 0x8334], eax
004125c9  mov        dword ptr [esi + 0x8418], eax
004125cf  mov        dword ptr [esi + 0x84fc], eax
004125d5  mov        dword ptr [esi + 0x85e0], eax
004125db  mov        dword ptr [esi + 0x86c4], eax
004125e1  mov        dword ptr [esi + 0x87a8], eax
004125e7  mov        dword ptr [esi + 0x888c], eax
004125ed  mov        dword ptr [esi + 0x8970], eax
004125f3  pop        esi
004125f4  ret        
