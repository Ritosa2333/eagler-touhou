
// block 004489e0
004489e0  sub        esp, 0x48
004489e3  mov        eax, dword ptr [0x4ad138]
004489e8  xor        eax, esp
004489ea  mov        dword ptr [esp + 0x44], eax
004489ee  push       edi
004489ef  mov        edi, ecx
004489f1  mov        eax, dword ptr [edi + 0x24]
004489f4  cmp        eax, 4
004489f7  ja         0x448f82

// block 004489fd
004489fd  push       ebx
004489fe  push       ebp
004489ff  push       esi
00448a00  jmp        dword ptr [eax*4 + 0x448f98]

// block 00448a07
00448a07  mov        dword ptr [edi + 0x30], 0x19
00448a0e  mov        dword ptr [edi + 0xf8], 1
00448a18  mov        eax, dword ptr [edi + 0x30]
00448a1b  test       eax, eax
00448a1d  je         0x448a2e

// block 00448a1f
00448a1f  jg         0x448a27

// block 00448a21
00448a21  dec        eax
00448a22  mov        dword ptr [edi + 0x28], eax
00448a25  jmp        0x448a35

// block 00448a27
00448a27  xor        eax, eax
00448a29  mov        dword ptr [edi + 0x28], eax
00448a2c  jmp        0x448a35

// block 00448a2e
00448a2e  mov        dword ptr [edi + 0x28], 0

// block 00448a35
00448a35  mov        eax, 8
00448a3a  mov        dword ptr [0x4b452c], 0x4aedf0
00448a44  mov        dword ptr [0x4b0cb0], eax
00448a49  mov        dword ptr [0x4b0cb4], eax
00448a4e  mov        esi, 1
00448a53  lea        ebx, [edi + 0x5a80]
00448a59  lea        esp, [esp]

// block 00448a60
00448a60  push       esi
00448a61  lea        eax, [esp + 0x18]
00448a65  push       0x4a1024
00448a6a  push       eax
00448a6b  call       0x46cbbb

// block 00448a70
00448a70  add        esp, 0xc
00448a73  lea        ecx, [esp + 0x14]
00448a77  push       ecx
00448a78  call       0x43b6f0

// block 00448a7d
00448a7d  mov        dword ptr [ebx], eax
00448a7f  inc        esi
00448a80  add        ebx, 4
00448a83  cmp        esi, 0x19
00448a86  jle        0x448a60

// block 00448a88
00448a88  mov        edx, dword ptr [edi + 0x454]
00448a8e  push       edx
00448a8f  mov        edx, dword ptr [0x4ce8cc]
00448a95  call       0x461920

// block 00448a9a
00448a9a  test       eax, eax
00448a9c  jne        0x448acb

// block 00448a9e
00448a9e  mov        ecx, dword ptr [edi + 0x14]
00448aa1  push       eax
00448aa2  push       0x63
00448aa4  lea        eax, [esp + 0x18]
00448aa8  push       eax
00448aa9  push       ecx
00448aaa  mov        eax, 0x17
00448aaf  xor        ecx, ecx
00448ab1  call       0x4615a0

// block 00448ab6
00448ab6  mov        eax, dword ptr [esp + 0x10]
00448aba  push       eax
00448abb  mov        ebx, 3
00448ac0  mov        dword ptr [edi + 0x454], eax
00448ac6  call       0x4619e0

// block 00448acb
00448acb  mov        eax, dword ptr [edi + 0x14]
00448ace  push       0
00448ad0  push       0x75
00448ad2  lea        edx, [esp + 0x18]
00448ad6  push       edx
00448ad7  push       eax
00448ad8  mov        eax, 0x17
00448add  xor        ecx, ecx
00448adf  call       0x4615a0

// block 00448ae4
00448ae4  mov        ecx, dword ptr [esp + 0x10]
00448ae8  mov        dword ptr [edi + 0x49c], ecx
00448aee  mov        ecx, 1
00448af3  mov        eax, edi
00448af5  call       0x43ef40

// block 00448afa
00448afa  cmp        dword ptr [edi + 0x2b8], 6
00448b01  jle        0x448f7f

