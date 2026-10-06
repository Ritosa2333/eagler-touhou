
// block 00462950
00462950  push       esi
00462951  push       eax
00462952  call       0x4627e0

// block 00462957
00462957  mov        ecx, dword ptr [esp + 8]
0046295b  mov        esi, eax
0046295d  or         dword ptr [esi + 4], 2
00462961  mov        dword ptr [esi + 0x20], ecx
00462964  call       0x462420

// block 00462969
00462969  mov        eax, esi
0046296b  pop        esi
0046296c  ret        4
