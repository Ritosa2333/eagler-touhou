
// block 00452a60
00452a60  mov        eax, dword ptr [esp + 8]
00452a64  push       ebp
00452a65  mov        ebp, dword ptr [esp + 8]
00452a69  push       edi
00452a6a  xor        edi, edi
00452a6c  cmp        eax, 8
00452a6f  ja         0x452e6b

// block 00452a75
00452a75  push       ebx
00452a76  push       esi
00452a77  jmp        dword ptr [eax*4 + 0x452ed0]

// block 00452a7e
00452a7e  push       0x24
00452a80  call       0x46c9ea

// block 00452a85
00452a85  add        esp, 4
00452a88  cmp        eax, edi
00452a8a  je         0x452ab2

// block 00452a8c
00452a8c  and        dword ptr [eax + 4], 0xfffffffe
00452a90  mov        dword ptr [eax + 8], edi
00452a93  mov        dword ptr [eax + 0xc], edi
00452a96  mov        dword ptr [eax + 0x10], edi
00452a99  mov        dword ptr [eax], edi
00452a9b  mov        esi, eax
00452a9d  mov        dword ptr [eax + 0x14], eax
00452aa0  mov        dword ptr [eax + 0x18], edi
00452aa3  mov        dword ptr [eax + 0x1c], edi
00452aa6  mov        dword ptr [esi + 8], 0x451ed0
00452aad  jmp        0x452c07

// block 00452ab2
00452ab2  xor        esi, esi
00452ab4  mov        dword ptr [esi + 8], 0x451ed0
00452abb  jmp        0x452c07

// block 00452ac0
00452ac0  push       0x24
00452ac2  call       0x46c9ea

// block 00452ac7
00452ac7  add        esp, 4
00452aca  cmp        eax, edi
00452acc  je         0x452af4

// block 00452ace
00452ace  and        dword ptr [eax + 4], 0xfffffffe
00452ad2  mov        dword ptr [eax + 8], edi
00452ad5  mov        dword ptr [eax + 0xc], edi
00452ad8  mov        dword ptr [eax + 0x10], edi
00452adb  mov        dword ptr [eax], edi
00452add  mov        esi, eax
00452adf  mov        dword ptr [eax + 0x14], eax
00452ae2  mov        dword ptr [eax + 0x18], edi
00452ae5  mov        dword ptr [eax + 0x1c], edi
00452ae8  mov        dword ptr [esi + 8], 0x4526e0
00452aef  jmp        0x452e4f

// block 00452af4
00452af4  xor        esi, esi
00452af6  mov        dword ptr [esi + 8], 0x4526e0
00452afd  jmp        0x452e4f

// block 00452b02
00452b02  push       0x24
00452b04  call       0x46c9ea

// block 00452b09
00452b09  add        esp, 4
00452b0c  cmp        eax, edi
00452b0e  je         0x452b33

// block 00452b10
00452b10  and        dword ptr [eax + 4], 0xfffffffe
00452b14  mov        dword ptr [eax + 8], edi
00452b17  mov        dword ptr [eax + 0xc], edi
00452b1a  mov        dword ptr [eax + 0x10], edi
00452b1d  mov        dword ptr [eax], edi
00452b1f  mov        esi, eax
00452b21  mov        dword ptr [eax + 0x14], eax
00452b24  mov        dword ptr [eax + 0x18], edi
00452b27  mov        dword ptr [eax + 0x1c], edi
00452b2a  mov        dword ptr [esi + 8], 0x4523e0
00452b31  jmp        0x452b78

// block 00452b33
00452b33  xor        esi, esi
00452b35  mov        dword ptr [esi + 8], 0x4523e0
00452b3c  jmp        0x452b78

// block 00452b3e
00452b3e  push       0x24
00452b40  mov        dword ptr [ebp + 0x18], 0xff
00452b47  call       0x46c9ea

