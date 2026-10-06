
// block 00423560
00423560  cmp        dword ptr [eax + 8], 0
00423564  mov        edx, 2
00423569  je         0x423571

// block 0042356b
0042356b  mov        ecx, dword ptr [eax + 8]
0042356e  or         dword ptr [ecx + 4], edx

// block 00423571
00423571  cmp        dword ptr [eax + 0xc], 0
00423575  je         0x42357d

// block 00423577
00423577  mov        eax, dword ptr [eax + 0xc]
0042357a  or         dword ptr [eax + 4], edx

// block 0042357d
0042357d  ret        
