00539a10  55                                 push       ebp
00539a11  8b ec                              mov        ebp, esp
00539a13  51                                 push       ecx
00539a14  6b 45 08 0c                        imul       eax, dword ptr [ebp + 8], 0xc
00539a18  8b 88 38 8b 5c 00                  mov        ecx, dword ptr [eax + 0x5c8b38]
00539a1e  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00539a21  6b 55 fc 0c                        imul       edx, dword ptr [ebp - 4], 0xc
00539a25  8b 82 3c 8b 5c 00                  mov        eax, dword ptr [edx + 0x5c8b3c]
00539a2b  3b 45 08                           cmp        eax, dword ptr [ebp + 8]
00539a2e  75 0f                              jne        0x539a3f
00539a30  6b 4d fc 0c                        imul       ecx, dword ptr [ebp - 4], 0xc
00539a34  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
00539a37  89 91 3c 8b 5c 00                  mov        dword ptr [ecx + 0x5c8b3c], edx
00539a3d  eb 0d                              jmp        0x539a4c
00539a3f  6b 45 fc 0c                        imul       eax, dword ptr [ebp - 4], 0xc
00539a43  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
00539a46  89 88 40 8b 5c 00                  mov        dword ptr [eax + 0x5c8b40], ecx
00539a4c  6b 55 08 0c                        imul       edx, dword ptr [ebp + 8], 0xc
00539a50  81 c2 38 8b 5c 00                  add        edx, 0x5c8b38
00539a56  6b 45 0c 0c                        imul       eax, dword ptr [ebp + 0xc], 0xc
00539a5a  05 38 8b 5c 00                     add        eax, 0x5c8b38
00539a5f  8b 0a                              mov        ecx, dword ptr [edx]
00539a61  89 08                              mov        dword ptr [eax], ecx
00539a63  8b 4a 04                           mov        ecx, dword ptr [edx + 4]
00539a66  89 48 04                           mov        dword ptr [eax + 4], ecx
00539a69  8b 52 08                           mov        edx, dword ptr [edx + 8]
00539a6c  89 50 08                           mov        dword ptr [eax + 8], edx
00539a6f  6b 45 0c 0c                        imul       eax, dword ptr [ebp + 0xc], 0xc
00539a73  6b 88 3c 8b 5c 00 0c               imul       ecx, dword ptr [eax + 0x5c8b3c], 0xc
00539a7a  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
00539a7d  89 91 38 8b 5c 00                  mov        dword ptr [ecx + 0x5c8b38], edx
00539a83  6b 45 0c 0c                        imul       eax, dword ptr [ebp + 0xc], 0xc
00539a87  6b 88 40 8b 5c 00 0c               imul       ecx, dword ptr [eax + 0x5c8b40], 0xc
00539a8e  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
00539a91  89 91 38 8b 5c 00                  mov        dword ptr [ecx + 0x5c8b38], edx
00539a97  6b 45 08 0c                        imul       eax, dword ptr [ebp + 8], 0xc
00539a9b  c7 80 38 8b 5c 00 00 00 00 00      mov        dword ptr [eax + 0x5c8b38], 0
00539aa5  8b e5                              mov        esp, ebp
00539aa7  5d                                 pop        ebp
00539aa8  c3                                 ret        
