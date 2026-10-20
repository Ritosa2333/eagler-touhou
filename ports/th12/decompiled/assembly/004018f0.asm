
// block 004018f0
004018f0  sub        esp, 0xc
004018f3  push       ebx
004018f4  push       ebp
004018f5  mov        ebp, dword ptr [esp + 0x18]
004018f9  push       esi
004018fa  push       edi
004018fb  mov        edi, dword ptr [esp + 0x24]
004018ff  mov        eax, edi
00401901  lea        edx, [eax + 1]

// block 00401904
00401904  mov        cl, byte ptr [eax]
00401906  inc        eax
00401907  test       cl, cl
00401909  jne        0x401904

// block 0040190b
0040190b  sub        eax, edx
0040190d  mov        dword ptr [esp + 0x14], eax
00401911  mov        eax, dword ptr [ebp + 0x490]
00401917  and        eax, 0xffafffff
0040191c  or         eax, 0x280001
00401921  mov        dword ptr [ebp + 0x490], eax
00401927  mov        ecx, dword ptr [edi + 0x100]
0040192d  mov        dword ptr [ebp + 0x438], ecx
00401933  mov        edx, dword ptr [edi + 0x104]
00401939  mov        dword ptr [ebp + 0x43c], edx
0040193f  mov        eax, dword ptr [edi + 0x108]
00401945  mov        dword ptr [ebp + 0x440], eax
0040194b  fld        dword ptr [edi + 0x114]
00401951  fstp       dword ptr [esp + 0x20]
00401955  lea        esi, [ebp + 0x14]
00401958  fld        dword ptr [edi + 0x110]
0040195e  or         dword ptr [esi + 0x47c], 8
00401965  fstp       dword ptr [esi + 0x40]
00401968  mov        ecx, 2
0040196d  fld        dword ptr [esp + 0x20]
00401971  fstp       dword ptr [esi + 0x44]
00401974  mov        eax, dword ptr [edi + 0x120]
0040197a  fld        qword ptr [0x4a4128]
00401980  dec        eax
00401981  fld        qword ptr [0x4a4120]
00401987  cmp        eax, 4
0040198a  ja         0x4019f4

// block 0040198c
0040198c  jmp        dword ptr [eax*4 + 0x401fec]

// block 00401993
00401993  and        dword ptr [ebp + 0x494], 0xfffffffd
0040199a  fld        dword ptr [edi + 0x110]
004019a0  fmul       st(1)
004019a2  fstp       dword ptr [esp + 0x20]
004019a6  fld        dword ptr [0x4a4118]
004019ac  jmp        0x401a28

// block 004019ae
004019ae  and        dword ptr [ebp + 0x494], 0xfffffffd

// block 004019b5
004019b5  fld        dword ptr [edi + 0x110]
004019bb  fmul       st(1)
004019bd  fstp       dword ptr [esp + 0x20]
004019c1  fld        dword ptr [0x4a4114]
004019c7  jmp        0x401a28

// block 004019c9
004019c9  and        dword ptr [ebp + 0x494], 0xfffffffd

// block 004019d0
004019d0  fld        dword ptr [edi + 0x110]
004019d6  fmul       st(2)
004019d8  fstp       dword ptr [esp + 0x20]
004019dc  fld        dword ptr [0x4a3e68]
004019e2  jmp        0x401a28

// block 004019e4
004019e4  or         dword ptr [ebp + 0x494], ecx
004019ea  jmp        0x4019d0

// block 004019ec
004019ec  or         dword ptr [ebp + 0x494], ecx
004019f2  jmp        0x4019b5

// block 004019f4
004019f4  fld1       
004019f6  fcomp      dword ptr [edi + 0x110]
004019fc  fnstsw     ax
004019fe  test       ah, 0x44
00401a01  jnp        0x401a0b

// block 00401a03
00401a03  or         dword ptr [ebp + 0x494], ecx
00401a09  jmp        0x401a12

// block 00401a0b
00401a0b  and        dword ptr [ebp + 0x494], 0xfffffffd

