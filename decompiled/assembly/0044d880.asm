
// block 0044d880
0044d880  movzx      eax, word ptr [eax]
0044d883  test       eax, 0xf000
0044d888  je         0x44d8ac

// block 0044d88a
0044d88a  push       esi
0044d88b  movzx      eax, ax
0044d88e  mov        esi, eax
0044d890  shr        esi, 8
0044d893  and        esi, 0xf
0044d896  add        dword ptr [ecx], esi
0044d898  mov        esi, eax
0044d89a  shr        esi, 4
0044d89d  and        esi, 0xf
0044d8a0  add        dword ptr [ecx + 4], esi
0044d8a3  and        eax, 0xf
0044d8a6  add        dword ptr [ecx + 8], eax
0044d8a9  inc        dword ptr [edx]
0044d8ab  pop        esi

// block 0044d8ac
0044d8ac  ret        
