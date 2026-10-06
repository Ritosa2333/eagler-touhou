
// block 00463240
00463240  imul       ecx, ecx, 0x148
00463246  add        ecx, 0x4d48b8
0046324c  mov        eax, dword ptr [ecx]
0046324e  mov        dword ptr [ecx + 4], eax
00463251  mov        dword ptr [ecx], esi
00463253  call       0x465440

// block 00463258
00463258  mov        eax, esi
0046325a  ret        
