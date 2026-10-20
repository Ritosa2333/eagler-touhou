
// block 00449eb0
00449eb0  mov        ecx, dword ptr [0x4b0ce0]
00449eb6  shl        eax, 4
00449eb9  xor        eax, ecx
00449ebb  and        eax, 0x10
00449ebe  xor        ecx, eax
00449ec0  mov        dword ptr [0x4b0ce0], ecx
00449ec6  ret        
