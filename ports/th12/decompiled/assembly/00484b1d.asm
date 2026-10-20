
// block 00484b1d
00484b1d  mov        edi, edi
00484b1f  push       ebp
00484b20  mov        ebp, esp
00484b22  sub        esp, 0x24
00484b25  push       ebx
00484b26  push       esi
00484b27  push       edi
00484b28  push       dword ptr [ebp + 8]
00484b2b  lea        ecx, [ebp - 0x24]
00484b2e  xor        edi, edi
00484b30  call       0x46d3b4

// block 00484b35
00484b35  mov        eax, dword ptr [ebp - 0x24]
00484b38  mov        esi, dword ptr [eax + 0xd4]
00484b3e  mov        dword ptr [ebp - 4], edi

// block 00484b41
00484b41  mov        ebx, dword ptr [ebp - 4]
00484b44  shl        ebx, 2
00484b47  push       dword ptr [ebx + esi + 0x1c]
00484b4b  call       0x475860

// block 00484b50
00484b50  push       dword ptr [ebx + esi]
00484b53  mov        dword ptr [ebp - 0x14], eax
00484b56  call       0x475860

// block 00484b5b
00484b5b  add        eax, edi
00484b5d  inc        dword ptr [ebp - 4]
00484b60  cmp        dword ptr [ebp - 4], 7
00484b64  pop        ecx
00484b65  pop        ecx
00484b66  mov        ecx, dword ptr [ebp - 0x14]
00484b69  lea        edi, [eax + ecx + 2]
00484b6d  jb         0x484b41

// block 00484b6f
00484b6f  lea        eax, [esi + 0x38]
00484b72  mov        dword ptr [ebp - 0x10], eax
00484b75  mov        dword ptr [ebp - 0xc], 0xc

// block 00484b7c
00484b7c  mov        ebx, dword ptr [ebp - 0x10]
00484b7f  push       dword ptr [ebx + 0x30]
00484b82  call       0x475860

// block 00484b87
00484b87  push       dword ptr [ebx]
00484b89  mov        dword ptr [ebp - 0x14], eax
00484b8c  call       0x475860

// block 00484b91
00484b91  add        dword ptr [ebp - 0x10], 4
00484b95  pop        ecx
00484b96  pop        ecx
00484b97  mov        ecx, dword ptr [ebp - 0x14]
00484b9a  add        eax, edi
00484b9c  dec        dword ptr [ebp - 0xc]
00484b9f  lea        edi, [eax + ecx + 2]
00484ba3  jne        0x484b7c

// block 00484ba5
00484ba5  push       dword ptr [esi + 0x9c]
00484bab  call       0x475860

// block 00484bb0
00484bb0  push       dword ptr [esi + 0x98]
00484bb6  mov        ebx, eax
00484bb8  call       0x475860

// block 00484bbd
00484bbd  push       dword ptr [esi + 0xa0]
00484bc3  add        eax, edi
00484bc5  lea        edi, [eax + ebx + 2]
00484bc9  call       0x475860

// block 00484bce
00484bce  push       dword ptr [esi + 0xa4]
00484bd4  lea        edi, [edi + eax + 1]
00484bd8  call       0x475860

// block 00484bdd
00484bdd  push       dword ptr [esi + 0xa8]
00484be3  lea        edi, [edi + eax + 1]
00484be7  call       0x475860

// block 00484bec
00484bec  lea        eax, [edi + eax + 0xb9]
00484bf3  push       eax
00484bf4  mov        dword ptr [ebp - 8], eax
00484bf7  call       0x4777ce

// block 00484bfc
00484bfc  mov        ebx, eax
00484bfe  add        esp, 0x18
00484c01  test       ebx, ebx
00484c03  je         0x484e60

// block 00484c09
00484c09  push       0xb8
00484c0e  push       esi
00484c0f  push       ebx
00484c10  lea        edi, [ebx + 0xb8]
00484c16  call       0x47a330

// block 00484c1b
00484c1b  and        dword ptr [ebp - 4], 0
00484c1f  lea        eax, [esi + 0x1c]
00484c22  mov        dword ptr [ebp - 0x10], ebx
00484c25  add        esp, 0xc
00484c28  sub        dword ptr [ebp - 0x10], esi
00484c2b  mov        dword ptr [ebp - 0xc], eax

// block 00484c2e
00484c2e  mov        eax, dword ptr [ebp - 4]
00484c31  mov        dword ptr [ebx + eax*4], edi
00484c34  mov        eax, dword ptr [ebp - 0xc]
00484c37  push       dword ptr [eax - 0x1c]
00484c3a  mov        eax, ebx
00484c3c  sub        eax, edi
00484c3e  add        eax, dword ptr [ebp - 8]
00484c41  push       eax
00484c42  push       edi
00484c43  call       0x47a2b9

