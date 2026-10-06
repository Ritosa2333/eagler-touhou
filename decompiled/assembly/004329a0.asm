
// block 004329a0
004329a0  sub        esp, 0x6c
004329a3  mov        eax, dword ptr [0x4ad138]
004329a8  xor        eax, esp
004329aa  mov        dword ptr [esp + 0x68], eax
004329ae  push       ebp
004329af  mov        ebp, dword ptr [esp + 0x74]
004329b3  mov        eax, dword ptr [ebp + 4]
004329b6  push       edi
004329b7  dec        eax
004329b8  mov        edi, 9
004329bd  cmp        eax, edi
004329bf  ja         0x4336b9

// block 004329c5
004329c5  push       ebx
004329c6  push       esi
004329c7  jmp        dword ptr [eax*4 + 0x4336cc]

// block 004329ce
004329ce  cmp        dword ptr [ebp + 0x14], 0xa
004329d2  jl         0x4336b7

// block 004329d8
004329d8  mov        ecx, dword ptr [0x4b44e8]
004329de  mov        dword ptr [ebp + 4], 3
004329e5  mov        dword ptr [ebp + 0x40], 4
004329ec  cmp        dword ptr [ecx + 0x74], 0
004329f0  mov        eax, 1
004329f5  je         0x432a0e

// block 004329f7
004329f7  mov        edx, dword ptr [ebp + 0x10c]
004329fd  mov        dword ptr [ebp + edx*4 + 0xc8], 2
00432a08  add        dword ptr [ebp + 0x10c], eax

// block 00432a0e
00432a0e  mov        dword ptr [ebp + 0x108], eax
00432a14  mov        eax, dword ptr [ebp + 0x40]
00432a17  test       eax, eax
00432a19  je         0x432a2a

// block 00432a1b
00432a1b  jg         0x432a23

// block 00432a1d
00432a1d  dec        eax
00432a1e  mov        dword ptr [ebp + 0x38], eax
00432a21  jmp        0x432a31

// block 00432a23
00432a23  xor        eax, eax
00432a25  mov        dword ptr [ebp + 0x38], eax
00432a28  jmp        0x432a31

// block 00432a2a
00432a2a  mov        dword ptr [ebp + 0x38], 0

// block 00432a31
00432a31  movzx      ebx, word ptr [ebp + 0x38]
00432a35  mov        eax, dword ptr [ebp + 0x1e8]
00432a3b  add        bx, 7
00432a3f  push       eax
00432a40  call       0x461970

// block 00432a45
00432a45  pop        esi
00432a46  pop        ebx
00432a47  pop        edi
00432a48  mov        dword ptr [ebp + 0x1fc], 0
00432a52  pop        ebp
00432a53  mov        ecx, dword ptr [esp + 0x68]
00432a57  xor        ecx, esp
00432a59  call       0x46c932

// block 00432a5e
00432a5e  add        esp, 0x6c
00432a61  ret        4

// block 00432a64
00432a64  cmp        dword ptr [ebp + 0x14], 0xa
00432a68  jl         0x4336b7

// block 00432a6e
00432a6e  mov        dword ptr [ebp + 4], 3
00432a75  mov        dword ptr [ebp + 0x40], 4
00432a7c  mov        ecx, dword ptr [ebp + 0x10c]
00432a82  mov        dword ptr [ebp + ecx*4 + 0xc8], 2
00432a8d  mov        esi, 1
00432a92  add        dword ptr [ebp + 0x10c], esi
00432a98  mov        edx, dword ptr [ebp + 0x10c]
00432a9e  mov        dword ptr [ebp + edx*4 + 0xc8], 0
00432aa9  add        dword ptr [ebp + 0x10c], esi
00432aaf  mov        dword ptr [ebp + 0x108], esi
00432ab5  mov        eax, dword ptr [ebp + 0x40]
00432ab8  test       eax, eax
00432aba  je         0x432acd

// block 00432abc
00432abc  cmp        eax, esi
00432abe  jg         0x432ac6

// block 00432ac0
00432ac0  dec        eax
00432ac1  mov        dword ptr [ebp + 0x38], eax
00432ac4  jmp        0x432ad0

// block 00432ac6
00432ac6  mov        eax, esi
00432ac8  mov        dword ptr [ebp + 0x38], eax
00432acb  jmp        0x432ad0

// block 00432acd
00432acd  mov        dword ptr [ebp + 0x38], esi

// block 00432ad0
00432ad0  movzx      ebx, word ptr [ebp + 0x38]
00432ad4  mov        eax, dword ptr [ebp + 0x1e8]
00432ada  add        bx, 7
00432ade  push       eax
00432adf  call       0x461970

// block 00432ae4
00432ae4  mov        dword ptr [ebp + 0x1fc], esi
00432aea  pop        esi
00432aeb  pop        ebx
00432aec  pop        edi
00432aed  pop        ebp
00432aee  mov        ecx, dword ptr [esp + 0x68]
00432af2  xor        ecx, esp
00432af4  call       0x46c932

// block 00432af9
00432af9  add        esp, 0x6c
00432afc  ret        4

// block 00432aff
00432aff  mov        ecx, dword ptr [ebp + 0x38]
00432b02  lea        esi, [ebp + 0x38]
00432b05  mov        dword ptr [esi + 4], ecx
00432b08  test       byte ptr [0x4d48c4], 0x10
00432b0f  jne        0x432b1a

// block 00432b11
00432b11  test       byte ptr [0x4d48c0], 0x10
00432b18  je         0x432b23

// block 00432b1a
00432b1a  push       -1
00432b1c  mov        eax, esi
00432b1e  call       0x464970

// block 00432b23
00432b23  test       byte ptr [0x4d48c4], 0x20
00432b2a  jne        0x432b35

// block 00432b2c
00432b2c  test       byte ptr [0x4d48c0], 0x20
00432b33  je         0x432b3e

