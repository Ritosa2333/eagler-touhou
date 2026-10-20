
// block 00459a60
00459a60  push       ebx
00459a61  push       ebp
00459a62  mov        ebp, dword ptr [esp + 0xc]
00459a66  push       esi
00459a67  mov        esi, eax
00459a69  mov        eax, dword ptr [ebp + 0x47c]
00459a6f  shr        eax, 5
00459a72  and        eax, 7
00459a75  mov        ebx, 1
00459a7a  cmp        byte ptr [esi + 0x4b5640], al
00459a80  je         0x459b7f

// block 00459a86
00459a86  call       0x45a3c0

// block 00459a8b
00459a8b  mov        eax, dword ptr [ebp + 0x47c]
00459a91  shr        eax, 5
00459a94  and        al, 7
00459a96  mov        byte ptr [esi + 0x4b5640], al
00459a9c  movzx      eax, al
00459a9f  cmp        eax, 6
00459aa2  ja         0x459b7f

// block 00459aa8
00459aa8  jmp        dword ptr [eax*4 + 0x459ccc]

// block 00459aaf
00459aaf  mov        eax, dword ptr [0x4ce8f0]
00459ab4  mov        ecx, dword ptr [eax]
00459ab6  mov        edx, dword ptr [ecx + 0xe4]
00459abc  push       5
00459abe  push       0x13
00459ac0  push       eax
00459ac1  call       edx

// block 00459ac3
00459ac3  push       6
00459ac5  jmp        0x459b57

// block 00459aca
00459aca  mov        eax, dword ptr [0x4ce8f0]
00459acf  mov        ecx, dword ptr [eax]
00459ad1  mov        edx, dword ptr [ecx + 0xe4]
00459ad7  push       5
00459ad9  push       0x13
00459adb  push       eax
00459adc  call       edx

// block 00459ade
00459ade  push       2
00459ae0  jmp        0x459b57

// block 00459ae2
00459ae2  mov        eax, dword ptr [0x4ce8f0]
00459ae7  mov        ecx, dword ptr [eax]
00459ae9  mov        edx, dword ptr [ecx + 0xe4]
00459aef  push       5
00459af1  push       0x13
00459af3  push       eax
00459af4  call       edx

// block 00459af6
00459af6  mov        eax, dword ptr [0x4ce8f0]
00459afb  mov        ecx, dword ptr [eax]
00459afd  mov        edx, dword ptr [ecx + 0xe4]
00459b03  push       2
00459b05  push       0x14
00459b07  push       eax
00459b08  call       edx

// block 00459b0a
00459b0a  push       3
00459b0c  jmp        0x459b6a

// block 00459b0e
00459b0e  push       2
00459b10  jmp        0x459b44

// block 00459b12
00459b12  mov        eax, dword ptr [0x4ce8f0]
00459b17  mov        ecx, dword ptr [eax]
00459b19  mov        edx, dword ptr [ecx + 0xe4]
00459b1f  push       0xa
00459b21  push       0x13
00459b23  push       eax
00459b24  call       edx

// block 00459b26
00459b26  push       6
00459b28  jmp        0x459b57

// block 00459b2a
00459b2a  mov        eax, dword ptr [0x4ce8f0]
00459b2f  mov        ecx, dword ptr [eax]
00459b31  mov        edx, dword ptr [ecx + 0xe4]
00459b37  push       4
00459b39  push       0x13
00459b3b  push       eax
00459b3c  call       edx

// block 00459b3e
00459b3e  push       6
00459b40  jmp        0x459b57

// block 00459b42
00459b42  push       9

// block 00459b44
00459b44  mov        eax, dword ptr [0x4ce8f0]
00459b49  mov        ecx, dword ptr [eax]
00459b4b  mov        edx, dword ptr [ecx + 0xe4]
00459b51  push       0x13
00459b53  push       eax
00459b54  call       edx

// block 00459b56
00459b56  push       ebx

// block 00459b57
00459b57  mov        eax, dword ptr [0x4ce8f0]
00459b5c  mov        ecx, dword ptr [eax]
00459b5e  mov        edx, dword ptr [ecx + 0xe4]
00459b64  push       0x14
00459b66  push       eax
00459b67  call       edx

// block 00459b69
00459b69  push       ebx

// block 00459b6a
00459b6a  mov        eax, dword ptr [0x4ce8f0]
00459b6f  mov        ecx, dword ptr [eax]
00459b71  mov        edx, dword ptr [ecx + 0xe4]
00459b77  push       0xab
00459b7c  push       eax
00459b7d  call       edx

// block 00459b7f
00459b7f  push       edi
00459b80  test       byte ptr [ebp + 0x47e], bl
00459b86  je         0x459b90

