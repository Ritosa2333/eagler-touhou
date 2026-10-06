
// block 0040e060
0040e060  fldz       
0040e062  sub        esp, 8
0040e065  push       ebx
0040e066  push       ebp
0040e067  push       esi
0040e068  push       edi
0040e069  mov        edi, dword ptr [0x4b43cc]
0040e06f  mov        ebx, eax
0040e071  mov        eax, dword ptr [edi + 0x34]
0040e074  mov        esi, ecx
0040e076  test       al, 1
0040e078  jne        0x40e098

// block 0040e07a
0040e07a  xor        ebp, ebp
0040e07c  fst        dword ptr [edi + 0x2c]
0040e07f  or         eax, 1
0040e082  mov        dword ptr [edi + 0x28], ebp
0040e085  mov        dword ptr [edi + 0x24], 0xfff0bdc1
0040e08c  mov        dword ptr [edi + 0x30], 0x4b2ed0
0040e093  mov        dword ptr [edi + 0x34], eax
0040e096  jmp        0x40e09a

// block 0040e098
0040e098  xor        ebp, ebp

// block 0040e09a
0040e09a  lea        edx, [edi + 0x38]
0040e09d  fstp       dword ptr [edi + 0x2c]
0040e0a0  mov        dword ptr [edi + 0x28], ebp
0040e0a3  mov        dword ptr [edi + 0x24], 0xffffffff
0040e0aa  mov        dword ptr [edi + 0x78], esi
0040e0ad  mov        eax, ebx
0040e0af  sub        edx, ebx

// block 0040e0b1
0040e0b1  mov        cl, byte ptr [eax]
0040e0b3  mov        byte ptr [edx + eax], cl
0040e0b6  inc        eax
0040e0b7  test       cl, cl
0040e0b9  jne        0x40e0b1

// block 0040e0bb
0040e0bb  mov        eax, dword ptr [edi + 0x7c]
0040e0be  mov        ecx, dword ptr [0x4b4518]
0040e0c4  and        eax, 0xffffffe7
0040e0c7  or         eax, 3
0040e0ca  mov        dword ptr [edi + 0x7c], eax
0040e0cd  cmp        dword ptr [ecx + 0x10], 1
0040e0d1  je         0x40e170

// block 0040e0d7
0040e0d7  mov        ecx, dword ptr [0x4b0c90]
0040e0dd  mov        ebp, dword ptr [0x4b451c]
0040e0e3  lea        eax, [esi + esi*8]
0040e0e6  mov        esi, dword ptr [0x4b0c94]
0040e0ec  lea        ecx, [esi + ecx*2]
0040e0ef  imul       ecx, ecx, 0x45f4
0040e0f5  shl        eax, 4
0040e0f8  add        ecx, eax
0040e0fa  mov        edx, ebx
0040e0fc  lea        esi, [ecx + ebp + 0x66c]

// block 0040e103
0040e103  mov        cl, byte ptr [edx]
0040e105  mov        byte ptr [esi], cl
0040e107  inc        edx
0040e108  inc        esi
0040e109  test       cl, cl
0040e10b  jne        0x40e103

// block 0040e10d
0040e10d  mov        ecx, dword ptr [0x4b0c90]
0040e113  mov        edx, dword ptr [0x4b0c94]
0040e119  lea        edx, [edx + ecx*2]
0040e11c  imul       edx, edx, 0x45f4
0040e122  add        edx, eax
0040e124  lea        ecx, [edx + ebp + 0x66c]
0040e12b  mov        edx, dword ptr [ecx + 0x84]
0040e131  cmp        edx, 0x1869f
0040e137  jge        0x40e140

// block 0040e139
0040e139  inc        edx
0040e13a  mov        dword ptr [ecx + 0x84], edx

// block 0040e140
0040e140  lea        esi, [eax + ebp + 0x1aa24]
0040e147  mov        edx, esi
0040e149  mov        ecx, ebx
0040e14b  sub        edx, ebx
0040e14d  lea        ecx, [ecx]

// block 0040e150
0040e150  mov        al, byte ptr [ecx]
0040e152  mov        byte ptr [ecx + edx], al
0040e155  inc        ecx
0040e156  test       al, al
0040e158  jne        0x40e150

// block 0040e15a
0040e15a  mov        eax, dword ptr [esi + 0x84]
0040e160  cmp        eax, 0x1869f
0040e165  jge        0x40e16e

// block 0040e167
0040e167  inc        eax
0040e168  mov        dword ptr [esi + 0x84], eax

// block 0040e16e
0040e16e  xor        ebp, ebp

