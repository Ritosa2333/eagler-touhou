
// block 00403b60
00403b60  push       ebp
00403b61  mov        ebp, dword ptr [esp + 8]
00403b65  mov        eax, dword ptr [ebp + 0x35bc]
00403b6b  test       al, 8
00403b6d  jne        0x403ead

// block 00403b73
00403b73  push       ebx
00403b74  push       esi
00403b75  xor        ebx, ebx
00403b77  push       edi
00403b78  test       al, 4
00403b7a  je         0x403b89

// block 00403b7c
00403b7c  cmp        dword ptr [ebp + 0x35c4], 0x3c
00403b83  jge        0x403d2c

// block 00403b89
00403b89  mov        esi, dword ptr [0x4ce8cc]
00403b8f  call       0x45a3c0

// block 00403b94
00403b94  fld        dword ptr [0x4cee04]
00403b9a  fstp       dword ptr [ebp + 0x36f4]
00403ba0  lea        esi, [ebp + 0x360c]
00403ba6  fld        dword ptr [0x4cee08]
00403bac  mov        ecx, 0x46
00403bb1  fstp       dword ptr [ebp + 0x36f8]
00403bb7  mov        edi, 0x4ced1c

// block 00403bbc
00403bbc  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00403bbe
00403bbe  mov        edi, 0x4ced1c
00403bc3  mov        dword ptr [0x4cee34], 0x4ced1c
00403bcd  call       0x430a70

// block 00403bd2
00403bd2  mov        edx, dword ptr [0x4cee34]
00403bd8  mov        eax, dword ptr [0x4ce8f0]
00403bdd  mov        ecx, dword ptr [eax]
00403bdf  add        edx, 0xcc
00403be5  push       edx
00403be6  push       eax
00403be7  mov        eax, dword ptr [ecx + 0xbc]
00403bed  call       eax

// block 00403bef
00403bef  mov        dword ptr [0x4cee38], 2
00403bf9  cmp        dword ptr [0x4cf278], ebx
00403bff  je         0x403c25

// block 00403c01
00403c01  mov        esi, dword ptr [0x4ce8cc]
00403c07  call       0x45a3c0

// block 00403c0c
00403c0c  mov        eax, dword ptr [0x4ce8f0]
00403c11  push       ebx
00403c12  mov        dword ptr [0x4cf278], ebx
00403c18  mov        ecx, dword ptr [eax]
00403c1a  mov        edx, dword ptr [ecx + 0xe4]
00403c20  push       0x1c
00403c22  push       eax
00403c23  call       edx

// block 00403c25
00403c25  mov        esi, dword ptr [0x4ce8cc]
00403c2b  call       0x45a3c0

// block 00403c30
00403c30  mov        eax, dword ptr [0x4ce8f0]
00403c35  mov        ecx, dword ptr [eax]
00403c37  mov        edx, dword ptr [ecx + 0xe4]
00403c3d  push       ebx
00403c3e  push       0xe
00403c40  push       eax
00403c41  call       edx

// block 00403c43
00403c43  mov        esi, dword ptr [0x4ce8cc]
00403c49  call       0x45a3c0

// block 00403c4e
00403c4e  mov        eax, dword ptr [0x4ce8f0]
00403c53  mov        ecx, dword ptr [eax]
00403c55  mov        edx, dword ptr [ecx + 0xe4]
00403c5b  push       8
00403c5d  push       0x17
00403c5f  push       eax
00403c60  call       edx

// block 00403c62
00403c62  mov        edi, dword ptr [0x4ce8cc]
00403c68  mov        eax, 0x1a
00403c6d  call       0x4611d0

// block 00403c72
00403c72  mov        esi, dword ptr [0x4ce8cc]
00403c78  call       0x45a3c0

// block 00403c7d
00403c7d  mov        eax, dword ptr [0x4ce8f0]
00403c82  mov        ecx, dword ptr [eax]
00403c84  mov        edx, dword ptr [ecx + 0xe4]
00403c8a  push       4
00403c8c  push       0x17
00403c8e  push       eax
00403c8f  call       edx

// block 00403c91
00403c91  mov        edi, dword ptr [0x4ce8cc]
00403c97  mov        eax, 0x1b
00403c9c  call       0x4611d0

// block 00403ca1
00403ca1  mov        esi, dword ptr [0x4ce8cc]
00403ca7  call       0x45a3c0

// block 00403cac
00403cac  mov        eax, dword ptr [0x4ce8f0]
00403cb1  mov        ecx, dword ptr [eax]
00403cb3  mov        edx, dword ptr [ecx + 0xe4]
00403cb9  push       4
00403cbb  push       0x17
00403cbd  push       eax
00403cbe  call       edx

// block 00403cc0
00403cc0  mov        esi, dword ptr [0x4ce8cc]
00403cc6  mov        edi, dword ptr [ebp + 0x3720]
00403ccc  call       0x45a3c0

// block 00403cd1
00403cd1  mov        eax, dword ptr [0x4ce8f0]
00403cd6  mov        ecx, dword ptr [eax]
00403cd8  mov        edx, dword ptr [ecx + 0xe4]
00403cde  push       edi
00403cdf  push       0x22
00403ce1  push       eax
00403ce2  call       edx

// block 00403ce4
00403ce4  mov        esi, dword ptr [0x4ce8cc]
00403cea  mov        edi, dword ptr [ebp + 0x3708]
00403cf0  call       0x45a3c0

// block 00403cf5
00403cf5  mov        eax, dword ptr [0x4ce8f0]
00403cfa  mov        ecx, dword ptr [eax]
00403cfc  mov        edx, dword ptr [ecx + 0xe4]
00403d02  push       edi
00403d03  push       0x24
00403d05  push       eax
00403d06  call       edx

