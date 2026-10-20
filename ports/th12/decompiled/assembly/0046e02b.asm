
// block 0046e02b
0046e02b  mov        edi, edi
0046e02d  push       ebp
0046e02e  mov        ebp, esp
0046e030  push       ebx
0046e031  mov        ebx, dword ptr [ebp + 8]
0046e034  push       esi
0046e035  push       edi
0046e036  mov        edi, ecx
0046e038  cmp        edi, ebx
0046e03a  je         0x46e07d

// block 0046e03c
0046e03c  mov        eax, dword ptr [ebx + 8]
0046e03f  mov        dword ptr [edi + 8], eax
0046e042  test       eax, eax
0046e044  mov        eax, dword ptr [ebx + 4]
0046e047  je         0x46e07a

// block 0046e049
0046e049  test       eax, eax
0046e04b  je         0x46e074

// block 0046e04d
0046e04d  push       eax
0046e04e  call       0x475860

// block 0046e053
0046e053  mov        esi, eax
0046e055  inc        esi
0046e056  push       esi
0046e057  call       0x46d04a

// block 0046e05c
0046e05c  pop        ecx
0046e05d  pop        ecx
0046e05e  mov        dword ptr [edi + 4], eax
0046e061  test       eax, eax
0046e063  je         0x46e07d

// block 0046e065
0046e065  push       dword ptr [ebx + 4]
0046e068  push       esi
0046e069  push       eax
0046e06a  call       0x47a2b9

// block 0046e06f
0046e06f  add        esp, 0xc
0046e072  jmp        0x46e07d

// block 0046e074
0046e074  and        dword ptr [edi + 4], 0
0046e078  jmp        0x46e07d

// block 0046e07a
0046e07a  mov        dword ptr [edi + 4], eax

// block 0046e07d
0046e07d  mov        eax, edi
0046e07f  pop        edi
0046e080  pop        esi
0046e081  pop        ebx
0046e082  pop        ebp
0046e083  ret        4
