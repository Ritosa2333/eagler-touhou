
// block 004dc973
004dc973  mov        eax, dword ptr [esp + 8]
004dc977  mov        edx, ecx
004dc979  mov        ecx, dword ptr [esp + 4]
004dc97d  push       edi
004dc97e  mov        dword ptr [edx], eax
004dc980  lea        eax, [edx + 4]
004dc983  mov        dword ptr [eax], ecx
004dc985  mov        dword ptr [eax + 4], 0x20
004dc98c  mov        dword ptr [edx + 0x10], eax
004dc98f  mov        dword ptr [edx + 0xa0], eax
004dc995  mov        dword ptr [edx + 0x130], eax
004dc99b  mov        dword ptr [edx + 0x1c0], eax
004dc9a1  xor        eax, eax
004dc9a3  mov        ecx, 0xbd
004dc9a8  mov        dword ptr [edx + 0x250], eax
004dc9ae  mov        dword ptr [edx + 0x254], eax
004dc9b4  mov        dword ptr [edx + 0x258], eax
004dc9ba  mov        edi, dword ptr [edx + 0x260]
004dc9c0  mov        dword ptr [edx + 0x25c], eax

// block 004dc9c6
004dc9c6  rep stosd  dword ptr es:[edi], eax

// block 004dc9c8
004dc9c8  mov        ecx, edx
004dc9ca  stosb      byte ptr es:[edi], al
004dc9cb  call       0x4dc9d4

// block 004dc9d0
004dc9d0  pop        edi
004dc9d1  ret        8
