
// block 0042f3b0
0042f3b0  cmp        dword ptr [0x4cea94], 0
0042f3b7  je         0x42f40a

// block 0042f3b9
0042f3b9  mov        eax, dword ptr [0x4ce8f0]
0042f3be  mov        ecx, dword ptr [eax]
0042f3c0  mov        edx, dword ptr [ecx + 0xe4]
0042f3c6  push       esi
0042f3c7  push       edi
0042f3c8  push       1
0042f3ca  push       0xab
0042f3cf  push       eax
0042f3d0  call       edx

// block 0042f3d2
0042f3d2  mov        eax, dword ptr [0x4ce8cc]
0042f3d7  push       eax
0042f3d8  mov        eax, dword ptr [0x4ceaa4]
0042f3dd  call       0x45c900

// block 0042f3e2
0042f3e2  mov        edi, dword ptr [0x4ce8cc]
0042f3e8  mov        eax, 0x1c
0042f3ed  call       0x4611d0

// block 0042f3f2
0042f3f2  mov        esi, dword ptr [0x4ce8cc]
0042f3f8  call       0x45a3c0

// block 0042f3fd
0042f3fd  mov        eax, dword ptr [0x4b450c]
0042f402  pop        edi
0042f403  pop        esi
0042f404  test       eax, eax
0042f406  je         0x42f40a

// block 0042f408
0042f408  call       eax

// block 0042f40a
0042f40a  mov        eax, 1
0042f40f  ret        