// block 00432b35
00432b35  push       1
00432b37  mov        eax, esi
00432b39  call       0x464970

// block 00432b3e
00432b3e  mov        edx, dword ptr [esi + 4]
00432b41  cmp        edx, dword ptr [esi]
00432b43  je         0x432b62

// block 00432b45
00432b45  movzx      ebx, word ptr [esi]
00432b48  mov        eax, dword ptr [ebp + 0x1e8]
00432b4e  add        bx, 7
00432b52  push       eax
00432b53  call       0x461970

// block 00432b58
00432b58  mov        edx, 0xa
00432b5d  call       0x453d90

// block 00432b62
00432b62  test       dword ptr [0x4d48c4], 0x80001
00432b6c  je         0x432c56

// block 00432b72
00432b72  mov        edx, 7
00432b77  call       0x453d90

// block 00432b7c
00432b7c  mov        esi, dword ptr [esi]
00432b7e  cmp        esi, 3
00432b81  ja         0x432c4c

// block 00432b87
00432b87  jmp        dword ptr [esi*4 + 0x4336f4]

// block 00432b8e
00432b8e  mov        ecx, dword ptr [ebp + 0x1ec]
00432b94  push       ecx
00432b95  mov        ebx, 1
00432b9a  call       0x461970

// block 00432b9f
00432b9f  mov        edx, dword ptr [ebp + 0x1e8]
00432ba5  push       edx
00432ba6  call       0x461970

// block 00432bab
00432bab  mov        dword ptr [ebp + 4], 4
00432bb2  jmp        0x432c4c

// block 00432bb7
00432bb7  lea        esi, [ebp + 0x1e8]
00432bbd  mov        edi, 0x5b
00432bc2  lea        ebx, [esp + 0x34]
00432bc6  call       0x462060

// block 00432bcb
00432bcb  mov        eax, dword ptr [eax]
00432bcd  push       eax
00432bce  lea        ebx, [edi - 0x55]
00432bd1  call       0x461970

// block 00432bd6
00432bd6  mov        ecx, dword ptr [0x4b44e8]
00432bdc  mov        edx, dword ptr [ecx + 0x74]
00432bdf  neg        edx
00432be1  sbb        edx, edx
00432be3  add        edx, 5
00432be6  mov        dword ptr [ebp + 4], edx
00432be9  jmp        0x432c4c

// block 00432beb
00432beb  lea        esi, [ebp + 0x1e8]
00432bf1  mov        edi, 0x5c
00432bf6  lea        ebx, [esp + 0x2c]
00432bfa  call       0x462060

// block 00432bff
00432bff  mov        eax, dword ptr [eax]
00432c01  push       eax
00432c02  lea        ebx, [edi - 0x56]
00432c05  call       0x461970

// block 00432c0a
00432c0a  mov        dword ptr [ebp + 4], 7
00432c11  jmp        0x432c4c

// block 00432c13
00432c13  lea        esi, [ebp + 0x1e8]
00432c19  mov        edi, 0x5d
00432c1e  lea        ebx, [esp + 0x24]
00432c22  call       0x462060

// block 00432c27
00432c27  mov        ecx, dword ptr [eax]
00432c29  push       ecx
00432c2a  lea        ebx, [edi - 0x57]
00432c2d  call       0x461970

// block 00432c32
00432c32  mov        edx, dword ptr [0x4b44e8]
00432c38  mov        dword ptr [ebp + 4], 5
00432c3f  mov        eax, dword ptr [edx + 0x74]
00432c42  neg        eax
00432c44  sbb        eax, eax
00432c46  add        eax, 5
00432c49  mov        dword ptr [ebp + 4], eax

// block 00432c4c
00432c4c  push       0
00432c4e  lea        eax, [ebp + 0x10]
00432c51  call       0x4067e0

// block 00432c56
00432c56  cmp        dword ptr [ebp + 0x1fc], 0
00432c5d  jne        0x432cd9

// block 00432c5f
00432c5f  test       dword ptr [0x4d48c4], 0x200000
00432c69  je         0x432cd9

// block 00432c6b
00432c6b  mov        edx, 7
00432c70  call       0x453d90

// block 00432c75
00432c75  lea        esi, [ebp + 0x1e8]
00432c7b  mov        edi, 0x5d
00432c80  lea        ebx, [esp + 0x18]
00432c84  call       0x462060

// block 00432c89
00432c89  mov        ecx, dword ptr [eax]
00432c8b  push       ecx
00432c8c  lea        ebx, [edi - 0x57]
00432c8f  call       0x461970

// block 00432c94
00432c94  push       0
00432c96  lea        eax, [ebp + 0x10]
00432c99  mov        dword ptr [ebp + 4], 4
00432ca0  call       0x4067e0

// block 00432ca5
00432ca5  mov        eax, dword ptr [ebp + 0x40]
00432ca8  test       eax, eax
00432caa  je         0x432cc1

// block 00432cac
00432cac  cmp        eax, 3
00432caf  jg         0x432cb7

// block 00432cb1
00432cb1  dec        eax
00432cb2  mov        dword ptr [ebp + 0x38], eax
00432cb5  jmp        0x432cc8

// block 00432cb7
00432cb7  mov        eax, 3
00432cbc  mov        dword ptr [ebp + 0x38], eax
00432cbf  jmp        0x432cc8

// block 00432cc1
00432cc1  mov        dword ptr [ebp + 0x38], 3

// block 00432cc8
00432cc8  mov        edx, dword ptr [ebp + 0x1ec]
00432cce  push       edx
00432ccf  mov        ebx, 1
00432cd4  call       0x461970

// block 00432cd9
00432cd9  test       dword ptr [0x4d48c4], 0x10000
00432ce3  je         0x432d42

// block 00432ce5
00432ce5  mov        edx, 7
00432cea  call       0x453d90

