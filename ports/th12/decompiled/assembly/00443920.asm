
// block 00443920
00443920  mov        eax, dword ptr [edi + 0x2d0]
00443926  push       ebx
00443927  mov        ebx, dword ptr [0x4ce8cc]
0044392d  push       ebp
0044392e  push       esi
0044392f  push       eax
00443930  mov        edx, ebx
00443932  call       0x461920

// block 00443937
00443937  xor        ebp, ebp
00443939  cmp        eax, ebp
0044393b  jne        0x443943

// block 0044393d
0044393d  mov        dword ptr [edi + 0x2d0], ebp

// block 00443943
00443943  add        eax, 0x10
00443946  cmp        eax, ebp
00443948  je         0x443966

// block 0044394a
0044394a  mov        ecx, 0x3b
0044394f  nop        

// block 00443950
00443950  mov        edx, dword ptr [eax]
00443952  cmp        word ptr [edx + 0x3ea], cx
00443959  je         0x4441ef

// block 0044395f
0044395f  mov        eax, dword ptr [eax + 4]
00443962  cmp        eax, ebp
00443964  jne        0x443950

// block 00443966
00443966  xor        eax, eax

// block 00443968
00443968  push       eax
00443969  mov        edx, ebx
0044396b  call       0x461920

// block 00443970
00443970  mov        esi, eax
00443972  cmp        esi, ebp
00443974  je         0x44399d

// block 00443976
00443976  movsx      ecx, word ptr [edi + 0x5a68]
0044397d  mov        eax, 0x66666667
00443982  imul       ecx
00443984  sar        edx, 2
00443987  mov        ecx, edx
00443989  shr        ecx, 0x1f
0044398c  lea        ecx, [edx + ecx + 0x2c]
00443990  mov        edx, dword ptr [esi + 0x3f8]
00443996  mov        eax, esi
00443998  call       0x454b80

// block 0044399d
0044399d  mov        edx, dword ptr [edi + 0x2d0]
004439a3  push       edx
004439a4  mov        edx, ebx
004439a6  call       0x461920

// block 004439ab
004439ab  cmp        eax, ebp
004439ad  jne        0x4439b5

// block 004439af
004439af  mov        dword ptr [edi + 0x2d0], ebp

// block 004439b5
004439b5  add        eax, 0x10
004439b8  cmp        eax, ebp
004439ba  je         0x4439d7

// block 004439bc
004439bc  mov        ecx, 0x3c

// block 004439c1
004439c1  mov        edx, dword ptr [eax]
004439c3  cmp        word ptr [edx + 0x3ea], cx
004439ca  je         0x4441f8

// block 004439d0
004439d0  mov        eax, dword ptr [eax + 4]
004439d3  cmp        eax, ebp
004439d5  jne        0x4439c1

// block 004439d7
004439d7  xor        eax, eax

// block 004439d9
004439d9  push       eax
004439da  mov        edx, ebx
004439dc  call       0x461920

// block 004439e1
004439e1  mov        esi, eax
004439e3  cmp        esi, ebp
004439e5  je         0x443a08

// block 004439e7
004439e7  movsx      eax, word ptr [edi + 0x5a68]
004439ee  cdq        
004439ef  mov        ecx, 0xa
004439f4  idiv       ecx
004439f6  mov        eax, esi
004439f8  mov        ecx, edx
004439fa  mov        edx, dword ptr [esi + 0x3f8]
00443a00  add        ecx, 0x2c
00443a03  call       0x454b80

// block 00443a08
00443a08  mov        edx, dword ptr [edi + 0x2d0]
00443a0e  push       edx
00443a0f  mov        edx, ebx
00443a11  call       0x461920

// block 00443a16
00443a16  cmp        eax, ebp
00443a18  jne        0x443a20

// block 00443a1a
00443a1a  mov        dword ptr [edi + 0x2d0], ebp

// block 00443a20
00443a20  add        eax, 0x10
00443a23  cmp        eax, ebp
00443a25  je         0x443a46

// block 00443a27
00443a27  mov        ecx, 0x45
00443a2c  lea        esp, [esp]

// block 00443a30
00443a30  mov        edx, dword ptr [eax]
00443a32  cmp        word ptr [edx + 0x3ea], cx
00443a39  je         0x444201

// block 00443a3f
00443a3f  mov        eax, dword ptr [eax + 4]
00443a42  cmp        eax, ebp
00443a44  jne        0x443a30

// block 00443a46
00443a46  xor        eax, eax

// block 00443a48
00443a48  push       eax
00443a49  mov        edx, ebx
00443a4b  call       0x461920

