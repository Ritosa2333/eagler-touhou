
// block 00422f20
00422f20  mov        eax, dword ptr [0x4b0ca0]
00422f25  sub        eax, 1
00422f28  mov        dword ptr [0x4b0ca0], eax
00422f2d  jns        0x422f33

// block 00422f2f
00422f2f  xor        eax, eax
00422f31  jmp        0x422f3d

// block 00422f33
00422f33  cmp        eax, 8
00422f36  jle        0x422f42

// block 00422f38
00422f38  mov        eax, 8

// block 00422f3d
00422f3d  mov        dword ptr [0x4b0ca0], eax

// block 00422f42
00422f42  mov        ecx, dword ptr [0x4b43e4]
00422f48  test       ecx, ecx
00422f4a  je         0x422f5a

// block 00422f4c
00422f4c  mov        edx, dword ptr [0x4b0ca4]
00422f52  push       edx
00422f53  push       eax
00422f54  push       ecx
00422f55  call       0x41cf40

// block 00422f5a
00422f5a  ret        
