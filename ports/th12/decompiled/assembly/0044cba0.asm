
// block 0044cba0
0044cba0  push       esi
0044cba1  mov        esi, eax
0044cba3  lea        ecx, [esi + esi*2]
0044cba6  mov        eax, dword ptr [ecx*4 + 0x4b4540]
0044cbad  lea        eax, [eax + eax*2]
0044cbb0  add        eax, eax
0044cbb2  add        eax, eax
0044cbb4  cmp        dword ptr [eax + 0x4b4544], esi
0044cbba  jne        0x44cbc4

// block 0044cbbc
0044cbbc  mov        dword ptr [eax + 0x4b4544], edx
0044cbc2  jmp        0x44cbca

// block 0044cbc4
0044cbc4  mov        dword ptr [eax + 0x4b4548], edx

// block 0044cbca
0044cbca  mov        esi, dword ptr [ecx*4 + 0x4b4540]
0044cbd1  lea        eax, [edx + edx*2]
0044cbd4  add        eax, eax
0044cbd6  mov        dword ptr [eax + eax + 0x4b4540], esi
0044cbdd  mov        esi, dword ptr [ecx*4 + 0x4b4544]
0044cbe4  add        eax, eax
0044cbe6  mov        dword ptr [eax + 0x4b4544], esi
0044cbec  mov        esi, dword ptr [ecx*4 + 0x4b4548]
0044cbf3  mov        dword ptr [eax + 0x4b4548], esi
0044cbf9  mov        esi, dword ptr [eax + 0x4b4544]
0044cbff  lea        esi, [esi + esi*2]
0044cc02  mov        dword ptr [esi*4 + 0x4b4540], edx
0044cc09  mov        eax, dword ptr [eax + 0x4b4548]
0044cc0f  lea        eax, [eax + eax*2]
0044cc12  mov        dword ptr [eax*4 + 0x4b4540], edx
0044cc19  mov        dword ptr [ecx*4 + 0x4b4540], 0
0044cc24  pop        esi
0044cc25  ret        