// block 00401a12
00401a12  fild       dword ptr [ebp + 0x18fac]
00401a18  fmul       dword ptr [edi + 0x110]
00401a1e  fstp       dword ptr [esp + 0x20]
00401a22  fld        dword ptr [0x4a4110]

// block 00401a28
00401a28  mov        eax, dword ptr [edi + 0x130]
00401a2e  fstp       dword ptr [esp + 0x10]
00401a32  sub        eax, 0
00401a35  fld        dword ptr [esp + 0x20]
00401a39  fld        qword ptr [0x4a4108]
00401a3f  fld        qword ptr [0x4a3cf8]
00401a45  je         0x401aed

// block 00401a4b
00401a4b  sub        eax, ecx
00401a4d  jne        0x401b77

// block 00401a53
00401a53  mov        eax, dword ptr [edi + 0x120]
00401a59  add        eax, -2
00401a5c  cmp        eax, 3
00401a5f  ja         0x401ad2

// block 00401a61
00401a61  jmp        dword ptr [eax*4 + 0x402000]

// block 00401a68
00401a68  cmp        byte ptr [edi], 0
00401a6b  mov        eax, edi
00401a6d  je         0x401b77

// block 00401a73
00401a73  cmp        byte ptr [eax], 0x2e
00401a76  fld        dword ptr [ebp + 0x438]
00401a7c  jne        0x401a8a

// block 00401a7e
00401a7e  fld        dword ptr [edi + 0x110]
00401a84  fmul       st(3)
00401a86  fsubp      st(1)
00401a88  jmp        0x401a8c

// block 00401a8a
00401a8a  fsub       st(3)

// block 00401a8c
00401a8c  inc        eax
00401a8d  fstp       dword ptr [ebp + 0x438]
00401a93  cmp        byte ptr [eax], 0
00401a96  jne        0x401a73

// block 00401a98
00401a98  jmp        0x401b77

// block 00401a9d
00401a9d  cmp        byte ptr [edi], 0
00401aa0  mov        eax, edi
00401aa2  je         0x401b77

// block 00401aa8
00401aa8  cmp        byte ptr [eax], 0x2c
00401aab  fld        dword ptr [ebp + 0x438]
00401ab1  jne        0x401abf

// block 00401ab3
00401ab3  fld        dword ptr [edi + 0x110]
00401ab9  fmul       st(3)
00401abb  fsubp      st(1)
00401abd  jmp        0x401ac1

// block 00401abf
00401abf  fsub       st(3)

// block 00401ac1
00401ac1  inc        eax
00401ac2  fstp       dword ptr [ebp + 0x438]
00401ac8  cmp        byte ptr [eax], 0
00401acb  jne        0x401aa8

// block 00401acd
00401acd  jmp        0x401b77

// block 00401ad2
00401ad2  fstp       st(1)
00401ad4  fld        dword ptr [ebp + 0x438]
00401ada  fild       dword ptr [esp + 0x14]
00401ade  fmul       st(3)
00401ae0  fsubp      st(1)
00401ae2  fstp       dword ptr [ebp + 0x438]
00401ae8  jmp        0x401b79

// block 00401aed
00401aed  mov        eax, dword ptr [edi + 0x120]
00401af3  cmp        eax, ecx
00401af5  je         0x401b49

// block 00401af7
00401af7  add        eax, -3
00401afa  cmp        eax, 1
00401afd  ja         0x401b2f

// block 00401aff
00401aff  cmp        byte ptr [edi], 0
00401b02  mov        eax, edi
00401b04  je         0x401b77

// block 00401b06
00401b06  cmp        byte ptr [eax], 0x2c
00401b09  fld        dword ptr [ebp + 0x438]
00401b0f  jne        0x401b1b

// block 00401b11
00401b11  fld        dword ptr [edi + 0x110]
00401b17  fmul       st(3)
00401b19  jmp        0x401b1d

// block 00401b1b
00401b1b  fld        st(3)

// block 00401b1d
00401b1d  fmul       st(2)
00401b1f  inc        eax
00401b20  fsubp      st(1)
00401b22  fstp       dword ptr [ebp + 0x438]
00401b28  cmp        byte ptr [eax], 0
00401b2b  jne        0x401b06

