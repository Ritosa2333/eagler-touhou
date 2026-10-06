
// block 0040d810
0040d810  mov        eax, dword ptr [ecx + 0x84]
0040d816  cmp        eax, 0x1869f
0040d81b  jge        0x40d824

// block 0040d81d
0040d81d  inc        eax
0040d81e  mov        dword ptr [ecx + 0x84], eax

// block 0040d824
0040d824  ret        
