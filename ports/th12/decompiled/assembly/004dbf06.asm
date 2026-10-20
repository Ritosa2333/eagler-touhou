
// block 004dbf06
004dbf06  je         0x4dbf12

// block 004dbf08
004dbf08  mov        eax, dword ptr [ebx]
004dbf0a  xchg       dword ptr [ebp + 0x599], eax
004dbf10  mov        dword ptr [ebx], eax

// block 004dbf12
004dbf12  lea        esi, [ebp + 0x5c5]
004dbf18  cmp        dword ptr [esi], 0
004dbf1b  je         0x4dc02b

// block 004dbf21
004dbf21  push       4
004dbf23  push       0x1000
004dbf28  push       0x1800
004dbf2d  push       0
004dbf2f  call       dword ptr [ebp + 0x51]

// block 004dbf32
004dbf32  mov        dword ptr [ebp + 0x148], eax

// block 004dbf38
004dbf38  mov        eax, dword ptr [esi + 4]
004dbf3b  add        eax, 0x10e
004dbf40  je         0x4dbffd

// block 004dbf46
004dbf46  push       4
004dbf48  push       0x1000
004dbf4d  push       eax
004dbf4e  push       0
004dbf50  call       dword ptr [ebp + 0x51]

// block 004dbf53
004dbf53  mov        dword ptr [ebp + 0x144], eax

// block 004dbffd
004dbffd  add        esi, 0xc
004dc000  cmp        dword ptr [esi], 0
004dc003  jne        0x4dbf38

// block 004dc009
004dc009  push       0x8000
004dc00e  push       0
004dc010  push       dword ptr [ebp + 0x148]
004dc016  call       dword ptr [ebp + 0x5e]

// block 004dc019
004dc019  mov        ebx, dword ptr [ebp + 0x595]
004dc01f  or         ebx, ebx
004dc021  je         0x4dc02b

// block 004dc023
004dc023  mov        eax, dword ptr [ebx]
004dc025  xchg       dword ptr [ebp + 0x599], eax

// block 004dc02b
004dc02b  mov        edx, dword ptr [ebp + 0x488]
004dc031  mov        eax, dword ptr [ebp + 0x591]
004dc037  sub        edx, eax
004dc039  je         0x4dc0b4

// block 004dc03b
004dc03b  mov        eax, edx
004dc03d  shr        eax, 0x10
004dc040  xor        ebx, ebx
004dc042  mov        esi, dword ptr [ebp + 0x59d]
004dc048  add        esi, dword ptr [ebp + 0x488]

// block 004dc04e
004dc04e  cmp        dword ptr [esi], 0
004dc051  je         0x4dc0b4

// block 004dc053
004dc053  mov        ecx, dword ptr [esi + 4]
004dc056  sub        ecx, 8
004dc059  shr        ecx, 1
004dc05b  mov        edi, dword ptr [esi]
004dc05d  add        edi, dword ptr [ebp + 0x488]
004dc063  add        esi, 8

// block 004dc066
004dc066  mov        bx, word ptr [esi]
004dc069  shr        ebx, 0xc
004dc06c  cmp        ebx, 1
004dc06f  je         0x4dc07d

// block 004dc071
004dc071  cmp        ebx, 2
004dc074  je         0x4dc08c

// block 004dc076
004dc076  cmp        ebx, 3
004dc079  je         0x4dc09b

// block 004dc07b
004dc07b  jmp        0x4dc0a9

// block 004dc07d
004dc07d  mov        bx, word ptr [esi]
004dc080  and        ebx, 0xfff
004dc086  add        word ptr [edi + ebx], ax
004dc08a  jmp        0x4dc0a9

// block 004dc08c
004dc08c  mov        bx, word ptr [esi]
004dc08f  and        ebx, 0xfff
004dc095  add        word ptr [edi + ebx], dx
004dc099  jmp        0x4dc0a9

// block 004dc09b
004dc09b  mov        bx, word ptr [esi]
004dc09e  and        ebx, 0xfff
004dc0a4  add        dword ptr [edi + ebx], edx
004dc0a7  jmp        0x4dc0a9

// block 004dc0a9
004dc0a9  or         word ptr [esi], 0xffff
004dc0ad  add        esi, 2
004dc0b0  loop       0x4dc066

// block 004dc0b2
004dc0b2  jmp        0x4dc04e

// block 004dc0b4
004dc0b4  mov        edx, dword ptr [ebp + 0x488]
004dc0ba  mov        esi, dword ptr [ebp + 0x5a5]
004dc0c0  or         esi, esi
004dc0c2  je         0x4dc0d5

// block 004dc0c4
004dc0c4  add        esi, edx

// block 004dc0c6
004dc0c6  lodsd      eax, dword ptr [esi]
004dc0c7  or         eax, eax
004dc0c9  je         0x4dc0d5

