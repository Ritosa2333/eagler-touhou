
// block 0043d0b0
0043d0b0  push       esi
0043d0b1  mov        esi, dword ptr [0x4b451c]
0043d0b7  test       esi, esi
0043d0b9  je         0x43d0f0

// block 0043d0bb
0043d0bb  mov        eax, dword ptr [esi]
0043d0bd  test       eax, eax
0043d0bf  je         0x43d0d0

// block 0043d0c1
0043d0c1  push       eax
0043d0c2  call       0x46c941

// block 0043d0c7
0043d0c7  add        esp, 4
0043d0ca  mov        dword ptr [esi], 0

// block 0043d0d0
0043d0d0  mov        eax, dword ptr [esi + 4]
0043d0d3  test       eax, eax
0043d0d5  je         0x43d0e7

// block 0043d0d7
0043d0d7  push       eax
0043d0d8  call       0x46c941

// block 0043d0dd
0043d0dd  add        esp, 4
0043d0e0  mov        dword ptr [esi + 4], 0

// block 0043d0e7
0043d0e7  push       esi
0043d0e8  call       0x46ca4f

// block 0043d0ed
0043d0ed  add        esp, 4

// block 0043d0f0
0043d0f0  mov        dword ptr [0x4b451c], 0
0043d0fa  pop        esi
0043d0fb  ret        
