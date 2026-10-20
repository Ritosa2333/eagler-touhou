
// block 00427d20
00427d20  mov        ecx, 0xfffffc00
00427d25  cmp        dword ptr [eax + 0x8c], ecx
00427d2b  jge        0x427d33

// block 00427d2d
00427d2d  mov        dword ptr [eax + 0x8c], ecx

// block 00427d33
00427d33  ret        
