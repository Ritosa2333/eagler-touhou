
// block 00451c70
00451c70  xor        eax, eax

// block 00451c72
00451c72  cmp        dword ptr [eax*4 + 0x4ce90c], 0
00451c7a  je         0x451c87

// block 00451c7c
00451c7c  inc        eax
00451c7d  cmp        eax, 1
00451c80  jl         0x451c72

// block 00451c82
00451c82  xor        eax, eax
00451c84  ret        8

// block 00451c87
00451c87  mov        ecx, dword ptr [0x4ce8f4]
00451c8d  mov        edx, dword ptr [ecx]
00451c8f  push       0
00451c91  lea        eax, [eax*4 + 0x4ce90c]
