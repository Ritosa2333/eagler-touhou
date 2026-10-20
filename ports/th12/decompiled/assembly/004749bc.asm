
// block 004749bc
004749bc  mov        edi, edi
004749be  push       ebp
004749bf  mov        ebp, esp
004749c1  sub        esp, 0x1c4
004749c7  mov        eax, dword ptr [0x4ad138]
004749cc  xor        eax, ebp
004749ce  mov        dword ptr [ebp - 4], eax
004749d1  push       ebx
004749d2  push       edi
004749d3  mov        ebx, ecx
004749d5  call       0x475467

// block 004749da
004749da  push       dword ptr [ebp + 8]
004749dd  mov        edi, eax
004749df  lea        eax, [ebp - 0x198]
004749e5  push       eax
004749e6  lea        eax, [ebp - 0x1b0]
004749ec  push       eax
004749ed  push       0x83
004749f2  lea        eax, [ebp - 0x88]
004749f8  push       eax
004749f9  push       ebx
004749fa  add        edi, 0x1d0
00474a00  call       0x47478b

// block 00474a05
00474a05  add        esp, 0x18
00474a08  test       eax, eax
00474a0a  jne        0x474a13

// block 00474a0c
00474a0c  xor        eax, eax
00474a0e  jmp        0x474cb0

// block 00474a13
00474a13  mov        eax, dword ptr [ebp + 8]
00474a16  shl        eax, 4
00474a19  lea        ebx, [eax + esi]
00474a1c  push       dword ptr [ebx + 0x48]
00474a1f  lea        eax, [ebp - 0x88]
00474a25  push       eax
00474a26  call       0x472ac0

// block 00474a2b
00474a2b  pop        ecx
00474a2c  pop        ecx
00474a2d  test       eax, eax
00474a2f  je         0x474cad

// block 00474a35
00474a35  lea        eax, [ebp - 0x88]
00474a3b  push       eax
00474a3c  call       0x475860

// block 00474a41
00474a41  add        eax, 5
00474a44  push       eax
00474a45  mov        dword ptr [ebp - 0x18c], eax
00474a4b  call       0x4777ce

// block 00474a50
00474a50  pop        ecx
00474a51  pop        ecx
00474a52  mov        dword ptr [ebp - 0x190], eax
00474a58  test       eax, eax
00474a5a  je         0x474a0c

// block 00474a5c
00474a5c  mov        eax, dword ptr [ebx + 0x48]
00474a5f  mov        ecx, dword ptr [ebp + 8]
00474a62  mov        dword ptr [ebp - 0x1a4], eax
00474a68  lea        eax, [esi + ecx*4 + 0xc]
00474a6c  add        ecx, 6
00474a6f  imul       ecx, ecx, 6
00474a72  mov        dword ptr [ebp - 0x194], eax
00474a78  mov        eax, dword ptr [eax]
00474a7a  mov        dword ptr [ebp - 0x1a8], eax
00474a80  lea        eax, [ecx + esi]
00474a83  push       6
00474a85  push       eax
00474a86  mov        dword ptr [ebp - 0x19c], eax
00474a8c  lea        eax, [ebp - 0x1c4]
00474a92  push       eax
00474a93  call       0x47a330

// block 00474a98
00474a98  mov        eax, dword ptr [esi + 4]
00474a9b  lea        ecx, [ebp - 0x88]
00474aa1  push       ecx
00474aa2  mov        ecx, dword ptr [ebp - 0x18c]
00474aa8  mov        dword ptr [ebp - 0x1b4], eax
00474aae  mov        eax, dword ptr [ebp - 0x190]
00474ab4  add        ecx, -4
00474ab7  add        eax, 4
00474aba  push       ecx
00474abb  push       eax
00474abc  call       0x47a2b9

// block 00474ac1
00474ac1  add        esp, 0x18
00474ac4  test       eax, eax
00474ac6  je         0x474ad7