// block 00401b2d
00401b2d  jmp        0x401b77

// block 00401b2f
00401b2f  fstp       st(1)
00401b31  fld        dword ptr [ebp + 0x438]
00401b37  fild       dword ptr [esp + 0x14]
00401b3b  fmul       st(3)
00401b3d  fmul       st(2)
00401b3f  fsubp      st(1)
00401b41  fstp       dword ptr [ebp + 0x438]
00401b47  jmp        0x401b79

// block 00401b49
00401b49  cmp        byte ptr [edi], 0
00401b4c  mov        eax, edi
00401b4e  je         0x401b77

// block 00401b50
00401b50  cmp        byte ptr [eax], 0x2e
00401b53  fld        dword ptr [ebp + 0x438]
00401b59  jne        0x401b65

// block 00401b5b
00401b5b  fld        dword ptr [edi + 0x110]
00401b61  fmul       st(3)
00401b63  jmp        0x401b67

// block 00401b65
00401b65  fld        st(3)

// block 00401b67
00401b67  fmul       st(2)
00401b69  inc        eax
00401b6a  fsubp      st(1)
00401b6c  fstp       dword ptr [ebp + 0x438]
00401b72  cmp        byte ptr [eax], 0
00401b75  jne        0x401b50

// block 00401b77
00401b77  fstp       st(1)

// block 00401b79
00401b79  mov        eax, dword ptr [edi + 0x134]
00401b7f  fld        dword ptr [esp + 0x10]
00401b83  sub        eax, 0
00401b86  je         0x401b98

// block 00401b88
00401b88  sub        eax, ecx
00401b8a  fstp       st(1)
00401b8c  jne        0x401bac

// block 00401b8e
00401b8e  fld        dword ptr [ebp + 0x43c]
00401b94  fsub       st(1)
00401b96  jmp        0x401ba6

// block 00401b98
00401b98  fld        dword ptr [ebp + 0x43c]
00401b9e  fld        st(1)
00401ba0  fmulp      st(3)
00401ba2  fsubrp     st(2)
00401ba4  fxch       st(1)

// block 00401ba6
00401ba6  fstp       dword ptr [ebp + 0x43c]

// block 00401bac
00401bac  cmp        byte ptr [edi], 0
00401baf  mov        ecx, edi
00401bb1  mov        dword ptr [esp + 0x14], edi
00401bb5  je         0x401fd9

// block 00401bbb
00401bbb  mov        al, byte ptr [ecx]
00401bbd  cmp        al, 0xa
00401bbf  jne        0x401be6

// block 00401bc1
00401bc1  fld        dword ptr [edi + 0x114]
00401bc7  fmul       st(1)
00401bc9  fadd       dword ptr [ebp + 0x43c]
00401bcf  fstp       dword ptr [ebp + 0x43c]
00401bd5  fld        dword ptr [edi + 0x100]
00401bdb  fstp       dword ptr [ebp + 0x438]
00401be1  jmp        0x401fcb

// block 00401be6
00401be6  cmp        al, 0x20
00401be8  jne        0x401bfd

// block 00401bea
00401bea  fld        dword ptr [ebp + 0x438]
00401bf0  fadd       st(2)
00401bf2  fstp       dword ptr [ebp + 0x438]
00401bf8  jmp        0x401fcb

// block 00401bfd
00401bfd  mov        edx, dword ptr [edi + 0x120]
00401c03  fstp       st(1)
00401c05  fstp       st(0)
00401c07  cmp        edx, 5
00401c0a  ja         0x401e9a

// block 00401c10
00401c10  jmp        dword ptr [edx*4 + 0x402010]

// block 00401c17
00401c17  mov        edx, dword ptr [ebp + 0x18fb4]
00401c1d  fstp       st(0)
00401c1f  movzx      eax, al
00401c22  fstp       st(0)
00401c24  sub        eax, 0x20
00401c27  lea        ecx, [eax + eax*8]
00401c2a  mov        eax, dword ptr [edx + 0x118]
00401c30  lea        ecx, [eax + ecx*8]
00401c33  mov        eax, esi
00401c35  call       0x402390

