
// block 00466460
00466460  push       esi
00466461  mov        esi, eax
00466463  cmp        dword ptr [esi + 4], 0
00466467  jne        0x466472

// block 00466469
00466469  mov        eax, 0x800401f0
0046646e  pop        esi
0046646f  ret        4

// block 00466472
00466472  push       ebx
00466473  xor        ebx, ebx
00466475  push       edi
00466476  xor        edi, edi
00466478  mov        dword ptr [esi + 0x30], ebx
0046647b  mov        dword ptr [esi + 0x34], ebx
0046647e  cmp        dword ptr [esi + 0x10], ebx
00466481  jbe        0x4664ab

// block 00466483
00466483  mov        eax, dword ptr [esi + 4]
00466486  mov        eax, dword ptr [eax + edi*4]
00466489  mov        ecx, dword ptr [eax]
0046648b  mov        edx, dword ptr [ecx + 0x48]
0046648e  push       eax
0046648f  call       edx

// block 00466491
00466491  or         ebx, eax
00466493  mov        eax, dword ptr [esi + 4]
00466496  mov        eax, dword ptr [eax + edi*4]
00466499  mov        ecx, dword ptr [eax]
0046649b  mov        edx, dword ptr [ecx + 0x34]
0046649e  push       0
004664a0  push       eax
004664a1  call       edx

// block 004664a3
004664a3  inc        edi
004664a4  or         ebx, eax
004664a6  cmp        edi, dword ptr [esi + 0x10]
004664a9  jb         0x466483

// block 004664ab
004664ab  cmp        dword ptr [esp + 0x10], 0
004664b0  mov        dword ptr [esi + 0x1c], 0
004664b7  je         0x4664d9

// block 004664b9
004664b9  mov        esi, dword ptr [esi + 0xc]
004664bc  cmp        dword ptr [esi + 0x78], 1
004664c0  jne        0x4664d9

// block 004664c2
004664c2  mov        eax, dword ptr [esi + 0x8c]
004664c8  push       eax
004664c9  call       dword ptr [0x4980a4]

// block 004664cf
004664cf  mov        dword ptr [esi + 0x8c], 0xffffffff

// block 004664d9
004664d9  pop        edi
004664da  mov        eax, ebx
004664dc  pop        ebx
004664dd  pop        esi
004664de  ret        4
