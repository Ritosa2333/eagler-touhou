
// block 004627e0
004627e0  push       0x24
004627e2  call       0x46c9ea

// block 004627e7
004627e7  xor        ecx, ecx
004627e9  add        esp, 4
004627ec  cmp        eax, ecx
004627ee  je         0x46281c

// block 004627f0
004627f0  and        dword ptr [eax + 4], 0xfffffffe
004627f4  mov        edx, dword ptr [esp + 4]
004627f8  mov        dword ptr [eax + 8], ecx
004627fb  mov        dword ptr [eax + 0xc], ecx
004627fe  mov        dword ptr [eax + 0x10], ecx
00462801  mov        dword ptr [eax], ecx
00462803  mov        dword ptr [eax + 0x14], eax
00462806  mov        dword ptr [eax + 0x18], ecx
00462809  mov        dword ptr [eax + 0x1c], ecx
0046280c  or         dword ptr [eax + 4], 1
00462810  mov        dword ptr [eax + 8], edx
00462813  mov        dword ptr [eax + 0xc], ecx
00462816  mov        dword ptr [eax + 0x10], ecx
00462819  ret        4

// block 0046281c
0046281c  mov        edx, dword ptr [esp + 4]
00462820  xor        eax, eax
00462822  or         dword ptr [eax + 4], 1
00462826  mov        dword ptr [eax + 8], edx
00462829  mov        dword ptr [eax + 0xc], ecx
0046282c  mov        dword ptr [eax + 0x10], ecx
0046282f  ret        4
