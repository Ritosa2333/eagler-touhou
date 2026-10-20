
// block 0040e810
0040e810  cmp        dword ptr [eax + 8], 0
0040e814  mov        edx, 2
0040e819  je         0x40e821

// block 0040e81b
0040e81b  mov        ecx, dword ptr [eax + 8]
0040e81e  or         dword ptr [ecx + 4], edx

// block 0040e821
0040e821  cmp        dword ptr [eax + 0xc], 0
0040e825  je         0x40e82d

// block 0040e827
0040e827  mov        eax, dword ptr [eax + 0xc]
0040e82a  or         dword ptr [eax + 4], edx

// block 0040e82d
0040e82d  ret        
