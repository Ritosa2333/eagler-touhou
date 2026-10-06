
// block 0046e28d
0046e28d  mov        edi, edi
0046e28f  push       ebp
0046e290  mov        ebp, esp
0046e292  mov        eax, dword ptr [ebp + 0x14]
0046e295  push       esi
0046e296  push       edi
0046e297  xor        edi, edi
0046e299  cmp        eax, edi
0046e29b  je         0x46e2e4

// block 0046e29d
0046e29d  cmp        dword ptr [ebp + 8], edi
0046e2a0  jne        0x46e2bd

// block 0046e2a2
0046e2a2  call       0x46e96f

// block 0046e2a7
0046e2a7  push       0x16
0046e2a9  pop        esi
0046e2aa  mov        dword ptr [eax], esi

// block 0046e2ac
0046e2ac  push       edi
0046e2ad  push       edi
0046e2ae  push       edi
0046e2af  push       edi
0046e2b0  push       edi
0046e2b1  call       0x470f2d

// block 0046e2b6
0046e2b6  add        esp, 0x14
0046e2b9  mov        eax, esi
0046e2bb  jmp        0x46e2e6

// block 0046e2bd
0046e2bd  cmp        dword ptr [ebp + 0x10], edi
0046e2c0  je         0x46e2a2

// block 0046e2c2
0046e2c2  cmp        dword ptr [ebp + 0xc], eax
0046e2c5  jae        0x46e2d5

// block 0046e2c7
0046e2c7  call       0x46e96f

// block 0046e2cc
0046e2cc  push       0x22
0046e2ce  pop        ecx
0046e2cf  mov        dword ptr [eax], ecx
0046e2d1  mov        esi, ecx
0046e2d3  jmp        0x46e2ac

// block 0046e2d5
0046e2d5  push       eax
0046e2d6  push       dword ptr [ebp + 0x10]
0046e2d9  push       dword ptr [ebp + 8]
0046e2dc  call       0x47a6a0

// block 0046e2e1
0046e2e1  add        esp, 0xc

// block 0046e2e4
0046e2e4  xor        eax, eax

// block 0046e2e6
0046e2e6  pop        edi
0046e2e7  pop        esi
0046e2e8  pop        ebp
0046e2e9  ret        
