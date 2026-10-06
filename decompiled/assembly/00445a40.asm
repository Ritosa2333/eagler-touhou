
// block 00445a40
00445a40  push       ecx
00445a41  push       ebx
00445a42  mov        ebx, dword ptr [0x4b0ca8]
00445a48  sub        ebx, 4
00445a4b  neg        ebx
00445a4d  sbb        ebx, ebx
00445a4f  push       ebp
00445a50  mov        ebp, dword ptr [esp + 0x10]
00445a54  mov        eax, dword ptr [ebp + 0x24]
00445a57  and        ebx, 0xfffffffd
00445a5a  add        ebx, 0xb2
00445a60  mov        dword ptr [esp + 0x10], ebx
00445a64  cmp        eax, 4
00445a67  ja         0x445f92

// block 00445a6d
00445a6d  push       esi
00445a6e  push       edi
00445a6f  jmp        dword ptr [eax*4 + 0x445fa0]

// block 00445a76
00445a76  mov        dword ptr [ebp + 0x30], 2
00445a7d  cmp        dword ptr [0x4b0ca8], 4
00445a84  jne        0x445b28

// block 00445a8a
00445a8a  mov        ecx, dword ptr [0x4b451c]
00445a90  mov        eax, dword ptr [0x4b0c90]
00445a95  mov        dl, 0x10
00445a97  test       byte ptr [ecx + eax*2 + 0x1e9d0], dl
00445a9e  jne        0x445ae0

// block 00445aa0
00445aa0  cmp        dword ptr [ebp + 0x28], 0
00445aa4  jne        0x445ac9

// block 00445aa6
00445aa6  mov        eax, dword ptr [ebp + 0x30]
00445aa9  test       eax, eax
00445aab  je         0x445ac2

// block 00445aad
00445aad  cmp        eax, 1
00445ab0  jg         0x445ab8

// block 00445ab2
00445ab2  dec        eax
00445ab3  mov        dword ptr [ebp + 0x28], eax
00445ab6  jmp        0x445ac9

// block 00445ab8
00445ab8  mov        eax, 1
00445abd  mov        dword ptr [ebp + 0x28], eax
00445ac0  jmp        0x445ac9

// block 00445ac2
00445ac2  mov        dword ptr [ebp + 0x28], 1

// block 00445ac9
00445ac9  mov        eax, dword ptr [ebp + 0xfc]
00445acf  mov        dword ptr [ebp + eax*4 + 0xb8], 0
00445ada  inc        dword ptr [ebp + 0xfc]

// block 00445ae0
00445ae0  mov        eax, dword ptr [0x4b0c90]
00445ae5  test       byte ptr [ecx + eax*2 + 0x1e9d1], dl
00445aec  jne        0x445b28

// block 00445aee
00445aee  mov        ecx, 1
00445af3  cmp        dword ptr [ebp + 0x28], ecx
00445af6  jne        0x445b15

// block 00445af8
00445af8  mov        eax, dword ptr [ebp + 0x30]
00445afb  test       eax, eax
00445afd  je         0x445b0e

// block 00445aff
00445aff  jg         0x445b07

// block 00445b01
00445b01  dec        eax
00445b02  mov        dword ptr [ebp + 0x28], eax
00445b05  jmp        0x445b15

// block 00445b07
00445b07  xor        eax, eax
00445b09  mov        dword ptr [ebp + 0x28], eax
00445b0c  jmp        0x445b15

// block 00445b0e
00445b0e  mov        dword ptr [ebp + 0x28], 0

// block 00445b15
00445b15  mov        edx, dword ptr [ebp + 0xfc]
00445b1b  mov        dword ptr [ebp + edx*4 + 0xb8], ecx
00445b22  add        dword ptr [ebp + 0xfc], ecx

