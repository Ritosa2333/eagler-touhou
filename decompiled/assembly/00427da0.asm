
// block 00427da0
00427da0  mov        eax, dword ptr [ecx + 4]
00427da3  mov        edx, dword ptr [ecx + 8]
00427da6  mov        dword ptr [eax + 8], edx
00427da9  mov        eax, dword ptr [ecx + 8]
00427dac  test       eax, eax
00427dae  je         0x427db6

// block 00427db0
00427db0  mov        ecx, dword ptr [ecx + 4]
00427db3  mov        dword ptr [eax + 4], ecx

// block 00427db6
00427db6  xor        eax, eax
00427db8  ret        
