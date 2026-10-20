
// block 00412660
00412660  mov        eax, dword ptr [0x4b4514]
00412665  cmp        byte ptr [eax + 0x8984], 0
0041266c  jne        0x412674

// block 0041266e
0041266e  mov        dword ptr [eax + 0x8980], ecx

// block 00412674
00412674  ret        