// block 00432cef
00432cef  lea        esi, [ebp + 0x1e8]
00432cf5  mov        edi, 0x5b
00432cfa  lea        ebx, [esp + 0x1c]
00432cfe  call       0x462060

// block 00432d03
00432d03  mov        eax, dword ptr [eax]
00432d05  push       eax
00432d06  lea        ebx, [edi - 0x55]
00432d09  call       0x461970

// block 00432d0e
00432d0e  push       0
00432d10  lea        eax, [ebp + 0x10]
00432d13  mov        dword ptr [ebp + 4], 4
00432d1a  call       0x4067e0

// block 00432d1f
00432d1f  mov        eax, dword ptr [ebp + 0x40]
00432d22  test       eax, eax
00432d24  je         0x432d3b

// block 00432d26
00432d26  cmp        eax, 1
00432d29  jg         0x432d31

// block 00432d2b
00432d2b  dec        eax
00432d2c  mov        dword ptr [ebp + 0x38], eax
00432d2f  jmp        0x432d42

// block 00432d31
00432d31  mov        eax, 1
00432d36  mov        dword ptr [ebp + 0x38], eax
00432d39  jmp        0x432d42

// block 00432d3b
00432d3b  mov        dword ptr [ebp + 0x38], 1

// block 00432d42
00432d42  cmp        dword ptr [ebp + 0x1fc], 0
00432d49  jne        0x4336b7

// block 00432d4f
00432d4f  jmp        0x432f38

// block 00432d54
00432d54  mov        eax, dword ptr [ebp + 0x14]
00432d57  cmp        eax, 0x14
00432d5a  jl         0x4336b7

// block 00432d60
00432d60  jne        0x432dac

// block 00432d62
00432d62  lea        eax, [ebp + 0x38]
00432d65  call       0x464900

// block 00432d6a
00432d6a  mov        dword ptr [ebp + 0x40], 2
00432d71  mov        dword ptr [ebp + 0x108], 1
00432d7b  mov        ecx, dword ptr [eax + 8]
00432d7e  test       ecx, ecx
00432d80  je         0x432d95

// block 00432d82
00432d82  cmp        ecx, 1
00432d85  jg         0x432d8c

// block 00432d87
00432d87  dec        ecx
00432d88  mov        dword ptr [eax], ecx
00432d8a  jmp        0x432d9b

// block 00432d8c
00432d8c  mov        ecx, 1
00432d91  mov        dword ptr [eax], ecx
00432d93  jmp        0x432d9b

// block 00432d95
00432d95  mov        dword ptr [eax], 1

// block 00432d9b
00432d9b  mov        ecx, dword ptr [ebp + 0x1e8]
00432da1  push       ecx
00432da2  mov        ebx, 0xe
00432da7  call       0x461970

// block 00432dac
00432dac  mov        eax, dword ptr [ebp + 0x14]
00432daf  cmp        eax, 0x1e
00432db2  jl         0x4336b7

// block 00432db8
00432db8  jne        0x432dce

// block 00432dba
00432dba  movzx      ebx, word ptr [ebp + 0x38]
00432dbe  mov        edx, dword ptr [ebp + 0x1e8]
00432dc4  add        bx, 0xf
00432dc8  push       edx
00432dc9  call       0x461970

// block 00432dce
00432dce  mov        eax, dword ptr [ebp + 0x38]
00432dd1  lea        esi, [ebp + 0x38]
00432dd4  mov        dword ptr [esi + 4], eax
00432dd7  test       byte ptr [0x4d48c4], 0x10
00432dde  jne        0x432de9

// block 00432de0
00432de0  test       byte ptr [0x4d48c0], 0x10
00432de7  je         0x432df2

// block 00432de9
00432de9  push       -1
00432deb  mov        eax, esi
00432ded  call       0x464970

// block 00432df2
00432df2  test       byte ptr [0x4d48c4], 0x20
00432df9  jne        0x432e04

// block 00432dfb
00432dfb  test       byte ptr [0x4d48c0], 0x20
00432e02  je         0x432e0d

// block 00432e04
00432e04  push       1
00432e06  mov        eax, esi
00432e08  call       0x464970

// block 00432e0d
00432e0d  mov        ecx, dword ptr [esi + 4]
00432e10  cmp        ecx, dword ptr [esi]
00432e12  je         0x432e31

// block 00432e14
00432e14  movzx      ebx, word ptr [esi]
00432e17  mov        edx, dword ptr [ebp + 0x1e8]
00432e1d  add        bx, 0xf
00432e21  push       edx
00432e22  call       0x461970

// block 00432e27
00432e27  mov        edx, 0xa
00432e2c  call       0x453d90

// block 00432e31
00432e31  test       dword ptr [0x4d48c4], 0x80001
00432e3b  je         0x432eb2

// block 00432e3d
00432e3d  mov        esi, dword ptr [esi]
00432e3f  sub        esi, 0
00432e42  je         0x432e70

// block 00432e44
00432e44  sub        esi, 1
00432e47  jne        0x432ea8

// block 00432e49
00432e49  lea        esi, [ebp + 0x1e8]
00432e4f  mov        edi, 0x65
00432e54  lea        ebx, [esp + 0x20]
00432e58  call       0x462060

// block 00432e5d
00432e5d  mov        eax, dword ptr [eax]
00432e5f  push       eax
00432e60  lea        ebx, [edi - 0x5f]
00432e63  call       0x461970

// block 00432e68
00432e68  mov        dword ptr [ebp + 4], ebx
00432e6b  lea        edx, [edi - 0x5c]
00432e6e  jmp        0x432ea3

// block 00432e70
00432e70  lea        esi, [ebp + 0x1e8]
00432e76  mov        edi, 0x64
00432e7b  lea        ebx, [esp + 0x28]
00432e7f  call       0x462060

// block 00432e84
00432e84  mov        ecx, dword ptr [eax]
00432e86  push       ecx
00432e87  lea        ebx, [edi - 0x5e]
00432e8a  call       0x461970

