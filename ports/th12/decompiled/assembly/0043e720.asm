
// block 0043e720
0043e720  sub        esp, 0x108
0043e726  mov        eax, dword ptr [0x4ad138]
0043e72b  xor        eax, esp
0043e72d  mov        dword ptr [esp + 0x104], eax
0043e734  push       ebx
0043e735  push       esi
0043e736  mov        esi, dword ptr [0x4b4528]
0043e73c  mov        eax, dword ptr [esi + 0x114]
0043e742  mov        ecx, dword ptr [esi + 0x34]
0043e745  mov        edx, dword ptr [ecx + eax*4]
0043e748  push       edx
0043e749  lea        eax, [esp + 0xc]
0043e74d  push       0x4a0f40
0043e752  push       eax
0043e753  call       0x46cbbb

// block 0043e758
0043e758  mov        ecx, dword ptr [esi + 0x2dc]
0043e75e  xor        ebx, ebx
0043e760  add        esp, 0xc
0043e763  cmp        ecx, ebx
0043e765  je         0x43e776

// block 0043e767
0043e767  mov        edx, dword ptr [ecx]
0043e769  mov        eax, dword ptr [edx + 0x14]
0043e76c  push       1
0043e76e  call       eax

// block 0043e770
0043e770  mov        dword ptr [esi + 0x2dc], ebx

// block 0043e776
0043e776  mov        eax, dword ptr [esi + 0x2e0]
0043e77c  mov        dword ptr [esi + 0x2dc], ebx
0043e782  cmp        eax, ebx
0043e784  je         0x43e795

// block 0043e786
0043e786  push       eax
0043e787  call       0x46c941

// block 0043e78c
0043e78c  add        esp, 4
0043e78f  mov        dword ptr [esi + 0x2e0], ebx

// block 0043e795
0043e795  push       edi
0043e796  push       ebx
0043e797  push       ebx
0043e798  lea        eax, [esp + 0x14]
0043e79c  call       0x463c10

// block 0043e7a1
0043e7a1  push       0x1098
0043e7a6  mov        dword ptr [esi + 0x2e0], eax
0043e7ac  call       0x46c9ea

// block 0043e7b1
0043e7b1  mov        edi, eax
0043e7b3  add        esp, 4
0043e7b6  cmp        edi, ebx
0043e7b8  je         0x43e7df

// block 0043e7ba
0043e7ba  push       0x1098
0043e7bf  mov        dword ptr [edi], 0x49fc28
0043e7c5  push       ebx
0043e7c6  push       edi
0043e7c7  mov        dword ptr [edi + 0x1090], ebx
0043e7cd  mov        dword ptr [edi + 0x1094], ebx
0043e7d3  call       0x477420

// block 0043e7d8
0043e7d8  add        esp, 0xc
0043e7db  mov        ecx, edi
0043e7dd  jmp        0x43e7e1

// block 0043e7df
0043e7df  xor        ecx, ecx

// block 0043e7e1
0043e7e1  mov        eax, dword ptr [esi + 0x2e0]
0043e7e7  mov        dword ptr [esi + 0x2d8], ecx
0043e7ed  mov        edx, dword ptr [ecx]
0043e7ef  mov        edx, dword ptr [edx]
0043e7f1  push       eax
0043e7f2  call       edx

// block 0043e7f4
0043e7f4  mov        eax, dword ptr [esi + 0x2d8]
0043e7fa  mov        ecx, dword ptr [eax + 8]
0043e7fd  mov        eax, ecx
0043e7ff  mov        dword ptr [esi + 0x1f4], ecx
0043e805  pop        edi
0043e806  cmp        eax, ebx
0043e808  je         0x43e80f

// block 0043e80a
0043e80a  jg         0x43e80f

// block 0043e80c
0043e80c  lea        ebx, [eax - 1]

// block 0043e80f
0043e80f  mov        ecx, dword ptr [esp + 0x10c]
0043e816  mov        dword ptr [esi + 0x1ec], ebx
0043e81c  mov        dword ptr [esi + 0x30], 1
0043e823  pop        esi
0043e824  pop        ebx
0043e825  xor        ecx, esp
0043e827  xor        eax, eax
0043e829  call       0x46c932

// block 0043e82e
0043e82e  add        esp, 0x108
0043e834  ret        