// block 00401c3a
00401c3a  jmp        0x401e9e

// block 00401c3f
00401c3f  mov        edx, dword ptr [ebp + 0x18fb4]
00401c45  fstp       st(0)
00401c47  movzx      eax, al
00401c4a  fstp       st(0)
00401c4c  add        eax, 0x42
00401c4f  lea        ecx, [eax + eax*8]
00401c52  mov        eax, dword ptr [edx + 0x118]
00401c58  lea        ecx, [eax + ecx*8]
00401c5b  mov        eax, esi
00401c5d  call       0x402390

// block 00401c62
00401c62  jmp        0x401e9e

// block 00401c67
00401c67  fstp       st(1)
00401c69  fmul       dword ptr [edi + 0x110]
00401c6f  fstp       dword ptr [esp + 0x20]
00401c73  cmp        al, 0x2f
00401c75  jne        0x401c95

// block 00401c77
00401c77  mov        ecx, dword ptr [ebp + 0x18fb4]
00401c7d  mov        ecx, dword ptr [ecx + 0x118]
00401c83  add        ecx, 0x42a8
00401c89  mov        eax, esi
00401c8b  call       0x402390

// block 00401c90
00401c90  jmp        0x401e9e

// block 00401c95
00401c95  cmp        al, 0x3a
00401c97  jne        0x401cb7

// block 00401c99
00401c99  mov        edx, dword ptr [ebp + 0x18fb4]
00401c9f  mov        ecx, dword ptr [edx + 0x118]
00401ca5  add        ecx, 0x42f0
00401cab  mov        eax, esi
00401cad  call       0x402390

// block 00401cb2
00401cb2  jmp        0x401e9e

// block 00401cb7
00401cb7  cmp        al, 0x2d
00401cb9  jne        0x401cd9

// block 00401cbb
00401cbb  mov        eax, dword ptr [ebp + 0x18fb4]
00401cc1  mov        ecx, dword ptr [eax + 0x118]
00401cc7  add        ecx, 0x4338
00401ccd  mov        eax, esi
00401ccf  call       0x402390

// block 00401cd4
00401cd4  jmp        0x401e9e

// block 00401cd9
00401cd9  cmp        al, 0x2a
00401cdb  jne        0x401cfb

// block 00401cdd
00401cdd  mov        ecx, dword ptr [ebp + 0x18fb4]
00401ce3  mov        ecx, dword ptr [ecx + 0x118]
00401ce9  add        ecx, 0x4380
00401cef  mov        eax, esi
00401cf1  call       0x402390

// block 00401cf6
00401cf6  jmp        0x401e9e

// block 00401cfb
00401cfb  cmp        al, 0x25
00401cfd  jne        0x401d1d

// block 00401cff
00401cff  mov        edx, dword ptr [ebp + 0x18fb4]
00401d05  mov        ecx, dword ptr [edx + 0x118]
00401d0b  add        ecx, 0x43c8
00401d11  mov        eax, esi
00401d13  call       0x402390

// block 00401d18
00401d18  jmp        0x401e9e

// block 00401d1d
00401d1d  cmp        al, 0x24
00401d1f  jne        0x401d3f

// block 00401d21
00401d21  mov        eax, dword ptr [ebp + 0x18fb4]
00401d27  mov        ecx, dword ptr [eax + 0x118]
00401d2d  add        ecx, 0x49b0
00401d33  mov        eax, esi
00401d35  call       0x402390

// block 00401d3a
00401d3a  jmp        0x401e9e

// block 00401d3f
00401d3f  cmp        al, 0x2e
00401d41  jne        0x401d71

// block 00401d43
00401d43  mov        ecx, dword ptr [ebp + 0x18fb4]
00401d49  mov        ecx, dword ptr [ecx + 0x118]
00401d4f  add        ecx, 0x4410
00401d55  mov        eax, esi
00401d57  call       0x402390

// block 00401d5c
00401d5c  fld        dword ptr [edi + 0x110]
00401d62  fmul       qword ptr [0x4a4108]
00401d68  fstp       dword ptr [esp + 0x20]
00401d6c  jmp        0x401e9e

