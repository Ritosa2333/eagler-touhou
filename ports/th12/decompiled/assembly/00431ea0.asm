
// block 00431ea0
00431ea0  mov        eax, dword ptr [0x4b4510]
00431ea5  cmp        dword ptr [eax + 8], 0
00431ea9  mov        edx, 2
00431eae  je         0x431eb6

// block 00431eb0
00431eb0  mov        ecx, dword ptr [eax + 8]
00431eb3  or         dword ptr [ecx + 4], edx

// block 00431eb6
00431eb6  cmp        dword ptr [eax + 0xc], 0
00431eba  je         0x431ec2

// block 00431ebc
00431ebc  mov        eax, dword ptr [eax + 0xc]
00431ebf  or         dword ptr [eax + 4], edx

// block 00431ec2
00431ec2  ret        