// block 00452b4c
00452b4c  add        esp, 4
00452b4f  cmp        eax, edi
00452b51  je         0x452b6f

// block 00452b53
00452b53  and        dword ptr [eax + 4], 0xfffffffe
00452b57  mov        dword ptr [eax + 8], edi
00452b5a  mov        dword ptr [eax + 0xc], edi
00452b5d  mov        dword ptr [eax + 0x10], edi
00452b60  mov        dword ptr [eax], edi
00452b62  mov        dword ptr [eax + 0x14], eax
00452b65  mov        dword ptr [eax + 0x18], edi
00452b68  mov        dword ptr [eax + 0x1c], edi
00452b6b  mov        esi, eax
00452b6d  jmp        0x452b71

// block 00452b6f
00452b6f  xor        esi, esi

// block 00452b71
00452b71  mov        dword ptr [esi + 8], 0x451ed0

// block 00452b78
00452b78  mov        dword ptr [esi + 0xc], edi
00452b7b  mov        dword ptr [esi + 0x10], edi
00452b7e  mov        dword ptr [esi + 0x20], ebp
00452b81  or         dword ptr [esi + 4], 3
00452b85  mov        ebx, 0xe
00452b8a  call       0x462380

// block 00452b8f
00452b8f  push       0x24
00452b91  mov        dword ptr [ebp + 8], esi
00452b94  call       0x46c9ea

// block 00452b99
00452b99  add        esp, 4
00452b9c  cmp        eax, edi
00452b9e  je         0x452bc6

// block 00452ba0
00452ba0  mov        dword ptr [eax + 8], edi
00452ba3  mov        dword ptr [eax + 0xc], edi
00452ba6  mov        dword ptr [eax + 0x10], edi
00452ba9  mov        dword ptr [eax], edi
00452bab  and        dword ptr [eax + 4], 0xfffffffe
00452baf  mov        dword ptr [eax + 0x14], eax
00452bb2  mov        dword ptr [eax + 0x18], edi
00452bb5  mov        esi, eax
00452bb7  mov        dword ptr [eax + 0x1c], edi
00452bba  mov        dword ptr [esi + 8], 0x452470
00452bc1  jmp        0x452e01

// block 00452bc6
00452bc6  xor        esi, esi
00452bc8  mov        dword ptr [esi + 8], 0x452470
00452bcf  jmp        0x452e01

// block 00452bd4
00452bd4  push       0x24
00452bd6  call       0x46c9ea

// block 00452bdb
00452bdb  add        esp, 4
00452bde  cmp        eax, edi
00452be0  je         0x452bfe

// block 00452be2
00452be2  and        dword ptr [eax + 4], 0xfffffffe
00452be6  mov        dword ptr [eax + 8], edi
00452be9  mov        dword ptr [eax + 0xc], edi
00452bec  mov        dword ptr [eax + 0x10], edi
00452bef  mov        dword ptr [eax], edi
00452bf1  mov        dword ptr [eax + 0x14], eax
00452bf4  mov        dword ptr [eax + 0x18], edi
00452bf7  mov        dword ptr [eax + 0x1c], edi
00452bfa  mov        esi, eax
00452bfc  jmp        0x452c00

// block 00452bfe
00452bfe  xor        esi, esi

// block 00452c00
00452c00  mov        dword ptr [esi + 8], 0x4523e0

// block 00452c07
00452c07  mov        dword ptr [esi + 0xc], edi
00452c0a  mov        dword ptr [esi + 0x10], edi
00452c0d  mov        dword ptr [esi + 0x20], ebp
00452c10  or         dword ptr [esi + 4], 3
00452c14  mov        ebx, 0xe
00452c19  call       0x462380

// block 00452c1e
00452c1e  push       0x24
00452c20  mov        dword ptr [ebp + 8], esi
00452c23  call       0x46c9ea

// block 00452c28
00452c28  add        esp, 4
00452c2b  cmp        eax, edi
00452c2d  je         0x452c55

