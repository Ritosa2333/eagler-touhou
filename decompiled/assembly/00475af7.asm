
// block 00475af7
00475af7  mov        edi, edi
00475af9  push       ebp
00475afa  mov        ebp, esp
00475afc  sub        esp, 0x30
00475aff  mov        eax, dword ptr [0x4ad138]
00475b04  xor        eax, ebp
00475b06  mov        dword ptr [ebp - 4], eax
00475b09  mov        eax, dword ptr [ebp + 8]
00475b0c  xor        ecx, ecx
00475b0e  push       ebx
00475b0f  mov        ebx, dword ptr [ebp + 0xc]
00475b12  movzx      edx, word ptr [ebx + 0xa]
00475b16  push       esi
00475b17  mov        dword ptr [ebp - 0x2c], ecx
00475b1a  mov        dword ptr [ebp - 0x10], ecx
00475b1d  mov        dword ptr [ebp - 0xc], ecx
00475b20  mov        dword ptr [ebp - 8], ecx
00475b23  movzx      ecx, word ptr [eax + 0xa]
00475b27  mov        esi, edx
00475b29  xor        esi, ecx
00475b2b  and        esi, 0x8000
00475b31  mov        dword ptr [ebp - 0x20], esi
00475b34  mov        esi, 0x7fff
00475b39  and        ecx, esi
00475b3b  and        edx, esi
00475b3d  lea        esi, [edx + ecx]
00475b40  push       edi
00475b41  movzx      esi, si
00475b44  mov        edi, 0x7fff
00475b49  mov        dword ptr [ebp - 0x30], esi
00475b4c  cmp        cx, di
00475b4f  jae        0x475d43

// block 00475b55
00475b55  cmp        dx, di
00475b58  jae        0x475d43

// block 00475b5e
00475b5e  mov        edi, 0xbffd
00475b63  cmp        si, di
00475b66  ja         0x475d43

// block 00475b6c
00475b6c  mov        edi, 0x3fbf
00475b71  cmp        si, di
00475b74  ja         0x475b80

// block 00475b76
00475b76  xor        ecx, ecx

// block 00475b78
00475b78  mov        dword ptr [eax + 8], ecx
00475b7b  jmp        0x475d5e

// block 00475b80
00475b80  mov        edi, 0x7fffffff
00475b85  test       cx, cx
00475b88  jne        0x475ba7

// block 00475b8a
00475b8a  inc        esi
00475b8b  mov        dword ptr [ebp - 0x30], esi
00475b8e  test       dword ptr [eax + 8], edi
00475b91  jne        0x475ba7

// block 00475b93
00475b93  xor        ecx, ecx
00475b95  cmp        dword ptr [eax + 4], ecx
00475b98  jne        0x475ba9

// block 00475b9a
00475b9a  cmp        dword ptr [eax], ecx
00475b9c  jne        0x475ba9

// block 00475b9e
00475b9e  mov        word ptr [eax + 0xa], cx
00475ba2  jmp        0x475d63

// block 00475ba7
00475ba7  xor        ecx, ecx

// block 00475ba9
00475ba9  cmp        dx, cx
00475bac  jne        0x475bc0

// block 00475bae
00475bae  inc        esi
00475baf  mov        dword ptr [ebp - 0x30], esi
00475bb2  test       dword ptr [ebx + 8], edi
00475bb5  jne        0x475bc0

// block 00475bb7
00475bb7  cmp        dword ptr [ebx + 4], ecx
00475bba  jne        0x475bc0

// block 00475bbc
00475bbc  cmp        dword ptr [ebx], ecx
00475bbe  je         0x475b78

// block 00475bc0
00475bc0  mov        dword ptr [ebp - 0x1c], ecx
00475bc3  lea        edi, [ebp - 0xc]
00475bc6  mov        dword ptr [ebp - 0x18], 5

