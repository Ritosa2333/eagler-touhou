
// block 00423070
00423070  mov        eax, dword ptr [0x4b4518]
00423075  cmp        dword ptr [eax + 0x1d4], 0
0042307c  mov        edx, 0xfffffffd
00423081  je         0x42308c

// block 00423083
00423083  mov        ecx, dword ptr [eax + 0x1d4]
00423089  and        dword ptr [ecx + 4], edx

// block 0042308c
0042308c  cmp        dword ptr [eax + 0xc], 0
00423090  je         0x423098

// block 00423092
00423092  mov        eax, dword ptr [eax + 0xc]
00423095  and        dword ptr [eax + 4], edx

// block 00423098
00423098  ret        