// block 0040e170
0040e170  and        dword ptr [edi + 0x7c], 0xffffffdf
0040e174  mov        ecx, dword ptr [0x4b43c4]
0040e17a  mov        eax, dword ptr [edi + 0x7c]
0040e17d  cmp        dword ptr [ecx + 0x3c], ebp
0040e180  je         0x40e188

// block 0040e182
0040e182  or         eax, 0x20
0040e185  mov        dword ptr [edi + 0x7c], eax

// block 0040e188
0040e188  mov        eax, dword ptr [0x4b43b8]
0040e18d  and        dword ptr [edi + 0x7c], 0xffffffbf
0040e191  push       ebp
0040e192  push       1
0040e194  lea        edx, [esp + 0x1c]
0040e198  mov        dword ptr [edi + 0x90], 1
0040e1a2  mov        ecx, dword ptr [eax + 0x18fb4]
0040e1a8  push       edx
0040e1a9  push       ecx
0040e1aa  mov        eax, 0x17
0040e1af  xor        ecx, ecx
0040e1b1  call       0x4615a0

// block 0040e1b6
0040e1b6  mov        edx, dword ptr [esp + 0x14]
0040e1ba  push       ebp
0040e1bb  push       0x4a
0040e1bd  lea        eax, [esp + 0x1c]
0040e1c1  mov        dword ptr [edi + 0x14], edx
0040e1c4  mov        ecx, dword ptr [0x4cee70]
0040e1ca  push       eax
0040e1cb  push       ecx
0040e1cc  mov        eax, 0x17
0040e1d1  xor        ecx, ecx
0040e1d3  call       0x4615a0

// block 0040e1d8
0040e1d8  mov        edx, dword ptr [esp + 0x14]
0040e1dc  mov        ecx, dword ptr [0x4b43b8]
0040e1e2  push       ebp
0040e1e3  push       2
0040e1e5  lea        eax, [esp + 0x1c]
0040e1e9  mov        dword ptr [edi + 0x18], edx
0040e1ec  mov        edx, dword ptr [ecx + 0x18fb4]
0040e1f2  push       eax
0040e1f3  push       edx
0040e1f4  mov        eax, 0x17
0040e1f9  xor        ecx, ecx
0040e1fb  call       0x4615a0

// block 0040e200
0040e200  mov        eax, dword ptr [esp + 0x14]
0040e204  mov        edx, dword ptr [0x4ce8cc]
0040e20a  mov        dword ptr [edi + 0x1c], eax
0040e20d  mov        ecx, dword ptr [edi + 0x18]
0040e210  push       ecx
0040e211  call       0x461920

// block 0040e216
0040e216  cmp        eax, ebp
0040e218  jne        0x40e21d

// block 0040e21a
0040e21a  mov        dword ptr [edi + 0x18], ebp

// block 0040e21d
0040e21d  push       ebx
0040e21e  push       ebp
0040e21f  push       ebp
0040e220  push       ebp
0040e221  push       0xffffff
0040e226  mov        esi, eax
0040e228  call       0x460800

// block 0040e22d
0040e22d  add        esp, 0x14
0040e230  mov        edx, 0x22
0040e235  call       0x453d90

// block 0040e23a
0040e23a  mov        eax, dword ptr [0x4b43c8]
0040e23f  mov        ecx, dword ptr [eax + 0x4debdc]
0040e245  push       ebp
0040e246  push       0x7f
0040e248  lea        edx, [esp + 0x1c]
0040e24c  push       edx
0040e24d  push       ecx
0040e24e  mov        eax, 0x17
0040e253  xor        ecx, ecx
0040e255  call       0x4615a0

// block 0040e25a
0040e25a  mov        ecx, dword ptr [esp + 0x14]
0040e25e  mov        edx, dword ptr [0x4b43dc]
0040e264  mov        esi, dword ptr [0x4ce8cc]
0040e26a  mov        dword ptr [edi + 0x20], ecx
0040e26d  mov        eax, dword ptr [edx + 0x1c]
0040e270  mov        edx, dword ptr [eax + 0x1074]
0040e276  add        eax, 0x1074
0040e27b  mov        dword ptr [edi + 0xac], edx
0040e281  mov        edx, dword ptr [eax + 4]
0040e284  mov        dword ptr [edi + 0xb0], edx
0040e28a  mov        eax, dword ptr [eax + 8]
0040e28d  push       ecx
0040e28e  mov        edx, esi
0040e290  mov        dword ptr [edi + 0xb4], eax
0040e296  call       0x461920

// block 0040e29b
0040e29b  cmp        eax, ebp
0040e29d  je         0x40e2d5

