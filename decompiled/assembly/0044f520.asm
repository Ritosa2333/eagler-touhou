
// block 0044f520
0044f520  mov        eax, dword ptr [esi + 4]
0044f523  test       eax, eax
0044f525  je         0x44f536

// block 0044f527
0044f527  mov        ecx, dword ptr [eax]
0044f529  mov        edx, dword ptr [ecx + 8]
0044f52c  push       eax
0044f52d  call       edx

// block 0044f52f
0044f52f  mov        dword ptr [esi + 4], 0

// block 0044f536
0044f536  ret        
