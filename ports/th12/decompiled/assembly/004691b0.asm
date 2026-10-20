
// block 004691b0
004691b0  add        eax, 0x1030
004691b5  je         0x4691d1

// block 004691b7
004691b7  jmp        0x4691c0

// block 004691c0
004691c0  mov        edx, dword ptr [eax]
004691c2  cmp        dword ptr [edx + 0x1010], ecx
004691c8  je         0x4691d3

// block 004691ca
004691ca  mov        eax, dword ptr [eax + 4]
004691cd  test       eax, eax
004691cf  jne        0x4691c0

// block 004691d1
004691d1  xor        eax, eax

// block 004691d3
004691d3  ret        