// block 00452c2f
00452c2f  mov        dword ptr [eax + 8], edi
00452c32  mov        dword ptr [eax + 0xc], edi
00452c35  mov        dword ptr [eax + 0x10], edi
00452c38  mov        dword ptr [eax], edi
00452c3a  and        dword ptr [eax + 4], 0xfffffffe
00452c3e  mov        dword ptr [eax + 0x14], eax
00452c41  mov        dword ptr [eax + 0x18], edi
00452c44  mov        esi, eax
00452c46  mov        dword ptr [eax + 0x1c], edi
00452c49  mov        dword ptr [esi + 8], 0x452350
00452c50  jmp        0x452e01

// block 00452c55
00452c55  xor        esi, esi
00452c57  mov        dword ptr [esi + 8], 0x452350
00452c5e  jmp        0x452e01

// block 00452c63
00452c63  push       0x24
00452c65  call       0x46c9ea

// block 00452c6a
00452c6a  add        esp, 4
00452c6d  cmp        eax, edi
00452c6f  je         0x452c8d

// block 00452c71
00452c71  and        dword ptr [eax + 4], 0xfffffffe
00452c75  mov        dword ptr [eax + 8], edi
00452c78  mov        dword ptr [eax + 0xc], edi
00452c7b  mov        dword ptr [eax + 0x10], edi
00452c7e  mov        dword ptr [eax], edi
00452c80  mov        dword ptr [eax + 0x14], eax
00452c83  mov        dword ptr [eax + 0x18], edi
00452c86  mov        dword ptr [eax + 0x1c], edi
00452c89  mov        esi, eax
00452c8b  jmp        0x452c8f

// block 00452c8d
00452c8d  xor        esi, esi

// block 00452c8f
00452c8f  or         dword ptr [esi + 4], 3
00452c93  mov        ebx, 0xe
00452c98  mov        dword ptr [esi + 8], 0x4525d0
00452c9f  mov        dword ptr [esi + 0xc], edi
00452ca2  mov        dword ptr [esi + 0x10], edi
00452ca5  mov        dword ptr [esi + 0x20], ebp
00452ca8  call       0x462380

// block 00452cad
00452cad  push       0x24
00452caf  mov        dword ptr [ebp + 8], esi
00452cb2  call       0x46c9ea

// block 00452cb7
00452cb7  add        esp, 4
00452cba  cmp        eax, edi
00452cbc  je         0x452ce4

// block 00452cbe
00452cbe  and        dword ptr [eax + 4], 0xfffffffe
00452cc2  mov        dword ptr [eax + 8], edi
00452cc5  mov        dword ptr [eax + 0xc], edi
00452cc8  mov        dword ptr [eax + 0x10], edi
00452ccb  mov        dword ptr [eax], edi
00452ccd  mov        esi, eax
00452ccf  mov        dword ptr [eax + 0x14], eax
00452cd2  mov        dword ptr [eax + 0x18], edi
00452cd5  mov        dword ptr [eax + 0x1c], edi
00452cd8  mov        dword ptr [esi + 8], 0x452680
00452cdf  jmp        0x452e01

// block 00452ce4
00452ce4  xor        esi, esi
00452ce6  mov        dword ptr [esi + 8], 0x452680
00452ced  jmp        0x452e01

// block 00452cf2
00452cf2  push       0x24
00452cf4  call       0x46c9ea

// block 00452cf9
00452cf9  add        esp, 4
00452cfc  cmp        eax, edi
00452cfe  je         0x452d1c

// block 00452d00
00452d00  and        dword ptr [eax + 4], 0xfffffffe
00452d04  mov        dword ptr [eax + 8], edi
00452d07  mov        dword ptr [eax + 0xc], edi
00452d0a  mov        dword ptr [eax + 0x10], edi
00452d0d  mov        dword ptr [eax], edi
00452d0f  mov        dword ptr [eax + 0x14], eax
00452d12  mov        dword ptr [eax + 0x18], edi
00452d15  mov        dword ptr [eax + 0x1c], edi
00452d18  mov        esi, eax
00452d1a  jmp        0x452d1e