// block 00474ac8
00474ac8  xor        eax, eax
00474aca  push       eax
00474acb  push       eax
00474acc  push       eax
00474acd  push       eax
00474ace  push       eax
00474acf  call       0x470dc6

// block 00474ad4
00474ad4  add        esp, 0x14

// block 00474ad7
00474ad7  mov        eax, dword ptr [ebp - 0x190]
00474add  mov        ecx, dword ptr [ebp - 0x194]
00474ae3  add        eax, 4
00474ae6  mov        dword ptr [ebx + 0x48], eax
00474ae9  movzx      eax, word ptr [ebp - 0x1b0]
00474af0  mov        dword ptr [ecx], eax
00474af2  push       6
00474af4  lea        eax, [ebp - 0x1b0]
00474afa  push       eax
00474afb  push       dword ptr [ebp - 0x19c]
00474b01  call       0x47a330

// block 00474b06
00474b06  add        esp, 0xc
00474b09  cmp        dword ptr [ebp + 8], 2
00474b0d  jne        0x474c0a

// block 00474b13
00474b13  mov        eax, dword ptr [ebp - 0x198]
00474b19  and        dword ptr [ebp - 0x18c], 0
00474b20  mov        dword ptr [esi + 4], eax
00474b23  mov        eax, dword ptr [edi + 0x24]
00474b26  mov        ecx, dword ptr [edi + 0x20]
00474b29  mov        dword ptr [ebp - 0x19c], eax
00474b2f  mov        eax, edi

// block 00474b31
00474b31  mov        edx, dword ptr [esi + 4]
00474b34  cmp        edx, dword ptr [eax]
00474b36  je         0x474b6e

// block 00474b38
00474b38  mov        edx, dword ptr [eax]
00474b3a  inc        dword ptr [ebp - 0x18c]
00474b40  mov        dword ptr [eax], ecx
00474b42  mov        ecx, dword ptr [ebp - 0x19c]
00474b48  mov        dword ptr [ebp - 0x1bc], edx
00474b4e  mov        edx, dword ptr [eax + 4]
00474b51  mov        dword ptr [eax + 4], ecx
00474b54  mov        ecx, dword ptr [ebp - 0x1bc]
00474b5a  add        eax, 8
00474b5d  cmp        dword ptr [ebp - 0x18c], 5
00474b64  mov        dword ptr [ebp - 0x19c], edx
00474b6a  jl         0x474b31

// block 00474b6c
00474b6c  jmp        0x474b90

// block 00474b6e
00474b6e  mov        eax, dword ptr [ebp - 0x18c]
00474b74  test       eax, eax
00474b76  je         0x474b90

// block 00474b78
00474b78  lea        eax, [edi + eax*8]
00474b7b  mov        edx, dword ptr [eax]
00474b7d  mov        dword ptr [edi], edx
00474b7f  mov        edx, dword ptr [eax + 4]
00474b82  mov        dword ptr [edi + 4], edx
00474b85  mov        dword ptr [eax], ecx
00474b87  mov        ecx, dword ptr [ebp - 0x19c]
00474b8d  mov        dword ptr [eax + 4], ecx

// block 00474b90
00474b90  cmp        dword ptr [ebp - 0x18c], 5
00474b97  jne        0x474c01

// block 00474b99
00474b99  push       1
00474b9b  push       dword ptr [esi + 0x14]
00474b9e  lea        eax, [ebp - 0x188]
00474ba4  push       dword ptr [esi + 4]
00474ba7  push       eax
00474ba8  push       0x7f
00474baa  push       0x49d478
00474baf  push       1
00474bb1  push       0
00474bb3  call       0x48384c

// block 00474bb8
00474bb8  add        esp, 0x20
00474bbb  test       eax, eax
00474bbd  je         0x474bf8

// block 00474bbf
00474bbf  xor        eax, eax