// block 00432e8f
00432e8f  xor        eax, eax
00432e91  mov        edx, 7
00432e96  cmp        dword ptr [ebp + 4], edx
00432e99  sete       al
00432e9c  lea        eax, [eax + eax + 6]
00432ea0  mov        dword ptr [ebp + 4], eax

// block 00432ea3
00432ea3  call       0x453d90

// block 00432ea8
00432ea8  push       0
00432eaa  lea        eax, [ebp + 0x10]
00432ead  call       0x4067e0

// block 00432eb2
00432eb2  test       dword ptr [0x4d48c4], 0x102
00432ebc  je         0x432f38

// block 00432ebe
00432ebe  mov        edx, 9
00432ec3  call       0x453d90

// block 00432ec8
00432ec8  mov        eax, dword ptr [ebp + 0x38]
00432ecb  sub        eax, 0
00432ece  je         0x432f01

// block 00432ed0
00432ed0  sub        eax, 1
00432ed3  jne        0x432f38

// block 00432ed5
00432ed5  lea        esi, [ebp + 0x1e8]
00432edb  lea        edi, [eax + 0x65]
00432ede  lea        ebx, [esp + 0x30]
00432ee2  call       0x462060

// block 00432ee7
00432ee7  mov        ecx, dword ptr [eax]
00432ee9  push       ecx
00432eea  lea        ebx, [edi - 0x5f]
00432eed  call       0x461970

// block 00432ef2
00432ef2  push       0
00432ef4  lea        eax, [ebp + 0x10]
00432ef7  mov        dword ptr [ebp + 4], ebx
00432efa  call       0x4067e0

// block 00432eff
00432eff  jmp        0x432f38

// block 00432f01
00432f01  mov        eax, dword ptr [ebp + 0x40]
00432f04  test       eax, eax
00432f06  je         0x432f1d

// block 00432f08
00432f08  cmp        eax, 1
00432f0b  jg         0x432f13

// block 00432f0d
00432f0d  dec        eax
00432f0e  mov        dword ptr [ebp + 0x38], eax
00432f11  jmp        0x432f24

// block 00432f13
00432f13  mov        eax, 1
00432f18  mov        dword ptr [ebp + 0x38], eax
00432f1b  jmp        0x432f24

// block 00432f1d
00432f1d  mov        dword ptr [ebp + 0x38], 1

// block 00432f24
00432f24  movzx      ebx, word ptr [ebp + 0x38]
00432f28  mov        edx, dword ptr [ebp + 0x1e8]
00432f2e  add        bx, 0xf
00432f32  push       edx
00432f33  call       0x461970

// block 00432f38
00432f38  test       dword ptr [0x4d48c4], 0x100
00432f42  je         0x4336b7

// block 00432f48
00432f48  mov        eax, dword ptr [ebp + 0x40]
00432f4b  test       eax, eax
00432f4d  je         0x432f5e

// block 00432f4f
00432f4f  jg         0x432f57

// block 00432f51
00432f51  dec        eax
00432f52  mov        dword ptr [ebp + 0x38], eax
00432f55  jmp        0x432f65

// block 00432f57
00432f57  xor        eax, eax
00432f59  mov        dword ptr [ebp + 0x38], eax
00432f5c  jmp        0x432f65

// block 00432f5e
00432f5e  mov        dword ptr [ebp + 0x38], 0

// block 00432f65
00432f65  mov        eax, dword ptr [ebp + 0x1ec]
00432f6b  push       eax
00432f6c  mov        ebx, 1
00432f71  call       0x461970

// block 00432f76
00432f76  mov        ecx, dword ptr [ebp + 0x1e8]
00432f7c  push       ecx
00432f7d  call       0x461970

// block 00432f82
00432f82  push       0
00432f84  lea        eax, [ebp + 0x10]
00432f87  mov        dword ptr [ebp + 4], 4
00432f8e  call       0x4067e0

// block 00432f93
00432f93  pop        esi
00432f94  pop        ebx
00432f95  pop        edi
00432f96  pop        ebp
00432f97  mov        ecx, dword ptr [esp + 0x68]
00432f9b  xor        ecx, esp
00432f9d  call       0x46c932

// block 00432fa2
00432fa2  add        esp, 0x6c
00432fa5  ret        4

// block 00432fa8
00432fa8  mov        esi, 0xa
00432fad  cmp        dword ptr [ebp + 0x14], esi
00432fb0  jl         0x4336b7

// block 00432fb6
00432fb6  mov        edx, dword ptr [ebp + 0x38]
00432fb9  lea        edi, [ebp + 0x38]
00432fbc  mov        dword ptr [edi + 4], edx
00432fbf  test       byte ptr [0x4d48c4], 0x10
00432fc6  jne        0x432fd1

// block 00432fc8
00432fc8  test       byte ptr [0x4d48c0], 0x10
00432fcf  je         0x432fda

// block 00432fd1
00432fd1  push       -1
00432fd3  mov        eax, edi
00432fd5  call       0x464970

// block 00432fda
00432fda  test       byte ptr [0x4d48c4], 0x20
00432fe1  mov        ebx, 1
00432fe6  jne        0x432ff1

// block 00432fe8
00432fe8  test       byte ptr [0x4d48c0], 0x20
00432fef  je         0x432ff9

// block 00432ff1
00432ff1  push       ebx
00432ff2  mov        eax, edi
00432ff4  call       0x464970

// block 00432ff9
00432ff9  mov        eax, dword ptr [edi + 4]
00432ffc  cmp        eax, dword ptr [edi]
00432ffe  je         0x433007

// block 00433000
00433000  mov        edx, esi
00433002  call       0x453d90

// block 00433007
00433007  mov        eax, dword ptr [0x4d48c4]
0043300c  test       eax, 0x80001
00433011  je         0x433155

