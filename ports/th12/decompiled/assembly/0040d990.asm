
// block 0040d990
0040d990  mov        eax, dword ptr [0x4b43cc]
0040d995  cmp        dword ptr [eax + 8], 0
0040d999  mov        edx, 2
0040d99e  je         0x40d9a6

// block 0040d9a0
0040d9a0  mov        ecx, dword ptr [eax + 8]
0040d9a3  or         dword ptr [ecx + 4], edx

// block 0040d9a6
0040d9a6  cmp        dword ptr [eax + 0xc], 0
0040d9aa  je         0x40d9b2

// block 0040d9ac
0040d9ac  mov        eax, dword ptr [eax + 0xc]
0040d9af  or         dword ptr [eax + 4], edx

// block 0040d9b2
0040d9b2  ret        
