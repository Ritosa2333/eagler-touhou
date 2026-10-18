; present paced frame scheduler
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
0041a030  55                                 push       ebp
0041a031  8b ec                              mov        ebp, esp
0041a033  83 ec 20                           sub        esp, 0x20
0041a036  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041a039  b8 01 00 00 00                     mov        eax, 1
0041a03e  85 c0                              test       eax, eax
0041a040  0f 84 56 01 00 00                  je         0x41a19c
0041a046  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0041a04c  e8 6f b5 02 00                     call       0x4455c0
0041a051  6a 02                              push       2
0041a053  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041a058  e8 83 3c 00 00                     call       0x41dce0
0041a05d  8b 0d d8 66 5b 00                  mov        ecx, dword ptr [0x5b66d8]
0041a063  e8 a8 87 ff ff                     call       0x412810
0041a068  89 45 f8                           mov        dword ptr [ebp - 8], eax
0041a06b  83 7d f8 00                        cmp        dword ptr [ebp - 8], 0
0041a06f  75 16                              jne        0x41a087
0041a071  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041a076  e8 b5 fd 0b 00                     call       0x4d9e30
0041a07b  b8 01 00 00 00                     mov        eax, 1
0041a080  e9 19 01 00 00                     jmp        0x41a19e
0041a087  83 7d f8 ff                        cmp        dword ptr [ebp - 8], -1
0041a08b  75 14                              jne        0x41a0a1
0041a08d  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041a092  e8 99 fd 0b 00                     call       0x4d9e30
0041a097  b8 02 00 00 00                     mov        eax, 2
0041a09c  e9 fd 00 00 00                     jmp        0x41a19e
0041a0a1  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041a0a4  8a 51 70                           mov        dl, byte ptr [ecx + 0x70]
0041a0a7  80 c2 01                           add        dl, 1
0041a0aa  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041a0ad  88 50 70                           mov        byte ptr [eax + 0x70], dl
0041a0b0  0f b6 0d 84 4f 5c 00               movzx      ecx, byte ptr [0x5c4f84]
0041a0b7  83 c1 01                           add        ecx, 1
0041a0ba  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0041a0bd  0f be 42 70                        movsx      eax, byte ptr [edx + 0x70]
0041a0c1  3b c8                              cmp        ecx, eax
0041a0c3  0f 8f a7 00 00 00                  jg         0x41a170
0041a0c9  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041a0ce  e8 5d 86 ff ff                     call       0x412730
0041a0d3  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
0041a0d6  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0041a0d9  8b 11                              mov        edx, dword ptr [ecx]
0041a0db  8b 45 f4                           mov        eax, dword ptr [ebp - 0xc]
0041a0de  50                                 push       eax
0041a0df  8b 8a a4 00 00 00                  mov        ecx, dword ptr [edx + 0xa4]
0041a0e5  ff d1                              call       ecx
0041a0e7  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0041a0ed  e8 4e b9 02 00                     call       0x445a40
0041a0f2  c7 05 f8 5a 5c 00 ff 00 00 00      mov        dword ptr [0x5c5af8], 0xff
0041a0fc  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041a101  e8 5a 39 0c 00                     call       0x4dda60
0041a106  8b 0d d8 66 5b 00                  mov        ecx, dword ptr [0x5b66d8]
0041a10c  e8 8f 89 ff ff                     call       0x412aa0
0041a111  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0041a117  e8 a4 b4 02 00                     call       0x4455c0
0041a11c  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041a121  e8 0a 86 ff ff                     call       0x412730
0041a126  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0041a129  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0041a12c  8b 02                              mov        eax, dword ptr [edx]
0041a12e  8b 88 04 01 00 00                  mov        ecx, dword ptr [eax + 0x104]
0041a134  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
0041a137  6a 00                              push       0
0041a139  6a 00                              push       0
0041a13b  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0041a13e  52                                 push       edx
0041a13f  ff 55 e8                           call       dword ptr [ebp - 0x18]
0041a142  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041a147  e8 e4 85 ff ff                     call       0x412730
0041a14c  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
0041a14f  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
0041a152  8b 08                              mov        ecx, dword ptr [eax]
0041a154  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
0041a157  52                                 push       edx
0041a158  8b 81 a8 00 00 00                  mov        eax, dword ptr [ecx + 0xa8]
0041a15e  ff d0                              call       eax
0041a160  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041a163  c6 41 70 00                        mov        byte ptr [ecx + 0x70], 0
0041a167  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041a16a  e8 e1 f8 ff ff                     call       0x419a50
0041a16f  90                                 nop
0041a170  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041a173  e8 98 29 00 00                     call       0x41cb10
0041a178  dd 5d e0                           fstp       qword ptr [ebp - 0x20]
0041a17b  f2 0f 10 45 e0                     movsd      xmm0, qword ptr [ebp - 0x20]
0041a180  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0041a183  f2 0f 5c 82 c8 20 00 00            subsd      xmm0, qword ptr [edx + 0x20c8]
0041a18b  f2 0f 11 05 18 5b 5c 00            movsd      qword ptr [0x5c5b18], xmm0
0041a193  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041a196  e8 a5 83 ff ff                     call       0x412540
0041a19b  90                                 nop
0041a19c  33 c0                              xor        eax, eax
0041a19e  8b e5                              mov        esp, ebp
0041a1a0  5d                                 pop        ebp
0041a1a1  c3                                 ret