// block 00433017
00433017  push       0
00433019  lea        eax, [ebp + 0x10]
0043301c  mov        dword ptr [ebp + 4], esi
0043301f  call       0x4067e0

// block 00433024
00433024  mov        eax, dword ptr [ebp + 0x118]
0043302a  lea        edi, [ebp + 0x110]
00433030  test       eax, eax
00433032  je         0x433041

// block 00433034
00433034  jg         0x43303b

// block 00433036
00433036  dec        eax
00433037  mov        dword ptr [edi], eax
00433039  jmp        0x433047

// block 0043303b
0043303b  xor        eax, eax
0043303d  mov        dword ptr [edi], eax
0043303f  jmp        0x433047

// block 00433041
00433041  mov        dword ptr [edi], 0

// block 00433047
00433047  cmp        dword ptr [ebp + 0x1f4], 0
0043304e  mov        dword ptr [ebp + 0x118], 0x5b
00433058  mov        dword ptr [ebp + 0x1e0], ebx
0043305e  je         0x433089

// block 00433060
00433060  test       byte ptr [0x4b0ce0], 0x10
00433067  jne        0x433089

// block 00433069
00433069  mov        esi, dword ptr [0x4b4518]
0043306f  mov        ecx, dword ptr [esi + 0x1c]
00433072  add        esi, 0x1c
00433075  add        ecx, 0xc
00433078  push       ecx
00433079  call       0x46d5b7

// block 0043307e
0043307e  mov        edx, dword ptr [esi]
00433080  mov        dword ptr [edx + 0x68], 8
00433087  jmp        0x4330a9

// block 00433089
00433089  mov        esi, dword ptr [0x4b4518]
0043308f  mov        eax, dword ptr [esi + 0x1c]
00433092  add        esi, 0x1c
00433095  add        eax, 0xc
00433098  push       eax
00433099  call       0x46d5b7

// block 0043309e
0043309e  mov        ecx, dword ptr [esi]
004330a0  mov        edx, dword ptr [0x4b0cb0]
004330a6  mov        dword ptr [ecx + 0x68], edx

// block 004330a9
004330a9  mov        eax, dword ptr [0x4b451c]
004330ae  lea        esi, [ebp + 0x2cc]
004330b4  add        eax, 0x1e9c0
004330b9  mov        edx, esi
004330bb  add        esp, 4
004330be  sub        edx, eax

// block 004330c0
004330c0  mov        cl, byte ptr [eax]
004330c2  mov        byte ptr [edx + eax], cl
004330c5  add        eax, ebx
004330c7  test       cl, cl
004330c9  jne        0x4330c0

// block 004330cb
004330cb  mov        dword ptr [ebp + 0x1f0], 0
004330d5  mov        ecx, 0x4a0ee4
004330da  mov        eax, esi
004330dc  lea        esp, [esp]

// block 004330e0
004330e0  mov        dl, byte ptr [eax]
004330e2  cmp        dl, byte ptr [ecx]
004330e4  jne        0x433100

// block 004330e6
004330e6  test       dl, dl
004330e8  je         0x4330fc

// block 004330ea
004330ea  mov        dl, byte ptr [eax + 1]
004330ed  cmp        dl, byte ptr [ecx + 1]
004330f0  jne        0x433100

// block 004330f2
004330f2  add        eax, 2
004330f5  add        ecx, 2
004330f8  test       dl, dl
004330fa  jne        0x4330e0

// block 004330fc
004330fc  xor        eax, eax
004330fe  jmp        0x433105

// block 00433100
00433100  sbb        eax, eax
00433102  sbb        eax, -1

// block 00433105
00433105  test       eax, eax
00433107  je         0x433112

// block 00433109
00433109  push       -1
0043310b  mov        eax, edi
0043310d  call       0x464970

// block 00433112
00433112  mov        eax, 8
00433117  jmp        0x433120

// block 00433120
00433120  cmp        byte ptr [eax + ebp + 0x2cb], 0x20
00433128  jne        0x433130

// block 0043312a
0043312a  sub        eax, ebx
0043312c  test       eax, eax
0043312e  jg         0x433120

// block 00433130
00433130  mov        edx, 7
00433135  mov        dword ptr [ebp + 0x1f0], eax
0043313b  call       0x453d90

// block 00433140
00433140  pop        esi
00433141  pop        ebx
00433142  pop        edi
00433143  pop        ebp
00433144  mov        ecx, dword ptr [esp + 0x68]
00433148  xor        ecx, esp
0043314a  call       0x46c932

// block 0043314f
0043314f  add        esp, 0x6c
00433152  ret        4

// block 00433155
00433155  test       eax, 0x102
0043315a  je         0x4336b7

// block 00433160
00433160  lea        eax, [ebp + 0x10]
00433163  push       0
00433165  mov        dword ptr [ebp + 4], 0xe
0043316c  mov        dword ptr [esp + 0x18], eax
00433170  call       0x4067e0

// block 00433175
00433175  mov        eax, edi
00433177  call       0x464940

// block 0043317c
0043317c  mov        eax, dword ptr [ebp + 0x1e8]
00433182  mov        dword ptr [ebp + 0x108], ebx
00433188  movzx      ebx, word ptr [edi]
0043318b  add        bx, 7
0043318f  push       eax
00433190  mov        dword ptr [ebp + 0x40], 4
00433197  call       0x461970

// block 0043319c
0043319c  lea        ebx, [ebp + 0x204]
004331a2  mov        dword ptr [esp + 0x10], 0x19
004331aa  lea        ebx, [ebx]

// block 004331b0
004331b0  mov        esi, dword ptr [ebx]
004331b2  call       0x43b7c0

// block 004331b7
004331b7  mov        dword ptr [ebx], 0
004331bd  add        ebx, 4
004331c0  sub        dword ptr [esp + 0x10], 1
004331c5  jne        0x4331b0