// block 00403d08
00403d08  mov        esi, dword ptr [0x4ce8cc]

// block 00403d2c
00403d2c  mov        eax, dword ptr [ebp + 0x35bc]
00403d32  test       al, 4
00403d34  je         0x403d45

// block 00403d36
00403d36  cmp        dword ptr [ebp + 0x35c4], 0x1e
00403d3d  jl         0x403d45

// block 00403d3f
00403d3f  mov        byte ptr [ebp + 0x2777], bl

// block 00403d45
00403d45  test       al, 1
00403d47  je         0x403dc3

// block 00403d49
00403d49  mov        esi, dword ptr [0x4ce8cc]
00403d4f  call       0x45a3c0

// block 00403d54
00403d54  mov        eax, dword ptr [0x4ce8f0]
00403d59  mov        ecx, dword ptr [eax]
00403d5b  mov        edx, dword ptr [ecx + 0xe4]
00403d61  push       ebx
00403d62  push       0xe
00403d64  push       eax
00403d65  call       edx

// block 00403d67
00403d67  mov        edi, 1
00403d6c  cmp        dword ptr [0x4cf278], edi
00403d72  je         0x403d98

// block 00403d74
00403d74  mov        esi, dword ptr [0x4ce8cc]
00403d7a  call       0x45a3c0

// block 00403d7f
00403d7f  mov        eax, dword ptr [0x4ce8f0]
00403d84  push       edi
00403d85  mov        dword ptr [0x4cf278], edi
00403d8b  mov        ecx, dword ptr [eax]
00403d8d  mov        edx, dword ptr [ecx + 0xe4]
00403d93  push       0x1c
00403d95  push       eax
00403d96  call       edx

// block 00403d98
00403d98  push       8
00403d9a  push       ebp
00403d9b  call       0x4046c0

// block 00403da0
00403da0  push       9
00403da2  push       ebp
00403da3  call       0x4046c0

// block 00403da8
00403da8  push       0xa
00403daa  push       ebp
00403dab  call       0x4046c0

// block 00403db0
00403db0  push       0xb
00403db2  push       ebp
00403db3  call       0x4046c0

// block 00403db8
00403db8  mov        esi, dword ptr [0x4ce8cc]
00403dbe  call       0x45a3c0

// block 00403dc3
00403dc3  mov        edi, dword ptr [0x4ce8cc]
00403dc9  mov        dword ptr [edi + 0x88ed50], ebx
00403dcf  mov        dword ptr [edi + 0x88ed4c], 0x80808080
00403dd9  cmp        dword ptr [ebp + 0x2778], ebx
00403ddf  je         0x403df5

// block 00403de1
00403de1  mov        dword ptr [edi + 0x88ed50], 1
00403deb  mov        dword ptr [edi + 0x88ed4c], 0xff404040

// block 00403df5
00403df5  cmp        dword ptr [ebp + 0x35c4], ebx
00403dfb  jle        0x403e45

// block 00403dfd
00403dfd  fld        dword ptr [0x4a3da8]
00403e03  push       ecx
00403e04  lea        esi, [ebp + 0x35c0]
00403e0a  fstp       dword ptr [esp]
00403e0d  call       0x464a20

// block 00403e12
00403e12  cmp        dword ptr [ebp + 0x35c4], ebx
00403e18  jg         0x403e45

// block 00403e1a
00403e1a  mov        eax, dword ptr [ebp + 0x35bc]
00403e20  mov        byte ptr [ebp + 0x2777], 0xff
00403e27  test       al, 2
00403e29  je         0x403e34

// block 00403e2b
00403e2b  or         eax, 8
00403e2e  mov        dword ptr [ebp + 0x35bc], eax

// block 00403e34
00403e34  and        dword ptr [ebp + 0x35bc], 0xfffffff9
00403e3b  mov        dword ptr [ebp + 0x2774], 0xffffff

// block 00403e45
00403e45  mov        esi, edi
00403e47  call       0x45a3c0

// block 00403e4c
00403e4c  mov        eax, dword ptr [0x4ce8f0]
00403e51  mov        ecx, dword ptr [eax]
00403e53  mov        edx, dword ptr [ecx + 0xe4]
00403e59  push       ebx
00403e5a  push       0xe
00403e5c  push       eax
00403e5d  call       edx

// block 00403e5f
00403e5f  mov        esi, dword ptr [0x4ce8cc]
00403e65  call       0x45a3c0

// block 00403e6a
00403e6a  mov        eax, dword ptr [0x4ce8f0]
00403e6f  mov        ecx, dword ptr [eax]
00403e71  mov        edx, dword ptr [ecx + 0xe4]
00403e77  push       8
00403e79  push       0x17
00403e7b  push       eax
00403e7c  call       edx

// block 00403e7e
00403e7e  cmp        dword ptr [0x4cf278], ebx
00403e84  je         0x403eaa

// block 00403e86
00403e86  mov        esi, dword ptr [0x4ce8cc]
00403e8c  call       0x45a3c0

// block 00403e91
00403e91  mov        eax, dword ptr [0x4ce8f0]
00403e96  push       ebx
00403e97  mov        dword ptr [0x4cf278], ebx
00403e9d  mov        ecx, dword ptr [eax]
00403e9f  mov        edx, dword ptr [ecx + 0xe4]
00403ea5  push       0x1c
00403ea7  push       eax
00403ea8  call       edx

// block 00403eaa
00403eaa  pop        edi
00403eab  pop        esi
00403eac  pop        ebx

// block 00403ead
00403ead  mov        eax, 1
00403eb2  pop        ebp
00403eb3  ret        4