// block 004dc0cb
004dc0cb  add        eax, edx
004dc0cd  mov        edi, eax
004dc0cf  lodsw      ax, word ptr [esi]
004dc0d1  stosw      word ptr es:[edi], ax
004dc0d3  jmp        0x4dc0c6

// block 004dc0d5
004dc0d5  mov        esi, 0x32bc
004dc0da  mov        edx, dword ptr [ebp + 0x488]
004dc0e0  add        esi, edx

// block 004dc0e2
004dc0e2  mov        eax, dword ptr [esi + 0xc]
004dc0e5  test       eax, eax
004dc0e7  je         0x4dc1fa

// block 004dc0ed
004dc0ed  add        eax, edx
004dc0ef  mov        ebx, eax
004dc0f1  push       eax
004dc0f2  call       dword ptr [ebp + 0xfa9]

// block 004dc0f8
004dc0f8  test       eax, eax
004dc0fa  jne        0x4dc103

// block 004dc0fc
004dc0fc  push       ebx
004dc0fd  call       dword ptr [ebp + 0xfad]

// block 004dc103
004dc103  mov        dword ptr [ebp + 0x5a9], eax
004dc109  mov        dword ptr [ebp + 0x5ad], 0

// block 004dc113
004dc113  mov        edx, dword ptr [ebp + 0x488]
004dc119  mov        eax, dword ptr [esi]
004dc11b  test       eax, eax
004dc11d  jne        0x4dc122

// block 004dc11f
004dc11f  mov        eax, dword ptr [esi + 0x10]

// block 004dc122
004dc122  add        eax, edx
004dc124  add        eax, dword ptr [ebp + 0x5ad]
004dc12a  mov        ebx, dword ptr [eax]
004dc12c  mov        edi, dword ptr [esi + 0x10]
004dc12f  add        edi, edx
004dc131  add        edi, dword ptr [ebp + 0x5ad]
004dc137  test       ebx, ebx
004dc139  je         0x4dc1e4

// block 004dc13f
004dc13f  test       ebx, 0x80000000
004dc145  jne        0x4dc14b

// block 004dc147
004dc147  add        ebx, edx
004dc149  inc        ebx
004dc14a  inc        ebx

// block 004dc14b
004dc14b  push       ebx
004dc14c  and        ebx, 0x7fffffff
004dc152  push       ebx
004dc153  push       dword ptr [ebp + 0x5a9]
004dc159  call       dword ptr [ebp + 0xfa5]

// block 004dc15f
004dc15f  test       eax, eax
004dc161  pop        ebx
004dc162  jne        0x4dc1d6

// block 004dc164
004dc164  test       ebx, 0x80000000
004dc16a  jne        0x4dc185

// block 004dc16c
004dc16c  push       edi
004dc16d  mov        eax, dword ptr [esi + 0xc]
004dc170  add        eax, dword ptr [ebp + 0x488]
004dc176  push       eax
004dc177  push       ebx
004dc178  lea        eax, [ebp + 0x4db]
004dc17e  push       eax
004dc17f  push       edi
004dc180  jmp        0x4dc297

// block 004dc185
004dc185  and        ebx, 0x7fffffff
004dc18b  mov        eax, dword ptr [ebp + 0x48c]
004dc191  cmp        dword ptr [ebp + 0x5a9], eax
004dc197  jne        0x4dc1bd

// block 004dc199
004dc199  push       edi
004dc19a  mov        edx, ebx
004dc19c  dec        edx
004dc19d  shl        edx, 2
004dc1a0  mov        ebx, dword ptr [ebp + 0x5a9]
004dc1a6  mov        edi, dword ptr [ebx + 0x3c]
004dc1a9  mov        edi, dword ptr [ebx + edi + 0x78]
004dc1ad  add        ebx, dword ptr [ebx + edi + 0x1c]
004dc1b1  mov        eax, dword ptr [ebx + edx]
004dc1b4  add        eax, dword ptr [ebp + 0x5a9]
004dc1ba  pop        edi
004dc1bb  jmp        0x4dc1d6

// block 004dc1bd
004dc1bd  push       edi
004dc1be  mov        eax, dword ptr [esi + 0xc]
004dc1c1  add        eax, dword ptr [ebp + 0x488]
004dc1c7  push       eax
004dc1c8  push       ebx
004dc1c9  lea        eax, [ebp + 0x52c]
004dc1cf  push       eax
004dc1d0  push       edi
004dc1d1  jmp        0x4dc297

// block 004dc1d6
004dc1d6  mov        dword ptr [edi], eax
004dc1d8  add        dword ptr [ebp + 0x5ad], 4
004dc1df  jmp        0x4dc113