// block 00445b28
00445b28  mov        ecx, dword ptr [ebp + 0x14]
00445b2b  push       0
00445b2d  push       0x70
00445b2f  lea        eax, [esp + 0x18]
00445b33  push       eax
00445b34  push       ecx
00445b35  mov        eax, 0x17
00445b3a  xor        ecx, ecx
00445b3c  call       0x4615a0

// block 00445b41
00445b41  mov        edx, dword ptr [esp + 0x10]
00445b45  mov        dword ptr [ebp + 0x488], edx
00445b4b  mov        eax, dword ptr [0x4b0c90]
00445b50  lea        esi, [eax + ebx]
00445b53  mov        ecx, dword ptr [ebp + esi*4 + 0x2c8]
00445b5a  push       ecx
00445b5b  mov        ebx, 1
00445b60  call       0x461970

// block 00445b65
00445b65  mov        ebx, dword ptr [esp + 0x18]
00445b69  mov        dword ptr [ebp + esi*4 + 0x2c8], 0
00445b74  mov        edx, dword ptr [0x4b0c90]
00445b7a  lea        esi, [edx + ebx]
00445b7d  mov        edi, ebp
00445b7f  call       0x43efa0

// block 00445b84
00445b84  mov        eax, dword ptr [0x4b0c90]
00445b89  add        eax, ebx
00445b8b  mov        ecx, dword ptr [ebp + eax*4 + 0x2c8]
00445b92  push       ecx
00445b93  mov        ebx, 3
00445b98  call       0x4619e0

// block 00445b9d
00445b9d  mov        edx, dword ptr [0x4b0c90]
00445ba3  mov        esi, dword ptr [esp + 0x18]
00445ba7  movzx      ebx, word ptr [ebp + 0x28]
00445bab  add        edx, esi
00445bad  mov        eax, dword ptr [ebp + edx*4 + 0x2c8]
00445bb4  add        bx, 0x11
00445bb8  push       eax
00445bb9  call       0x461970

// block 00445bbe
00445bbe  mov        ecx, 1
00445bc3  mov        eax, ebp
00445bc5  call       0x43ef40

// block 00445bca
00445bca  mov        eax, dword ptr [0x4b0c90]
00445bcf  mov        ebx, dword ptr [0x4b451c]
00445bd5  mov        ecx, eax
00445bd7  imul       ecx, ecx, 0x22fa
00445bdd  add        ecx, dword ptr [0x4b0ca8]
00445be3  cmp        dword ptr [ebx + ecx*4 + 0x598], 0
00445beb  jne        0x445c51

// block 00445bed
00445bed  add        eax, esi
00445bef  mov        edx, dword ptr [ebp + eax*4 + 0x2c8]
00445bf6  lea        edi, [ebp + eax*4 + 0x2c8]
00445bfd  push       edx
00445bfe  mov        edx, dword ptr [0x4ce8cc]
00445c04  call       0x461920

// block 00445c09
00445c09  test       eax, eax
00445c0b  jne        0x445c0f

// block 00445c0d
00445c0d  mov        dword ptr [edi], eax

// block 00445c0f
00445c0f  add        eax, 0x10
00445c12  je         0x445c3b

// block 00445c14
00445c14  jmp        0x445c20

// block 00445c20
00445c20  mov        ecx, dword ptr [eax]
00445c22  mov        edx, 0xa8
00445c27  cmp        word ptr [ecx + 0x3ea], dx
00445c2e  je         0x445cde

// block 00445c34
00445c34  mov        eax, dword ptr [eax + 4]
00445c37  test       eax, eax
00445c39  jne        0x445c20

// block 00445c3b
00445c3b  mov        dword ptr [esp + 0x18], 0

// block 00445c43
00445c43  lea        eax, [esp + 0x18]
00445c47  call       0x461d80

// block 00445c4c
00445c4c  mov        eax, dword ptr [0x4b0c90]