// block 00484c48
00484c48  add        esp, 0xc
00484c4b  test       eax, eax
00484c4d  je         0x484c5e

// block 00484c4f
00484c4f  xor        eax, eax
00484c51  push       eax
00484c52  push       eax
00484c53  push       eax
00484c54  push       eax
00484c55  push       eax
00484c56  call       0x470dc6

// block 00484c5b
00484c5b  add        esp, 0x14

// block 00484c5e
00484c5e  push       edi
00484c5f  call       0x475860

// block 00484c64
00484c64  mov        ecx, dword ptr [ebp - 0x10]
00484c67  lea        edi, [edi + eax + 1]
00484c6b  mov        eax, dword ptr [ebp - 0xc]
00484c6e  mov        dword ptr [ecx + eax], edi
00484c71  push       dword ptr [eax]
00484c73  mov        eax, ebx
00484c75  sub        eax, edi
00484c77  add        eax, dword ptr [ebp - 8]
00484c7a  push       eax
00484c7b  push       edi
00484c7c  call       0x47a2b9

// block 00484c81
00484c81  add        esp, 0x10
00484c84  test       eax, eax
00484c86  je         0x484c97

// block 00484c88
00484c88  xor        eax, eax
00484c8a  push       eax
00484c8b  push       eax
00484c8c  push       eax
00484c8d  push       eax
00484c8e  push       eax
00484c8f  call       0x470dc6

// block 00484c94
00484c94  add        esp, 0x14

// block 00484c97
00484c97  push       edi
00484c98  call       0x475860

// block 00484c9d
00484c9d  inc        dword ptr [ebp - 4]
00484ca0  add        dword ptr [ebp - 0xc], 4
00484ca4  cmp        dword ptr [ebp - 4], 7
00484ca8  pop        ecx
00484ca9  lea        edi, [edi + eax + 1]
00484cad  jb         0x484c2e

// block 00484cb3
00484cb3  lea        eax, [ebx + 0x68]
00484cb6  mov        dword ptr [ebp - 4], eax
00484cb9  lea        eax, [esi + 0x38]
00484cbc  mov        dword ptr [ebp - 0xc], eax
00484cbf  mov        dword ptr [ebp - 0x14], 0xc
00484cc6  jmp        0x484ccb

// block 00484cc8
00484cc8  mov        eax, dword ptr [ebp - 0xc]

// block 00484ccb
00484ccb  mov        ecx, dword ptr [ebp - 0x10]
00484cce  mov        dword ptr [eax + ecx], edi
00484cd1  push       dword ptr [eax]
00484cd3  mov        eax, ebx
00484cd5  sub        eax, edi
00484cd7  add        eax, dword ptr [ebp - 8]
00484cda  push       eax
00484cdb  push       edi
00484cdc  call       0x47a2b9

// block 00484ce1
00484ce1  add        esp, 0xc
00484ce4  test       eax, eax
00484ce6  je         0x484cf7

// block 00484ce8
00484ce8  xor        eax, eax
00484cea  push       eax
00484ceb  push       eax
00484cec  push       eax
00484ced  push       eax
00484cee  push       eax
00484cef  call       0x470dc6

// block 00484cf4
00484cf4  add        esp, 0x14

// block 00484cf7
00484cf7  push       edi
00484cf8  call       0x475860

// block 00484cfd
00484cfd  lea        edi, [edi + eax + 1]
00484d01  mov        eax, dword ptr [ebp - 4]
00484d04  mov        dword ptr [eax], edi
00484d06  mov        eax, dword ptr [ebp - 0xc]
00484d09  push       dword ptr [eax + 0x30]
00484d0c  mov        eax, ebx
00484d0e  sub        eax, edi
00484d10  add        eax, dword ptr [ebp - 8]
00484d13  push       eax
00484d14  push       edi
00484d15  call       0x47a2b9

// block 00484d1a
00484d1a  add        esp, 0x10
00484d1d  test       eax, eax
00484d1f  je         0x484d30

// block 00484d21
00484d21  xor        eax, eax
00484d23  push       eax
00484d24  push       eax
00484d25  push       eax
00484d26  push       eax
00484d27  push       eax
00484d28  call       0x470dc6

// block 00484d2d
00484d2d  add        esp, 0x14

// block 00484d30
00484d30  push       edi
00484d31  call       0x475860

// block 00484d36
00484d36  add        dword ptr [ebp - 0xc], 4
00484d3a  add        dword ptr [ebp - 4], 4
00484d3e  dec        dword ptr [ebp - 0x14]
00484d41  pop        ecx
00484d42  lea        edi, [edi + eax + 1]
00484d46  jne        0x484cc8

