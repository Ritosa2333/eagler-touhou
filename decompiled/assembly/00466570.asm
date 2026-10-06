
// block 00466570
00466570  cmp        dword ptr [esi + 4], 0
00466574  jne        0x46657c

// block 00466576
00466576  mov        eax, 0x800401f0
0046657b  ret        

// block 0046657c
0046657c  cmp        dword ptr [esi + 0x34], 0
00466580  je         0x466576

// block 00466582
00466582  mov        ecx, dword ptr [esi + 0xc]
00466585  mov        dword ptr [esi + 0x34], 0
0046658c  mov        eax, dword ptr [ecx + 0x98]
