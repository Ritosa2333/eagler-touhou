
// block 0048d08b
0048d08b  push       0x10
0048d08d  push       0x4ab100
0048d092  call       0x46fcec

// block 0048d097
0048d097  xor        ebx, ebx
0048d099  mov        dword ptr [ebp - 0x1c], ebx
0048d09c  push       1
0048d09e  call       0x46ec9a

// block 0048d0a3
0048d0a3  pop        ecx
0048d0a4  mov        dword ptr [ebp - 4], ebx
0048d0a7  push       3
0048d0a9  pop        edi

// block 0048d0aa
0048d0aa  mov        dword ptr [ebp - 0x20], edi
0048d0ad  cmp        edi, dword ptr [0x4d6300]
0048d0b3  jge        0x48d10c

// block 0048d0b5
0048d0b5  mov        esi, edi
0048d0b7  shl        esi, 2
0048d0ba  mov        eax, dword ptr [0x4d52e0]
0048d0bf  add        eax, esi
0048d0c1  cmp        dword ptr [eax], ebx
0048d0c3  je         0x48d109

// block 0048d0c5
0048d0c5  mov        eax, dword ptr [eax]
0048d0c7  test       byte ptr [eax + 0xc], 0x83
0048d0cb  je         0x48d0dc

// block 0048d0cd
0048d0cd  push       eax
0048d0ce  call       0x48ebf2

// block 0048d0d3
0048d0d3  pop        ecx
0048d0d4  cmp        eax, -1
0048d0d7  je         0x48d0dc

// block 0048d0d9
0048d0d9  inc        dword ptr [ebp - 0x1c]

// block 0048d0dc
0048d0dc  cmp        edi, 0x14
0048d0df  jl         0x48d109

// block 0048d0e1
0048d0e1  mov        eax, dword ptr [0x4d52e0]
0048d0e6  mov        eax, dword ptr [esi + eax]
0048d0e9  add        eax, 0x20
0048d0ec  push       eax
0048d0ed  call       dword ptr [0x498094]

// block 0048d0f3
0048d0f3  mov        eax, dword ptr [0x4d52e0]
0048d0f8  push       dword ptr [esi + eax]
0048d0fb  call       0x46c941

// block 0048d100
0048d100  pop        ecx
0048d101  mov        eax, dword ptr [0x4d52e0]
0048d106  mov        dword ptr [esi + eax], ebx

// block 0048d109
0048d109  inc        edi
0048d10a  jmp        0x48d0aa

// block 0048d10c
0048d10c  mov        dword ptr [ebp - 4], 0xfffffffe
0048d113  call       0x48d121

// block 0048d118
0048d118  mov        eax, dword ptr [ebp - 0x1c]
0048d11b  call       0x46fd31

// block 0048d120
0048d120  ret        
