
// block 0044cd60
0044cd60  push       esi
0044cd61  mov        esi, ecx
0044cd63  mov        eax, dword ptr [esi + 0xc]
0044cd66  push       edi
0044cd67  xor        edi, edi
0044cd69  cmp        eax, edi
0044cd6b  je         0x44cd79

// block 0044cd6d
0044cd6d  push       eax
0044cd6e  call       0x46c941

// block 0044cd73
0044cd73  add        esp, 4
0044cd76  mov        dword ptr [esi + 0xc], edi

// block 0044cd79
0044cd79  mov        dword ptr [esi + 0xc], edi
0044cd7c  mov        dword ptr [esi + 4], edi
0044cd7f  mov        dword ptr [esi + 8], edi
0044cd82  pop        edi
0044cd83  pop        esi
0044cd84  ret        
