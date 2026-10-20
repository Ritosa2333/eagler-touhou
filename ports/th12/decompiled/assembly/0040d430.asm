
// block 0040d430
0040d430  push       esi
0040d431  mov        esi, dword ptr [ecx + 0x4b0]
0040d437  movsx      ecx, word ptr [esi + 0x9f4]
0040d43e  mov        eax, edx
0040d440  mov        edx, ecx
0040d442  imul       edx, edx, 0xd0
0040d448  cmp        dword ptr [edx + 0x4af284], 0
0040d44f  jl         0x40d469

// block 0040d451
0040d451  movsx      esi, word ptr [esi + 0x9f6]
0040d458  imul       ecx, ecx, 0x34
0040d45b  add        eax, esi
0040d45d  lea        edx, [eax + esi*2]
0040d460  add        ecx, edx
0040d462  mov        eax, dword ptr [ecx*4 + 0x4af284]

// block 0040d469
0040d469  pop        esi
0040d46a  ret        