// block 0040e29f
0040e29f  fld        dword ptr [edi + 0xac]
0040e2a5  fadd       qword ptr [0x4a3d70]
0040e2ab  fadd       qword ptr [0x4a3d28]
0040e2b1  fstp       dword ptr [eax + 0x430]
0040e2b7  fld        dword ptr [edi + 0xb0]
0040e2bd  fadd       qword ptr [0x4a3d68]
0040e2c3  fstp       dword ptr [eax + 0x434]
0040e2c9  fld        dword ptr [edi + 0xb4]
0040e2cf  fstp       dword ptr [eax + 0x438]

// block 0040e2d5
0040e2d5  mov        ecx, dword ptr [edi + 0x20]
0040e2d8  push       ecx
0040e2d9  mov        edx, esi
0040e2db  call       0x461920

// block 0040e2e0
0040e2e0  cmp        eax, ebp
0040e2e2  jne        0x40e2e7

// block 0040e2e4
0040e2e4  mov        dword ptr [edi + 0x20], ebp

// block 0040e2e7
0040e2e7  add        eax, 0x10
0040e2ea  cmp        eax, ebp
0040e2ec  je         0x40e316

// block 0040e2ee
0040e2ee  mov        ecx, 0x7d
0040e2f3  jmp        0x40e300

// block 0040e300
0040e300  mov        edx, dword ptr [eax]
0040e302  cmp        word ptr [edx + 0x3ea], cx
0040e309  je         0x40e3cd

// block 0040e30f
0040e30f  mov        eax, dword ptr [eax + 4]
0040e312  cmp        eax, ebp
0040e314  jne        0x40e300

// block 0040e316
0040e316  xor        eax, eax

// block 0040e318
0040e318  mov        ebx, dword ptr [esp + 0x1c]
0040e31c  mov        dword ptr [eax + 0x404], ebx
0040e322  mov        eax, dword ptr [edi + 0x20]
0040e325  push       eax
0040e326  mov        edx, esi
0040e328  call       0x461920

// block 0040e32d
0040e32d  cmp        eax, ebp
0040e32f  jne        0x40e334

// block 0040e331
0040e331  mov        dword ptr [edi + 0x20], ebp

// block 0040e334
0040e334  add        eax, 0x10
0040e337  cmp        eax, ebp
0040e339  je         0x40e356

// block 0040e33b
0040e33b  mov        ecx, 0x7e

// block 0040e340
0040e340  mov        edx, dword ptr [eax]
0040e342  cmp        word ptr [edx + 0x3ea], cx
0040e349  je         0x40e3d4

// block 0040e34f
0040e34f  mov        eax, dword ptr [eax + 4]
0040e352  cmp        eax, ebp
0040e354  jne        0x40e340

// block 0040e356
0040e356  xor        eax, eax

// block 0040e358
0040e358  mov        dword ptr [eax + 0x404], ebx
0040e35e  mov        dword ptr [edi + 0x88], ebx
0040e364  mov        eax, dword ptr [0x4b0ca8]
0040e369  mov        ecx, dword ptr [0x4b0cb0]
0040e36f  add        eax, ecx
0040e371  imul       eax, eax, 0x1e8480
0040e377  cmp        eax, 0x5f5e100
0040e37c  mov        dword ptr [edi + 0x80], eax
0040e382  mov        dword ptr [edi + 0x84], eax
0040e388  jl         0x40e394

// block 0040e38a
0040e38a  mov        dword ptr [edi + 0x84], 0x5f5e0ff

// block 0040e394
0040e394  mov        eax, dword ptr [0x4b43c8]
0040e399  mov        ecx, dword ptr [eax + 0x4debdc]
0040e39f  push       ebp
0040e3a0  push       0x89
0040e3a5  lea        edx, [esp + 0x24]
0040e3a9  push       edx
0040e3aa  push       ecx
0040e3ab  mov        eax, 0x17
0040e3b0  xor        ecx, ecx
0040e3b2  call       0x4615a0

// block 0040e3b7
0040e3b7  mov        eax, dword ptr [0x4b0cb0]
0040e3bc  dec        eax
0040e3bd  cmp        eax, 6
0040e3c0  ja         0x40e5b0

// block 0040e3c6
0040e3c6  jmp        dword ptr [eax*4 + 0x40e5bc]

// block 0040e3cd
0040e3cd  mov        eax, edx
0040e3cf  jmp        0x40e318

// block 0040e3d4
0040e3d4  mov        eax, edx
0040e3d6  jmp        0x40e358

// block 0040e5b0
0040e5b0  pop        edi
0040e5b1  pop        esi
0040e5b2  pop        ebp
0040e5b3  pop        ebx
0040e5b4  add        esp, 8
0040e5b7  ret        4
