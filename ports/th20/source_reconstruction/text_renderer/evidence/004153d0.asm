004153d0  55                                 push       ebp
004153d1  8b ec                              mov        ebp, esp
004153d3  51                                 push       ecx
004153d4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004153d7  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
004153da  50                                 push       eax
004153db  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
004153de  51                                 push       ecx
004153df  8b 55 08                           mov        edx, dword ptr [ebp + 8]
004153e2  52                                 push       edx
004153e3  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004153e6  e8 35 fd ff ff                     call       0x415120
004153eb  0f b6 c0                           movzx      eax, al
004153ee  85 c0                              test       eax, eax
004153f0  74 04                              je         0x4153f6
004153f2  b0 01                              mov        al, 1
004153f4  eb 3c                              jmp        0x415432
004153f6  83 7d 10 19                        cmp        dword ptr [ebp + 0x10], 0x19
004153fa  74 06                              je         0x415402
004153fc  83 7d 10 1a                        cmp        dword ptr [ebp + 0x10], 0x1a
00415400  75 14                              jne        0x415416
00415402  6a 15                              push       0x15
00415404  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
00415407  51                                 push       ecx
00415408  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0041540b  52                                 push       edx
0041540c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041540f  e8 0c fd ff ff                     call       0x415120
00415414  eb 1c                              jmp        0x415432
00415416  83 7d 10 17                        cmp        dword ptr [ebp + 0x10], 0x17
0041541a  75 14                              jne        0x415430
0041541c  6a 16                              push       0x16
0041541e  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00415421  50                                 push       eax
00415422  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00415425  51                                 push       ecx
00415426  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00415429  e8 f2 fc ff ff                     call       0x415120
0041542e  eb 02                              jmp        0x415432
00415430  32 c0                              xor        al, al
00415432  8b e5                              mov        esp, ebp
00415434  5d                                 pop        ebp
00415435  c2 0c 00                           ret        0xc
