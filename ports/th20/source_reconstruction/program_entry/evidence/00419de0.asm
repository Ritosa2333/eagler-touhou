; software timed frame scheduler
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
00419de0  55                                 push       ebp
00419de1  8b ec                              mov        ebp, esp
00419de3  83 ec 20                           sub        esp, 0x20
00419de6  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00419de9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419dec  e8 1f 2d 00 00                     call       0x41cb10
00419df1  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00419df4  dd 98 c8 20 00 00                  fstp       qword ptr [eax + 0x20c8]
00419dfa  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419dfd  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00419e00  f2 0f 10 81 d0 20 00 00            movsd      xmm0, qword ptr [ecx + 0x20d0]
00419e08  66 0f 2f 82 c8 20 00 00            comisd     xmm0, xmmword ptr [edx + 0x20c8]
00419e10  76 16                              jbe        0x419e28
00419e12  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00419e15  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419e18  f2 0f 10 81 c8 20 00 00            movsd      xmm0, qword ptr [ecx + 0x20c8]
00419e20  f2 0f 11 80 d8 20 00 00            movsd      qword ptr [eax + 0x20d8], xmm0
00419e28  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00419e2b  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00419e2e  f2 0f 10 80 c8 20 00 00            movsd      xmm0, qword ptr [eax + 0x20c8]
00419e36  f2 0f 11 82 d0 20 00 00            movsd      qword ptr [edx + 0x20d0], xmm0
00419e3e  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419e41  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00419e44  f2 0f 10 81 d8 20 00 00            movsd      xmm0, qword ptr [ecx + 0x20d8]
00419e4c  f2 0f 5c 82 c8 20 00 00            subsd      xmm0, qword ptr [edx + 0x20c8]
00419e54  f2 0f 59 05 88 cd 56 00            mulsd      xmm0, qword ptr [0x56cd88]
00419e5c  66 0f 2f 05 70 cd 56 00            comisd     xmm0, xmmword ptr [0x56cd70]
00419e64  72 09                              jb         0x419e6f
00419e66  6a 01                              push       1
00419e68  ff 15 6c c1 56 00                  call       dword ptr [0x56c16c] ; KERNEL32.dll!Sleep
00419e6e  90                                 nop
00419e6f  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00419e72  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419e75  f2 0f 10 80 c8 20 00 00            movsd      xmm0, qword ptr [eax + 0x20c8]
00419e7d  66 0f 2f 81 d8 20 00 00            comisd     xmm0, xmmword ptr [ecx + 0x20d8]
00419e85  0f 86 96 01 00 00                  jbe        0x41a021
00419e8b  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00419e8e  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00419e91  f2 0f 10 82 c8 20 00 00            movsd      xmm0, qword ptr [edx + 0x20c8]
00419e99  66 0f 2f 80 d8 20 00 00            comisd     xmm0, xmmword ptr [eax + 0x20d8]
00419ea1  76 28                              jbe        0x419ecb
00419ea3  f2 0f 10 05 68 c4 56 00            movsd      xmm0, qword ptr [0x56c468]
00419eab  f2 0f 5e 05 70 c4 56 00            divsd      xmm0, qword ptr [0x56c470]
00419eb3  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419eb6  f2 0f 58 81 d8 20 00 00            addsd      xmm0, qword ptr [ecx + 0x20d8]
00419ebe  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00419ec1  f2 0f 11 82 d8 20 00 00            movsd      qword ptr [edx + 0x20d8], xmm0
00419ec9  eb c0                              jmp        0x419e8b
00419ecb  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
00419ed1  e8 ea b6 02 00                     call       0x4455c0
00419ed6  6a 02                              push       2
00419ed8  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419edd  e8 fe 3d 00 00                     call       0x41dce0
00419ee2  8b 0d d8 66 5b 00                  mov        ecx, dword ptr [0x5b66d8]
00419ee8  e8 23 89 ff ff                     call       0x412810
00419eed  89 45 f8                           mov        dword ptr [ebp - 8], eax
00419ef0  83 7d f8 00                        cmp        dword ptr [ebp - 8], 0
00419ef4  75 16                              jne        0x419f0c
00419ef6  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419efb  e8 30 ff 0b 00                     call       0x4d9e30
00419f00  b8 01 00 00 00                     mov        eax, 1
00419f05  e9 19 01 00 00                     jmp        0x41a023
00419f0c  83 7d f8 ff                        cmp        dword ptr [ebp - 8], -1
00419f10  75 14                              jne        0x419f26
00419f12  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419f17  e8 14 ff 0b 00                     call       0x4d9e30
00419f1c  b8 02 00 00 00                     mov        eax, 2
00419f21  e9 fd 00 00 00                     jmp        0x41a023
00419f26  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00419f29  8a 48 70                           mov        cl, byte ptr [eax + 0x70]
00419f2c  80 c1 01                           add        cl, 1
00419f2f  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00419f32  88 4a 70                           mov        byte ptr [edx + 0x70], cl
00419f35  0f b6 05 84 4f 5c 00               movzx      eax, byte ptr [0x5c4f84]
00419f3c  83 c0 01                           add        eax, 1
00419f3f  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419f42  0f be 51 70                        movsx      edx, byte ptr [ecx + 0x70]
00419f46  3b c2                              cmp        eax, edx
00419f48  0f 8f a7 00 00 00                  jg         0x419ff5
00419f4e  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419f53  e8 d8 87 ff ff                     call       0x412730
00419f58  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
00419f5b  8b 45 f4                           mov        eax, dword ptr [ebp - 0xc]
00419f5e  8b 08                              mov        ecx, dword ptr [eax]
00419f60  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
00419f63  52                                 push       edx
00419f64  8b 81 a4 00 00 00                  mov        eax, dword ptr [ecx + 0xa4]
00419f6a  ff d0                              call       eax
00419f6c  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
00419f72  e8 c9 ba 02 00                     call       0x445a40
00419f77  c7 05 f8 5a 5c 00 ff 00 00 00      mov        dword ptr [0x5c5af8], 0xff
00419f81  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419f86  e8 d5 3a 0c 00                     call       0x4dda60
00419f8b  8b 0d d8 66 5b 00                  mov        ecx, dword ptr [0x5b66d8]
00419f91  e8 0a 8b ff ff                     call       0x412aa0
00419f96  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
00419f9c  e8 1f b6 02 00                     call       0x4455c0
00419fa1  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419fa6  e8 85 87 ff ff                     call       0x412730
00419fab  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
00419fae  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419fb1  8b 11                              mov        edx, dword ptr [ecx]
00419fb3  8b 82 04 01 00 00                  mov        eax, dword ptr [edx + 0x104]
00419fb9  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
00419fbc  6a 00                              push       0
00419fbe  6a 00                              push       0
00419fc0  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419fc3  51                                 push       ecx
00419fc4  ff 55 e8                           call       dword ptr [ebp - 0x18]
00419fc7  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419fcc  e8 5f 87 ff ff                     call       0x412730
00419fd1  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
00419fd4  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
00419fd7  8b 02                              mov        eax, dword ptr [edx]
00419fd9  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
00419fdc  51                                 push       ecx
00419fdd  8b 90 a8 00 00 00                  mov        edx, dword ptr [eax + 0xa8]
00419fe3  ff d2                              call       edx
00419fe5  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00419fe8  c6 40 70 00                        mov        byte ptr [eax + 0x70], 0
00419fec  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419fef  e8 ac f9 ff ff                     call       0x4199a0
00419ff4  90                                 nop
00419ff5  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419ff8  e8 13 2b 00 00                     call       0x41cb10
00419ffd  dd 5d e0                           fstp       qword ptr [ebp - 0x20]
0041a000  f2 0f 10 45 e0                     movsd      xmm0, qword ptr [ebp - 0x20]
0041a005  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041a008  f2 0f 5c 81 c8 20 00 00            subsd      xmm0, qword ptr [ecx + 0x20c8]
0041a010  f2 0f 11 05 18 5b 5c 00            movsd      qword ptr [0x5c5b18], xmm0
0041a018  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041a01b  e8 20 85 ff ff                     call       0x412540
0041a020  90                                 nop
0041a021  33 c0                              xor        eax, eax
0041a023  8b e5                              mov        esp, ebp
0041a025  5d                                 pop        ebp
0041a026  c3                                 ret