// block 00401d71
00401d71  movzx      eax, al
00401d74  add        eax, 0xb3
00401d79  lea        edx, [eax + eax*8]
00401d7c  mov        eax, dword ptr [ebp + 0x18fb4]
00401d82  mov        ecx, dword ptr [eax + 0x118]
00401d88  lea        ecx, [ecx + edx*8]
00401d8b  mov        eax, esi
00401d8d  call       0x402390

// block 00401d92
00401d92  jmp        0x401e9e

// block 00401d97
00401d97  fstp       st(0)
00401d99  fmul       dword ptr [edi + 0x110]
00401d9f  fstp       dword ptr [esp + 0x20]
00401da3  fld        dword ptr [edi + 0x104]
00401da9  fstp       dword ptr [ebp + 0x43c]
00401daf  mov        al, byte ptr [ecx]
00401db1  cmp        al, 0x2f
00401db3  jne        0x401dd3

// block 00401db5
00401db5  mov        edx, dword ptr [ebp + 0x18fb4]
00401dbb  mov        ecx, dword ptr [edx + 0x118]
00401dc1  add        ecx, 0x4728
00401dc7  mov        eax, esi
00401dc9  call       0x402390

// block 00401dce
00401dce  jmp        0x401e9e

// block 00401dd3
00401dd3  cmp        al, 0x2e
00401dd5  jne        0x401df5

// block 00401dd7
00401dd7  mov        eax, dword ptr [ebp + 0x18fb4]
00401ddd  mov        ecx, dword ptr [eax + 0x118]
00401de3  add        ecx, 0x4770
00401de9  mov        eax, esi
00401deb  call       0x402390

// block 00401df0
00401df0  jmp        0x401e9e

// block 00401df5
00401df5  cmp        al, 0x73
00401df7  jne        0x401e17

// block 00401df9
00401df9  mov        ecx, dword ptr [ebp + 0x18fb4]
00401dff  mov        ecx, dword ptr [ecx + 0x118]
00401e05  add        ecx, 0x47b8
00401e0b  mov        eax, esi
00401e0d  call       0x402390

// block 00401e12
00401e12  jmp        0x401e9e

// block 00401e17
00401e17  cmp        al, 0x2a
00401e19  jne        0x401e36

// block 00401e1b
00401e1b  mov        edx, dword ptr [ebp + 0x18fb4]
00401e21  mov        ecx, dword ptr [edx + 0x118]
00401e27  add        ecx, 0x4800
00401e2d  mov        eax, esi
00401e2f  call       0x402390

// block 00401e34
00401e34  jmp        0x401e9e

// block 00401e36
00401e36  cmp        al, 0x2c
00401e38  jne        0x401e77

// block 00401e3a
00401e3a  mov        eax, dword ptr [ebp + 0x18fb4]
00401e40  mov        ecx, dword ptr [eax + 0x118]
00401e46  add        ecx, 0x4848
00401e4c  mov        eax, esi
00401e4e  call       0x402390

// block 00401e53
00401e53  fld        dword ptr [edi + 0x110]
00401e59  fmul       qword ptr [0x4a4108]
00401e5f  fstp       dword ptr [esp + 0x20]
00401e63  fld        dword ptr [edi + 0x104]
00401e69  fadd       qword ptr [0x4a3fd8]
00401e6f  fstp       dword ptr [ebp + 0x43c]
00401e75  jmp        0x401e9e

// block 00401e77
00401e77  mov        edx, dword ptr [ebp + 0x18fb4]
00401e7d  movzx      eax, al
00401e80  add        eax, 0xc3
00401e85  lea        ecx, [eax + eax*8]
00401e88  mov        eax, dword ptr [edx + 0x118]
00401e8e  lea        ecx, [eax + ecx*8]
00401e91  mov        eax, esi
00401e93  call       0x402390

// block 00401e98
00401e98  jmp        0x401e9e

// block 00401e9a
00401e9a  fstp       st(0)
00401e9c  fstp       st(0)