// block 004dc1e4
004dc1e4  mov        dword ptr [esi], eax
004dc1e6  mov        dword ptr [esi + 0xc], eax
004dc1e9  mov        dword ptr [esi + 0x10], eax
004dc1ec  add        esi, 0x14
004dc1ef  mov        edx, dword ptr [ebp + 0x488]
004dc1f5  jmp        0x4dc0e2

// block 004dc1fa
004dc1fa  mov        esi, dword ptr [ebp + 0x488]
004dc200  mov        edi, dword ptr [esi + 0x3c]
004dc203  add        edi, esi
004dc205  push       ecx
004dc206  push       esp
004dc207  push       ecx
004dc208  push       esp
004dc209  push       4
004dc20b  push       dword ptr [edi + 0x54]
004dc20e  push       esi
004dc20f  call       dword ptr [ebp + 0x6a]

// block 004dc212
004dc212  push       dword ptr [edi + 0x54]
004dc215  push       esi
004dc216  movzx      ecx, word ptr [edi + 6]
004dc21a  movzx      eax, word ptr [edi + 0x14]
004dc21e  lea        edi, [edi + eax - 0x10]
004dc222  lea        esi, [ebp + 0x5c5]

// block 004dc228
004dc228  lodsd      eax, dword ptr [esi]
004dc229  test       eax, eax
004dc22b  je         0x4dc26d

// block 004dc22d
004dc22d  add        edi, 0x28
004dc230  cmp        eax, dword ptr [edi + 0xc]
004dc233  loopne     0x4dc22d

// block 004dc235
004dc235  jne        0x4dc26d

// block 004dc237
004dc237  inc        ecx
004dc238  push       ecx
004dc239  push       esi
004dc23a  push       1
004dc23c  test       byte ptr [esi + 7], 0xe0
004dc240  je         0x4dc245

// block 004dc242
004dc242  shl        dword ptr [esp], 1

// block 004dc245
004dc245  test       byte ptr [esi + 7], 0x80
004dc249  je         0x4dc24e

// block 004dc24b
004dc24b  shl        dword ptr [esp], 1

// block 004dc24e
004dc24e  test       byte ptr [esi + 7], 0x20
004dc252  je         0x4dc258

// block 004dc254
004dc254  shl        dword ptr [esp], 4

// block 004dc258
004dc258  push       dword ptr [edi + 8]
004dc25b  add        eax, dword ptr [ebp + 0x488]
004dc261  push       eax
004dc262  call       dword ptr [ebp + 0x6a]

// block 004dc265
004dc265  pop        ecx
004dc266  lodsd      eax, dword ptr [esi]
004dc267  lodsd      eax, dword ptr [esi]
004dc268  mov        dword ptr [edi + 0x24], eax
004dc26b  loop       0x4dc228

// block 004dc26d
004dc26d  call       dword ptr [ebp + 0x6a]

// block 004dc270
004dc270  pop        ecx

// block 004dc297
004dc297  mov        eax, dword ptr [ebp + 0x48c]
004dc29d  lea        ecx, [ebp + 0x4a1]
004dc2a3  push       ecx
004dc2a4  push       eax
004dc2a5  call       dword ptr [ebp + 0xfa5]

// block 004dc2ab
004dc2ab  mov        dword ptr [ebp + 0x5b1], eax
004dc2b1  lea        eax, [ebp + 0x4ad]
004dc2b7  push       eax
004dc2b8  call       dword ptr [ebp + 0xfad]

// block 004dc2be
004dc2be  mov        dword ptr [ebp + 0x490], eax
004dc2c4  lea        ecx, [ebp + 0x4b8]
004dc2ca  push       ecx
004dc2cb  push       eax
004dc2cc  call       dword ptr [ebp + 0xfa5]

// block 004dc2d2
004dc2d2  mov        dword ptr [ebp + 0x5b5], eax
004dc2d8  mov        eax, dword ptr [ebp + 0x490]
004dc2de  lea        ecx, [ebp + 0x4c4]
004dc2e4  push       ecx
004dc2e5  push       eax
004dc2e6  call       dword ptr [ebp + 0xfa5]

// block 004dc2ec
004dc2ec  call       eax

// block 004dc2ee
004dc2ee  add        esp, 0x10
004dc2f1  pop        edi
004dc2f2  push       0x30
004dc2f4  lea        ebx, [ebp + 0x4ce]
004dc2fa  push       ebx
004dc2fb  push       edi
004dc2fc  push       0
004dc2fe  call       dword ptr [ebp + 0x5b5]

// block 004dc304
004dc304  push       -1
004dc306  call       dword ptr [ebp + 0x5b1]

// block 004dc30c
004dc30c  add        byte ptr [eax], al
004dc30e  add        byte ptr [eax], al
004dc310  add        byte ptr [eax], al
004dc312  add        byte ptr [eax], al
004dc314  add        byte ptr [eax], al
004dc316  add        byte ptr [eax], al
004dc318  imul       esp, dword ptr [ebp + 0x72], 0x6e