// block 00475bcd
00475bcd  mov        ecx, dword ptr [ebp - 0x1c]
00475bd0  mov        edx, dword ptr [ebp - 0x18]
00475bd3  add        ecx, ecx
00475bd5  mov        dword ptr [ebp - 0x28], edx
00475bd8  test       edx, edx
00475bda  jle        0x475c2b

// block 00475bdc
00475bdc  add        ebx, 8
00475bdf  mov        dword ptr [ebp - 0x24], ebx
00475be2  lea        ebx, [eax + ecx]

// block 00475be5
00475be5  mov        edx, dword ptr [ebp - 0x24]
00475be8  movzx      edx, word ptr [edx]
00475beb  movzx      ecx, word ptr [ebx]
00475bee  and        dword ptr [ebp - 0x14], 0
00475bf2  imul       ecx, edx
00475bf5  mov        edx, dword ptr [edi - 4]
00475bf8  lea        esi, [edx + ecx]
00475bfb  cmp        esi, edx
00475bfd  jb         0x475c03

// block 00475bff
00475bff  cmp        esi, ecx
00475c01  jae        0x475c0a

// block 00475c03
00475c03  mov        dword ptr [ebp - 0x14], 1

// block 00475c0a
00475c0a  cmp        dword ptr [ebp - 0x14], 0
00475c0e  mov        dword ptr [edi - 4], esi
00475c11  je         0x475c16

// block 00475c13
00475c13  inc        word ptr [edi]

// block 00475c16
00475c16  sub        dword ptr [ebp - 0x24], 2
00475c1a  inc        ebx
00475c1b  inc        ebx
00475c1c  dec        dword ptr [ebp - 0x28]
00475c1f  cmp        dword ptr [ebp - 0x28], 0
00475c23  jg         0x475be5

// block 00475c25
00475c25  mov        ebx, dword ptr [ebp + 0xc]
00475c28  mov        esi, dword ptr [ebp - 0x30]

// block 00475c2b
00475c2b  inc        edi
00475c2c  inc        edi
00475c2d  inc        dword ptr [ebp - 0x1c]
00475c30  dec        dword ptr [ebp - 0x18]
00475c33  cmp        dword ptr [ebp - 0x18], 0
00475c37  jg         0x475bcd

// block 00475c39
00475c39  add        esi, 0xc002
00475c3f  mov        edi, 0xffff
00475c44  test       si, si
00475c47  jle        0x475c81

// block 00475c49
00475c49  test       dword ptr [ebp - 8], 0x80000000
00475c50  jne        0x475c7c

// block 00475c52
00475c52  mov        ecx, dword ptr [ebp - 0x10]
00475c55  mov        ebx, dword ptr [ebp - 0xc]
00475c58  mov        edx, dword ptr [ebp - 0xc]
00475c5b  shl        dword ptr [ebp - 0x10], 1
00475c5e  shr        ecx, 0x1f
00475c61  add        ebx, ebx
00475c63  or         ebx, ecx
00475c65  mov        ecx, dword ptr [ebp - 8]
00475c68  shr        edx, 0x1f
00475c6b  add        ecx, ecx
00475c6d  or         ecx, edx
00475c6f  add        esi, edi
00475c71  mov        dword ptr [ebp - 0xc], ebx
00475c74  mov        dword ptr [ebp - 8], ecx
00475c77  test       si, si
00475c7a  jg         0x475c49

// block 00475c7c
00475c7c  test       si, si
00475c7f  jg         0x475cd0

// block 00475c81
00475c81  add        esi, edi
00475c83  test       si, si
00475c86  jge        0x475cd0

// block 00475c88
00475c88  mov        ecx, esi
00475c8a  neg        ecx
00475c8c  movzx      ecx, cx
00475c8f  mov        dword ptr [ebp - 0x14], ecx
00475c92  add        esi, ecx

// block 00475c94
00475c94  test       byte ptr [ebp - 0x10], 1
00475c98  je         0x475c9d

// block 00475c9a
00475c9a  inc        dword ptr [ebp - 0x2c]

