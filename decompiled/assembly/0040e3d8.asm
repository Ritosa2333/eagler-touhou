
// block 0040e3d8
0040e3d8  mov        eax, dword ptr [0x4b43dc]
0040e3dd  mov        ecx, dword ptr [eax + 0x48]
0040e3e0  push       ebp
0040e3e1  push       0xe
0040e3e3  lea        edx, [esp + 0x24]
0040e3e7  push       edx
0040e3e8  push       ecx
0040e3e9  mov        eax, 0x17
0040e3ee  xor        ecx, ecx
0040e3f0  call       0x4615a0

// block 0040e3f5
0040e3f5  mov        edx, dword ptr [esp + 0x1c]
0040e3f9  mov        ecx, dword ptr [0x4b43dc]
0040e3ff  push       ebp
0040e400  push       0x15
0040e402  lea        eax, [esp + 0x24]
0040e406  mov        dword ptr [edi + 0x10], edx
0040e409  mov        edx, dword ptr [ecx + 0x48]
0040e40c  push       eax
0040e40d  push       edx
0040e40e  jmp        0x40e5a4

// block 0040e5a4
0040e5a4  mov        eax, 0x17
0040e5a9  xor        ecx, ecx
0040e5ab  call       0x4615a0
