
// block 0044ea80
0044ea80  push       esi
0044ea81  mov        esi, ecx
0044ea83  cmp        dword ptr [esi + 0x18], 0x10
0044ea87  jb         0x44ea95

// block 0044ea89
0044ea89  mov        eax, dword ptr [esi + 4]
0044ea8c  push       eax
0044ea8d  call       0x46ca4f

// block 0044ea92
0044ea92  add        esp, 4

// block 0044ea95
0044ea95  xor        eax, eax
0044ea97  mov        dword ptr [esi + 0x18], 0xf
0044ea9e  mov        dword ptr [esi + 0x14], eax
0044eaa1  mov        byte ptr [esi + 4], al
0044eaa4  pop        esi
0044eaa5  ret        

// block 00491687
00491687  push       0x44
00491689  mov        eax, 0x496d2b
0049168e  call       0x491e59

// block 00491693
00491693  push       0x49f3b0
00491698  lea        ecx, [ebp - 0x28]
0049169b  call       0x4916bf

// block 004916a0
004916a0  and        dword ptr [ebp - 4], 0
004916a4  lea        eax, [ebp - 0x28]
004916a7  push       eax
004916a8  lea        ecx, [ebp - 0x50]
004916ab  call       0x4915e9

// block 004916b0
004916b0  push       0x4ab250
004916b5  lea        eax, [ebp - 0x50]
004916b8  push       eax
004916b9  call       0x46ff8f

// block 004916be
004916be  int3       

// block 00496d23
00496d23  lea        ecx, [ebp - 0x28]
00496d26  jmp        0x44ea80

// block 00496d2b
00496d2b  mov        edx, dword ptr [esp + 8]
00496d2f  lea        eax, [edx + 0xc]
00496d32  mov        ecx, dword ptr [edx - 0x54]
00496d35  xor        ecx, eax
00496d37  call       0x46c932

// block 00496d3c
00496d3c  mov        eax, 0x4ab268
00496d41  jmp        0x491aab