// block 00443a50
00443a50  mov        esi, eax
00443a52  cmp        esi, ebp
00443a54  je         0x443a7d

// block 00443a56
00443a56  movsx      ecx, word ptr [edi + 0x5a68]
00443a5d  mov        eax, 0x66666667
00443a62  imul       ecx
00443a64  sar        edx, 2
00443a67  mov        ecx, edx
00443a69  shr        ecx, 0x1f
00443a6c  lea        ecx, [edx + ecx + 0x2c]
00443a70  mov        edx, dword ptr [esi + 0x3f8]
00443a76  mov        eax, esi
00443a78  call       0x454b80

// block 00443a7d
00443a7d  mov        edx, dword ptr [edi + 0x2d0]
00443a83  push       edx
00443a84  mov        edx, ebx
00443a86  call       0x461920

// block 00443a8b
00443a8b  cmp        eax, ebp
00443a8d  jne        0x443a95

// block 00443a8f
00443a8f  mov        dword ptr [edi + 0x2d0], ebp

// block 00443a95
00443a95  add        eax, 0x10
00443a98  cmp        eax, ebp
00443a9a  je         0x443ab7

// block 00443a9c
00443a9c  mov        ecx, 0x46

// block 00443aa1
00443aa1  mov        edx, dword ptr [eax]
00443aa3  cmp        word ptr [edx + 0x3ea], cx
00443aaa  je         0x44420a

// block 00443ab0
00443ab0  mov        eax, dword ptr [eax + 4]
00443ab3  cmp        eax, ebp
00443ab5  jne        0x443aa1

// block 00443ab7
00443ab7  xor        eax, eax

// block 00443ab9
00443ab9  push       eax
00443aba  mov        edx, ebx
00443abc  call       0x461920

// block 00443ac1
00443ac1  mov        esi, eax
00443ac3  cmp        esi, ebp
00443ac5  je         0x443ae8

// block 00443ac7
00443ac7  movsx      eax, word ptr [edi + 0x5a68]
00443ace  cdq        
00443acf  mov        ecx, 0xa
00443ad4  idiv       ecx
00443ad6  mov        eax, esi
00443ad8  mov        ecx, edx
00443ada  mov        edx, dword ptr [esi + 0x3f8]
00443ae0  add        ecx, 0x2c
00443ae3  call       0x454b80

// block 00443ae8
00443ae8  mov        edx, dword ptr [edi + 0x2d0]
00443aee  push       edx
00443aef  mov        edx, ebx
00443af1  call       0x461920

// block 00443af6
00443af6  cmp        eax, ebp
00443af8  jne        0x443b00

// block 00443afa
00443afa  mov        dword ptr [edi + 0x2d0], ebp

// block 00443b00
00443b00  add        eax, 0x10
00443b03  cmp        eax, ebp
00443b05  je         0x443b26

// block 00443b07
00443b07  mov        ecx, 0x3d
00443b0c  lea        esp, [esp]

// block 00443b10
00443b10  mov        edx, dword ptr [eax]
00443b12  cmp        word ptr [edx + 0x3ea], cx
00443b19  je         0x444213

// block 00443b1f
00443b1f  mov        eax, dword ptr [eax + 4]
00443b22  cmp        eax, ebp
00443b24  jne        0x443b10

// block 00443b26
00443b26  xor        eax, eax

// block 00443b28
00443b28  push       eax
00443b29  mov        edx, ebx
00443b2b  call       0x461920

// block 00443b30
00443b30  mov        esi, eax
00443b32  cmp        esi, ebp
00443b34  je         0x443b5d

// block 00443b36
00443b36  movsx      ecx, word ptr [edi + 0x5a6a]
00443b3d  mov        eax, 0x66666667
00443b42  imul       ecx
00443b44  sar        edx, 2
00443b47  mov        ecx, edx
00443b49  shr        ecx, 0x1f
00443b4c  lea        ecx, [edx + ecx + 0x2c]
00443b50  mov        edx, dword ptr [esi + 0x3f8]
00443b56  mov        eax, esi
00443b58  call       0x454b80

// block 00443b5d
00443b5d  mov        edx, dword ptr [edi + 0x2d0]
00443b63  push       edx
00443b64  mov        edx, ebx
00443b66  call       0x461920

// block 00443b6b
00443b6b  cmp        eax, ebp
00443b6d  jne        0x443b75

// block 00443b6f
00443b6f  mov        dword ptr [edi + 0x2d0], ebp

// block 00443b75
00443b75  add        eax, 0x10
00443b78  cmp        eax, ebp
00443b7a  je         0x443b97

// block 00443b7c
00443b7c  mov        ecx, 0x3e