// block 004331c7
004331c7  mov        dword ptr [ebp + 4], 4
004331ce  mov        eax, dword ptr [edi + 8]
004331d1  test       eax, eax
004331d3  je         0x4331e8

// block 004331d5
004331d5  cmp        eax, 1
004331d8  jg         0x4331df

// block 004331da
004331da  dec        eax
004331db  mov        dword ptr [edi], eax
004331dd  jmp        0x4331ee

// block 004331df
004331df  mov        eax, 1
004331e4  mov        dword ptr [edi], eax
004331e6  jmp        0x4331ee

// block 004331e8
004331e8  mov        dword ptr [edi], 1

// block 004331ee
004331ee  mov        eax, dword ptr [esp + 0x14]
004331f2  push       0
004331f4  call       0x4067e0

// block 004331f9
004331f9  mov        edx, 9
004331fe  call       0x453d90

// block 00433203
00433203  pop        esi
00433204  pop        ebx
00433205  pop        edi
00433206  pop        ebp
00433207  mov        ecx, dword ptr [esp + 0x68]
0043320b  xor        ecx, esp
0043320d  call       0x46c932

// block 00433212
00433212  add        esp, 0x6c
00433215  ret        4

// block 00433218
00433218  cmp        dword ptr [ebp + 0x14], 0xa
0043321c  jl         0x4336b7

// block 00433222
00433222  mov        ecx, dword ptr [ebp + 0x110]
00433228  lea        esi, [ebp + 0x110]
0043322e  mov        dword ptr [esi + 4], ecx
00433231  test       byte ptr [0x4d48c4], 0x10
00433238  jne        0x433243

// block 0043323a
0043323a  test       byte ptr [0x4d48c0], 0x10
00433241  je         0x43324c

// block 00433243
00433243  push       -0xd
00433245  mov        eax, esi
00433247  call       0x464970

// block 0043324c
0043324c  test       byte ptr [0x4d48c4], 0x20
00433253  jne        0x43325e

// block 00433255
00433255  test       byte ptr [0x4d48c0], 0x20
0043325c  je         0x433267

// block 0043325e
0043325e  push       0xd
00433260  mov        eax, esi
00433262  call       0x464970

// block 00433267
00433267  mov        al, 0x40
00433269  test       byte ptr [0x4d48c4], al
0043326f  jne        0x433279

// block 00433271
00433271  test       byte ptr [0x4d48c0], al
00433277  je         0x4332a0

// block 00433279
00433279  mov        ecx, dword ptr [esi]
0043327b  mov        eax, 0x4ec4ec4f
00433280  imul       ecx
00433282  sar        edx, 2
00433285  mov        eax, edx
00433287  shr        eax, 0x1f
0043328a  add        eax, edx
0043328c  imul       eax, eax, 0xd
0043328f  sub        ecx, eax
00433291  mov        eax, esi
00433293  je         0x433299

// block 00433295
00433295  push       -1
00433297  jmp        0x43329b

// block 00433299
00433299  push       0xc

// block 0043329b
0043329b  call       0x464970

// block 004332a0
004332a0  mov        al, 0x80
004332a2  mov        ebx, 1
004332a7  test       byte ptr [0x4d48c4], al
004332ad  jne        0x4332b7

// block 004332af
004332af  test       byte ptr [0x4d48c0], al
004332b5  je         0x4332e0

// block 004332b7
004332b7  mov        ecx, dword ptr [esi]
004332b9  mov        eax, 0x4ec4ec4f
004332be  imul       ecx
004332c0  sar        edx, 2
004332c3  mov        eax, edx
004332c5  shr        eax, 0x1f
004332c8  add        eax, edx
004332ca  imul       eax, eax, 0xd
004332cd  sub        ecx, eax
004332cf  mov        eax, esi
004332d1  cmp        ecx, 0xc
004332d4  je         0x4332d9

// block 004332d6
004332d6  push       ebx
004332d7  jmp        0x4332db

// block 004332d9
004332d9  push       -0xc

// block 004332db
004332db  call       0x464970

// block 004332e0
004332e0  mov        ecx, dword ptr [esi + 4]
004332e3  cmp        ecx, dword ptr [esi]
004332e5  je         0x4332f1

// block 004332e7
004332e7  mov        edx, 0xa
004332ec  call       0x453d90

// block 004332f1
004332f1  test       dword ptr [0x4d48c4], 0x80001
004332fb  je         0x433345

// block 004332fd
004332fd  mov        eax, dword ptr [esi]
004332ff  cmp        eax, 0x58
00433302  jge        0x43339a

// block 00433308
00433308  mov        ecx, dword ptr [ebp + 0x1f0]
0043330e  cmp        ecx, 8
00433311  jge        0x43338b

// block 00433313
00433313  mov        dl, byte ptr [eax + 0x4a0e88]
00433319  mov        byte ptr [ecx + ebp + 0x2cc], dl

// block 00433320
00433320  add        dword ptr [ebp + 0x1f0], ebx
00433326  cmp        dword ptr [ebp + 0x1f0], 8
0043332d  jl         0x43333b

// block 0043332f
0043332f  mov        ecx, 0x5a
00433334  mov        edx, esi
00433336  call       0x40f790

// block 0043333b
0043333b  mov        edx, 7
00433340  call       0x453d90

// block 00433345
00433345  test       dword ptr [0x4d48c4], 0x102
0043334f  je         0x4336b7

// block 00433355
00433355  mov        edx, edi
00433357  call       0x453d90

// block 0043335c
0043335c  mov        eax, dword ptr [ebp + 0x1f0]
00433362  test       eax, eax
00433364  jne        0x4334a0

// block 0043336a
0043336a  push       eax
0043336b  lea        eax, [ebp + 0x10]
0043336e  mov        dword ptr [ebp + 4], edi
00433371  call       0x4067e0

