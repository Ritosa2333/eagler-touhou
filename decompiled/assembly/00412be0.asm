
// block 00412be0
00412be0  mov        ecx, dword ptr [0x4b43dc]
00412be6  lea        eax, [edx + 0x12c0]
00412bec  push       esi
00412bed  push       edi
00412bee  cmp        dword ptr [ecx + 0x68], eax
00412bf1  jne        0x412bfc

// block 00412bf3
00412bf3  mov        esi, dword ptr [edx + 0x12c4]
00412bf9  mov        dword ptr [ecx + 0x68], esi

// block 00412bfc
00412bfc  cmp        dword ptr [ecx + 0x6c], eax
00412bff  jne        0x412c0a

// block 00412c01
00412c01  mov        edx, dword ptr [edx + 0x12c8]
00412c07  mov        dword ptr [ecx + 0x6c], edx

// block 00412c0a
00412c0a  mov        edx, dword ptr [eax + 4]
00412c0d  xor        esi, esi
00412c0f  cmp        edx, esi
00412c11  je         0x412c19

// block 00412c13
00412c13  mov        edi, dword ptr [eax + 8]
00412c16  mov        dword ptr [edx + 8], edi

// block 00412c19
00412c19  mov        edx, dword ptr [eax + 8]
00412c1c  cmp        edx, esi
00412c1e  je         0x412c26

// block 00412c20
00412c20  mov        edi, dword ptr [eax + 4]
00412c23  mov        dword ptr [edx + 4], edi

// block 00412c26
00412c26  mov        dword ptr [eax + 4], esi
00412c29  mov        dword ptr [eax + 8], esi
00412c2c  dec        dword ptr [ecx + 0x70]
00412c2f  pop        edi
00412c30  pop        esi
00412c31  ret        
