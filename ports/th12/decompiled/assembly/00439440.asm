
// block 00439440
00439440  mov        eax, dword ptr [0x4b0c48]
00439445  sub        eax, dword ptr [esp + 4]
00439449  mov        ecx, dword ptr [0x4b0cd4]
0043944f  cmp        eax, ecx
00439451  mov        dword ptr [0x4b0c48], eax
00439456  jge        0x43945e

// block 00439458
00439458  mov        dword ptr [0x4b0c48], ecx

// block 0043945e
0043945e  xor        eax, eax
00439460  ret        4