// block 00445c51
00445c51  mov        edx, eax
00445c53  imul       edx, edx, 0x22fa
00445c59  add        edx, dword ptr [0x4b0ca8]
00445c5f  cmp        dword ptr [ebx + edx*4 + 0x4b8c], 0
00445c67  jne        0x445cb8

// block 00445c69
00445c69  mov        edx, dword ptr [0x4ce8cc]
00445c6f  add        eax, esi
00445c71  lea        esi, [ebp + eax*4 + 0x2c8]
00445c78  mov        eax, dword ptr [esi]
00445c7a  push       eax
00445c7b  call       0x461920

// block 00445c80
00445c80  test       eax, eax
00445c82  jne        0x445c86

// block 00445c84
00445c84  mov        dword ptr [esi], eax

// block 00445c86
00445c86  add        eax, 0x10
00445c89  je         0x445ca7

// block 00445c8b
00445c8b  jmp        0x445c90

// block 00445c90
00445c90  mov        ecx, dword ptr [eax]
00445c92  mov        edx, 0xa9
00445c97  cmp        word ptr [ecx + 0x3ea], dx
00445c9e  je         0x445ceb

// block 00445ca0
00445ca0  mov        eax, dword ptr [eax + 4]
00445ca3  test       eax, eax
00445ca5  jne        0x445c90

// block 00445ca7
00445ca7  mov        dword ptr [esp + 0x18], 0

// block 00445caf
00445caf  lea        eax, [esp + 0x18]
00445cb3  call       0x461d80

// block 00445cb8
00445cb8  cmp        dword ptr [ebp + 0x2b8], 6
00445cbf  jle        0x445f90

// block 00445cc5
00445cc5  mov        ecx, 2
00445cca  mov        eax, ebp
00445ccc  call       0x43ef40

// block 00445cd1
00445cd1  pop        edi
00445cd2  pop        esi
00445cd3  pop        ebp
00445cd4  mov        eax, 1
00445cd9  pop        ebx
00445cda  pop        ecx
00445cdb  ret        4

// block 00445cde
00445cde  mov        eax, ecx
00445ce0  mov        ecx, dword ptr [eax]
00445ce2  mov        dword ptr [esp + 0x18], ecx
00445ce6  jmp        0x445c43

// block 00445ceb
00445ceb  mov        eax, ecx
00445ced  mov        ecx, dword ptr [eax]
00445cef  mov        dword ptr [esp + 0x18], ecx
00445cf3  jmp        0x445caf

// block 00445cf5
00445cf5  mov        edx, dword ptr [ebp + 0x28]
00445cf8  lea        esi, [ebp + 0x28]
00445cfb  mov        al, 0x10
00445cfd  mov        dword ptr [esi + 4], edx
00445d00  test       byte ptr [0x4d48c4], al
00445d06  jne        0x445d10

// block 00445d08
00445d08  test       byte ptr [0x4d48c0], al
00445d0e  je         0x445d19

// block 00445d10
00445d10  push       -1
00445d12  mov        eax, esi
00445d14  call       0x464970

// block 00445d19
00445d19  mov        al, 0x20
00445d1b  test       byte ptr [0x4d48c4], al
00445d21  jne        0x445d2b

// block 00445d23
00445d23  test       byte ptr [0x4d48c0], al
00445d29  je         0x445d34

// block 00445d2b
00445d2b  push       1
00445d2d  mov        eax, esi
00445d2f  call       0x464970

// block 00445d34
00445d34  mov        eax, dword ptr [esi + 4]
00445d37  mov        edi, 7
00445d3c  cmp        eax, dword ptr [esi]
00445d3e  je         0x445d82

// block 00445d40
00445d40  lea        edx, [edi + 3]
00445d43  call       0x453d90

// block 00445d48
00445d48  mov        ecx, dword ptr [0x4b0c90]
00445d4e  add        ecx, ebx
00445d50  mov        edx, dword ptr [ebp + ecx*4 + 0x2c8]
00445d57  push       edx
00445d58  lea        ebx, [edi - 4]
00445d5b  call       0x4619e0