// block 00484d48
00484d48  mov        eax, ebx
00484d4a  sub        eax, edi
00484d4c  add        eax, dword ptr [ebp - 8]
00484d4f  mov        dword ptr [ebx + 0x98], edi
00484d55  push       dword ptr [esi + 0x98]
00484d5b  push       eax
00484d5c  push       edi
00484d5d  call       0x47a2b9

// block 00484d62
00484d62  add        esp, 0xc
00484d65  test       eax, eax
00484d67  je         0x484d78

// block 00484d69
00484d69  xor        eax, eax
00484d6b  push       eax
00484d6c  push       eax
00484d6d  push       eax
00484d6e  push       eax
00484d6f  push       eax
00484d70  call       0x470dc6

// block 00484d75
00484d75  add        esp, 0x14

// block 00484d78
00484d78  push       edi
00484d79  call       0x475860

// block 00484d7e
00484d7e  lea        edi, [edi + eax + 1]
00484d82  mov        eax, ebx
00484d84  sub        eax, edi
00484d86  add        eax, dword ptr [ebp - 8]
00484d89  mov        dword ptr [ebx + 0x9c], edi
00484d8f  push       dword ptr [esi + 0x9c]
00484d95  push       eax
00484d96  push       edi
00484d97  call       0x47a2b9

// block 00484d9c
00484d9c  add        esp, 0x10
00484d9f  test       eax, eax
00484da1  je         0x484db2

// block 00484da3
00484da3  xor        eax, eax
00484da5  push       eax
00484da6  push       eax
00484da7  push       eax
00484da8  push       eax
00484da9  push       eax
00484daa  call       0x470dc6

// block 00484daf
00484daf  add        esp, 0x14

// block 00484db2
00484db2  push       edi
00484db3  call       0x475860

// block 00484db8
00484db8  lea        edi, [edi + eax + 1]
00484dbc  mov        eax, ebx
00484dbe  sub        eax, edi
00484dc0  add        eax, dword ptr [ebp - 8]
00484dc3  mov        dword ptr [ebx + 0xa0], edi
00484dc9  push       dword ptr [esi + 0xa0]
00484dcf  push       eax
00484dd0  push       edi
00484dd1  call       0x47a2b9

// block 00484dd6
00484dd6  add        esp, 0x10
00484dd9  test       eax, eax
00484ddb  je         0x484dec

// block 00484ddd
00484ddd  xor        eax, eax
00484ddf  push       eax
00484de0  push       eax
00484de1  push       eax
00484de2  push       eax
00484de3  push       eax
00484de4  call       0x470dc6

// block 00484de9
00484de9  add        esp, 0x14

// block 00484dec
00484dec  push       edi
00484ded  call       0x475860

// block 00484df2
00484df2  lea        edi, [edi + eax + 1]
00484df6  mov        eax, ebx
00484df8  sub        eax, edi
00484dfa  add        eax, dword ptr [ebp - 8]
00484dfd  mov        dword ptr [ebx + 0xa4], edi
00484e03  push       dword ptr [esi + 0xa4]
00484e09  push       eax
00484e0a  push       edi
00484e0b  call       0x47a2b9

// block 00484e10
00484e10  add        esp, 0x10
00484e13  test       eax, eax
00484e15  je         0x484e26

// block 00484e17
00484e17  xor        eax, eax
00484e19  push       eax
00484e1a  push       eax
00484e1b  push       eax
00484e1c  push       eax
00484e1d  push       eax
00484e1e  call       0x470dc6

// block 00484e23
00484e23  add        esp, 0x14

// block 00484e26
00484e26  push       edi
00484e27  call       0x475860

// block 00484e2c
00484e2c  lea        eax, [edi + eax + 1]
00484e30  mov        ecx, ebx
00484e32  sub        ecx, eax
00484e34  add        ecx, dword ptr [ebp - 8]
00484e37  mov        dword ptr [ebx + 0xa8], eax
00484e3d  push       dword ptr [esi + 0xa8]
00484e43  push       ecx
00484e44  push       eax
00484e45  call       0x47a2b9

// block 00484e4a
00484e4a  add        esp, 0x10
00484e4d  test       eax, eax
00484e4f  je         0x484e60

// block 00484e51
00484e51  xor        eax, eax
00484e53  push       eax
00484e54  push       eax
00484e55  push       eax
00484e56  push       eax
00484e57  push       eax
00484e58  call       0x470dc6

// block 00484e5d
00484e5d  add        esp, 0x14

// block 00484e60
00484e60  cmp        byte ptr [ebp - 0x18], 0
00484e64  je         0x484e6d

// block 00484e66
00484e66  mov        eax, dword ptr [ebp - 0x1c]
00484e69  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00484e6d
00484e6d  pop        edi
00484e6e  pop        esi
00484e6f  mov        eax, ebx
00484e71  pop        ebx
00484e72  leave      
00484e73  ret        
