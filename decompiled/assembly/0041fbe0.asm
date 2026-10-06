
// block 0041fbe0
0041fbe0  push       -1
0041fbe2  push       0x49769b
0041fbe7  mov        eax, dword ptr fs:[0]
0041fbed  push       eax
0041fbee  push       ecx
0041fbef  push       esi
0041fbf0  push       edi
0041fbf1  mov        eax, dword ptr [0x4ad138]
0041fbf6  xor        eax, esp
0041fbf8  push       eax
0041fbf9  lea        eax, [esp + 0x10]
0041fbfd  mov        dword ptr fs:[0], eax
0041fc03  mov        edi, dword ptr [0x4b43e4]
0041fc09  mov        esi, dword ptr [edi + 0x6d30]
0041fc0f  test       esi, esi
0041fc11  je         0x41fc2b

// block 0041fc13
0041fc13  call       0x41cd90

// block 0041fc18
0041fc18  push       esi
0041fc19  call       0x46ca4f

// block 0041fc1e
0041fc1e  add        esp, 4
0041fc21  mov        dword ptr [edi + 0x6d30], 0

// block 0041fc2b
0041fc2b  push       0xac
0041fc30  call       0x46c9ea

// block 0041fc35
0041fc35  add        esp, 4
0041fc38  mov        dword ptr [esp + 0xc], eax
0041fc3c  mov        dword ptr [esp + 0x18], 0
0041fc44  test       eax, eax
0041fc46  je         0x41fc5e

// block 0041fc48
0041fc48  mov        ecx, dword ptr [edi + 0x6d34]
0041fc4e  mov        edx, dword ptr [ecx + ebx*8 + 4]
0041fc52  add        edx, ecx
0041fc54  push       edx
0041fc55  mov        esi, eax
0041fc57  call       0x41f900

// block 0041fc5c
0041fc5c  jmp        0x41fc60

// block 0041fc5e
0041fc5e  xor        eax, eax

// block 0041fc60
0041fc60  mov        dword ptr [edi + 0x6d30], eax
0041fc66  mov        dword ptr [eax], ebx
0041fc68  lea        eax, [ebx + 1]
0041fc6b  cmp        dword ptr [0x4b0cb8], eax
0041fc71  mov        dword ptr [0x4b0cb8], eax
0041fc76  je         0x41fc82

// block 0041fc78
0041fc78  mov        dword ptr [0x4b0cc0], 0

// block 0041fc82
0041fc82  mov        ecx, dword ptr [esp + 0x10]
0041fc86  mov        dword ptr fs:[0], ecx
0041fc8d  pop        ecx
0041fc8e  pop        edi
0041fc8f  pop        esi
0041fc90  add        esp, 0x10
0041fc93  ret        