// block 00445d60
00445d60  mov        eax, dword ptr [0x4b0c90]
00445d65  mov        ecx, dword ptr [esp + 0x18]
00445d69  movzx      ebx, word ptr [esi]
00445d6c  add        eax, ecx
00445d6e  mov        edx, dword ptr [ebp + eax*4 + 0x2c8]
00445d75  add        bx, di
00445d78  push       edx
00445d79  call       0x461970

// block 00445d7e
00445d7e  mov        ebx, dword ptr [esp + 0x18]

// block 00445d82
00445d82  mov        eax, dword ptr [0x4d48c4]
00445d87  test       eax, 0x102
00445d8c  je         0x445db1

// block 00445d8e
00445d8e  mov        ecx, 4
00445d93  mov        eax, ebp
00445d95  call       0x43ef40

// block 00445d9a
00445d9a  mov        edx, 9
00445d9f  call       0x453d90

// block 00445da4
00445da4  pop        edi
00445da5  pop        esi
00445da6  pop        ebp
00445da7  mov        eax, 1
00445dac  pop        ebx
00445dad  pop        ecx
00445dae  ret        4

// block 00445db1
00445db1  test       eax, 0x80001
00445db6  je         0x445f90

// block 00445dbc
00445dbc  mov        ecx, 3
00445dc1  mov        eax, ebp
00445dc3  call       0x43ef40

// block 00445dc8
00445dc8  mov        edx, edi
00445dca  call       0x453d90

// block 00445dcf
00445dcf  mov        ecx, dword ptr [esi]
00445dd1  mov        eax, dword ptr [0x4b0c90]
00445dd6  lea        edi, [ecx + eax*4 + 0x90]
00445ddd  add        eax, ebx
00445ddf  lea        esi, [ebp + eax*4 + 0x2c8]
00445de6  lea        ebx, [esp + 0x18]
00445dea  call       0x462060

// block 00445def
00445def  mov        edx, dword ptr [esp + 0x18]
00445df3  push       edx
00445df4  mov        ebx, 6
00445df9  call       0x461970

// block 00445dfe
00445dfe  test       byte ptr [0x4b0ce0], 0x10
00445e05  jne        0x445f90

// block 00445e0b
00445e0b  fld1       
00445e0d  push       ecx
00445e0e  fstp       dword ptr [esp]
00445e11  call       0x430270

// block 00445e16
00445e16  pop        edi
00445e17  pop        esi
00445e18  pop        ebp
00445e19  lea        eax, [ebx - 5]
00445e1c  pop        ebx
00445e1d  pop        ecx
00445e1e  ret        4

// block 00445e21
00445e21  cmp        dword ptr [ebp + 0x2b8], 0xa
00445e28  jne        0x445e8e

// block 00445e2a
00445e2a  test       byte ptr [0x4b0ce0], 0x10
00445e31  jne        0x445e61

// block 00445e33
00445e33  fld        dword ptr [0x4a4260]
00445e39  sub        esp, 8
00445e3c  fstp       dword ptr [esp + 4]
00445e40  fld        dword ptr [0x4a3ec0]
00445e46  fstp       dword ptr [esp]
00445e49  call       0x411b80

// block 00445e4e
00445e4e  push       0x40
00445e50  push       0
00445e52  push       0
00445e54  push       0
00445e56  push       0x20
00445e58  push       5
00445e5a  call       0x4529a0

// block 00445e5f
00445e5f  jmp        0x445e8e

// block 00445e61
00445e61  mov        ecx, dword ptr [ebp + 0x28]
00445e64  lea        eax, [ebp + 0x28]
00445e67  mov        dword ptr [0x4b0c94], ecx
00445e6d  mov        dword ptr [0x4ce8c0], ecx
00445e73  call       0x464900

// block 00445e78
00445e78  mov        esi, 0x70
00445e7d  mov        edi, ebp
00445e7f  call       0x43efd0