// block 00443b81
00443b81  mov        edx, dword ptr [eax]
00443b83  cmp        word ptr [edx + 0x3ea], cx
00443b8a  je         0x44421c

// block 00443b90
00443b90  mov        eax, dword ptr [eax + 4]
00443b93  cmp        eax, ebp
00443b95  jne        0x443b81

// block 00443b97
00443b97  xor        eax, eax

// block 00443b99
00443b99  push       eax
00443b9a  mov        edx, ebx
00443b9c  call       0x461920

// block 00443ba1
00443ba1  mov        esi, eax
00443ba3  cmp        esi, ebp
00443ba5  je         0x443bc8

// block 00443ba7
00443ba7  movsx      eax, word ptr [edi + 0x5a6a]
00443bae  cdq        
00443baf  mov        ecx, 0xa
00443bb4  idiv       ecx
00443bb6  mov        eax, esi
00443bb8  mov        ecx, edx
00443bba  mov        edx, dword ptr [esi + 0x3f8]
00443bc0  add        ecx, 0x2c
00443bc3  call       0x454b80

// block 00443bc8
00443bc8  mov        edx, dword ptr [edi + 0x2d0]
00443bce  push       edx
00443bcf  mov        edx, ebx
00443bd1  call       0x461920

// block 00443bd6
00443bd6  cmp        eax, ebp
00443bd8  jne        0x443be0

// block 00443bda
00443bda  mov        dword ptr [edi + 0x2d0], ebp

// block 00443be0
00443be0  add        eax, 0x10
00443be3  cmp        eax, ebp
00443be5  je         0x443c06

// block 00443be7
00443be7  mov        ecx, 0x47
00443bec  lea        esp, [esp]

// block 00443bf0
00443bf0  mov        edx, dword ptr [eax]
00443bf2  cmp        word ptr [edx + 0x3ea], cx
00443bf9  je         0x444225

// block 00443bff
00443bff  mov        eax, dword ptr [eax + 4]
00443c02  cmp        eax, ebp
00443c04  jne        0x443bf0

// block 00443c06
00443c06  xor        eax, eax

// block 00443c08
00443c08  push       eax
00443c09  mov        edx, ebx
00443c0b  call       0x461920

// block 00443c10
00443c10  mov        esi, eax
00443c12  cmp        esi, ebp
00443c14  je         0x443c3d

// block 00443c16
00443c16  movsx      ecx, word ptr [edi + 0x5a6a]
00443c1d  mov        eax, 0x66666667
00443c22  imul       ecx
00443c24  sar        edx, 2
00443c27  mov        ecx, edx
00443c29  shr        ecx, 0x1f
00443c2c  lea        ecx, [edx + ecx + 0x2c]
00443c30  mov        edx, dword ptr [esi + 0x3f8]
00443c36  mov        eax, esi
00443c38  call       0x454b80

// block 00443c3d
00443c3d  mov        edx, dword ptr [edi + 0x2d0]
00443c43  push       edx
00443c44  mov        edx, ebx
00443c46  call       0x461920

// block 00443c4b
00443c4b  cmp        eax, ebp
00443c4d  jne        0x443c55

// block 00443c4f
00443c4f  mov        dword ptr [edi + 0x2d0], ebp

// block 00443c55
00443c55  add        eax, 0x10
00443c58  cmp        eax, ebp
00443c5a  je         0x443c77

// block 00443c5c
00443c5c  mov        ecx, 0x48

// block 00443c61
00443c61  mov        edx, dword ptr [eax]
00443c63  cmp        word ptr [edx + 0x3ea], cx
00443c6a  je         0x44422e

// block 00443c70
00443c70  mov        eax, dword ptr [eax + 4]
00443c73  cmp        eax, ebp
00443c75  jne        0x443c61

// block 00443c77
00443c77  xor        eax, eax

// block 00443c79
00443c79  push       eax
00443c7a  mov        edx, ebx
00443c7c  call       0x461920

// block 00443c81
00443c81  mov        esi, eax
00443c83  cmp        esi, ebp
00443c85  je         0x443ca8

// block 00443c87
00443c87  movsx      eax, word ptr [edi + 0x5a6a]
00443c8e  cdq        
00443c8f  mov        ecx, 0xa
00443c94  idiv       ecx
00443c96  mov        eax, esi
00443c98  mov        ecx, edx
00443c9a  mov        edx, dword ptr [esi + 0x3f8]
00443ca0  add        ecx, 0x2c
00443ca3  call       0x454b80

// block 00443ca8
00443ca8  mov        edx, dword ptr [edi + 0x2d0]
00443cae  push       edx
00443caf  mov        edx, ebx
00443cb1  call       0x461920

