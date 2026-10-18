00539180  55                                 push       ebp
00539181  8b ec                              mov        ebp, esp
00539183  6b 45 08 0c                        imul       eax, dword ptr [ebp + 8], 0xc
00539187  6b 4d 0c 0c                        imul       ecx, dword ptr [ebp + 0xc], 0xc
0053918b  8b 90 38 8b 5c 00                  mov        edx, dword ptr [eax + 0x5c8b38]
00539191  89 91 38 8b 5c 00                  mov        dword ptr [ecx + 0x5c8b38], edx
00539197  6b 45 08 0c                        imul       eax, dword ptr [ebp + 8], 0xc
0053919b  6b 88 38 8b 5c 00 0c               imul       ecx, dword ptr [eax + 0x5c8b38], 0xc
005391a2  8b 91 40 8b 5c 00                  mov        edx, dword ptr [ecx + 0x5c8b40]
005391a8  3b 55 08                           cmp        edx, dword ptr [ebp + 8]
005391ab  75 16                              jne        0x5391c3
005391ad  6b 45 08 0c                        imul       eax, dword ptr [ebp + 8], 0xc
005391b1  6b 88 38 8b 5c 00 0c               imul       ecx, dword ptr [eax + 0x5c8b38], 0xc
005391b8  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
005391bb  89 91 40 8b 5c 00                  mov        dword ptr [ecx + 0x5c8b40], edx
005391c1  eb 14                              jmp        0x5391d7
005391c3  6b 45 08 0c                        imul       eax, dword ptr [ebp + 8], 0xc
005391c7  6b 88 38 8b 5c 00 0c               imul       ecx, dword ptr [eax + 0x5c8b38], 0xc
005391ce  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
005391d1  89 91 3c 8b 5c 00                  mov        dword ptr [ecx + 0x5c8b3c], edx
005391d7  6b 45 08 0c                        imul       eax, dword ptr [ebp + 8], 0xc
005391db  c7 80 38 8b 5c 00 00 00 00 00      mov        dword ptr [eax + 0x5c8b38], 0
005391e5  5d                                 pop        ebp
005391e6  c3                                 ret        
