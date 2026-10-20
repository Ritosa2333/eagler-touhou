
// block 00465aa0
00465aa0  push       -1
00465aa2  push       0x496e0b
00465aa7  mov        eax, dword ptr fs:[0]
00465aad  push       eax
00465aae  sub        esp, 0x4c
00465ab1  mov        eax, dword ptr [0x4ad138]
00465ab6  xor        eax, esp
00465ab8  mov        dword ptr [esp + 0x48], eax
00465abc  push       ebx
00465abd  push       ebp
00465abe  push       esi
00465abf  push       edi
00465ac0  mov        eax, dword ptr [0x4ad138]
00465ac5  xor        eax, esp
00465ac7  push       eax
00465ac8  lea        eax, [esp + 0x60]
00465acc  mov        dword ptr fs:[0], eax
00465ad2  mov        ebp, dword ptr [esp + 0x8c]
00465ad9  mov        edi, edx
00465adb  mov        edx, dword ptr [esp + 0x80]
00465ae2  mov        eax, dword ptr [esp + 0x70]
00465ae6  mov        ebx, dword ptr [esp + 0x7c]
00465aea  mov        dword ptr [esp + 0x20], ecx
00465aee  mov        ecx, dword ptr [esp + 0x84]
00465af5  mov        dword ptr [esp + 0x50], edx
00465af9  mov        edx, dword ptr [esp + 0x88]
00465b00  xor        esi, esi
00465b02  mov        dword ptr [esp + 0x1c], eax
00465b06  mov        dword ptr [esp + 0x54], ecx
00465b0a  mov        dword ptr [esp + 0x58], edx
00465b0e  cmp        dword ptr [eax], esi
00465b10  jne        0x465b1c

// block 00465b12
00465b12  mov        eax, 0x800401f0
00465b17  jmp        0x465d18

// block 00465b1c
00465b1c  push       0x9c
00465b21  mov        dword ptr [esp + 0x1c], esi
00465b25  mov        dword ptr [esp + 0x18], esi
00465b29  call       0x46c9ea

// block 00465b2e
00465b2e  add        esp, 4
00465b31  cmp        eax, esi
00465b33  je         0x465b45

// block 00465b35
00465b35  mov        dword ptr [eax + 0x90], esi
00465b3b  mov        dword ptr [eax], esi
00465b3d  mov        dword ptr [eax + 0x2c], esi
00465b40  mov        dword ptr [eax + 0x7c], esi
00465b43  mov        esi, eax

// block 00465b45
00465b45  mov        eax, dword ptr [esp + 0x78]
00465b49  mov        ecx, dword ptr [esp + 0x74]
00465b4d  mov        edx, dword ptr [esp + 0x50]
00465b51  mov        dword ptr [esi + 0x90], eax
00465b57  mov        dword ptr [esi + 0x80], edi
00465b5d  mov        dword ptr [esi + 0x84], edi
00465b63  xor        eax, eax
00465b65  mov        dword ptr [esi + 0x88], ecx
00465b6b  mov        ecx, dword ptr [esp + 0x58]
00465b6f  mov        dword ptr [esi + 0x7c], 1
00465b76  mov        dword ptr [esp + 0x28], eax
00465b7a  mov        dword ptr [esp + 0x2c], eax
00465b7e  mov        dword ptr [esp + 0x30], eax
00465b82  mov        dword ptr [esp + 0x38], eax
00465b86  mov        dword ptr [esp + 0x3c], eax
00465b8a  mov        dword ptr [esp + 0x40], eax
00465b8e  mov        dword ptr [esp + 0x44], eax
00465b92  mov        dword ptr [esp + 0x48], eax
00465b96  mov        dword ptr [esp + 0x34], eax
00465b9a  mov        eax, dword ptr [esp + 0x54]
00465b9e  mov        dword ptr [esp + 0x40], edx
00465ba2  mov        dword ptr [esp + 0x44], eax
00465ba6  mov        eax, dword ptr [esp + 0x1c]
00465baa  mov        eax, dword ptr [eax]
00465bac  mov        edi, ebp
00465bae  shl        edi, 4
00465bb1  mov        dword ptr [esp + 0x28], 0x24
00465bb9  mov        dword ptr [esp + 0x2c], 0x18188
00465bc1  mov        dword ptr [esp + 0x30], edi
00465bc5  mov        dword ptr [esp + 0x3c], ebx
00465bc9  mov        dword ptr [esp + 0x48], ecx
00465bcd  mov        edx, dword ptr [esi + 0x90]
00465bd3  add        edx, 0x20
00465bd6  mov        dword ptr [esp + 0x38], edx
00465bda  mov        ecx, dword ptr [eax]
00465bdc  push       0
00465bde  lea        edx, [esp + 0x1c]
00465be2  push       edx
00465be3  lea        edx, [esp + 0x30]
00465be7  push       edx
00465be8  push       eax
00465be9  mov        eax, dword ptr [ecx + 0xc]
00465bec  call       eax

// block 00465bee
00465bee  test       eax, eax
00465bf0  jl         0x465c9a

