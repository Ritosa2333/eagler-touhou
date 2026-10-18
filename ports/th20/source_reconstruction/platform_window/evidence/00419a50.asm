00419a50  55                                 push       ebp
00419a51  8b ec                              mov        ebp, esp
00419a53  83 ec 18                           sub        esp, 0x18
00419a56  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00419a59  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419a5c  e8 af 30 00 00                     call       0x41cb10
00419a61  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00419a64  dd 98 c8 20 00 00                  fstp       qword ptr [eax + 0x20c8]
00419a6a  0f b6 0d 89 4f 5c 00               movzx      ecx, byte ptr [0x5c4f89]
00419a71  83 f9 01                           cmp        ecx, 1
00419a74  0f 85 9a 00 00 00                  jne        0x419b14
00419a7a  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00419a7d  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00419a80  f2 0f 10 82 c8 20 00 00            movsd      xmm0, qword ptr [edx + 0x20c8]
00419a88  f2 0f 5c 80 e8 20 00 00            subsd      xmm0, qword ptr [eax + 0x20e8]
00419a90  f2 0f 10 0d 68 c4 56 00            movsd      xmm1, qword ptr [0x56c468]
00419a98  f2 0f 5e 0d 70 c4 56 00            divsd      xmm1, qword ptr [0x56c470]
00419aa0  66 0f 2f c8                        comisd     xmm1, xmm0
00419aa4  76 6e                              jbe        0x419b14
00419aa6  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419aa9  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00419aac  f2 0f 10 81 c8 20 00 00            movsd      xmm0, qword ptr [ecx + 0x20c8]
00419ab4  f2 0f 5c 82 e8 20 00 00            subsd      xmm0, qword ptr [edx + 0x20e8]
00419abc  66 0f 2f 05 50 cd 56 00            comisd     xmm0, xmmword ptr [0x56cd50]
00419ac4  76 4e                              jbe        0x419b14
00419ac6  f2 0f 10 05 68 c4 56 00            movsd      xmm0, qword ptr [0x56c468]
00419ace  f2 0f 5e 05 70 c4 56 00            divsd      xmm0, qword ptr [0x56c470]
00419ad6  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00419ad9  f2 0f 58 80 e8 20 00 00            addsd      xmm0, qword ptr [eax + 0x20e8]
00419ae1  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419ae4  f2 0f 5c 81 c8 20 00 00            subsd      xmm0, qword ptr [ecx + 0x20c8]
00419aec  f2 0f 59 05 88 cd 56 00            mulsd      xmm0, qword ptr [0x56cd88]
00419af4  f2 0f 5c 05 80 cd 56 00            subsd      xmm0, qword ptr [0x56cd80]
00419afc  f2 0f 2c d0                        cvttsd2si  edx, xmm0
00419b00  89 55 f8                           mov        dword ptr [ebp - 8], edx
00419b03  83 7d f8 00                        cmp        dword ptr [ebp - 8], 0
00419b07  7e 0b                              jle        0x419b14
00419b09  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
00419b0c  50                                 push       eax
00419b0d  ff 15 6c c1 56 00                  call       dword ptr [0x56c16c] ; KERNEL32.dll!Sleep
00419b13  90                                 nop        
00419b14  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419b17  e8 f4 2f 00 00                     call       0x41cb10
00419b1c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419b1f  dd 99 f0 20 00 00                  fstp       qword ptr [ecx + 0x20f0]
00419b25  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419b28  e8 33 fc ff ff                     call       0x419760
00419b2d  90                                 nop        
00419b2e  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419b33  e8 f8 8b ff ff                     call       0x412730
00419b38  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
00419b3b  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
00419b3e  8b 02                              mov        eax, dword ptr [edx]
00419b40  8b 48 44                           mov        ecx, dword ptr [eax + 0x44]
00419b43  89 4d ec                           mov        dword ptr [ebp - 0x14], ecx
00419b46  6a 00                              push       0
00419b48  6a 00                              push       0
00419b4a  6a 00                              push       0
00419b4c  6a 00                              push       0
00419b4e  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
00419b51  52                                 push       edx
00419b52  ff 55 ec                           call       dword ptr [ebp - 0x14]
00419b55  85 c0                              test       eax, eax
00419b57  7d 5d                              jge        0x419bb6
00419b59  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419b5e  e8 dd 3c 0c 00                     call       0x4dd840
00419b63  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
00419b69  e8 72 40 00 00                     call       0x41dbe0
00419b6e  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419b73  e8 b8 8b ff ff                     call       0x412730
00419b78  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
00419b7b  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00419b7e  8b 08                              mov        ecx, dword ptr [eax]
00419b80  8b 51 40                           mov        edx, dword ptr [ecx + 0x40]
00419b83  89 55 e8                           mov        dword ptr [ebp - 0x18], edx
00419b86  68 24 4e 5c 00                     push       0x5c4e24
00419b8b  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00419b8e  50                                 push       eax
00419b8f  ff 55 e8                           call       dword ptr [ebp - 0x18]
00419b92  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
00419b98  e8 53 3e 00 00                     call       0x41d9f0
00419b9d  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419ba2  e8 c9 21 0c 00                     call       0x4dbd70
00419ba7  e8 14 07 00 00                     call       0x41a2c0
00419bac  c7 05 6c 58 5c 00 02 00 00 00      mov        dword ptr [0x5c586c], 2
00419bb6  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419bb9  e8 52 2f 00 00                     call       0x41cb10
00419bbe  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419bc1  dd 99 e8 20 00 00                  fstp       qword ptr [ecx + 0x20e8]
00419bc7  e8 f4 f7 ff ff                     call       0x4193c0
00419bcc  90                                 nop        
00419bcd  8b e5                              mov        esp, ebp
00419bcf  5d                                 pop        ebp
00419bd0  c3                                 ret        
