
// block 0048e03c
0048e03c  mov        edi, edi
0048e03e  push       ebp
0048e03f  mov        ebp, esp
0048e041  sub        esp, 0x30
0048e044  mov        eax, dword ptr [0x4ad138]
0048e049  xor        eax, ebp
0048e04b  mov        dword ptr [ebp - 4], eax
0048e04e  mov        eax, dword ptr [ebp + 0x14]
0048e051  push       ebx
0048e052  mov        ebx, dword ptr [ebp + 0x10]
0048e055  push       esi
0048e056  mov        dword ptr [ebp - 0x30], eax
0048e059  push       edi
0048e05a  lea        eax, [ebp + 8]
0048e05d  push       eax
0048e05e  lea        eax, [ebp - 0x10]
0048e061  push       eax
0048e062  call       0x48df7f

// block 0048e067
0048e067  pop        ecx
0048e068  pop        ecx
0048e069  lea        eax, [ebp - 0x2c]
0048e06c  push       eax
0048e06d  push       0
0048e06f  push       0x11
0048e071  sub        esp, 0xc
0048e074  lea        esi, [ebp - 0x10]
0048e077  mov        edi, esp
0048e079  movsd      dword ptr es:[edi], dword ptr [esi]
0048e07a  movsd      dword ptr es:[edi], dword ptr [esi]
0048e07b  movsw      word ptr es:[edi], word ptr [esi]
0048e07d  call       0x48ee11

// block 0048e082
0048e082  mov        esi, dword ptr [ebp - 0x30]
0048e085  mov        dword ptr [ebx + 8], eax
0048e088  movsx      eax, byte ptr [ebp - 0x2a]
0048e08c  mov        dword ptr [ebx], eax
0048e08e  movsx      eax, word ptr [ebp - 0x2c]
0048e092  mov        dword ptr [ebx + 4], eax
0048e095  lea        eax, [ebp - 0x28]
0048e098  push       eax
0048e099  push       dword ptr [ebp + 0x18]
0048e09c  push       esi
0048e09d  call       0x47a2b9

// block 0048e0a2
0048e0a2  add        esp, 0x24
0048e0a5  test       eax, eax
0048e0a7  je         0x48e0b8

// block 0048e0a9
0048e0a9  xor        eax, eax
0048e0ab  push       eax
0048e0ac  push       eax
0048e0ad  push       eax
0048e0ae  push       eax
0048e0af  push       eax
0048e0b0  call       0x470dc6

// block 0048e0b5
0048e0b5  add        esp, 0x14

// block 0048e0b8
0048e0b8  mov        ecx, dword ptr [ebp - 4]
0048e0bb  pop        edi
0048e0bc  mov        dword ptr [ebx + 0xc], esi
0048e0bf  pop        esi
0048e0c0  mov        eax, ebx
0048e0c2  xor        ecx, ebp
0048e0c4  pop        ebx
0048e0c5  call       0x46c932

// block 0048e0ca
0048e0ca  leave      
0048e0cb  ret        