// block 00445e84
00445e84  lea        edx, [esi - 0x68]
00445e87  mov        eax, ebp
00445e89  call       0x43eee0

// block 00445e8e
00445e8e  cmp        dword ptr [ebp + 0x2b8], 0x28
00445e95  jl         0x445f90

// block 00445e9b
00445e9b  mov        ecx, dword ptr [ebp + 0x28]
00445e9e  lea        eax, [ebp + 0x28]
00445ea1  mov        dword ptr [0x4b0c94], ecx
00445ea7  mov        dword ptr [0x4ce8c0], ecx
00445ead  call       0x464900

// block 00445eb2
00445eb2  test       byte ptr [0x4b0ce0], 0x10
00445eb9  jne        0x445f25

// block 00445ebb
00445ebb  mov        edx, 2
00445ec0  mov        eax, ebp
00445ec2  call       0x43eee0

// block 00445ec7
00445ec7  cmp        dword ptr [0x4b0ca8], 4
00445ece  mov        edi, 7
00445ed3  jge        0x445efc

// block 00445ed5
00445ed5  mov        dword ptr [0x4cee40], edi
00445edb  pop        edi
00445edc  pop        esi
00445edd  mov        eax, 1
00445ee2  pop        ebp
00445ee3  mov        dword ptr [0x4b452c], 0x4aec30
00445eed  mov        dword ptr [0x4b0cb0], eax
00445ef2  mov        dword ptr [0x4b0cb4], eax
00445ef7  pop        ebx
00445ef8  pop        ecx
00445ef9  ret        4

// block 00445efc
00445efc  mov        dword ptr [0x4b0cb0], edi
00445f02  mov        dword ptr [0x4b0cb4], edi
00445f08  mov        dword ptr [0x4cee40], edi
00445f0e  pop        edi
00445f0f  pop        esi
00445f10  pop        ebp
00445f11  mov        dword ptr [0x4b452c], 0x4aedb0
00445f1b  mov        eax, 1
00445f20  pop        ebx
00445f21  pop        ecx
00445f22  ret        4

// block 00445f25
00445f25  mov        esi, 0x70
00445f2a  mov        edi, ebp
00445f2c  call       0x43efd0

// block 00445f31
00445f31  lea        edx, [esi - 0x68]
00445f34  mov        eax, ebp
00445f36  call       0x43eee0

// block 00445f3b
00445f3b  pop        edi
00445f3c  pop        esi
00445f3d  pop        ebp
00445f3e  mov        eax, 1
00445f43  pop        ebx
00445f44  pop        ecx
00445f45  ret        4

// block 00445f48
00445f48  cmp        dword ptr [ebp + 0x2b8], 6
00445f4f  jl         0x445f90

// block 00445f51
00445f51  lea        eax, [ebp + 0x28]
00445f54  mov        dword ptr [esp + 0x18], eax
00445f58  mov        eax, dword ptr [eax]
00445f5a  mov        dword ptr [0x4b0c94], eax
00445f5f  mov        dword ptr [0x4ce8c0], eax
00445f64  mov        eax, dword ptr [0x4b0c90]
00445f69  lea        esi, [eax + ebx]
00445f6c  mov        edi, ebp
00445f6e  call       0x43efd0

// block 00445f73
00445f73  mov        esi, 0x70
00445f78  call       0x43efd0

// block 00445f7d
00445f7d  lea        edx, [esi - 0x6a]
00445f80  mov        eax, ebp
00445f82  call       0x43eee0

// block 00445f87
00445f87  mov        eax, dword ptr [esp + 0x18]
00445f8b  call       0x464940

// block 00445f90
00445f90  pop        edi
00445f91  pop        esi

// block 00445f92
00445f92  pop        ebp
00445f93  mov        eax, 1
00445f98  pop        ebx
00445f99  pop        ecx
00445f9a  ret        4