// block 00443cb6
00443cb6  cmp        eax, ebp
00443cb8  jne        0x443cc0

// block 00443cba
00443cba  mov        dword ptr [edi + 0x2d0], ebp

// block 00443cc0
00443cc0  add        eax, 0x10
00443cc3  cmp        eax, ebp
00443cc5  je         0x443ce6

// block 00443cc7
00443cc7  mov        ecx, 0x3f
00443ccc  lea        esp, [esp]

// block 00443cd0
00443cd0  mov        edx, dword ptr [eax]
00443cd2  cmp        word ptr [edx + 0x3ea], cx
00443cd9  je         0x444237

// block 00443cdf
00443cdf  mov        eax, dword ptr [eax + 4]
00443ce2  cmp        eax, ebp
00443ce4  jne        0x443cd0

// block 00443ce6
00443ce6  xor        eax, eax

// block 00443ce8
00443ce8  push       eax
00443ce9  mov        edx, ebx
00443ceb  call       0x461920

// block 00443cf0
00443cf0  mov        esi, eax
00443cf2  cmp        esi, ebp
00443cf4  je         0x443d1d

// block 00443cf6
00443cf6  movsx      ecx, word ptr [edi + 0x5a6c]
00443cfd  mov        eax, 0x66666667
00443d02  imul       ecx
00443d04  sar        edx, 2
00443d07  mov        ecx, edx
00443d09  shr        ecx, 0x1f
00443d0c  lea        ecx, [edx + ecx + 0x2c]
00443d10  mov        edx, dword ptr [esi + 0x3f8]
00443d16  mov        eax, esi
00443d18  call       0x454b80

// block 00443d1d
00443d1d  mov        edx, dword ptr [edi + 0x2d0]
00443d23  push       edx
00443d24  mov        edx, ebx
00443d26  call       0x461920

// block 00443d2b
00443d2b  cmp        eax, ebp
00443d2d  jne        0x443d35

// block 00443d2f
00443d2f  mov        dword ptr [edi + 0x2d0], ebp

// block 00443d35
00443d35  add        eax, 0x10
00443d38  cmp        eax, ebp
00443d3a  je         0x443d57

// block 00443d3c
00443d3c  mov        ecx, 0x40

// block 00443d41
00443d41  mov        edx, dword ptr [eax]
00443d43  cmp        word ptr [edx + 0x3ea], cx
00443d4a  je         0x444240

// block 00443d50
00443d50  mov        eax, dword ptr [eax + 4]
00443d53  cmp        eax, ebp
00443d55  jne        0x443d41

// block 00443d57
00443d57  xor        eax, eax

// block 00443d59
00443d59  push       eax
00443d5a  mov        edx, ebx
00443d5c  call       0x461920

// block 00443d61
00443d61  mov        esi, eax
00443d63  cmp        esi, ebp
00443d65  je         0x443d88

// block 00443d67
00443d67  movsx      eax, word ptr [edi + 0x5a6c]
00443d6e  cdq        
00443d6f  mov        ecx, 0xa
00443d74  idiv       ecx
00443d76  mov        eax, esi
00443d78  mov        ecx, edx
00443d7a  mov        edx, dword ptr [esi + 0x3f8]
00443d80  add        ecx, 0x2c
00443d83  call       0x454b80

// block 00443d88
00443d88  mov        edx, dword ptr [edi + 0x2d0]
00443d8e  push       edx
00443d8f  mov        edx, ebx
00443d91  call       0x461920

// block 00443d96
00443d96  cmp        eax, ebp
00443d98  jne        0x443da0

// block 00443d9a
00443d9a  mov        dword ptr [edi + 0x2d0], ebp

// block 00443da0
00443da0  add        eax, 0x10
00443da3  cmp        eax, ebp
00443da5  je         0x443dc6

// block 00443da7
00443da7  mov        ecx, 0x49
00443dac  lea        esp, [esp]

// block 00443db0
00443db0  mov        edx, dword ptr [eax]
00443db2  cmp        word ptr [edx + 0x3ea], cx
00443db9  je         0x444249

// block 00443dbf
00443dbf  mov        eax, dword ptr [eax + 4]
00443dc2  cmp        eax, ebp
00443dc4  jne        0x443db0

// block 00443dc6
00443dc6  xor        eax, eax

// block 00443dc8
00443dc8  push       eax
00443dc9  mov        edx, ebx
00443dcb  call       0x461920

// block 00443dd0
00443dd0  mov        esi, eax
00443dd2  cmp        esi, ebp
00443dd4  je         0x443dfd

