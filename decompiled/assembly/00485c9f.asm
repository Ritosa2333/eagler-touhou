
// block 00485c9f
00485c9f  mov        edi, edi
00485ca1  push       ebp
00485ca2  mov        ebp, esp
00485ca4  sub        esp, 0x7c
00485ca7  mov        eax, dword ptr [0x4ad138]
00485cac  xor        eax, ebp
00485cae  mov        dword ptr [ebp - 4], eax
00485cb1  push       ebx
00485cb2  push       esi
00485cb3  push       edi
00485cb4  mov        edi, dword ptr [ebp + 8]
00485cb7  call       0x475467

// block 00485cbc
00485cbc  mov        esi, eax
00485cbe  mov        edx, edi
00485cc0  add        esi, 0x9c
00485cc6  call       0x485b44

// block 00485ccb
00485ccb  mov        ebx, dword ptr [0x4980e0]
00485cd1  mov        edi, eax
00485cd3  push       0x78
00485cd5  lea        eax, [ebp - 0x7c]
00485cd8  push       eax
00485cd9  mov        eax, dword ptr [esi + 0x14]
00485cdc  neg        eax
00485cde  sbb        eax, eax
00485ce0  and        eax, 0xfffff005
00485ce5  add        eax, 0x1002
00485cea  push       eax
00485ceb  push       edi
00485cec  call       ebx

// block 00485cee
00485cee  test       eax, eax
00485cf0  jne        0x485cfe

// block 00485cf2
00485cf2  and        dword ptr [esi + 8], 0
00485cf6  xor        eax, eax
00485cf8  inc        eax
00485cf9  jmp        0x485e60

// block 00485cfe
00485cfe  lea        eax, [ebp - 0x7c]
00485d01  push       eax
00485d02  push       dword ptr [esi + 4]
00485d05  call       0x46e3f8

// block 00485d0a
00485d0a  pop        ecx
00485d0b  pop        ecx
00485d0c  test       eax, eax
00485d0e  jne        0x485da5

// block 00485d14
00485d14  push       0x78
00485d16  lea        eax, [ebp - 0x7c]
00485d19  push       eax
00485d1a  mov        eax, dword ptr [esi + 0x10]
00485d1d  neg        eax
00485d1f  sbb        eax, eax
00485d21  and        eax, 0xfffff002
00485d26  add        eax, 0x1001
00485d2b  push       eax
00485d2c  push       edi
00485d2d  call       ebx

// block 00485d2f
00485d2f  test       eax, eax
00485d31  je         0x485cf2

// block 00485d33
00485d33  lea        eax, [ebp - 0x7c]
00485d36  push       eax
00485d37  push       dword ptr [esi]
00485d39  call       0x46e3f8

// block 00485d3e
00485d3e  pop        ecx
00485d3f  pop        ecx
00485d40  test       eax, eax
00485d42  jne        0x485d50

// block 00485d44
00485d44  or         dword ptr [esi + 8], 0x304
00485d4b  mov        dword ptr [esi + 0x18], edi
00485d4e  jmp        0x485da2

// block 00485d50
00485d50  test       byte ptr [esi + 8], 2
00485d54  jne        0x485da5

// block 00485d56
00485d56  mov        eax, dword ptr [esi + 0xc]
00485d59  test       eax, eax
00485d5b  je         0x485d89

// block 00485d5d
00485d5d  push       eax
00485d5e  lea        eax, [ebp - 0x7c]
00485d61  push       eax
00485d62  push       dword ptr [esi]
00485d64  call       0x48d94e

// block 00485d69
00485d69  add        esp, 0xc
00485d6c  test       eax, eax
00485d6e  jne        0x485d89

// block 00485d70
00485d70  push       dword ptr [esi]
00485d72  or         dword ptr [esi + 8], 2
00485d76  mov        dword ptr [esi + 0x1c], edi
00485d79  call       0x475860

// block 00485d7e
00485d7e  pop        ecx
00485d7f  cmp        eax, dword ptr [esi + 0xc]
00485d82  jne        0x485da5

// block 00485d84
00485d84  mov        dword ptr [esi + 0x18], edi
00485d87  jmp        0x485da5