// block 00465bf6
00465bf6  mov        eax, dword ptr [esp + 0x18]
00465bfa  mov        ecx, dword ptr [eax]
00465bfc  lea        edx, [esp + 0x14]
00465c00  push       edx
00465c01  push       0x49981c
00465c06  push       eax
00465c07  mov        eax, dword ptr [ecx]
00465c09  call       eax

// block 00465c0b
00465c0b  test       eax, eax
00465c0d  jl         0x465c9a

// block 00465c13
00465c13  xor        ecx, ecx
00465c15  mov        eax, 0x10
00465c1a  mov        edx, 8
00465c1f  mul        edx
00465c21  seto       cl
00465c24  neg        ecx
00465c26  or         ecx, eax
00465c28  push       ecx
00465c29  call       0x46c9ea

// block 00465c2e
00465c2e  mov        ebx, eax
00465c30  add        esp, 4
00465c33  test       ebx, ebx
00465c35  jne        0x465c41

// block 00465c37
00465c37  mov        eax, 0x8007000e
00465c3c  jmp        0x465d18

// block 00465c41
00465c41  xor        eax, eax
00465c43  lea        ecx, [ebp - 1]
00465c46  jmp        0x465c50

// block 00465c50
00465c50  mov        edx, dword ptr [esp + 0x90]
00465c57  mov        dword ptr [ebx + eax*8], ecx
00465c5a  mov        dword ptr [ebx + eax*8 + 4], edx
00465c5e  inc        eax
00465c5f  add        ecx, ebp
00465c61  cmp        eax, 0x10
00465c64  jb         0x465c50

// block 00465c66
00465c66  mov        eax, dword ptr [esp + 0x14]
00465c6a  mov        ecx, dword ptr [eax]
00465c6c  mov        edx, dword ptr [ecx + 0xc]
00465c6f  push       ebx
00465c70  push       0x10
00465c72  push       eax
00465c73  call       edx

// block 00465c75
00465c75  test       eax, eax
00465c77  mov        eax, dword ptr [esp + 0x14]
00465c7b  jge        0x465ca1

// block 00465c7d
00465c7d  test       eax, eax
00465c7f  je         0x465c91

// block 00465c81
00465c81  mov        ecx, dword ptr [eax]
00465c83  mov        edx, dword ptr [ecx + 8]
00465c86  push       eax
00465c87  call       edx

// block 00465c89
00465c89  mov        dword ptr [esp + 0x14], 0

// block 00465c91
00465c91  push       ebx
00465c92  call       0x46ca4f

// block 00465c97
00465c97  add        esp, 4

// block 00465c9a
00465c9a  mov        eax, 0x80004005
00465c9f  jmp        0x465d18

// block 00465ca1
00465ca1  test       eax, eax
00465ca3  je         0x465cb5

// block 00465ca5
00465ca5  mov        ecx, dword ptr [eax]
00465ca7  mov        edx, dword ptr [ecx + 8]
00465caa  push       eax
00465cab  call       edx

// block 00465cad
00465cad  mov        dword ptr [esp + 0x14], 0

// block 00465cb5
00465cb5  push       ebx
00465cb6  call       0x46ca4f

// block 00465cbb
00465cbb  push       0x7c
00465cbd  call       0x46c9ea

// block 00465cc2
00465cc2  mov        edx, eax
00465cc4  add        esp, 8
00465cc7  mov        dword ptr [esp + 0x24], edx
00465ccb  xor        ebx, ebx
00465ccd  mov        dword ptr [esp + 0x68], ebx
00465cd1  cmp        edx, ebx
00465cd3  je         0x465ce6

// block 00465cd5
00465cd5  mov        eax, dword ptr [esp + 0x18]
00465cd9  push       ebp
00465cda  push       eax
00465cdb  mov        eax, esi
00465cdd  mov        ecx, edi
00465cdf  call       0x466650

// block 00465ce4
00465ce4  jmp        0x465ce8

// block 00465ce6
00465ce6  xor        eax, eax

// block 00465ce8
00465ce8  mov        edx, dword ptr [esp + 0x20]
00465cec  mov        dword ptr [edx], eax
00465cee  lea        edi, [eax + 0x38]
00465cf1  mov        eax, dword ptr [esp + 0x1c]
00465cf5  mov        ecx, 9
00465cfa  lea        esi, [esp + 0x28]

// block 00465cfe
00465cfe  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00465d00
00465d00  mov        ecx, dword ptr [edx]
00465d02  mov        dword ptr [ecx + 0x5c], eax
00465d05  mov        ecx, dword ptr [edx]
00465d07  mov        eax, dword ptr [esp + 0x90]
00465d0e  mov        dword ptr [ecx + 0x74], eax
00465d11  mov        ecx, dword ptr [edx]
00465d13  mov        dword ptr [ecx + 0x78], ebx
00465d16  xor        eax, eax

// block 00465d18
00465d18  mov        ecx, dword ptr [esp + 0x60]
00465d1c  mov        dword ptr fs:[0], ecx
00465d23  pop        ecx
00465d24  pop        edi
00465d25  pop        esi
00465d26  pop        ebp
00465d27  pop        ebx
00465d28  mov        ecx, dword ptr [esp + 0x48]
00465d2c  xor        ecx, esp
00465d2e  call       0x46c932

// block 00465d33
00465d33  add        esp, 0x58
00465d36  ret        0x24