// block 00443dd6
00443dd6  movsx      ecx, word ptr [edi + 0x5a6c]
00443ddd  mov        eax, 0x66666667
00443de2  imul       ecx
00443de4  sar        edx, 2
00443de7  mov        ecx, edx
00443de9  shr        ecx, 0x1f
00443dec  lea        ecx, [edx + ecx + 0x2c]
00443df0  mov        edx, dword ptr [esi + 0x3f8]
00443df6  mov        eax, esi
00443df8  call       0x454b80

// block 00443dfd
00443dfd  mov        edx, dword ptr [edi + 0x2d0]
00443e03  push       edx
00443e04  mov        edx, ebx
00443e06  call       0x461920

// block 00443e0b
00443e0b  cmp        eax, ebp
00443e0d  jne        0x443e15

// block 00443e0f
00443e0f  mov        dword ptr [edi + 0x2d0], ebp

// block 00443e15
00443e15  add        eax, 0x10
00443e18  cmp        eax, ebp
00443e1a  je         0x443e37

// block 00443e1c
00443e1c  mov        ecx, 0x4a

// block 00443e21
00443e21  mov        edx, dword ptr [eax]
00443e23  cmp        word ptr [edx + 0x3ea], cx
00443e2a  je         0x444252

// block 00443e30
00443e30  mov        eax, dword ptr [eax + 4]
00443e33  cmp        eax, ebp
00443e35  jne        0x443e21

// block 00443e37
00443e37  xor        eax, eax

// block 00443e39
00443e39  push       eax
00443e3a  mov        edx, ebx
00443e3c  call       0x461920

// block 00443e41
00443e41  mov        esi, eax
00443e43  cmp        esi, ebp
00443e45  je         0x443e68

// block 00443e47
00443e47  movsx      eax, word ptr [edi + 0x5a6c]
00443e4e  cdq        
00443e4f  mov        ecx, 0xa
00443e54  idiv       ecx
00443e56  mov        eax, esi
00443e58  mov        ecx, edx
00443e5a  mov        edx, dword ptr [esi + 0x3f8]
00443e60  add        ecx, 0x2c
00443e63  call       0x454b80

// block 00443e68
00443e68  mov        edx, dword ptr [edi + 0x2d0]
00443e6e  push       edx
00443e6f  mov        edx, ebx
00443e71  call       0x461920

// block 00443e76
00443e76  cmp        eax, ebp
00443e78  jne        0x443e80

// block 00443e7a
00443e7a  mov        dword ptr [edi + 0x2d0], ebp

// block 00443e80
00443e80  add        eax, 0x10
00443e83  cmp        eax, ebp
00443e85  je         0x443ea6

// block 00443e87
00443e87  mov        ecx, 0x41
00443e8c  lea        esp, [esp]

// block 00443e90
00443e90  mov        edx, dword ptr [eax]
00443e92  cmp        word ptr [edx + 0x3ea], cx
00443e99  je         0x44425b

// block 00443e9f
00443e9f  mov        eax, dword ptr [eax + 4]
00443ea2  cmp        eax, ebp
00443ea4  jne        0x443e90

// block 00443ea6
00443ea6  xor        eax, eax

// block 00443ea8
00443ea8  push       eax
00443ea9  mov        edx, ebx
00443eab  call       0x461920

// block 00443eb0
00443eb0  mov        esi, eax
00443eb2  cmp        esi, ebp
00443eb4  je         0x443edd

// block 00443eb6
00443eb6  movsx      ecx, word ptr [edi + 0x5a6e]
00443ebd  mov        eax, 0x66666667
00443ec2  imul       ecx
00443ec4  sar        edx, 2
00443ec7  mov        ecx, edx
00443ec9  shr        ecx, 0x1f
00443ecc  lea        ecx, [edx + ecx + 0x2c]
00443ed0  mov        edx, dword ptr [esi + 0x3f8]
00443ed6  mov        eax, esi
00443ed8  call       0x454b80

// block 00443edd
00443edd  mov        edx, dword ptr [edi + 0x2d0]
00443ee3  push       edx
00443ee4  mov        edx, ebx
00443ee6  call       0x461920

// block 00443eeb
00443eeb  cmp        eax, ebp
00443eed  jne        0x443ef5

// block 00443eef
00443eef  mov        dword ptr [edi + 0x2d0], ebp

// block 00443ef5
00443ef5  add        eax, 0x10
00443ef8  cmp        eax, ebp
00443efa  je         0x443f17

// block 00443efc
00443efc  mov        ecx, 0x42

