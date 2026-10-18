; unlimited frame scheduler
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
00419c20  55                                 push       ebp
00419c21  8b ec                              mov        ebp, esp
00419c23  83 ec 20                           sub        esp, 0x20
00419c26  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00419c29  a1 e8 87 5b 00                     mov        eax, dword ptr [0x5b87e8]
00419c2e  c1 e8 02                           shr        eax, 2
00419c31  83 e0 01                           and        eax, 1
00419c34  74 40                              je         0x419c76
00419c36  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419c39  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00419c3c  f2 0f 10 81 c8 20 00 00            movsd      xmm0, qword ptr [ecx + 0x20c8]
00419c44  66 0f 2f 82 d8 20 00 00            comisd     xmm0, xmmword ptr [edx + 0x20d8]
00419c4c  76 28                              jbe        0x419c76
00419c4e  f2 0f 10 05 68 c4 56 00            movsd      xmm0, qword ptr [0x56c468]
00419c56  f2 0f 5e 05 70 c4 56 00            divsd      xmm0, qword ptr [0x56c470]
00419c5e  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00419c61  f2 0f 58 80 d8 20 00 00            addsd      xmm0, qword ptr [eax + 0x20d8]
00419c69  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419c6c  f2 0f 11 81 d8 20 00 00            movsd      qword ptr [ecx + 0x20d8], xmm0
00419c74  eb c0                              jmp        0x419c36
00419c76  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
00419c7c  e8 3f b9 02 00                     call       0x4455c0
00419c81  6a 02                              push       2
00419c83  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419c88  e8 53 40 00 00                     call       0x41dce0
00419c8d  8b 0d d8 66 5b 00                  mov        ecx, dword ptr [0x5b66d8]
00419c93  e8 78 8b ff ff                     call       0x412810
00419c98  89 45 f8                           mov        dword ptr [ebp - 8], eax
00419c9b  83 7d f8 00                        cmp        dword ptr [ebp - 8], 0
00419c9f  75 16                              jne        0x419cb7
00419ca1  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419ca6  e8 85 01 0c 00                     call       0x4d9e30
00419cab  b8 01 00 00 00                     mov        eax, 1
00419cb0  e9 18 01 00 00                     jmp        0x419dcd
00419cb7  83 7d f8 ff                        cmp        dword ptr [ebp - 8], -1
00419cbb  75 14                              jne        0x419cd1
00419cbd  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419cc2  e8 69 01 0c 00                     call       0x4d9e30
00419cc7  b8 02 00 00 00                     mov        eax, 2
00419ccc  e9 fc 00 00 00                     jmp        0x419dcd
00419cd1  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00419cd4  8a 42 70                           mov        al, byte ptr [edx + 0x70]
00419cd7  04 01                              add        al, 1
00419cd9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419cdc  88 41 70                           mov        byte ptr [ecx + 0x70], al
00419cdf  0f b6 15 84 4f 5c 00               movzx      edx, byte ptr [0x5c4f84]
00419ce6  83 c2 01                           add        edx, 1
00419ce9  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00419cec  0f be 48 70                        movsx      ecx, byte ptr [eax + 0x70]
00419cf0  3b d1                              cmp        edx, ecx
00419cf2  0f 8f a7 00 00 00                  jg         0x419d9f
00419cf8  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419cfd  e8 2e 8a ff ff                     call       0x412730
00419d02  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
00419d05  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
00419d08  8b 02                              mov        eax, dword ptr [edx]
00419d0a  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
00419d0d  51                                 push       ecx
00419d0e  8b 90 a4 00 00 00                  mov        edx, dword ptr [eax + 0xa4]
00419d14  ff d2                              call       edx
00419d16  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
00419d1c  e8 1f bd 02 00                     call       0x445a40
00419d21  c7 05 f8 5a 5c 00 ff 00 00 00      mov        dword ptr [0x5c5af8], 0xff
00419d2b  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419d30  e8 2b 3d 0c 00                     call       0x4dda60
00419d35  8b 0d d8 66 5b 00                  mov        ecx, dword ptr [0x5b66d8]
00419d3b  e8 60 8d ff ff                     call       0x412aa0
00419d40  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
00419d46  e8 75 b8 02 00                     call       0x4455c0
00419d4b  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419d50  e8 db 89 ff ff                     call       0x412730
00419d55  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
00419d58  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00419d5b  8b 08                              mov        ecx, dword ptr [eax]
00419d5d  8b 91 04 01 00 00                  mov        edx, dword ptr [ecx + 0x104]
00419d63  89 55 e8                           mov        dword ptr [ebp - 0x18], edx
00419d66  6a 00                              push       0
00419d68  6a 00                              push       0
00419d6a  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00419d6d  50                                 push       eax
00419d6e  ff 55 e8                           call       dword ptr [ebp - 0x18]
00419d71  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419d76  e8 b5 89 ff ff                     call       0x412730
00419d7b  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
00419d7e  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
00419d81  8b 11                              mov        edx, dword ptr [ecx]
00419d83  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
00419d86  50                                 push       eax
00419d87  8b 8a a8 00 00 00                  mov        ecx, dword ptr [edx + 0xa8]
00419d8d  ff d1                              call       ecx
00419d8f  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00419d92  c6 42 70 00                        mov        byte ptr [edx + 0x70], 0
00419d96  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419d99  e8 42 f6 ff ff                     call       0x4193e0
00419d9e  90                                 nop
00419d9f  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419da2  e8 69 2d 00 00                     call       0x41cb10
00419da7  dd 5d e0                           fstp       qword ptr [ebp - 0x20]
00419daa  f2 0f 10 45 e0                     movsd      xmm0, qword ptr [ebp - 0x20]
00419daf  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00419db2  f2 0f 5c 80 c8 20 00 00            subsd      xmm0, qword ptr [eax + 0x20c8]
00419dba  f2 0f 11 05 18 5b 5c 00            movsd      qword ptr [0x5c5b18], xmm0
00419dc2  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419dc5  e8 76 87 ff ff                     call       0x412540
00419dca  90                                 nop
00419dcb  33 c0                              xor        eax, eax
00419dcd  8b e5                              mov        esp, ebp
00419dcf  5d                                 pop        ebp
00419dd0  c3                                 ret
