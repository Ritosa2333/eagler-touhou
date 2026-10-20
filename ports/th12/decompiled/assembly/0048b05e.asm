
// block 0048b05e
0048b05e  mov        edi, edi
0048b060  push       ebp
0048b061  mov        ebp, esp
0048b063  push       ecx
0048b064  push       esi
0048b065  push       edi
0048b066  xor        edi, edi
0048b068  cmp        ebx, edi
0048b06a  jne        0x48b087

// block 0048b06c
0048b06c  call       0x46e96f

// block 0048b071
0048b071  push       0x16
0048b073  pop        esi
0048b074  push       edi
0048b075  push       edi
0048b076  push       edi
0048b077  push       edi
0048b078  push       edi
0048b079  mov        dword ptr [eax], esi
0048b07b  call       0x470f2d

// block 0048b080
0048b080  add        esp, 0x14
0048b083  mov        eax, esi
0048b085  jmp        0x48b0fe

// block 0048b087
0048b087  mov        eax, dword ptr [ebp + 8]
0048b08a  mov        dword ptr [ebx], edi
0048b08c  cmp        eax, edi
0048b08e  je         0x48b092

// block 0048b090
0048b090  mov        dword ptr [eax], edi

// block 0048b092
0048b092  cmp        dword ptr [ebp + 0xc], edi
0048b095  je         0x48b06c

// block 0048b097
0048b097  push       dword ptr [ebp + 0xc]
0048b09a  call       0x48af42

// block 0048b09f
0048b09f  pop        ecx
0048b0a0  mov        dword ptr [ebp - 4], eax
0048b0a3  cmp        eax, edi
0048b0a5  je         0x48b0fc

// block 0048b0a7
0048b0a7  push       eax
0048b0a8  call       0x475860

// block 0048b0ad
0048b0ad  mov        esi, eax
0048b0af  inc        esi
0048b0b0  push       1
0048b0b2  push       esi
0048b0b3  call       0x48e3da

// block 0048b0b8
0048b0b8  add        esp, 0xc
0048b0bb  mov        dword ptr [ebx], eax
0048b0bd  cmp        eax, edi
0048b0bf  jne        0x48b0d5

// block 0048b0c1
0048b0c1  call       0x46e96f

// block 0048b0c6
0048b0c6  mov        dword ptr [eax], 0xc
0048b0cc  call       0x46e96f

// block 0048b0d1
0048b0d1  mov        eax, dword ptr [eax]
0048b0d3  jmp        0x48b0fe

// block 0048b0d5
0048b0d5  push       dword ptr [ebp - 4]
0048b0d8  push       esi
0048b0d9  push       eax
0048b0da  call       0x47a2b9

// block 0048b0df
0048b0df  add        esp, 0xc
0048b0e2  test       eax, eax
0048b0e4  je         0x48b0f3

// block 0048b0e6
0048b0e6  push       edi
0048b0e7  push       edi
0048b0e8  push       edi
0048b0e9  push       edi
0048b0ea  push       edi
0048b0eb  call       0x470dc6

// block 0048b0f0
0048b0f0  add        esp, 0x14

// block 0048b0f3
0048b0f3  mov        eax, dword ptr [ebp + 8]
0048b0f6  cmp        eax, edi
0048b0f8  je         0x48b0fc

// block 0048b0fa
0048b0fa  mov        dword ptr [eax], esi

// block 0048b0fc
0048b0fc  xor        eax, eax

// block 0048b0fe
0048b0fe  pop        edi
0048b0ff  pop        esi
0048b100  leave      
0048b101  ret        
