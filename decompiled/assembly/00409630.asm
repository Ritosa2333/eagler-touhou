
// block 00409630
00409630  push       esi
00409631  mov        esi, dword ptr [0x4ce8cc]
00409637  push       edi
00409638  mov        edi, dword ptr [0x4b43c8]
0040963e  mov        edx, dword ptr [edi + 0x4debdc]
00409644  call       0x461be0

// block 00409649
00409649  push       0x4deb78
0040964e  lea        esi, [edi + 0x64]
00409651  push       0
00409653  push       esi
00409654  call       0x477420

// block 00409659
00409659  add        esp, 0xc
0040965c  mov        eax, 5
00409661  mov        dword ptr [edi + 0x10], esi
00409664  mov        word ptr [edi + 0x4de716], ax
0040966b  pop        edi
0040966c  pop        esi
0040966d  ret        
