
// block 0044f540
0044f540  mov        eax, dword ptr [esi + 8]
0044f543  test       eax, eax
0044f545  je         0x44f556

// block 0044f547
0044f547  mov        ecx, dword ptr [eax]
0044f549  mov        edx, dword ptr [ecx + 8]
0044f54c  push       eax
0044f54d  call       edx

// block 0044f54f
0044f54f  mov        dword ptr [esi + 8], 0

// block 0044f556
0044f556  ret        