// block 00448b07
00448b07  mov        ecx, 2
00448b0c  mov        eax, edi
00448b0e  call       0x43ef40

// block 00448b13
00448b13  jmp        0x448f7f

// block 00448b18
00448b18  mov        edx, dword ptr [edi + 0x28]
00448b1b  lea        esi, [edi + 0x28]
00448b1e  mov        al, 0x10
00448b20  mov        dword ptr [esi + 4], edx
00448b23  test       byte ptr [0x4d48c4], al
00448b29  jne        0x448b33

// block 00448b2b
00448b2b  test       byte ptr [0x4d48c0], al
00448b31  je         0x448b3c

// block 00448b33
00448b33  push       -1
00448b35  mov        eax, esi
00448b37  call       0x464970

// block 00448b3c
00448b3c  mov        bl, 0x20
00448b3e  test       byte ptr [0x4d48c4], bl
00448b44  jne        0x448b4e

// block 00448b46
00448b46  test       byte ptr [0x4d48c0], bl
00448b4c  je         0x448b57

// block 00448b4e
00448b4e  push       1
00448b50  mov        eax, esi
00448b52  call       0x464970

// block 00448b57
00448b57  mov        eax, dword ptr [esi + 4]
00448b5a  cmp        eax, dword ptr [esi]
00448b5c  je         0x448b68

// block 00448b5e
00448b5e  mov        edx, 0xa
00448b63  call       0x453d90

// block 00448b68
00448b68  mov        eax, dword ptr [0x4d48c4]
00448b6d  test       eax, 0x102
00448b72  je         0x448b8f

// block 00448b74
00448b74  mov        ecx, 4
00448b79  mov        eax, edi
00448b7b  call       0x43ef40

// block 00448b80
00448b80  mov        edx, 9
00448b85  call       0x453d90

// block 00448b8a
00448b8a  jmp        0x448f7f

// block 00448b8f
00448b8f  test       eax, 0x80001
00448b94  je         0x448f7f

// block 00448b9a
00448b9a  mov        ecx, dword ptr [esi]
00448b9c  lea        ebp, [edi + 0x5990]
00448ba2  mov        dword ptr [edi + 0x5a78], ecx
00448ba8  mov        eax, dword ptr [ebp + 8]
00448bab  test       eax, eax
00448bad  je         0x448bbe

// block 00448baf
00448baf  jg         0x448bb7

// block 00448bb1
00448bb1  dec        eax
00448bb2  mov        dword ptr [ebp], eax
00448bb5  jmp        0x448bc5

// block 00448bb7
00448bb7  xor        eax, eax
00448bb9  mov        dword ptr [ebp], eax
00448bbc  jmp        0x448bc5

// block 00448bbe
00448bbe  mov        dword ptr [ebp], 0

// block 00448bc5
00448bc5  mov        esi, dword ptr [0x4b4518]
00448bcb  mov        dword ptr [edi + 0x5998], 0x5b
00448bd5  mov        dword ptr [edi + 0x5a60], 1
00448bdf  mov        edx, dword ptr [esi + 0x1c]
00448be2  add        esi, 0x1c
00448be5  add        edx, 0xc
00448be8  push       edx
00448be9  call       0x46d5b7

// block 00448bee
00448bee  mov        eax, dword ptr [esi]
00448bf0  mov        dword ptr [eax + 0x68], 8
00448bf7  mov        eax, dword ptr [0x4b451c]
00448bfc  lea        esi, [edi + 0x5978]
00448c02  add        eax, 0x1e9c0
00448c07  mov        edx, esi
00448c09  add        esp, 4
00448c0c  sub        edx, eax
00448c0e  mov        edi, edi

// block 00448c10
00448c10  mov        cl, byte ptr [eax]
00448c12  mov        byte ptr [eax + edx], cl
00448c15  inc        eax
00448c16  test       cl, cl
00448c18  jne        0x448c10

// block 00448c1a
00448c1a  mov        dword ptr [edi + 0x5984], 0
00448c24  mov        ecx, 0x4a0ee4
00448c29  mov        eax, esi
00448c2b  jmp        0x448c30

