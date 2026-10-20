
// block 0045edd0
0045edd0  cmp        byte ptr [eax + 1], 0
0045edd4  je         0x45edf8

// block 0045edd6
0045edd6  movzx      eax, word ptr [eax]

// block 0045edd9
0045edd9  push       esi
0045edda  mov        esi, eax
0045eddc  shr        esi, 5
0045eddf  and        esi, 7
0045ede2  add        dword ptr [ecx], esi
0045ede4  mov        esi, eax
0045ede6  shr        esi, 2
0045ede9  and        esi, 7
0045edec  add        dword ptr [ecx + 4], esi
0045edef  and        eax, 3
0045edf2  add        dword ptr [ecx + 8], eax
0045edf5  inc        dword ptr [edx]
0045edf7  pop        esi

// block 0045edf8
0045edf8  ret        