// block 00443f01
00443f01  mov        edx, dword ptr [eax]
00443f03  cmp        word ptr [edx + 0x3ea], cx
00443f0a  je         0x444264

// block 00443f10
00443f10  mov        eax, dword ptr [eax + 4]
00443f13  cmp        eax, ebp
00443f15  jne        0x443f01

// block 00443f17
00443f17  xor        eax, eax

// block 00443f19
00443f19  push       eax
00443f1a  mov        edx, ebx
00443f1c  call       0x461920

// block 00443f21
00443f21  mov        esi, eax
00443f23  cmp        esi, ebp
00443f25  je         0x443f48

// block 00443f27
00443f27  movsx      eax, word ptr [edi + 0x5a6e]
00443f2e  cdq        
00443f2f  mov        ecx, 0xa
00443f34  idiv       ecx
00443f36  mov        eax, esi
00443f38  mov        ecx, edx
00443f3a  mov        edx, dword ptr [esi + 0x3f8]
00443f40  add        ecx, 0x2c
00443f43  call       0x454b80

// block 00443f48
00443f48  mov        edx, dword ptr [edi + 0x2d0]
00443f4e  push       edx
00443f4f  mov        edx, ebx
00443f51  call       0x461920

// block 00443f56
00443f56  cmp        eax, ebp
00443f58  jne        0x443f60

// block 00443f5a
00443f5a  mov        dword ptr [edi + 0x2d0], ebp

// block 00443f60
00443f60  add        eax, 0x10
00443f63  cmp        eax, ebp
00443f65  je         0x443f86

// block 00443f67
00443f67  mov        ecx, 0x4b
00443f6c  lea        esp, [esp]

// block 00443f70
00443f70  mov        edx, dword ptr [eax]
00443f72  cmp        word ptr [edx + 0x3ea], cx
00443f79  je         0x44426d

// block 00443f7f
00443f7f  mov        eax, dword ptr [eax + 4]
00443f82  cmp        eax, ebp
00443f84  jne        0x443f70

// block 00443f86
00443f86  xor        eax, eax

// block 00443f88
00443f88  push       eax
00443f89  mov        edx, ebx
00443f8b  call       0x461920

// block 00443f90
00443f90  mov        esi, eax
00443f92  cmp        esi, ebp
00443f94  je         0x443fbd

// block 00443f96
00443f96  movsx      ecx, word ptr [edi + 0x5a6e]
00443f9d  mov        eax, 0x66666667
00443fa2  imul       ecx
00443fa4  sar        edx, 2
00443fa7  mov        ecx, edx
00443fa9  shr        ecx, 0x1f
00443fac  lea        ecx, [edx + ecx + 0x2c]
00443fb0  mov        edx, dword ptr [esi + 0x3f8]
00443fb6  mov        eax, esi
00443fb8  call       0x454b80

// block 00443fbd
00443fbd  mov        edx, dword ptr [edi + 0x2d0]
00443fc3  push       edx
00443fc4  mov        edx, ebx
00443fc6  call       0x461920

// block 00443fcb
00443fcb  cmp        eax, ebp
00443fcd  jne        0x443fd5

// block 00443fcf
00443fcf  mov        dword ptr [edi + 0x2d0], ebp

// block 00443fd5
00443fd5  add        eax, 0x10
00443fd8  cmp        eax, ebp
00443fda  je         0x443ff7

// block 00443fdc
00443fdc  mov        ecx, 0x4c

// block 00443fe1
00443fe1  mov        edx, dword ptr [eax]
00443fe3  cmp        word ptr [edx + 0x3ea], cx
00443fea  je         0x444276

// block 00443ff0
00443ff0  mov        eax, dword ptr [eax + 4]
00443ff3  cmp        eax, ebp
00443ff5  jne        0x443fe1

// block 00443ff7
00443ff7  xor        eax, eax

// block 00443ff9
00443ff9  push       eax
00443ffa  mov        edx, ebx
00443ffc  call       0x461920

// block 00444001
00444001  mov        esi, eax
00444003  cmp        esi, ebp
00444005  je         0x444028

// block 00444007
00444007  movsx      eax, word ptr [edi + 0x5a6e]
0044400e  cdq        
0044400f  mov        ecx, 0xa
00444014  idiv       ecx
00444016  mov        eax, esi
00444018  mov        ecx, edx
0044401a  mov        edx, dword ptr [esi + 0x3f8]
00444020  add        ecx, 0x2c
00444023  call       0x454b80

// block 00444028
00444028  mov        edx, dword ptr [edi + 0x2d0]
0044402e  push       edx
0044402f  mov        edx, ebx
00444031  call       0x461920

// block 00444036
00444036  cmp        eax, ebp
00444038  jne        0x444040

