
// block 0046edc8
0046edc8  mov        edi, edi
0046edca  push       ebp
0046edcb  mov        ebp, esp
0046edcd  mov        ecx, dword ptr [0x4d6440]
0046edd3  mov        eax, dword ptr [0x4d6444]
0046edd8  imul       ecx, ecx, 0x14
0046eddb  add        ecx, eax
0046eddd  jmp        0x46edf0

// block 0046eddf
0046eddf  mov        edx, dword ptr [ebp + 8]
0046ede2  sub        edx, dword ptr [eax + 0xc]
0046ede5  cmp        edx, 0x100000
0046edeb  jb         0x46edf6

// block 0046eded
0046eded  add        eax, 0x14

// block 0046edf0
0046edf0  cmp        eax, ecx
0046edf2  jb         0x46eddf

// block 0046edf4
0046edf4  xor        eax, eax

// block 0046edf6
0046edf6  pop        ebp
0046edf7  ret        
