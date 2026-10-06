
// block 0043ca40
0043ca40  mov        edx, 1
0043ca45  shl        edx, cl
0043ca47  test       dword ptr [eax + 0x118], edx
0043ca4d  je         0x43ca57

// block 0043ca4f
0043ca4f  mov        eax, dword ptr [eax + ecx*4 + 0x94]
0043ca56  ret        

// block 0043ca57
0043ca57  mov        eax, 0x3e7
0043ca5c  ret        
