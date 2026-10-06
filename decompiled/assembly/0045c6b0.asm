
// block 0045c6b0
0045c6b0  push       esi
0045c6b1  mov        esi, eax
0045c6b3  cmp        dword ptr [esi + 0x4b56a0], 0
0045c6ba  je         0x45c6c1

// block 0045c6bc
0045c6bc  call       0x45a3c0

// block 0045c6c1
0045c6c1  cmp        byte ptr [esi + 0x4b5642], 4
0045c6c8  je         0x45c6e3

// block 0045c6ca
0045c6ca  mov        eax, dword ptr [0x4ce8f0]
0045c6cf  mov        ecx, dword ptr [eax]
0045c6d1  mov        edx, dword ptr [ecx + 0x164]
0045c6d7  push       0x44
0045c6d9  push       eax
0045c6da  call       edx

// block 0045c6dc
0045c6dc  mov        byte ptr [esi + 0x4b5642], 4

// block 0045c6e3
0045c6e3  mov        eax, esi
0045c6e5  call       0x459cf0

// block 0045c6ea
0045c6ea  mov        eax, dword ptr [0x4ce8f0]
0045c6ef  mov        ecx, dword ptr [eax]
0045c6f1  mov        edx, dword ptr [ecx + 0x10c]
0045c6f7  push       2
0045c6f9  push       4
0045c6fb  push       0
0045c6fd  push       eax
0045c6fe  call       edx

// block 0045c700
0045c700  mov        eax, dword ptr [0x4ce8f0]
0045c705  mov        ecx, dword ptr [eax]
0045c707  mov        edx, dword ptr [ecx + 0x10c]
0045c70d  push       2
0045c70f  push       1
0045c711  push       0
0045c713  push       eax
0045c714  call       edx

// block 0045c716
0045c716  mov        eax, dword ptr [0x4ce8f0]
0045c71b  mov        ecx, dword ptr [eax]
0045c71d  mov        edx, dword ptr [ecx + 0x10c]
0045c723  push       0
0045c725  push       5
0045c727  push       0
0045c729  push       eax
0045c72a  call       edx

// block 0045c72c
0045c72c  mov        eax, dword ptr [0x4ce8f0]
0045c731  mov        ecx, dword ptr [eax]
0045c733  mov        edx, dword ptr [ecx + 0x10c]
0045c739  push       0
0045c73b  push       2
0045c73d  push       0
0045c73f  push       eax
0045c740  call       edx

// block 0045c742
0045c742  mov        esi, dword ptr [0x4ce8cc]
0045c748  call       0x45a3c0

// block 0045c74d
0045c74d  mov        eax, dword ptr [0x4ce8f0]
0045c752  mov        ecx, dword ptr [eax]
0045c754  mov        edx, dword ptr [ecx + 0xe4]
0045c75a  push       0
0045c75c  push       0xe
0045c75e  push       eax
0045c75f  call       edx

// block 0045c761
0045c761  mov        edx, dword ptr [esp + 8]
0045c765  mov        eax, dword ptr [0x4ce8f0]
0045c76a  mov        ecx, dword ptr [eax]
0045c76c  push       0x14
0045c76e  push       edx
0045c76f  mov        edx, dword ptr [esp + 0x14]
0045c773  add        edx, -2
0045c776  push       edx
0045c777  push       6
0045c779  push       eax
0045c77a  mov        eax, dword ptr [ecx + 0x14c]
0045c780  call       eax

// block 0045c782
0045c782  mov        eax, dword ptr [0x4ce8cc]
0045c787  mov        cl, 0xff
0045c789  push       4
0045c78b  mov        byte ptr [eax + 0x4b5642], cl
0045c791  mov        byte ptr [eax + 0x4b5641], cl
0045c797  mov        byte ptr [eax + 0x4b5640], 7
0045c79e  mov        byte ptr [eax + 0x4b5643], cl
0045c7a4  mov        eax, dword ptr [0x4ce8f0]
0045c7a9  mov        ecx, dword ptr [eax]
0045c7ab  mov        edx, dword ptr [ecx + 0x10c]
0045c7b1  push       4
0045c7b3  push       0
0045c7b5  push       eax
0045c7b6  call       edx

// block 0045c7b8
0045c7b8  mov        eax, dword ptr [0x4ce8f0]
0045c7bd  mov        ecx, dword ptr [eax]
0045c7bf  mov        edx, dword ptr [ecx + 0x10c]
0045c7c5  push       4
0045c7c7  push       1
0045c7c9  push       0
0045c7cb  push       eax
0045c7cc  call       edx

// block 0045c7ce
0045c7ce  mov        eax, dword ptr [0x4ce8f0]
0045c7d3  mov        ecx, dword ptr [eax]
0045c7d5  push       2
0045c7d7  push       5
0045c7d9  mov        edx, dword ptr [ecx + 0x10c]
0045c7df  push       0
0045c7e1  push       eax
0045c7e2  call       edx

// block 0045c7e4
0045c7e4  mov        eax, dword ptr [0x4ce8f0]
0045c7e9  mov        ecx, dword ptr [eax]
0045c7eb  mov        edx, dword ptr [ecx + 0x10c]
0045c7f1  push       2
0045c7f3  push       2
0045c7f5  push       0
0045c7f7  push       eax
0045c7f8  call       edx

// block 0045c7fa
0045c7fa  xor        eax, eax
0045c7fc  pop        esi
0045c7fd  ret        8
