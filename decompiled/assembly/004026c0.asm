
// block 004026c0
004026c0  mov        eax, dword ptr [esi + 0x478]
004026c6  test       eax, eax
004026c8  je         0x4026d3

// block 004026ca
004026ca  push       eax
004026cb  call       0x46c941

// block 004026d0
004026d0  add        esp, 4

// block 004026d3
004026d3  mov        dword ptr [esi + 0x478], 0
004026dd  ret        
