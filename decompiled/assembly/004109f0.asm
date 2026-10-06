
// block 004109f0
004109f0  cmp        dword ptr [eax + 4], 0
004109f4  lea        ecx, [eax + 4]
004109f7  je         0x410a0b

// block 004109f9
004109f9  lea        esp, [esp]

// block 00410a00
00410a00  mov        eax, dword ptr [ecx]
00410a02  cmp        dword ptr [eax + 4], 0
00410a06  lea        ecx, [eax + 4]
00410a09  jne        0x410a00

// block 00410a0b
00410a0b  mov        ecx, dword ptr [eax + 4]
00410a0e  test       ecx, ecx
00410a10  je         0x410a1b

// block 00410a12
00410a12  mov        dword ptr [edx + 4], ecx
00410a15  mov        ecx, dword ptr [eax + 4]
00410a18  mov        dword ptr [ecx + 8], edx

// block 00410a1b
00410a1b  mov        dword ptr [eax + 4], edx
00410a1e  mov        dword ptr [edx + 8], eax
00410a21  ret        
