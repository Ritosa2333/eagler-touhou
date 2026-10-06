
// block 0044a0f0
0044a0f0  mov        eax, dword ptr [0x4b4534]
0044a0f5  cmp        dword ptr [eax + 8], 0
0044a0f9  mov        edx, 2
0044a0fe  je         0x44a106

// block 0044a100
0044a100  mov        ecx, dword ptr [eax + 8]
0044a103  or         dword ptr [ecx + 4], edx

// block 0044a106
0044a106  cmp        dword ptr [eax + 0xc], 0
0044a10a  je         0x44a112

// block 0044a10c
0044a10c  mov        ecx, dword ptr [eax + 0xc]
0044a10f  or         dword ptr [ecx + 4], edx

// block 0044a112
0044a112  cmp        dword ptr [eax + 0x24], 0
0044a116  je         0x44a11e

// block 0044a118
0044a118  mov        eax, dword ptr [eax + 0x24]
0044a11b  or         dword ptr [eax + 4], edx

// block 0044a11e
0044a11e  ret        
