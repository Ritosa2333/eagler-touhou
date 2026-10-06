
// block 00462970
00462970  push       esi
00462971  push       eax
00462972  call       0x4627e0

// block 00462977
00462977  mov        ecx, dword ptr [esp + 8]
0046297b  mov        esi, eax
0046297d  and        dword ptr [esi + 4], 0xfffffffd
00462981  mov        dword ptr [esi + 0x20], ecx
00462984  call       0x462380

// block 00462989
00462989  mov        eax, esi
0046298b  pop        esi
0046298c  ret        4
