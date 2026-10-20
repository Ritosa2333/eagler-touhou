
// block 0045eda0
0045eda0  movzx      eax, word ptr [eax]
0045eda3  test       eax, 0xf000
0045eda8  je         0x45edcc

// block 0045edaa
0045edaa  push       esi
0045edab  movzx      eax, ax
0045edae  mov        esi, eax
0045edb0  shr        esi, 8
0045edb3  and        esi, 0xf
0045edb6  add        dword ptr [ecx], esi
0045edb8  mov        esi, eax
0045edba  shr        esi, 4
0045edbd  and        esi, 0xf
0045edc0  add        dword ptr [ecx + 4], esi
0045edc3  and        eax, 0xf
0045edc6  add        dword ptr [ecx + 8], eax
0045edc9  inc        dword ptr [edx]
0045edcb  pop        esi

// block 0045edcc
0045edcc  ret        
