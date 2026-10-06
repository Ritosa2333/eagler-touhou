
// block 004273c0
004273c0  mov        edx, dword ptr [0x4b44e8]
004273c6  mov        eax, ecx
004273c8  test       edx, edx
004273ca  je         0x4273d8

// block 004273cc
004273cc  test       byte ptr [edx + 0x60], 4
004273d0  je         0x4273d8

// block 004273d2
004273d2  mov        eax, 1
004273d7  ret        

// block 004273d8
004273d8  jmp        0x427230
