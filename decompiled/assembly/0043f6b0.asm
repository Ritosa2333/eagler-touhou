
// block 0043f6b0
0043f6b0  push       esi
0043f6b1  push       edi
0043f6b2  push       0x5c38
0043f6b7  call       0x46c9ea

// block 0043f6bc
0043f6bc  add        esp, 4
0043f6bf  test       eax, eax
0043f6c1  je         0x43f6ce

// block 0043f6c3
0043f6c3  mov        esi, eax
0043f6c5  call       0x43f250

// block 0043f6ca
0043f6ca  mov        edi, eax
0043f6cc  jmp        0x43f6d0

// block 0043f6ce
0043f6ce  xor        edi, edi

// block 0043f6d0
0043f6d0  lea        esi, [edi + 0x5c1c]
0043f6d6  mov        dword ptr [0x4cf468], 0
0043f6e0  call       0x464c40

// block 0043f6e5
0043f6e5  lea        eax, [esi + 8]
0043f6e8  push       eax
0043f6e9  push       0
0043f6eb  push       edi
0043f6ec  push       0x43f320
0043f6f1  push       0
0043f6f3  push       0
0043f6f5  mov        dword ptr [esi + 0x18], 0x43f320
0043f6fc  mov        dword ptr [esi + 0x10], 1
0043f703  mov        dword ptr [esi + 0xc], 0
0043f70a  call       0x46e54b

// block 0043f70f
0043f70f  add        esp, 0x18
0043f712  mov        dword ptr [esi + 4], eax
0043f715  mov        eax, edi
0043f717  pop        edi
0043f718  pop        esi
0043f719  ret        
