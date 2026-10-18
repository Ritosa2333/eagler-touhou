00539960  55                                 push       ebp
00539961  8b ec                              mov        ebp, esp
00539963  51                                 push       ecx
00539964  6b 45 08 0c                        imul       eax, dword ptr [ebp + 8], 0xc
00539968  8b 88 3c 8b 5c 00                  mov        ecx, dword ptr [eax + 0x5c8b3c]
0053996e  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00539971  6b 55 fc 0c                        imul       edx, dword ptr [ebp - 4], 0xc
00539975  83 ba 40 8b 5c 00 00               cmp        dword ptr [edx + 0x5c8b40], 0
0053997c  74 0f                              je         0x53998d
0053997e  6b 45 fc 0c                        imul       eax, dword ptr [ebp - 4], 0xc
00539982  8b 88 40 8b 5c 00                  mov        ecx, dword ptr [eax + 0x5c8b40]
00539988  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0053998b  eb e4                              jmp        0x539971
0053998d  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00539990  8b e5                              mov        esp, ebp
00539992  5d                                 pop        ebp
00539993  c3                                 ret        