// block 00474bc1
00474bc1  mov        ecx, 0x1ff
00474bc6  and        word ptr [ebp + eax*2 - 0x188], cx
00474bce  inc        eax
00474bcf  cmp        eax, 0x7f
00474bd2  jb         0x474bc1

// block 00474bd4
00474bd4  push       0xfe
00474bd9  push       dword ptr [0x4ad9d0]
00474bdf  lea        eax, [ebp - 0x188]
00474be5  push       eax
00474be6  call       0x487768

// block 00474beb
00474beb  add        esp, 0xc
00474bee  neg        eax
00474bf0  sbb        eax, eax
00474bf2  inc        eax
00474bf3  mov        dword ptr [edi + 4], eax
00474bf6  jmp        0x474bfc

// block 00474bf8
00474bf8  and        dword ptr [edi + 4], 0

// block 00474bfc
00474bfc  mov        eax, dword ptr [esi + 4]
00474bff  mov        dword ptr [edi], eax

// block 00474c01
00474c01  mov        eax, dword ptr [edi + 4]
00474c04  mov        dword ptr [esi + 0xa8], eax

// block 00474c0a
00474c0a  cmp        dword ptr [ebp + 8], 1
00474c0e  jne        0x474c19

// block 00474c10
00474c10  mov        eax, dword ptr [ebp - 0x198]
00474c16  mov        dword ptr [esi + 8], eax

// block 00474c19
00474c19  mov        eax, dword ptr [ebp + 8]
00474c1c  imul       eax, eax, 0xc
00474c1f  push       esi
00474c20  call       dword ptr [eax + 0x49d438]

// block 00474c26
00474c26  pop        ecx
00474c27  test       eax, eax
00474c29  je         0x474c5c

// block 00474c2b
00474c2b  mov        eax, dword ptr [ebp - 0x1a4]
00474c31  push       dword ptr [ebp - 0x190]
00474c37  mov        dword ptr [ebx + 0x48], eax
00474c3a  call       0x46c941

// block 00474c3f
00474c3f  mov        eax, dword ptr [ebp - 0x1a8]
00474c45  pop        ecx
00474c46  mov        ecx, dword ptr [ebp - 0x194]
00474c4c  mov        dword ptr [ecx], eax
00474c4e  mov        eax, dword ptr [ebp - 0x1b4]
00474c54  mov        dword ptr [esi + 4], eax
00474c57  jmp        0x474a0c

// block 00474c5c
00474c5c  cmp        dword ptr [ebp - 0x1a4], 0x4ad9d8
00474c66  je         0x474c95

// block 00474c68
00474c68  mov        eax, dword ptr [ebp + 8]
00474c6b  add        eax, 5
00474c6e  shl        eax, 4
00474c71  lea        edi, [eax + esi]
00474c74  push       dword ptr [edi]
00474c76  call       dword ptr [0x49815c]

// block 00474c7c
00474c7c  test       eax, eax
00474c7e  jne        0x474c95

// block 00474c80
00474c80  push       dword ptr [edi]
00474c82  call       0x46c941

// block 00474c87
00474c87  push       dword ptr [ebx + 0x54]
00474c8a  call       0x46c941

// block 00474c8f
00474c8f  and        dword ptr [ebx + 0x4c], 0
00474c93  pop        ecx
00474c94  pop        ecx

// block 00474c95
00474c95  mov        ecx, dword ptr [ebp + 8]
00474c98  mov        eax, dword ptr [ebp - 0x190]
00474c9e  add        ecx, 5
00474ca1  shl        ecx, 4
00474ca4  mov        dword ptr [eax], 1
00474caa  mov        dword ptr [ecx + esi], eax

// block 00474cad
00474cad  mov        eax, dword ptr [ebx + 0x48]

// block 00474cb0
00474cb0  mov        ecx, dword ptr [ebp - 4]
00474cb3  pop        edi
00474cb4  xor        ecx, ebp
00474cb6  pop        ebx
00474cb7  call       0x46c932

// block 00474cbc
00474cbc  leave      
00474cbd  ret        
