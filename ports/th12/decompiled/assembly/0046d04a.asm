
// block 0046d04a
0046d04a  mov        edi, edi
0046d04c  push       ebp
0046d04d  mov        ebp, esp
0046d04f  push       esi
0046d050  mov        esi, dword ptr [ebp + 8]
0046d053  cmp        esi, -0x20
0046d056  ja         0x46d0fd

// block 0046d05c
0046d05c  push       ebx
0046d05d  push       edi
0046d05e  mov        edi, dword ptr [0x4981d4]

// block 0046d064
0046d064  cmp        dword ptr [0x4b3c04], 0
0046d06b  jne        0x46d085

// block 0046d06d
0046d06d  call       0x47315e

// block 0046d072
0046d072  push       0x1e
0046d074  call       0x472f8d

// block 0046d079
0046d079  push       0xff
0046d07e  call       0x472c61

// block 0046d085
0046d085  mov        eax, dword ptr [0x4d6458]
0046d08a  cmp        eax, 1
0046d08d  jne        0x46d09d

// block 0046d08f
0046d08f  test       esi, esi
0046d091  je         0x46d097

// block 0046d093
0046d093  mov        eax, esi
0046d095  jmp        0x46d09a

// block 0046d097
0046d097  xor        eax, eax
0046d099  inc        eax

// block 0046d09a
0046d09a  push       eax
0046d09b  jmp        0x46d0b9

// block 0046d09d
0046d09d  cmp        eax, 3
0046d0a0  jne        0x46d0ad

// block 0046d0a2
0046d0a2  push       esi
0046d0a3  call       0x46cf81

// block 0046d0a8
0046d0a8  pop        ecx
0046d0a9  test       eax, eax
0046d0ab  jne        0x46d0c3

// block 0046d0ad
0046d0ad  test       esi, esi
0046d0af  jne        0x46d0b2

// block 0046d0b1
0046d0b1  inc        esi

// block 0046d0b2
0046d0b2  add        esi, 0xf
0046d0b5  and        esi, 0xfffffff0
0046d0b8  push       esi

// block 0046d0b9
0046d0b9  push       0
0046d0bb  push       dword ptr [0x4b3c04]
0046d0c1  call       edi

// block 0046d0c3
0046d0c3  mov        ebx, eax
0046d0c5  test       ebx, ebx
0046d0c7  jne        0x46d0f7

// block 0046d0c9
0046d0c9  push       0xc
0046d0cb  pop        esi
0046d0cc  cmp        dword ptr [0x4b40bc], eax
0046d0d2  je         0x46d0e9

// block 0046d0d4
0046d0d4  push       dword ptr [ebp + 8]
0046d0d7  call       0x46ff67

// block 0046d0dc
0046d0dc  pop        ecx
0046d0dd  test       eax, eax
0046d0df  je         0x46d0f0

// block 0046d0e1
0046d0e1  mov        esi, dword ptr [ebp + 8]
0046d0e4  jmp        0x46d064

// block 0046d0e9
0046d0e9  call       0x46e96f

// block 0046d0ee
0046d0ee  mov        dword ptr [eax], esi

// block 0046d0f0
0046d0f0  call       0x46e96f

// block 0046d0f5
0046d0f5  mov        dword ptr [eax], esi

// block 0046d0f7
0046d0f7  pop        edi
0046d0f8  mov        eax, ebx
0046d0fa  pop        ebx
0046d0fb  jmp        0x46d111

// block 0046d0fd
0046d0fd  push       esi
0046d0fe  call       0x46ff67

// block 0046d103
0046d103  pop        ecx
0046d104  call       0x46e96f

// block 0046d109
0046d109  mov        dword ptr [eax], 0xc
0046d10f  xor        eax, eax

// block 0046d111
0046d111  pop        esi
0046d112  pop        ebp
0046d113  ret        
