
// block 0043e6e0
0043e6e0  mov        eax, dword ptr [esi + 0x8c]
0043e6e6  mov        dword ptr [esi], 0x49fc28
0043e6ec  test       eax, eax
0043e6ee  je         0x43e703

// block 0043e6f0
0043e6f0  push       eax
0043e6f1  call       0x46c941

// block 0043e6f6
0043e6f6  add        esp, 4
0043e6f9  mov        dword ptr [esi + 0x8c], 0

// block 0043e703
0043e703  push       esi
0043e704  call       0x46ca4f

// block 0043e709
0043e709  add        esp, 4
0043e70c  mov        eax, esi
0043e70e  ret        
