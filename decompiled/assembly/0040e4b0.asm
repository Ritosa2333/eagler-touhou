
// block 0040e4b0
0040e4b0  cmp        dword ptr [0x4b0cb8], 0x18
0040e4b7  push       ebp
0040e4b8  push       0xb
0040e4ba  jge        0x40e452

// block 0040e4bc
0040e4bc  mov        ecx, dword ptr [0x4b43dc]
0040e4c2  mov        edx, dword ptr [ecx + 0x4c]
0040e4c5  lea        eax, [esp + 0x24]
0040e4c9  push       eax
0040e4ca  push       edx
0040e4cb  mov        eax, 0x17
0040e4d0  xor        ecx, ecx
0040e4d2  call       0x4615a0

// block 0040e4d7
0040e4d7  mov        eax, dword ptr [esp + 0x1c]
0040e4db  mov        edx, dword ptr [0x4b43dc]
0040e4e1  push       ebp
0040e4e2  push       0xe
0040e4e4  lea        ecx, [esp + 0x24]
0040e4e8  mov        dword ptr [edi + 0x10], eax
0040e4eb  mov        eax, dword ptr [edx + 0x4c]
0040e4ee  push       ecx
0040e4ef  push       eax
0040e4f0  jmp        0x40e5a4
