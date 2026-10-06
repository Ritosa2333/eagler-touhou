
// block 0040e52d
0040e52d  cmp        dword ptr [0x4b0cb8], 0x18
0040e534  push       ebp
0040e535  jge        0x40e56f

// block 0040e537
0040e537  mov        ecx, dword ptr [0x4b43dc]
0040e53d  mov        edx, dword ptr [ecx + 0x4c]
0040e540  push       0xe
0040e542  lea        eax, [esp + 0x24]
0040e546  push       eax
0040e547  push       edx
0040e548  mov        eax, 0x17
0040e54d  xor        ecx, ecx
0040e54f  call       0x4615a0

// block 0040e554
0040e554  mov        eax, dword ptr [esp + 0x1c]
0040e558  mov        edx, dword ptr [0x4b43dc]
0040e55e  push       ebp
0040e55f  push       0x12
0040e561  lea        ecx, [esp + 0x24]
0040e565  mov        dword ptr [edi + 0x10], eax
0040e568  mov        eax, dword ptr [edx + 0x4c]
0040e56b  push       ecx
0040e56c  push       eax
0040e56d  jmp        0x40e5a4

// block 0040e56f
0040e56f  mov        edx, dword ptr [0x4b43dc]
0040e575  mov        eax, dword ptr [edx + 0x48]
0040e578  push       0xc
0040e57a  lea        ecx, [esp + 0x24]
0040e57e  push       ecx
0040e57f  push       eax
0040e580  mov        eax, 0x17
0040e585  xor        ecx, ecx
0040e587  call       0x4615a0

// block 0040e58c
0040e58c  push       ebp
0040e58d  push       0x14
