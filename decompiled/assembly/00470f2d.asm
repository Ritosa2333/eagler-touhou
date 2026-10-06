
// block 00470f2d
00470f2d  mov        edi, edi
00470f2f  push       ebp
00470f30  mov        ebp, esp
00470f32  push       dword ptr [0x4b3d60]
00470f38  call       0x4751de

// block 00470f3d
00470f3d  pop        ecx
00470f3e  test       eax, eax
00470f40  je         0x470f45

// block 00470f42
00470f42  pop        ebp
00470f43  jmp        eax

// block 00470f45
00470f45  push       2
00470f47  call       0x47b3ff

// block 00470f4c
00470f4c  pop        ecx
00470f4d  pop        ebp
00470f4e  jmp        0x470dc6
