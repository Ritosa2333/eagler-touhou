
// block 0040f870
0040f870  cmp        dword ptr [eax + 8], 0
0040f874  mov        edx, 0xfffffffd
0040f879  je         0x40f881

// block 0040f87b
0040f87b  mov        ecx, dword ptr [eax + 8]
0040f87e  and        dword ptr [ecx + 4], edx

// block 0040f881
0040f881  cmp        dword ptr [eax + 0xc], 0
0040f885  je         0x40f88d

// block 0040f887
0040f887  mov        eax, dword ptr [eax + 0xc]
0040f88a  and        dword ptr [eax + 4], edx

// block 0040f88d
0040f88d  ret        