// block 0044403a
0044403a  mov        dword ptr [edi + 0x2d0], ebp

// block 00444040
00444040  add        eax, 0x10
00444043  cmp        eax, ebp
00444045  je         0x444066

// block 00444047
00444047  mov        ecx, 0x43
0044404c  lea        esp, [esp]

// block 00444050
00444050  mov        edx, dword ptr [eax]
00444052  cmp        word ptr [edx + 0x3ea], cx
00444059  je         0x44427f

// block 0044405f
0044405f  mov        eax, dword ptr [eax + 4]
00444062  cmp        eax, ebp
00444064  jne        0x444050

// block 00444066
00444066  xor        eax, eax

// block 00444068
00444068  push       eax
00444069  mov        edx, ebx
0044406b  call       0x461920

// block 00444070
00444070  mov        esi, eax
00444072  cmp        esi, ebp
00444074  je         0x44409d

// block 00444076
00444076  movsx      ecx, word ptr [edi + 0x5a70]
0044407d  mov        eax, 0x66666667
00444082  imul       ecx
00444084  sar        edx, 2
00444087  mov        ecx, edx
00444089  shr        ecx, 0x1f
0044408c  lea        ecx, [edx + ecx + 0x2c]
00444090  mov        edx, dword ptr [esi + 0x3f8]
00444096  mov        eax, esi
00444098  call       0x454b80

// block 0044409d
0044409d  mov        edx, dword ptr [edi + 0x2d0]
004440a3  push       edx
004440a4  mov        edx, ebx
004440a6  call       0x461920

// block 004440ab
004440ab  cmp        eax, ebp
004440ad  jne        0x4440b5

// block 004440af
004440af  mov        dword ptr [edi + 0x2d0], ebp

// block 004440b5
004440b5  add        eax, 0x10
004440b8  cmp        eax, ebp
004440ba  je         0x4440d7

// block 004440bc
004440bc  mov        ecx, 0x44

// block 004440c1
004440c1  mov        edx, dword ptr [eax]
004440c3  cmp        word ptr [edx + 0x3ea], cx
004440ca  je         0x444288

// block 004440d0
004440d0  mov        eax, dword ptr [eax + 4]
004440d3  cmp        eax, ebp
004440d5  jne        0x4440c1

// block 004440d7
004440d7  xor        eax, eax

// block 004440d9
004440d9  push       eax
004440da  mov        edx, ebx
004440dc  call       0x461920

// block 004440e1
004440e1  mov        esi, eax
004440e3  cmp        esi, ebp
004440e5  je         0x444108

// block 004440e7
004440e7  movsx      eax, word ptr [edi + 0x5a70]
004440ee  cdq        
004440ef  mov        ecx, 0xa
004440f4  idiv       ecx
004440f6  mov        eax, esi
004440f8  mov        ecx, edx
004440fa  mov        edx, dword ptr [esi + 0x3f8]
00444100  add        ecx, 0x2c
00444103  call       0x454b80

// block 00444108
00444108  mov        edx, dword ptr [edi + 0x2d0]
0044410e  push       edx
0044410f  mov        edx, ebx
00444111  call       0x461920

// block 00444116
00444116  cmp        eax, ebp
00444118  jne        0x444120

// block 0044411a
0044411a  mov        dword ptr [edi + 0x2d0], ebp

// block 00444120
00444120  add        eax, 0x10
00444123  cmp        eax, ebp
00444125  je         0x444146

// block 00444127
00444127  mov        ecx, 0x4d
0044412c  lea        esp, [esp]

// block 00444130
00444130  mov        edx, dword ptr [eax]
00444132  cmp        word ptr [edx + 0x3ea], cx
00444139  je         0x444291

// block 0044413f
0044413f  mov        eax, dword ptr [eax + 4]
00444142  cmp        eax, ebp
00444144  jne        0x444130

// block 00444146
00444146  xor        eax, eax

// block 00444148
00444148  push       eax
00444149  mov        edx, ebx
0044414b  call       0x461920

// block 00444150
00444150  mov        esi, eax
00444152  cmp        esi, ebp
00444154  je         0x44417d

// block 00444156
00444156  movsx      ecx, word ptr [edi + 0x5a70]
0044415d  mov        eax, 0x66666667
00444162  imul       ecx
00444164  sar        edx, 2
00444167  mov        ecx, edx
00444169  shr        ecx, 0x1f
0044416c  lea        ecx, [edx + ecx + 0x2c]
00444170  mov        edx, dword ptr [esi + 0x3f8]
00444176  mov        eax, esi
00444178  call       0x454b80

