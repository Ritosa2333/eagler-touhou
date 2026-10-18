00417ab0  55                                 push       ebp
00417ab1  8b ec                              mov        ebp, esp
00417ab3  83 ec 30                           sub        esp, 0x30
00417ab6  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
00417ab9  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00417abc  e8 ff db ff ff                     call       0x4156c0
00417ac1  0f af 45 08                        imul       eax, dword ptr [ebp + 8]
00417ac5  89 45 d0                           mov        dword ptr [ebp - 0x30], eax
00417ac8  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00417acb  e8 d0 db ff ff                     call       0x4156a0
00417ad0  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
00417ad3  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00417ad6  8b 88 00 01 00 00                  mov        ecx, dword ptr [eax + 0x100]
00417adc  89 4d e0                           mov        dword ptr [ebp - 0x20], ecx
00417adf  83 7d e0 15                        cmp        dword ptr [ebp - 0x20], 0x15
00417ae3  74 19                              je         0x417afe
00417ae5  83 7d e0 19                        cmp        dword ptr [ebp - 0x20], 0x19
00417ae9  0f 84 89 00 00 00                  je         0x417b78
00417aef  83 7d e0 1a                        cmp        dword ptr [ebp - 0x20], 0x1a
00417af3  0f 84 2c 01 00 00                  je         0x417c25
00417af9  e9 c4 01 00 00                     jmp        0x417cc2
00417afe  c7 45 ec 00 00 00 00               mov        dword ptr [ebp - 0x14], 0
00417b05  eb 09                              jmp        0x417b10
00417b07  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
00417b0a  83 c2 01                           add        edx, 1
00417b0d  89 55 ec                           mov        dword ptr [ebp - 0x14], edx
00417b10  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
00417b13  3b 45 08                           cmp        eax, dword ptr [ebp + 8]
00417b16  7d 5b                              jge        0x417b73
00417b18  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00417b1b  e8 e0 dc ff ff                     call       0x415800
00417b20  0f af 45 ec                        imul       eax, dword ptr [ebp - 0x14]
00417b24  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
00417b27  8d 14 81                           lea        edx, [ecx + eax*4]
00417b2a  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
00417b2d  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00417b30  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
00417b33  8d 14 81                           lea        edx, [ecx + eax*4]
00417b36  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
00417b39  c7 45 dc 00 00 00 00               mov        dword ptr [ebp - 0x24], 0
00417b40  eb 12                              jmp        0x417b54
00417b42  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
00417b45  83 c0 01                           add        eax, 1
00417b48  89 45 dc                           mov        dword ptr [ebp - 0x24], eax
00417b4b  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
00417b4e  83 c1 04                           add        ecx, 4
00417b51  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
00417b54  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
00417b57  3b 55 10                           cmp        edx, dword ptr [ebp + 0x10]
00417b5a  7d 15                              jge        0x417b71
00417b5c  8b 45 f4                           mov        eax, dword ptr [ebp - 0xc]
00417b5f  0f b6 48 03                        movzx      ecx, byte ptr [eax + 3]
00417b63  81 f1 ff 00 00 00                  xor        ecx, 0xff
00417b69  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
00417b6c  88 4a 03                           mov        byte ptr [edx + 3], cl
00417b6f  eb d1                              jmp        0x417b42
00417b71  eb 94                              jmp        0x417b07
00417b73  e9 4e 01 00 00                     jmp        0x417cc6
00417b78  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
00417b7b  89 45 fc                           mov        dword ptr [ebp - 4], eax
00417b7e  c7 45 d8 00 00 00 00               mov        dword ptr [ebp - 0x28], 0
00417b85  eb 12                              jmp        0x417b99
00417b87  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
00417b8a  83 c1 02                           add        ecx, 2
00417b8d  89 4d d8                           mov        dword ptr [ebp - 0x28], ecx
00417b90  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00417b93  83 c2 02                           add        edx, 2
00417b96  89 55 fc                           mov        dword ptr [ebp - 4], edx
00417b99  8b 45 d8                           mov        eax, dword ptr [ebp - 0x28]
00417b9c  3b 45 d0                           cmp        eax, dword ptr [ebp - 0x30]
00417b9f  7d 7f                              jge        0x417c20
00417ba1  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00417ba4  66 8b 11                           mov        dx, word ptr [ecx]
00417ba7  66 c1 ea 0f                        shr        dx, 0xf
00417bab  66 83 e2 01                        and        dx, 1
00417baf  0f b7 c2                           movzx      eax, dx
00417bb2  83 f0 01                           xor        eax, 1
00417bb5  66 83 e0 01                        and        ax, 1
00417bb9  66 c1 e0 0f                        shl        ax, 0xf
00417bbd  b9 ff 7f 00 00                     mov        ecx, 0x7fff
00417bc2  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00417bc5  66 23 0a                           and        cx, word ptr [edx]
00417bc8  66 0b c8                           or         cx, ax
00417bcb  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00417bce  66 89 08                           mov        word ptr [eax], cx
00417bd1  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00417bd4  66 8b 11                           mov        dx, word ptr [ecx]
00417bd7  66 c1 ea 0f                        shr        dx, 0xf
00417bdb  66 83 e2 01                        and        dx, 1
00417bdf  0f b7 c2                           movzx      eax, dx
00417be2  85 c0                              test       eax, eax
00417be4  74 02                              je         0x417be8
00417be6  eb 33                              jmp        0x417c1b
00417be8  b9 ff 83 00 00                     mov        ecx, 0x83ff
00417bed  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00417bf0  66 23 0a                           and        cx, word ptr [edx]
00417bf3  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00417bf6  66 89 08                           mov        word ptr [eax], cx
00417bf9  b9 1f fc 00 00                     mov        ecx, 0xfc1f
00417bfe  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00417c01  66 23 0a                           and        cx, word ptr [edx]
00417c04  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00417c07  66 89 08                           mov        word ptr [eax], cx
00417c0a  b9 e0 ff 00 00                     mov        ecx, 0xffe0
00417c0f  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00417c12  66 23 0a                           and        cx, word ptr [edx]
00417c15  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00417c18  66 89 08                           mov        word ptr [eax], cx
00417c1b  e9 67 ff ff ff                     jmp        0x417b87
00417c20  e9 a1 00 00 00                     jmp        0x417cc6
00417c25  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
00417c28  89 4d f8                           mov        dword ptr [ebp - 8], ecx
00417c2b  c7 45 e8 00 00 00 00               mov        dword ptr [ebp - 0x18], 0
00417c32  eb 09                              jmp        0x417c3d
00417c34  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
00417c37  83 c2 01                           add        edx, 1
00417c3a  89 55 e8                           mov        dword ptr [ebp - 0x18], edx
00417c3d  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
00417c40  3b 45 08                           cmp        eax, dword ptr [ebp + 8]
00417c43  7d 7b                              jge        0x417cc0
00417c45  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00417c48  e8 b3 db ff ff                     call       0x415800
00417c4d  0f af 45 e8                        imul       eax, dword ptr [ebp - 0x18]
00417c51  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
00417c54  8d 14 41                           lea        edx, [ecx + eax*2]
00417c57  89 55 f8                           mov        dword ptr [ebp - 8], edx
00417c5a  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00417c5d  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
00417c60  8d 14 41                           lea        edx, [ecx + eax*2]
00417c63  89 55 f8                           mov        dword ptr [ebp - 8], edx
00417c66  c7 45 d4 00 00 00 00               mov        dword ptr [ebp - 0x2c], 0
00417c6d  eb 12                              jmp        0x417c81
00417c6f  8b 45 d4                           mov        eax, dword ptr [ebp - 0x2c]
00417c72  83 c0 01                           add        eax, 1
00417c75  89 45 d4                           mov        dword ptr [ebp - 0x2c], eax
00417c78  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
00417c7b  83 c1 02                           add        ecx, 2
00417c7e  89 4d f8                           mov        dword ptr [ebp - 8], ecx
00417c81  8b 55 d4                           mov        edx, dword ptr [ebp - 0x2c]
00417c84  3b 55 10                           cmp        edx, dword ptr [ebp + 0x10]
00417c87  7d 32                              jge        0x417cbb
00417c89  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
00417c8c  66 8b 08                           mov        cx, word ptr [eax]
00417c8f  66 c1 e9 0c                        shr        cx, 0xc
00417c93  66 83 e1 0f                        and        cx, 0xf
00417c97  0f b7 d1                           movzx      edx, cx
00417c9a  83 f2 0f                           xor        edx, 0xf
00417c9d  66 83 e2 0f                        and        dx, 0xf
00417ca1  66 c1 e2 0c                        shl        dx, 0xc
00417ca5  b8 ff 0f 00 00                     mov        eax, 0xfff
00417caa  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
00417cad  66 23 01                           and        ax, word ptr [ecx]
00417cb0  66 0b c2                           or         ax, dx
00417cb3  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
00417cb6  66 89 02                           mov        word ptr [edx], ax
00417cb9  eb b4                              jmp        0x417c6f
00417cbb  e9 74 ff ff ff                     jmp        0x417c34
00417cc0  eb 04                              jmp        0x417cc6
00417cc2  32 c0                              xor        al, al
00417cc4  eb 02                              jmp        0x417cc8
00417cc6  b0 01                              mov        al, 1
00417cc8  8b e5                              mov        esp, ebp
00417cca  5d                                 pop        ebp
00417ccb  c2 10 00                           ret        0x10
