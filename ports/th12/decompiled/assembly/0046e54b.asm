
// block 0046e54b
0046e54b  mov        edi, edi
0046e54d  push       ebp
0046e54e  mov        ebp, esp
0046e550  push       ecx
0046e551  push       ebx
0046e552  push       edi
0046e553  mov        edi, dword ptr [ebp + 0x10]
0046e556  xor        ebx, ebx
0046e558  mov        dword ptr [ebp - 4], ebx
0046e55b  cmp        edi, ebx
0046e55d  jne        0x46e57b

// block 0046e55f
0046e55f  call       0x46e96f

// block 0046e564
0046e564  push       ebx
0046e565  push       ebx
0046e566  push       ebx
0046e567  push       ebx
0046e568  push       ebx
0046e569  mov        dword ptr [eax], 0x16
0046e56f  call       0x470f2d

// block 0046e574
0046e574  add        esp, 0x14
0046e577  xor        eax, eax
0046e579  jmp        0x46e5f7

// block 0046e57b
0046e57b  push       esi
0046e57c  call       0x475279

// block 0046e581
0046e581  push       0x214
0046e586  push       1
0046e588  call       0x477813

// block 0046e58d
0046e58d  mov        esi, eax
0046e58f  pop        ecx
0046e590  pop        ecx
0046e591  cmp        esi, ebx
0046e593  je         0x46e5df

// block 0046e595
0046e595  call       0x475467

// block 0046e59a
0046e59a  push       dword ptr [eax + 0x6c]
0046e59d  push       esi
0046e59e  call       0x475307

// block 0046e5a3
0046e5a3  mov        eax, dword ptr [ebp + 0x14]
0046e5a6  or         dword ptr [esi + 4], 0xffffffff
0046e5aa  mov        dword ptr [esi + 0x58], eax
0046e5ad  mov        eax, dword ptr [ebp + 0x1c]
0046e5b0  pop        ecx
0046e5b1  pop        ecx
0046e5b2  mov        dword ptr [esi + 0x54], edi
0046e5b5  cmp        eax, ebx
0046e5b7  jne        0x46e5bc

// block 0046e5b9
0046e5b9  lea        eax, [ebp + 0x10]

// block 0046e5bc
0046e5bc  push       eax
0046e5bd  push       dword ptr [ebp + 0x18]
0046e5c0  push       esi
0046e5c1  push       0x46e4c8
0046e5c6  push       dword ptr [ebp + 0xc]
0046e5c9  push       dword ptr [ebp + 8]
0046e5cc  call       dword ptr [0x4980f0]

// block 0046e5d2
0046e5d2  cmp        eax, ebx
0046e5d4  jne        0x46e5f6

// block 0046e5d6
0046e5d6  call       dword ptr [0x4980e4]

// block 0046e5dc
0046e5dc  mov        dword ptr [ebp - 4], eax

// block 0046e5df
0046e5df  push       esi
0046e5e0  call       0x46c941

// block 0046e5e5
0046e5e5  pop        ecx
0046e5e6  cmp        dword ptr [ebp - 4], ebx
0046e5e9  je         0x46e5f4

// block 0046e5eb
0046e5eb  push       dword ptr [ebp - 4]
0046e5ee  call       0x46e995

// block 0046e5f3
0046e5f3  pop        ecx

// block 0046e5f4
0046e5f4  xor        eax, eax

// block 0046e5f6
0046e5f6  pop        esi

// block 0046e5f7
0046e5f7  pop        edi
0046e5f8  pop        ebx
0046e5f9  leave      
0046e5fa  ret        