// block 00448c30
00448c30  mov        dl, byte ptr [eax]
00448c32  cmp        dl, byte ptr [ecx]
00448c34  jne        0x448c50

// block 00448c36
00448c36  test       dl, dl
00448c38  je         0x448c4c

// block 00448c3a
00448c3a  mov        dl, byte ptr [eax + 1]
00448c3d  cmp        dl, byte ptr [ecx + 1]
00448c40  jne        0x448c50

// block 00448c42
00448c42  add        eax, 2
00448c45  add        ecx, 2
00448c48  test       dl, dl
00448c4a  jne        0x448c30

// block 00448c4c
00448c4c  xor        eax, eax
00448c4e  jmp        0x448c55

// block 00448c50
00448c50  sbb        eax, eax
00448c52  sbb        eax, -1

// block 00448c55
00448c55  test       eax, eax
00448c57  je         0x448c62

// block 00448c59
00448c59  push       -1
00448c5b  mov        eax, ebp
00448c5d  call       0x464970

// block 00448c62
00448c62  mov        eax, 8

// block 00448c67
00448c67  cmp        byte ptr [edi + eax + 0x5977], bl
00448c6e  jne        0x448c75

// block 00448c70
00448c70  dec        eax
00448c71  test       eax, eax
00448c73  jg         0x448c67

// block 00448c75
00448c75  mov        edx, 7
00448c7a  mov        dword ptr [edi + 0x5984], eax
00448c80  call       0x453d90

// block 00448c85
00448c85  mov        ecx, 3
00448c8a  mov        eax, edi
00448c8c  call       0x43ef40

// block 00448c91
00448c91  jmp        0x448f7f

// block 00448c96
00448c96  mov        ecx, dword ptr [edi + 0x5990]
00448c9c  lea        esi, [edi + 0x5990]
00448ca2  mov        al, 0x10
00448ca4  mov        dword ptr [esi + 4], ecx
00448ca7  test       byte ptr [0x4d48c4], al
00448cad  jne        0x448cb7

// block 00448caf
00448caf  test       byte ptr [0x4d48c0], al
00448cb5  je         0x448cc0

// block 00448cb7
00448cb7  push       -0xd
00448cb9  mov        eax, esi
00448cbb  call       0x464970

// block 00448cc0
00448cc0  mov        bl, 0x20
00448cc2  test       byte ptr [0x4d48c4], bl
00448cc8  jne        0x448cd2

// block 00448cca
00448cca  test       byte ptr [0x4d48c0], bl
00448cd0  je         0x448cdb

// block 00448cd2
00448cd2  push       0xd
00448cd4  mov        eax, esi
00448cd6  call       0x464970

// block 00448cdb
00448cdb  mov        al, 0x40
00448cdd  test       byte ptr [0x4d48c4], al
00448ce3  jne        0x448ced

// block 00448ce5
00448ce5  test       byte ptr [0x4d48c0], al
00448ceb  je         0x448d14

// block 00448ced
00448ced  mov        ecx, dword ptr [esi]
00448cef  mov        eax, 0x4ec4ec4f
00448cf4  imul       ecx
00448cf6  sar        edx, 2
00448cf9  mov        eax, edx
00448cfb  shr        eax, 0x1f
00448cfe  add        eax, edx
00448d00  imul       eax, eax, 0xd
00448d03  sub        ecx, eax
00448d05  mov        eax, esi
00448d07  je         0x448d0d

// block 00448d09
00448d09  push       -1
00448d0b  jmp        0x448d0f

// block 00448d0d
00448d0d  push       0xc

// block 00448d0f
00448d0f  call       0x464970

// block 00448d14
00448d14  mov        al, 0x80
00448d16  test       byte ptr [0x4d48c4], al
00448d1c  jne        0x448d26

// block 00448d1e
00448d1e  test       byte ptr [0x4d48c0], al
00448d24  je         0x448d50

