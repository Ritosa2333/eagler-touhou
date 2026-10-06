
// block 004130b0
004130b0  mov        eax, dword ptr [esi + 0x8c]
004130b6  mov        dword ptr [esi], 0x49fc28
004130bc  test       eax, eax
004130be  je         0x4130d3

// block 004130c0
004130c0  push       eax
004130c1  call       0x46c941

// block 004130c6
004130c6  add        esp, 4
004130c9  mov        dword ptr [esi + 0x8c], 0

// block 004130d3
004130d3  ret        