// block 00401e9e
00401e9e  mov        eax, dword ptr [ebp + 0x408]
00401ea4  fld        dword ptr [eax + 0x34]
00401ea7  fstp       dword ptr [esp + 0x18]
00401eab  fld        dword ptr [eax + 0x38]
00401eae  or         dword ptr [esi + 0x47c], 8
00401eb5  fstp       dword ptr [esi + 0x58]
00401eb8  fld        dword ptr [esp + 0x18]
00401ebc  fstp       dword ptr [esi + 0x5c]
00401ebf  cmp        dword ptr [edi + 0x124], 0
00401ec6  je         0x401f5d

// block 00401ecc
00401ecc  mov        eax, dword ptr [esp + 0x24]
00401ed0  fld        dword ptr [ebp + 0x438]
00401ed6  mov        ecx, dword ptr [eax + 0x10c]
00401edc  fld        qword ptr [0x4a3d00]
00401ee2  fadd       st(1), st(0)
00401ee4  and        ecx, 0xff000000
00401eea  mov        dword ptr [ebp + 0x3d0], ecx
00401ef0  fxch       st(1)
00401ef2  mov        edx, dword ptr [eax + 0x10c]
00401ef8  fstp       dword ptr [ebp + 0x438]
00401efe  push       0x4d4804
00401f03  shr        edx, 0x19
00401f06  fadd       dword ptr [ebp + 0x43c]
00401f0c  push       0x4d47e8
00401f11  mov        edi, 0x4d483c
00401f16  mov        ebx, 0x4d4820
00401f1b  fstp       dword ptr [ebp + 0x43c]
00401f21  mov        byte ptr [ebp + 0x3d3], dl
00401f27  call       0x45a570

// block 00401f2c
00401f2c  mov        ecx, dword ptr [0x4ce8cc]
00401f32  push       1
00401f34  mov        eax, esi
00401f36  call       0x459e50

// block 00401f3b
00401f3b  fld        dword ptr [ebp + 0x438]
00401f41  fld        qword ptr [0x4a3d00]
00401f47  fsub       st(1), st(0)
00401f49  fxch       st(1)
00401f4b  fstp       dword ptr [ebp + 0x438]
00401f51  fsubr      dword ptr [ebp + 0x43c]
00401f57  fstp       dword ptr [ebp + 0x43c]

// block 00401f5d
00401f5d  mov        eax, dword ptr [esp + 0x24]
00401f61  mov        ecx, dword ptr [eax + 0x10c]
00401f67  push       0x4d4804
00401f6c  push       0x4d47e8
00401f71  mov        edi, 0x4d483c
00401f76  mov        ebx, 0x4d4820
00401f7b  mov        dword ptr [ebp + 0x3d0], ecx
00401f81  call       0x45a570

// block 00401f86
00401f86  mov        ecx, dword ptr [0x4ce8cc]
00401f8c  push       1
00401f8e  mov        eax, esi
00401f90  call       0x459e50

// block 00401f95
00401f95  fld        dword ptr [ebp + 0x438]
00401f9b  fld        dword ptr [esp + 0x20]
00401f9f  mov        ecx, dword ptr [esp + 0x14]
00401fa3  mov        edi, dword ptr [esp + 0x24]
00401fa7  fld        st(0)
00401fa9  faddp      st(2)
00401fab  fxch       st(1)
00401fad  fstp       dword ptr [ebp + 0x438]
00401fb3  fld        qword ptr [0x4a4128]
00401fb9  fld        qword ptr [0x4a4120]
00401fbf  fld        dword ptr [esp + 0x10]
00401fc3  fxch       st(2)
00401fc5  fxch       st(3)
00401fc7  fxch       st(1)
00401fc9  fxch       st(2)

// block 00401fcb
00401fcb  inc        ecx
00401fcc  cmp        byte ptr [ecx], 0
00401fcf  mov        dword ptr [esp + 0x14], ecx
00401fd3  jne        0x401bbb

// block 00401fd9
00401fd9  fstp       st(1)
00401fdb  pop        edi
00401fdc  fstp       st(0)
00401fde  pop        esi
00401fdf  fstp       st(0)
00401fe1  pop        ebp
00401fe2  fstp       st(0)
00401fe4  pop        ebx
00401fe5  add        esp, 0xc
00401fe8  ret        8
