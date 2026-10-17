; clock source, unresolved lock and uint64 conversion dependencies
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
0041cb10  55                                 push       ebp
0041cb11  8b ec                              mov        ebp, esp
0041cb13  83 ec 40                           sub        esp, 0x40
0041cb16  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0041cb1b  33 c5                              xor        eax, ebp
0041cb1d  89 45 fc                           mov        dword ptr [ebp - 4], eax
0041cb20  89 4d d8                           mov        dword ptr [ebp - 0x28], ecx
0041cb23  6a 05                              push       5
0041cb25  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0041cb2a  e8 e1 f7 fe ff                     call       0x40c310
0041cb2f  50                                 push       eax
0041cb30  8d 4d f8                           lea        ecx, [ebp - 8]
0041cb33  e8 f8 eb fe ff                     call       0x40b730
0041cb38  90                                 nop
0041cb39  a1 d0 67 5b 00                     mov        eax, dword ptr [0x5b67d0]
0041cb3e  0b 05 d4 67 5b 00                  or         eax, dword ptr [0x5b67d4]
0041cb44  0f 84 88 00 00 00                  je         0x41cbd2
0041cb4a  8d 4d f0                           lea        ecx, [ebp - 0x10]
0041cb4d  51                                 push       ecx
0041cb4e  ff 15 1c c1 56 00                  call       dword ptr [0x56c11c] ; KERNEL32.dll!QueryPerformanceCounter
0041cb54  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0041cb57  2b 0d d8 67 5b 00                  sub        ecx, dword ptr [0x5b67d8]
0041cb5d  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
0041cb60  1b 15 dc 67 5b 00                  sbb        edx, dword ptr [0x5b67dc]
0041cb66  e8 95 62 12 00                     call       0x542e00
0041cb6b  8b 15 d4 67 5b 00                  mov        edx, dword ptr [0x5b67d4]
0041cb71  8b 0d d0 67 5b 00                  mov        ecx, dword ptr [0x5b67d0]
0041cb77  f2 0f 11 45 d0                     movsd      qword ptr [ebp - 0x30], xmm0
0041cb7c  e8 7f 62 12 00                     call       0x542e00
0041cb81  f2 0f 10 4d d0                     movsd      xmm1, qword ptr [ebp - 0x30]
0041cb86  f2 0f 5e c8                        divsd      xmm1, xmm0
0041cb8a  f2 0f 11 4d e8                     movsd      qword ptr [ebp - 0x18], xmm1
0041cb8f  f2 0f 10 05 38 88 5b 00            movsd      xmm0, qword ptr [0x5b8838]
0041cb97  66 0f 2f 45 e8                     comisd     xmm0, xmmword ptr [ebp - 0x18]
0041cb9c  76 0d                              jbe        0x41cbab
0041cb9e  f2 0f 10 45 e8                     movsd      xmm0, qword ptr [ebp - 0x18]
0041cba3  f2 0f 11 05 38 88 5b 00            movsd      qword ptr [0x5b8838], xmm0
0041cbab  f2 0f 10 45 e8                     movsd      xmm0, qword ptr [ebp - 0x18]
0041cbb0  f2 0f 5c 05 38 88 5b 00            subsd      xmm0, qword ptr [0x5b8838]
0041cbb8  f2 0f 11 45 c8                     movsd      qword ptr [ebp - 0x38], xmm0
0041cbbd  8d 4d f8                           lea        ecx, [ebp - 8]
0041cbc0  e8 3b ed fe ff                     call       0x40b900
0041cbc5  dd 45 c8                           fld        qword ptr [ebp - 0x38]
0041cbc8  e9 8f 00 00 00                     jmp        0x41cc5c
0041cbd2  ff 15 e8 c2 56 00                  call       dword ptr [0x56c2e8] ; WINMM.dll!timeGetTime
0041cbd8  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
0041cbdb  f2 0f 2a 45 e4                     cvtsi2sd   xmm0, dword ptr [ebp - 0x1c]
0041cbe0  8b 55 e4                           mov        edx, dword ptr [ebp - 0x1c]
0041cbe3  c1 ea 1f                           shr        edx, 0x1f
0041cbe6  f2 0f 58 04 d5 d0 cd 56 00         addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
0041cbef  f2 0f 11 45 dc                     movsd      qword ptr [ebp - 0x24], xmm0
0041cbf4  f2 0f 10 05 38 88 5b 00            movsd      xmm0, qword ptr [0x5b8838]
0041cbfc  66 0f 2f 45 dc                     comisd     xmm0, xmmword ptr [ebp - 0x24]
0041cc01  76 0d                              jbe        0x41cc10
0041cc03  f2 0f 10 45 dc                     movsd      xmm0, qword ptr [ebp - 0x24]
0041cc08  f2 0f 11 05 38 88 5b 00            movsd      qword ptr [0x5b8838], xmm0
0041cc10  f2 0f 10 05 38 88 5b 00            movsd      xmm0, qword ptr [0x5b8838]
0041cc18  f2 0f 59 05 88 cd 56 00            mulsd      xmm0, qword ptr [0x56cd88]
0041cc20  f2 0f 11 45 e8                     movsd      qword ptr [ebp - 0x18], xmm0
0041cc25  f2 0f 10 45 dc                     movsd      xmm0, qword ptr [ebp - 0x24]
0041cc2a  f2 0f 5c 45 e8                     subsd      xmm0, qword ptr [ebp - 0x18]
0041cc2f  f2 0f 5e 05 88 cd 56 00            divsd      xmm0, qword ptr [0x56cd88]
0041cc37  f2 0f 11 45 e8                     movsd      qword ptr [ebp - 0x18], xmm0
0041cc3c  f2 0f 10 45 e8                     movsd      xmm0, qword ptr [ebp - 0x18]
0041cc41  f2 0f 11 45 c0                     movsd      qword ptr [ebp - 0x40], xmm0
0041cc46  8d 4d f8                           lea        ecx, [ebp - 8]
0041cc49  e8 b2 ec fe ff                     call       0x40b900
0041cc4e  dd 45 c0                           fld        qword ptr [ebp - 0x40]
0041cc51  eb 09                              jmp        0x41cc5c
0041cc5c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041cc5f  33 cd                              xor        ecx, ebp
0041cc61  e8 88 5d 12 00                     call       0x5429ee
0041cc66  8b e5                              mov        esp, ebp
0041cc68  5d                                 pop        ebp
0041cc69  c3                                 ret