// block 00459b88
00459b88  mov        edi, dword ptr [ebp + 0x3c0]
00459b8e  jmp        0x459b96

// block 00459b90
00459b90  mov        edi, dword ptr [ebp + 0x3bc]

// block 00459b96
00459b96  cmp        dword ptr [esi + 0x88ed50], 0
00459b9d  mov        dword ptr [esp + 0x14], edi
00459ba1  je         0x459c36

// block 00459ba7
00459ba7  movzx      ecx, byte ptr [esi + 0x88ed4e]
00459bae  mov        eax, edi
00459bb0  shr        eax, 0x10
00459bb3  movzx      eax, al
00459bb6  imul       eax, ecx
00459bb9  shr        eax, 7
00459bbc  cmp        eax, 0x100
00459bc1  jb         0x459bc8

// block 00459bc3
00459bc3  mov        eax, 0xff

// block 00459bc8
00459bc8  movzx      edx, byte ptr [esp + 0x15]
00459bcd  mov        byte ptr [esp + 0x16], al
00459bd1  movzx      eax, byte ptr [esi + 0x88ed4d]
00459bd8  imul       eax, edx
00459bdb  shr        eax, 7
00459bde  cmp        eax, 0x100
00459be3  jb         0x459bea

// block 00459be5
00459be5  mov        eax, 0xff

// block 00459bea
00459bea  movzx      ecx, byte ptr [esp + 0x14]
00459bef  mov        byte ptr [esp + 0x15], al
00459bf3  movzx      eax, byte ptr [esi + 0x88ed4c]
00459bfa  imul       eax, ecx
00459bfd  shr        eax, 7
00459c00  cmp        eax, 0x100
00459c05  jb         0x459c0c

// block 00459c07
00459c07  mov        eax, 0xff

// block 00459c0c
00459c0c  movzx      edx, byte ptr [esp + 0x17]
00459c11  mov        byte ptr [esp + 0x14], al
00459c15  movzx      eax, byte ptr [esi + 0x88ed4f]
00459c1c  imul       eax, edx
00459c1f  shr        eax, 7
00459c22  cmp        eax, 0x100
00459c27  jb         0x459c2e

// block 00459c29
00459c29  mov        eax, 0xff

// block 00459c2e
00459c2e  mov        byte ptr [esp + 0x17], al
00459c32  mov        edi, dword ptr [esp + 0x14]

// block 00459c36
00459c36  cmp        dword ptr [esi + 0x4b5638], edi
00459c3c  je         0x459c5c

// block 00459c3e
00459c3e  call       0x45a3c0

// block 00459c43
00459c43  mov        dword ptr [esi + 0x4b5638], edi
00459c49  mov        eax, dword ptr [0x4ce8f0]
00459c4e  mov        ecx, dword ptr [eax]
00459c50  mov        edx, dword ptr [ecx + 0xe4]
00459c56  push       edi
00459c57  push       0x3c
00459c59  push       eax
00459c5a  call       edx

// block 00459c5c
00459c5c  mov        eax, dword ptr [ebp + 0x480]
00459c62  shr        eax, 1
00459c64  and        eax, ebx
00459c66  pop        edi
00459c67  cmp        byte ptr [esi + 0x4b5646], al
00459c6d  je         0x459cbd

// block 00459c6f
00459c6f  call       0x45a3c0

// block 00459c74
00459c74  mov        eax, dword ptr [ebp + 0x480]
00459c7a  shr        eax, 1
00459c7c  and        al, bl
00459c7e  mov        byte ptr [esi + 0x4b5646], al
00459c84  mov        eax, dword ptr [0x4ce8f0]
00459c89  mov        ecx, dword ptr [eax]
00459c8b  mov        edx, dword ptr [ecx + 0x114]
00459c91  jne        0x459ca0

// block 00459c93
00459c93  push       2
00459c95  push       5
00459c97  push       0
00459c99  push       eax
00459c9a  call       edx

// block 00459c9c
00459c9c  push       2
00459c9e  jmp        0x459ca9

// block 00459ca0
00459ca0  push       ebx
00459ca1  push       5
00459ca3  push       0
00459ca5  push       eax
00459ca6  call       edx

// block 00459ca8
00459ca8  push       ebx

// block 00459ca9
00459ca9  mov        eax, dword ptr [0x4ce8f0]
00459cae  mov        ecx, dword ptr [eax]
00459cb0  mov        edx, dword ptr [ecx + 0x114]
00459cb6  push       6
00459cb8  push       0
00459cba  push       eax
00459cbb  call       edx

// block 00459cbd
00459cbd  add        dword ptr [esi + 0xa8], ebx
00459cc3  pop        esi
00459cc4  pop        ebp
00459cc5  pop        ebx
00459cc6  ret        4
