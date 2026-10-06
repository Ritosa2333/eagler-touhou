
// block 0046e3f8
0046e3f8  mov        edi, edi
0046e3fa  push       ebp
0046e3fb  mov        ebp, esp
0046e3fd  push       esi
0046e3fe  xor        esi, esi
0046e400  cmp        dword ptr [0x4b40dc], esi
0046e406  jne        0x46e438

// block 0046e408
0046e408  cmp        dword ptr [ebp + 8], esi
0046e40b  jne        0x46e42c

// block 0046e40d
0046e40d  call       0x46e96f

// block 0046e412
0046e412  push       esi
0046e413  push       esi
0046e414  push       esi
0046e415  push       esi
0046e416  push       esi
0046e417  mov        dword ptr [eax], 0x16
0046e41d  call       0x470f2d

// block 0046e422
0046e422  add        esp, 0x14
0046e425  mov        eax, 0x7fffffff
0046e42a  jmp        0x46e447

// block 0046e42c
0046e42c  cmp        dword ptr [ebp + 0xc], esi
0046e42f  je         0x46e40d

// block 0046e431
0046e431  pop        esi
0046e432  pop        ebp
0046e433  jmp        0x46e2ea

// block 0046e438
0046e438  push       esi
0046e439  push       dword ptr [ebp + 0xc]
0046e43c  push       dword ptr [ebp + 8]
0046e43f  call       0x46e323

// block 0046e444
0046e444  add        esp, 0xc

// block 0046e447
0046e447  pop        esi
0046e448  pop        ebp
0046e449  ret        