// block 00433376
00433376  pop        esi
00433377  pop        ebx
00433378  pop        edi
00433379  pop        ebp
0043337a  mov        ecx, dword ptr [esp + 0x68]
0043337e  xor        ecx, esp
00433380  call       0x46c932

// block 00433385
00433385  add        esp, 0x6c
00433388  ret        4

// block 0043338b
0043338b  mov        al, byte ptr [eax + 0x4a0e88]
00433391  mov        byte ptr [ecx + ebp + 0x2cb], al
00433398  jmp        0x43333b

// block 0043339a
0043339a  jne        0x4333c1

// block 0043339c
0043339c  mov        eax, dword ptr [ebp + 0x1f0]
004333a2  cmp        eax, 8
004333a5  jge        0x4333b4

// block 004333a7
004333a7  mov        byte ptr [eax + ebp + 0x2cc], 0x20
004333af  jmp        0x433320

// block 004333b4
004333b4  mov        byte ptr [eax + ebp + 0x2cb], 0x20
004333bc  jmp        0x43333b

// block 004333c1
004333c1  cmp        eax, 0x59
004333c4  jne        0x4333ff

// block 004333c6
004333c6  mov        eax, dword ptr [ebp + 0x1f0]
004333cc  test       eax, eax
004333ce  je         0x4336b7

// block 004333d4
004333d4  dec        eax
004333d5  mov        dword ptr [ebp + 0x1f0], eax
004333db  mov        edx, edi
004333dd  mov        byte ptr [eax + ebp + 0x2cc], 0x20
004333e5  call       0x453d90

// block 004333ea
004333ea  pop        esi
004333eb  pop        ebx
004333ec  pop        edi
004333ed  pop        ebp
004333ee  mov        ecx, dword ptr [esp + 0x68]
004333f2  xor        ecx, esp
004333f4  call       0x46c932

// block 004333f9
004333f9  add        esp, 0x6c
004333fc  ret        4

// block 004333ff
004333ff  cmp        eax, 0x5a
00433402  jne        0x43333b

// block 00433408
00433408  lea        edx, [eax - 0x48]
0043340b  call       0x453d90

// block 00433410
00433410  mov        ecx, dword ptr [ebp + 0x38]
00433413  add        ecx, ebx
00433415  push       ecx
00433416  lea        edx, [esp + 0x3c]
0043341a  push       0x4a1024
0043341f  push       edx
00433420  call       0x46cbbb

// block 00433425
00433425  mov        eax, dword ptr [ebp + 0x38]
00433428  mov        esi, dword ptr [ebp + eax*4 + 0x204]
0043342f  add        esp, 0xc
00433432  call       0x43b7c0

// block 00433437
00433437  lea        esi, [ebp + 0x2cc]
0043343d  push       ebx
0043343e  mov        edx, esi
00433440  lea        ecx, [esp + 0x3c]
00433444  call       0x43bc10

// block 00433449
00433449  mov        edi, dword ptr [ebp + 0x38]
0043344c  lea        ecx, [esp + 0x38]
00433450  push       ecx
00433451  call       0x43b6f0

// block 00433456
00433456  mov        dword ptr [ebp + edi*4 + 0x204], eax
0043345d  push       0
0043345f  lea        eax, [ebp + 0x10]
00433462  mov        dword ptr [ebp + 4], 9
00433469  call       0x4067e0

// block 0043346e
0043346e  mov        edx, dword ptr [0x4b451c]
00433474  add        edx, 0x1e9c0
0043347a  mov        eax, esi
0043347c  sub        edx, esi
0043347e  mov        edi, edi

// block 00433480
00433480  mov        cl, byte ptr [eax]
00433482  mov        byte ptr [eax + edx], cl
00433485  add        eax, ebx
00433487  test       cl, cl
00433489  jne        0x433480

// block 0043348b
0043348b  pop        esi
0043348c  pop        ebx
0043348d  pop        edi
0043348e  pop        ebp
0043348f  mov        ecx, dword ptr [esp + 0x68]
00433493  xor        ecx, esp
00433495  call       0x46c932

// block 0043349a
0043349a  add        esp, 0x6c
0043349d  ret        4

// block 004334a0
004334a0  pop        esi
004334a1  dec        eax
004334a2  pop        ebx
004334a3  mov        dword ptr [ebp + 0x1f0], eax
004334a9  pop        edi
004334aa  mov        byte ptr [eax + ebp + 0x2cc], 0x20
004334b2  pop        ebp
004334b3  mov        ecx, dword ptr [esp + 0x68]
004334b7  xor        ecx, esp
004334b9  call       0x46c932

// block 004334be
004334be  add        esp, 0x6c
004334c1  ret        4

// block 004334c4
004334c4  cmp        dword ptr [ebp + 0x14], 0x14
004334c8  jl         0x4336b7

// block 004334ce
004334ce  mov        eax, dword ptr [ebp + 0x38]
004334d1  sub        eax, 0
004334d4  lea        esi, [ebp + 0x38]
004334d7  je         0x43351e

// block 004334d9
004334d9  sub        eax, 1
004334dc  jne        0x43353d

// block 004334de
004334de  mov        eax, esi
004334e0  call       0x464940

// block 004334e5
004334e5  movzx      ebx, word ptr [esi]
004334e8  mov        edx, dword ptr [ebp + 0x1e8]
004334ee  add        bx, 7
004334f2  push       edx
004334f3  call       0x461970

// block 004334f8
004334f8  push       0
004334fa  lea        eax, [ebp + 0x10]
004334fd  mov        dword ptr [ebp + 4], 3
00433504  call       0x4067e0

// block 00433509
00433509  pop        esi
0043350a  pop        ebx
0043350b  pop        edi
0043350c  pop        ebp
0043350d  mov        ecx, dword ptr [esp + 0x68]
00433511  xor        ecx, esp
00433513  call       0x46c932

// block 00433518
00433518  add        esp, 0x6c
0043351b  ret        4

