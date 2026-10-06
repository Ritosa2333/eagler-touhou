
// block 00462930
00462930  push       esi
00462931  push       eax
00462932  call       0x4627e0

// block 00462937
00462937  mov        ecx, dword ptr [esp + 8]
0046293b  mov        esi, eax
0046293d  or         dword ptr [esi + 4], 2
00462941  mov        dword ptr [esi + 0x20], ecx
00462944  call       0x462380

// block 00462949
00462949  mov        eax, esi
0046294b  pop        esi
0046294c  ret        4
