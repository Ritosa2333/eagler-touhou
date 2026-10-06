
// block 0044f0a0
0044f0a0  mov        ecx, dword ptr [esp + 4]
0044f0a4  sub        esp, 0xc
0044f0a7  test       ecx, ecx
0044f0a9  ja         0x44f0bc

// block 0044f0ab
0044f0ab  xor        ecx, ecx

// block 0044f0ad
0044f0ad  push       ecx
0044f0ae  call       0x46c9ea

// block 0044f0b3
0044f0b3  add        esp, 4
0044f0b6  add        esp, 0xc
0044f0b9  ret        4

// block 0044f0bc
0044f0bc  or         eax, 0xffffffff
0044f0bf  xor        edx, edx
0044f0c1  div        ecx
0044f0c3  cmp        eax, 1
0044f0c6  jae        0x44f0ad

// block 0044f0c8
0044f0c8  lea        eax, [esp + 0x10]
0044f0cc  push       eax
0044f0cd  lea        ecx, [esp + 4]
0044f0d1  mov        dword ptr [esp + 0x14], 0
0044f0d9  call       0x46df5e

// block 0044f0de
0044f0de  push       0x4ab5d0
0044f0e3  lea        ecx, [esp + 4]
0044f0e7  push       ecx
0044f0e8  mov        dword ptr [esp + 8], 0x49cd00
0044f0f0  call       0x46ff8f

// block 0044f0f5
0044f0f5  int3       