// block 00475c9d
00475c9d  mov        ecx, dword ptr [ebp - 8]
00475ca0  mov        ebx, dword ptr [ebp - 0xc]
00475ca3  mov        edx, dword ptr [ebp - 0xc]
00475ca6  shr        dword ptr [ebp - 8], 1
00475ca9  shl        ecx, 0x1f
00475cac  shr        ebx, 1
00475cae  or         ebx, ecx
00475cb0  mov        ecx, dword ptr [ebp - 0x10]
00475cb3  shl        edx, 0x1f
00475cb6  shr        ecx, 1
00475cb8  or         ecx, edx
00475cba  dec        dword ptr [ebp - 0x14]
00475cbd  mov        dword ptr [ebp - 0xc], ebx
00475cc0  mov        dword ptr [ebp - 0x10], ecx
00475cc3  jne        0x475c94

// block 00475cc5
00475cc5  cmp        dword ptr [ebp - 0x2c], 0
00475cc9  je         0x475cd0

// block 00475ccb
00475ccb  or         word ptr [ebp - 0x10], 1

// block 00475cd0
00475cd0  mov        ecx, 0x8000
00475cd5  mov        edx, ecx
00475cd7  cmp        word ptr [ebp - 0x10], dx
00475cdb  ja         0x475cee

// block 00475cdd
00475cdd  mov        edx, dword ptr [ebp - 0x10]
00475ce0  and        edx, 0x1ffff
00475ce6  cmp        edx, 0x18000
00475cec  jne        0x475d1d

// block 00475cee
00475cee  cmp        dword ptr [ebp - 0xe], -1
00475cf2  jne        0x475d1a

// block 00475cf4
00475cf4  and        dword ptr [ebp - 0xe], 0
00475cf8  cmp        dword ptr [ebp - 0xa], -1
00475cfc  jne        0x475d15

// block 00475cfe
00475cfe  and        dword ptr [ebp - 0xa], 0
00475d02  cmp        word ptr [ebp - 6], di
00475d06  jne        0x475d0f

// block 00475d08
00475d08  mov        word ptr [ebp - 6], cx
00475d0c  inc        esi
00475d0d  jmp        0x475d1d

// block 00475d0f
00475d0f  inc        word ptr [ebp - 6]
00475d13  jmp        0x475d1d

// block 00475d15
00475d15  inc        dword ptr [ebp - 0xa]
00475d18  jmp        0x475d1d

// block 00475d1a
00475d1a  inc        dword ptr [ebp - 0xe]

// block 00475d1d
00475d1d  mov        ecx, 0x7fff
00475d22  cmp        si, cx
00475d25  jae        0x475d43

// block 00475d27
00475d27  mov        cx, word ptr [ebp - 0xe]
00475d2b  or         esi, dword ptr [ebp - 0x20]
00475d2e  mov        word ptr [eax], cx
00475d31  mov        ecx, dword ptr [ebp - 0xc]
00475d34  mov        dword ptr [eax + 2], ecx
00475d37  mov        ecx, dword ptr [ebp - 8]
00475d3a  mov        dword ptr [eax + 6], ecx
00475d3d  mov        word ptr [eax + 0xa], si
00475d41  jmp        0x475d63

// block 00475d43
00475d43  xor        edx, edx
00475d45  xor        ecx, ecx
00475d47  cmp        word ptr [ebp - 0x20], cx
00475d4b  sete       dl
00475d4e  dec        edx
00475d4f  and        edx, 0x80000000
00475d55  add        edx, 0x7fff8000
00475d5b  mov        dword ptr [eax + 8], edx

// block 00475d5e
00475d5e  mov        dword ptr [eax], ecx
00475d60  mov        dword ptr [eax + 4], ecx

// block 00475d63
00475d63  mov        ecx, dword ptr [ebp - 4]
00475d66  pop        edi
00475d67  pop        esi
00475d68  xor        ecx, ebp
00475d6a  pop        ebx
00475d6b  call       0x46c932

// block 00475d70
00475d70  leave      
00475d71  ret        