// block 00452d1c
00452d1c  xor        esi, esi

// block 00452d1e
00452d1e  or         dword ptr [esi + 4], 3
00452d22  mov        ebx, 0xe
00452d27  mov        dword ptr [esi + 8], 0x4524c0
00452d2e  mov        dword ptr [esi + 0xc], edi
00452d31  mov        dword ptr [esi + 0x10], edi
00452d34  mov        dword ptr [esi + 0x20], ebp
00452d37  call       0x462380

// block 00452d3c
00452d3c  push       0x24
00452d3e  mov        dword ptr [ebp + 8], esi
00452d41  call       0x46c9ea

// block 00452d46
00452d46  add        esp, 4
00452d49  cmp        eax, edi
00452d4b  je         0x452d73

// block 00452d4d
00452d4d  and        dword ptr [eax + 4], 0xfffffffe
00452d51  mov        dword ptr [eax + 8], edi
00452d54  mov        dword ptr [eax + 0xc], edi
00452d57  mov        dword ptr [eax + 0x10], edi
00452d5a  mov        dword ptr [eax], edi
00452d5c  mov        esi, eax
00452d5e  mov        dword ptr [eax + 0x14], eax
00452d61  mov        dword ptr [eax + 0x18], edi
00452d64  mov        dword ptr [eax + 0x1c], edi
00452d67  mov        dword ptr [esi + 8], 0x452540
00452d6e  jmp        0x452e01

// block 00452d73
00452d73  xor        esi, esi
00452d75  mov        dword ptr [esi + 8], 0x452540
00452d7c  jmp        0x452e01

// block 00452d81
00452d81  push       0x24
00452d83  call       0x46c9ea

// block 00452d88
00452d88  add        esp, 4
00452d8b  cmp        eax, edi
00452d8d  je         0x452dab

// block 00452d8f
00452d8f  and        dword ptr [eax + 4], 0xfffffffe
00452d93  mov        dword ptr [eax + 8], edi
00452d96  mov        dword ptr [eax + 0xc], edi
00452d99  mov        dword ptr [eax + 0x10], edi
00452d9c  mov        dword ptr [eax], edi
00452d9e  mov        dword ptr [eax + 0x14], eax
00452da1  mov        dword ptr [eax + 0x18], edi
00452da4  mov        dword ptr [eax + 0x1c], edi
00452da7  mov        esi, eax
00452da9  jmp        0x452dad

// block 00452dab
00452dab  xor        esi, esi

// block 00452dad
00452dad  or         dword ptr [esi + 4], 3
00452db1  mov        ebx, 0xe
00452db6  mov        dword ptr [esi + 8], 0x4524c0
00452dbd  mov        dword ptr [esi + 0xc], edi
00452dc0  mov        dword ptr [esi + 0x10], edi
00452dc3  mov        dword ptr [esi + 0x20], ebp
00452dc6  call       0x462380

// block 00452dcb
00452dcb  push       0x24
00452dcd  mov        dword ptr [ebp + 8], esi
00452dd0  call       0x46c9ea

// block 00452dd5
00452dd5  add        esp, 4
00452dd8  cmp        eax, edi
00452dda  je         0x452df8

// block 00452ddc
00452ddc  and        dword ptr [eax + 4], 0xfffffffe
00452de0  mov        dword ptr [eax + 8], edi
00452de3  mov        dword ptr [eax + 0xc], edi
00452de6  mov        dword ptr [eax + 0x10], edi
00452de9  mov        dword ptr [eax], edi
00452deb  mov        dword ptr [eax + 0x14], eax
00452dee  mov        dword ptr [eax + 0x18], edi
00452df1  mov        dword ptr [eax + 0x1c], edi
00452df4  mov        esi, eax
00452df6  jmp        0x452dfa

