
// block 0040e730
0040e730  mov        eax, 0x66666667
0040e735  imul       dword ptr [esp + 4]
0040e739  sar        edx, 2
0040e73c  mov        eax, edx
0040e73e  shr        eax, 0x1f
0040e741  add        eax, edx
0040e743  add        dword ptr [ecx + 4], eax
0040e746  cmp        dword ptr [ecx + 4], 0x3b9aca00
0040e74d  jl         0x40e756

// block 0040e74f
0040e74f  mov        dword ptr [ecx + 4], 0x3b9ac9ff

// block 0040e756
0040e756  ret        4