// block 0043351e
0043351e  mov        eax, dword ptr [ebp + 0x1e8]
00433524  push       eax
00433525  mov        ebx, 1
0043352a  call       0x461970

// block 0043352f
0043352f  mov        eax, esi
00433531  mov        dword ptr [ebp + 4], 4
00433538  call       0x464940

// block 0043353d
0043353d  push       0
0043353f  lea        eax, [ebp + 0x10]
00433542  call       0x4067e0

// block 00433547
00433547  pop        esi
00433548  pop        ebx
00433549  pop        edi
0043354a  pop        ebp
0043354b  mov        ecx, dword ptr [esp + 0x68]
0043354f  xor        ecx, esp
00433551  call       0x46c932

// block 00433556
00433556  add        esp, 0x6c
00433559  ret        4

// block 0043355c
0043355c  cmp        dword ptr [ebp + 0x14], 0x14
00433560  jl         0x4336b7

// block 00433566
00433566  push       0
00433568  lea        eax, [ebp + 0x10]
0043356b  mov        dword ptr [ebp + 4], edi
0043356e  call       0x4067e0

// block 00433573
00433573  lea        eax, [ebp + 0x1e8]
00433579  call       0x461d80

// block 0043357e
0043357e  lea        eax, [ebp + 0x38]
00433581  call       0x464900

// block 00433586
00433586  mov        dword ptr [ebp + 0x40], 0x19
0043358d  mov        dword ptr [ebp + 0x108], 1
00433597  mov        ecx, dword ptr [eax + 8]
0043359a  test       ecx, ecx
0043359c  je         0x4335ab

// block 0043359e
0043359e  jg         0x4335a5

// block 004335a0
004335a0  dec        ecx
004335a1  mov        dword ptr [eax], ecx
004335a3  jmp        0x4335b1

// block 004335a5
004335a5  xor        ecx, ecx
004335a7  mov        dword ptr [eax], ecx
004335a9  jmp        0x4335b1

// block 004335ab
004335ab  mov        dword ptr [eax], 0

// block 004335b1
004335b1  mov        esi, 1
004335b6  add        ebp, 0x204
004335bc  lea        esp, [esp]

// block 004335c0
004335c0  push       esi
004335c1  lea        ecx, [esp + 0x3c]
004335c5  push       0x4a1024
004335ca  push       ecx
004335cb  call       0x46cbbb

// block 004335d0
004335d0  add        esp, 0xc
004335d3  lea        edx, [esp + 0x38]
004335d7  push       edx
004335d8  call       0x43b6f0

// block 004335dd
004335dd  mov        dword ptr [ebp], eax
004335e0  inc        esi
004335e1  add        ebp, 4
004335e4  cmp        esi, 0x19
004335e7  jle        0x4335c0

// block 004335e9
004335e9  pop        esi
004335ea  pop        ebx
004335eb  pop        edi
004335ec  pop        ebp
004335ed  mov        ecx, dword ptr [esp + 0x68]
004335f1  xor        ecx, esp
004335f3  call       0x46c932

// block 004335f8
004335f8  add        esp, 0x6c
004335fb  ret        4

// block 004335fe
004335fe  cmp        dword ptr [ebp + 0x14], 0xc
00433602  jl         0x4336b7

// block 00433608
00433608  mov        eax, dword ptr [ebp + 0x38]
0043360b  sub        eax, 0
0043360e  mov        dword ptr [ebp + 4], 0
00433615  je         0x4336b0

// block 0043361b
0043361b  sub        eax, 1
0043361e  je         0x433671

// block 00433620
00433620  sub        eax, 2
00433623  jne        0x4336b7

// block 00433629
00433629  mov        eax, dword ptr [ebp + 0x1ec]
0043362f  push       eax
00433630  mov        ebx, 1
00433635  call       0x461970

// block 0043363a
0043363a  mov        ecx, dword ptr [ebp + 0x1e8]
00433640  push       ecx
00433641  call       0x461970

// block 00433646
00433646  mov        eax, dword ptr [0x4b44e8]
0043364b  xor        edx, edx
0043364d  cmp        dword ptr [eax + 0x74], edx
00433650  pop        esi
00433651  setne      dl
00433654  pop        ebx
00433655  pop        edi
00433656  pop        ebp
00433657  add        edx, 0xa
0043365a  mov        dword ptr [0x4cee40], edx
00433660  mov        ecx, dword ptr [esp + 0x68]
00433664  xor        ecx, esp
00433666  call       0x46c932

// block 0043366b
0043366b  add        esp, 0x6c
0043366e  ret        4

// block 00433671
00433671  mov        ecx, dword ptr [ebp + 0x1ec]
00433677  push       ecx
00433678  mov        ebx, 1
0043367d  call       0x461970

// block 00433682
00433682  mov        edx, dword ptr [ebp + 0x1e8]
00433688  push       edx
00433689  call       0x461970

// block 0043368e
0043368e  lea        ecx, [ebx + 3]
00433691  mov        eax, 0x4ce8e8
00433696  call       0x40f720

// block 0043369b
0043369b  pop        esi
0043369c  pop        ebx
0043369d  pop        edi
0043369e  pop        ebp
0043369f  mov        ecx, dword ptr [esp + 0x68]
004336a3  xor        ecx, esp
004336a5  call       0x46c932

// block 004336aa
004336aa  add        esp, 0x6c
004336ad  ret        4

// block 004336b0
004336b0  mov        esi, ebp
004336b2  call       0x432960

// block 004336b7
004336b7  pop        esi
004336b8  pop        ebx

// block 004336b9
004336b9  mov        ecx, dword ptr [esp + 0x70]
004336bd  pop        edi
004336be  pop        ebp
004336bf  xor        ecx, esp
004336c1  call       0x46c932

// block 004336c6
004336c6  add        esp, 0x6c
004336c9  ret        4