// block 00485d89
00485d89  mov        edx, dword ptr [esi + 8]
00485d8c  test       dl, 1
00485d8f  jne        0x485da5

// block 00485d91
00485d91  push       edi
00485d92  call       0x485b20

// block 00485d97
00485d97  pop        ecx
00485d98  test       eax, eax
00485d9a  je         0x485da5

// block 00485d9c
00485d9c  or         edx, 1
00485d9f  mov        dword ptr [esi + 8], edx

// block 00485da2
00485da2  mov        dword ptr [esi + 0x1c], edi

// block 00485da5
00485da5  mov        ecx, dword ptr [esi + 8]
00485da8  mov        eax, 0x300
00485dad  and        ecx, eax
00485daf  cmp        ecx, eax
00485db1  je         0x485e55

// block 00485db7
00485db7  push       0x78
00485db9  lea        eax, [ebp - 0x7c]
00485dbc  push       eax
00485dbd  mov        eax, dword ptr [esi + 0x10]
00485dc0  neg        eax
00485dc2  sbb        eax, eax
00485dc4  and        eax, 0xfffff002
00485dc9  add        eax, 0x1001
00485dce  push       eax
00485dcf  push       edi
00485dd0  call       ebx

// block 00485dd2
00485dd2  test       eax, eax
00485dd4  je         0x485cf2

// block 00485dda
00485dda  lea        eax, [ebp - 0x7c]
00485ddd  push       eax
00485dde  push       dword ptr [esi]
00485de0  call       0x46e3f8

// block 00485de5
00485de5  pop        ecx
00485de6  xor        ebx, ebx
00485de8  pop        ecx
00485de9  test       eax, eax
00485deb  jne        0x485e1c

// block 00485ded
00485ded  or         dword ptr [esi + 8], 0x200
00485df4  mov        eax, dword ptr [esi + 8]
00485df7  cmp        dword ptr [esi + 0x10], ebx
00485dfa  je         0x485e06

// block 00485dfc
00485dfc  or         eax, 0x100
00485e01  mov        dword ptr [esi + 8], eax
00485e04  jmp        0x485e4d

// block 00485e06
00485e06  cmp        dword ptr [esi + 0xc], ebx
00485e09  je         0x485e46

// block 00485e0b
00485e0b  push       dword ptr [esi]
00485e0d  call       0x475860

// block 00485e12
00485e12  pop        ecx
00485e13  cmp        eax, dword ptr [esi + 0xc]
00485e16  jne        0x485e46

// block 00485e18
00485e18  push       1
00485e1a  jmp        0x485e38

// block 00485e1c
00485e1c  cmp        dword ptr [esi + 0x10], ebx
00485e1f  jne        0x485e55

// block 00485e21
00485e21  cmp        dword ptr [esi + 0xc], ebx
00485e24  je         0x485e55

// block 00485e26
00485e26  lea        eax, [ebp - 0x7c]
00485e29  push       eax
00485e2a  push       dword ptr [esi]
00485e2c  call       0x46e3f8

// block 00485e31
00485e31  pop        ecx
00485e32  pop        ecx
00485e33  test       eax, eax
00485e35  jne        0x485e55

// block 00485e37
00485e37  push       ebx

// block 00485e38
00485e38  push       edi
00485e39  mov        ecx, esi
00485e3b  call       0x485c2b

// block 00485e40
00485e40  pop        ecx
00485e41  pop        ecx
00485e42  test       eax, eax
00485e44  je         0x485e55

// block 00485e46
00485e46  or         dword ptr [esi + 8], 0x100

// block 00485e4d
00485e4d  cmp        dword ptr [esi + 0x18], ebx
00485e50  jne        0x485e55

// block 00485e52
00485e52  mov        dword ptr [esi + 0x18], edi

// block 00485e55
00485e55  mov        eax, dword ptr [esi + 8]
00485e58  shr        eax, 2
00485e5b  not        eax
00485e5d  and        eax, 1

// block 00485e60
00485e60  mov        ecx, dword ptr [ebp - 4]
00485e63  pop        edi
00485e64  pop        esi
00485e65  xor        ecx, ebp
00485e67  pop        ebx
00485e68  call       0x46c932

// block 00485e6d
00485e6d  leave      
00485e6e  ret        4
