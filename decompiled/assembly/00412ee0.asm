
// block 00412ee0
00412ee0  mov        eax, dword ptr [0x4b43dc]
00412ee5  cmp        dword ptr [eax + 8], 0
00412ee9  mov        edx, 2
00412eee  je         0x412ef6

// block 00412ef0
00412ef0  mov        ecx, dword ptr [eax + 8]
00412ef3  or         dword ptr [ecx + 4], edx

// block 00412ef6
00412ef6  cmp        dword ptr [eax + 0xc], 0
00412efa  je         0x412f02

// block 00412efc
00412efc  mov        eax, dword ptr [eax + 0xc]
00412eff  or         dword ptr [eax + 4], edx

// block 00412f02
00412f02  ret        
