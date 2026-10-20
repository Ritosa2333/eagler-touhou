
// block 00428ac0
00428ac0  push       esi
00428ac1  mov        esi, dword ptr [ecx + 0x4b0]
00428ac7  movsx      ecx, word ptr [esi + 0x450]
00428ace  mov        eax, edx
00428ad0  mov        edx, ecx
00428ad2  imul       edx, edx, 0xd0
00428ad8  cmp        dword ptr [edx + 0x4af284], 0
00428adf  jl         0x428af9

// block 00428ae1
00428ae1  movsx      esi, word ptr [esi + 0x452]
00428ae8  imul       ecx, ecx, 0x34
00428aeb  add        eax, esi
00428aed  lea        edx, [eax + esi*2]
00428af0  add        ecx, edx
00428af2  mov        eax, dword ptr [ecx*4 + 0x4af284]

// block 00428af9
00428af9  pop        esi
00428afa  ret        