// block 0044417d
0044417d  mov        edx, dword ptr [edi + 0x2d0]
00444183  push       edx
00444184  mov        edx, ebx
00444186  call       0x461920

// block 0044418b
0044418b  cmp        eax, ebp
0044418d  jne        0x444195

// block 0044418f
0044418f  mov        dword ptr [edi + 0x2d0], ebp

// block 00444195
00444195  add        eax, 0x10
00444198  cmp        eax, ebp
0044419a  je         0x4441b7

// block 0044419c
0044419c  mov        ecx, 0x4e

// block 004441a1
004441a1  mov        edx, dword ptr [eax]
004441a3  cmp        word ptr [edx + 0x3ea], cx
004441aa  je         0x44429a

// block 004441b0
004441b0  mov        eax, dword ptr [eax + 4]
004441b3  cmp        eax, ebp
004441b5  jne        0x4441a1

// block 004441b7
004441b7  xor        eax, eax

// block 004441b9
004441b9  push       eax
004441ba  mov        edx, ebx
004441bc  call       0x461920

// block 004441c1
004441c1  mov        esi, eax
004441c3  cmp        esi, ebp
004441c5  je         0x4442a3

// block 004441cb
004441cb  movsx      eax, word ptr [edi + 0x5a70]
004441d2  cdq        
004441d3  mov        ecx, 0xa
004441d8  idiv       ecx
004441da  mov        eax, esi
004441dc  mov        ecx, edx
004441de  mov        edx, dword ptr [esi + 0x3f8]
004441e4  pop        esi
004441e5  pop        ebp
004441e6  add        ecx, 0x2c
004441e9  pop        ebx
004441ea  jmp        0x454b80

// block 004441ef
004441ef  mov        eax, edx
004441f1  mov        eax, dword ptr [eax]
004441f3  jmp        0x443968

// block 004441f8
004441f8  mov        eax, edx
004441fa  mov        eax, dword ptr [eax]
004441fc  jmp        0x4439d9

// block 00444201
00444201  mov        eax, edx
00444203  mov        eax, dword ptr [eax]
00444205  jmp        0x443a48

// block 0044420a
0044420a  mov        eax, edx
0044420c  mov        eax, dword ptr [eax]
0044420e  jmp        0x443ab9

// block 00444213
00444213  mov        eax, edx
00444215  mov        eax, dword ptr [eax]
00444217  jmp        0x443b28

// block 0044421c
0044421c  mov        eax, edx
0044421e  mov        eax, dword ptr [eax]
00444220  jmp        0x443b99

// block 00444225
00444225  mov        eax, edx
00444227  mov        eax, dword ptr [eax]
00444229  jmp        0x443c08

// block 0044422e
0044422e  mov        eax, edx
00444230  mov        eax, dword ptr [eax]
00444232  jmp        0x443c79

// block 00444237
00444237  mov        eax, edx
00444239  mov        eax, dword ptr [eax]
0044423b  jmp        0x443ce8

// block 00444240
00444240  mov        eax, edx
00444242  mov        eax, dword ptr [eax]
00444244  jmp        0x443d59

// block 00444249
00444249  mov        eax, edx
0044424b  mov        eax, dword ptr [eax]
0044424d  jmp        0x443dc8

// block 00444252
00444252  mov        eax, edx
00444254  mov        eax, dword ptr [eax]
00444256  jmp        0x443e39

// block 0044425b
0044425b  mov        eax, edx
0044425d  mov        eax, dword ptr [eax]
0044425f  jmp        0x443ea8

// block 00444264
00444264  mov        eax, edx
00444266  mov        eax, dword ptr [eax]
00444268  jmp        0x443f19

// block 0044426d
0044426d  mov        eax, edx
0044426f  mov        eax, dword ptr [eax]
00444271  jmp        0x443f88

// block 00444276
00444276  mov        eax, edx
00444278  mov        eax, dword ptr [eax]
0044427a  jmp        0x443ff9

// block 0044427f
0044427f  mov        eax, edx
00444281  mov        eax, dword ptr [eax]
00444283  jmp        0x444068

// block 00444288
00444288  mov        eax, edx
0044428a  mov        eax, dword ptr [eax]
0044428c  jmp        0x4440d9

// block 00444291
00444291  mov        eax, edx
00444293  mov        eax, dword ptr [eax]
00444295  jmp        0x444148

// block 0044429a
0044429a  mov        eax, edx
0044429c  mov        eax, dword ptr [eax]
0044429e  jmp        0x4441b9

// block 004442a3
004442a3  pop        esi
004442a4  pop        ebp
004442a5  pop        ebx
004442a6  ret        
