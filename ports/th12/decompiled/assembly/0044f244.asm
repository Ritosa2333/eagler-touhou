
// block 0044f244
0044f244  push       ebx
0044f245  mov        eax, ebx
0044f247  call       0x462740

// block 0044f24c
0044f24c  lea        esi, [ebx + 0x24]
0044f24f  push       ebx
0044f250  mov        eax, esi
0044f252  call       0x462740

// block 0044f257
0044f257  xor        eax, eax
0044f259  mov        dword ptr [esi + 8], eax
0044f25c  mov        dword ptr [esi + 0xc], eax
0044f25f  mov        dword ptr [esi + 0x10], eax
0044f262  mov        dword ptr [ebx + 8], eax
0044f265  mov        dword ptr [ebx + 0xc], eax
0044f268  mov        dword ptr [ebx + 0x10], eax
0044f26b  mov        ecx, dword ptr [esp + 0xc]
0044f26f  mov        dword ptr fs:[0], ecx
0044f276  pop        ecx
0044f277  pop        esi
0044f278  pop        ebx
0044f279  add        esp, 0xc
0044f27c  ret        4