// block 00448d26
00448d26  mov        ecx, dword ptr [esi]
00448d28  mov        eax, 0x4ec4ec4f
00448d2d  imul       ecx
00448d2f  sar        edx, 2
00448d32  mov        eax, edx
00448d34  shr        eax, 0x1f
00448d37  add        eax, edx
00448d39  imul       eax, eax, 0xd
00448d3c  sub        ecx, eax
00448d3e  mov        eax, esi
00448d40  cmp        ecx, 0xc
00448d43  je         0x448d49

// block 00448d45
00448d45  push       1
00448d47  jmp        0x448d4b

// block 00448d49
00448d49  push       -0xc

// block 00448d4b
00448d4b  call       0x464970

// block 00448d50
00448d50  mov        ecx, dword ptr [esi + 4]
00448d53  cmp        ecx, dword ptr [esi]
00448d55  je         0x448d61

// block 00448d57
00448d57  mov        edx, 0xa
00448d5c  call       0x453d90

// block 00448d61
00448d61  test       dword ptr [0x4d48c4], 0x80001
00448d6b  je         0x448e98

// block 00448d71
00448d71  mov        eax, dword ptr [esi]
00448d73  cmp        eax, 0x58
00448d76  jge        0x448dc6

// block 00448d78
00448d78  mov        ecx, dword ptr [edi + 0x5984]
00448d7e  cmp        ecx, 8
00448d81  jge        0x448db4

// block 00448d83
00448d83  mov        dl, byte ptr [eax + 0x4a0e88]
00448d89  mov        byte ptr [ecx + edi + 0x5978], dl

// block 00448d90
00448d90  inc        dword ptr [edi + 0x5984]
00448d96  cmp        dword ptr [edi + 0x5984], 8
00448d9d  jl         0x448e8e

// block 00448da3
00448da3  mov        ecx, 0x5a
00448da8  mov        edx, esi
00448daa  call       0x40f790

// block 00448daf
00448daf  jmp        0x448e8e

// block 00448db4
00448db4  mov        al, byte ptr [eax + 0x4a0e88]
00448dba  mov        byte ptr [ecx + edi + 0x5977], al
00448dc1  jmp        0x448e8e

// block 00448dc6
00448dc6  jne        0x448de8

// block 00448dc8
00448dc8  mov        eax, dword ptr [edi + 0x5984]
00448dce  cmp        eax, 8
00448dd1  jge        0x448ddc

// block 00448dd3
00448dd3  mov        byte ptr [eax + edi + 0x5978], bl
00448dda  jmp        0x448d90

// block 00448ddc
00448ddc  mov        byte ptr [eax + edi + 0x5977], bl
00448de3  jmp        0x448e8e

// block 00448de8
00448de8  cmp        eax, 0x59
00448deb  jne        0x448e0e

// block 00448ded
00448ded  mov        eax, dword ptr [edi + 0x5984]
00448df3  test       eax, eax
00448df5  je         0x448f7f

// block 00448dfb
00448dfb  dec        eax
00448dfc  mov        dword ptr [edi + 0x5984], eax
00448e02  mov        byte ptr [eax + edi + 0x5978], bl
00448e09  jmp        0x448e8e

// block 00448e0e
00448e0e  cmp        eax, 0x5a
00448e11  jne        0x448e8e

// block 00448e13
00448e13  lea        edx, [eax - 0x48]
00448e16  call       0x453d90

// block 00448e1b
00448e1b  mov        ecx, dword ptr [edi + 0x28]
00448e1e  inc        ecx
00448e1f  push       ecx
00448e20  lea        edx, [esp + 0x18]
00448e24  push       0x4a1024
00448e29  push       edx
00448e2a  call       0x46cbbb

// block 00448e2f
00448e2f  mov        eax, dword ptr [edi + 0x28]
00448e32  mov        esi, dword ptr [edi + eax*4 + 0x5a80]
00448e39  add        esp, 0xc
00448e3c  call       0x43b7c0

// block 00448e41
00448e41  lea        esi, [edi + 0x5978]
00448e47  push       0
00448e49  mov        edx, esi
00448e4b  lea        ecx, [esp + 0x18]
00448e4f  call       0x43bc10

