
// block 00491756
00491756  mov        edi, edi
00491758  push       ebp
00491759  mov        ebp, esp
0049175b  push       esi
0049175c  mov        esi, ecx
0049175e  push       0
00491760  mov        dword ptr [esi + 0x18], 0xf
00491767  call       0x44edf0

// block 0049176c
0049176c  push       -1
0049176e  push       0
00491770  push       dword ptr [ebp + 8]
00491773  mov        ecx, esi
00491775  call       0x44eaf0

// block 0049177a
0049177a  mov        eax, esi
0049177c  pop        esi
0049177d  pop        ebp
0049177e  ret        4