// block 00452df8
00452df8  xor        esi, esi

// block 00452dfa
00452dfa  mov        dword ptr [esi + 8], 0x452580

// block 00452e01
00452e01  mov        ebx, dword ptr [esp + 0x2c]
00452e05  mov        dword ptr [esi + 0xc], edi
00452e08  mov        dword ptr [esi + 0x10], edi
00452e0b  mov        dword ptr [esi + 0x20], ebp
00452e0e  or         dword ptr [esi + 4], 3
00452e12  call       0x462420

// block 00452e17
00452e17  mov        dword ptr [ebp + 0xc], esi
00452e1a  jmp        0x452e69

// block 00452e1c
00452e1c  push       0x24
00452e1e  call       0x46c9ea

// block 00452e23
00452e23  add        esp, 4
00452e26  cmp        eax, edi
00452e28  je         0x452e46

// block 00452e2a
00452e2a  and        dword ptr [eax + 4], 0xfffffffe
00452e2e  mov        dword ptr [eax + 8], edi
00452e31  mov        dword ptr [eax + 0xc], edi
00452e34  mov        dword ptr [eax + 0x10], edi
00452e37  mov        dword ptr [eax], edi
00452e39  mov        dword ptr [eax + 0x14], eax
00452e3c  mov        dword ptr [eax + 0x18], edi
00452e3f  mov        dword ptr [eax + 0x1c], edi
00452e42  mov        esi, eax
00452e44  jmp        0x452e48

// block 00452e46
00452e46  xor        esi, esi

// block 00452e48
00452e48  mov        dword ptr [esi + 8], 0x4527f0

// block 00452e4f
00452e4f  mov        dword ptr [esi + 0xc], edi
00452e52  mov        dword ptr [esi + 0x10], edi
00452e55  mov        dword ptr [esi + 0x20], ebp
00452e58  or         dword ptr [esi + 4], 3
00452e5c  mov        ebx, 0xe
00452e61  call       0x462380

// block 00452e66
00452e66  mov        dword ptr [ebp + 8], esi

// block 00452e69
00452e69  pop        esi
00452e6a  pop        ebx

// block 00452e6b
00452e6b  mov        eax, dword ptr [ebp + 8]
00452e6e  fldz       
00452e70  mov        dword ptr [eax + 0x10], 0x452960
00452e77  mov        eax, dword ptr [ebp + 0x40]
00452e7a  test       al, 1
00452e7c  jne        0x452e98

// block 00452e7e
00452e7e  or         eax, 1
00452e81  fst        dword ptr [ebp + 0x38]
00452e84  mov        dword ptr [ebp + 0x34], edi
00452e87  mov        dword ptr [ebp + 0x30], 0xfff0bdc1
00452e8e  mov        dword ptr [ebp + 0x3c], 0x4b2ed0
00452e95  mov        dword ptr [ebp + 0x40], eax

// block 00452e98
00452e98  mov        ecx, dword ptr [esp + 0x10]
00452e9c  fstp       dword ptr [ebp + 0x38]
00452e9f  mov        edx, dword ptr [esp + 0x14]
00452ea3  mov        eax, dword ptr [esp + 0x18]
00452ea7  mov        dword ptr [ebp + 0x34], edi
00452eaa  mov        dword ptr [ebp + 0x30], 0xffffffff
00452eb1  mov        dword ptr [ebp + 0x10], ecx
00452eb4  mov        ecx, dword ptr [esp + 0x1c]
00452eb8  mov        dword ptr [ebp + 0x1c], edx
00452ebb  mov        edx, dword ptr [esp + 0x20]
00452ebf  pop        edi
00452ec0  mov        dword ptr [ebp + 0x20], eax
00452ec3  mov        dword ptr [ebp + 0x24], ecx
00452ec6  mov        dword ptr [ebp + 0x28], edx
00452ec9  pop        ebp
00452eca  ret        0x1c