// block 00448e54
00448e54  mov        ebp, dword ptr [edi + 0x28]
00448e57  lea        ecx, [esp + 0x14]
00448e5b  push       ecx
00448e5c  call       0x43b6f0

// block 00448e61
00448e61  mov        edx, dword ptr [0x4b451c]
00448e67  add        edx, 0x1e9c0
00448e6d  mov        dword ptr [edi + ebp*4 + 0x5a80], eax
00448e74  mov        eax, esi
00448e76  sub        edx, esi

// block 00448e78
00448e78  mov        cl, byte ptr [eax]
00448e7a  mov        byte ptr [eax + edx], cl
00448e7d  inc        eax
00448e7e  test       cl, cl
00448e80  jne        0x448e78

// block 00448e82
00448e82  mov        ecx, 2
00448e87  mov        eax, edi
00448e89  call       0x43ef40

// block 00448e8e
00448e8e  mov        edx, 7
00448e93  call       0x453d90

// block 00448e98
00448e98  test       dword ptr [0x4d48c4], 0x102
00448ea2  je         0x448f7f

// block 00448ea8
00448ea8  cmp        dword ptr [edi + 0x5984], 0
00448eaf  je         0x448b07

// block 00448eb5
00448eb5  mov        edx, 9
00448eba  call       0x453d90

// block 00448ebf
00448ebf  dec        dword ptr [edi + 0x5984]
00448ec5  mov        eax, dword ptr [edi + 0x5984]
00448ecb  mov        byte ptr [eax + edi + 0x5978], bl
00448ed2  jmp        0x448f7f

// block 00448ed7
00448ed7  cmp        dword ptr [edi + 0x2b8], 6
00448ede  jl         0x448f7f

// block 00448ee4
00448ee4  mov        esi, 0x75
00448ee9  call       0x43efd0

// block 00448eee
00448eee  mov        edx, dword ptr [edi + 0x66c]
00448ef4  push       edx
00448ef5  lea        ebx, [esi - 0x74]
00448ef8  call       0x461970

// block 00448efd
00448efd  mov        edx, ebx
00448eff  mov        eax, edi
00448f01  mov        dword ptr [edi + 0x66c], 0
00448f0b  call       0x43eee0

// block 00448f10
00448f10  lea        eax, [edi + 0x28]
00448f13  call       0x464940

// block 00448f18
00448f18  mov        esi, dword ptr [0x4b4518]
00448f1e  test       esi, esi
00448f20  je         0x448f31

// block 00448f22
00448f22  push       esi
00448f23  call       0x43b450

// block 00448f28
00448f28  push       esi
00448f29  call       0x46ca4f

// block 00448f2e
00448f2e  add        esp, 4

// block 00448f31
00448f31  push       0x4a1f48
00448f36  push       0
00448f38  call       0x4300d0

// block 00448f3d
00448f3d  push       0
00448f3f  push       0
00448f41  call       0x430150

// block 00448f46
00448f46  lea        ebp, [edi + 0x5a80]
00448f4c  mov        esi, ebp
00448f4e  mov        ebx, 0x19

// block 00448f53
00448f53  mov        edi, dword ptr [esi]
00448f55  test       edi, edi
00448f57  je         0x448f68

// block 00448f59
00448f59  push       edi
00448f5a  call       0x43b450

// block 00448f5f
00448f5f  push       edi
00448f60  call       0x46ca4f

// block 00448f65
00448f65  add        esp, 4

// block 00448f68
00448f68  add        esi, 4
00448f6b  sub        ebx, 1
00448f6e  jne        0x448f53

// block 00448f70
00448f70  push       0x190
00448f75  push       ebx
00448f76  push       ebp
00448f77  call       0x477420

// block 00448f7c
00448f7c  add        esp, 0xc

// block 00448f7f
00448f7f  pop        esi
00448f80  pop        ebp
00448f81  pop        ebx

// block 00448f82
00448f82  mov        ecx, dword ptr [esp + 0x48]
00448f86  pop        edi
00448f87  xor        ecx, esp
00448f89  mov        eax, 1
00448f8e  call       0x46c932

// block 00448f93
00448f93  add        esp, 0x48
00448f96  ret        
