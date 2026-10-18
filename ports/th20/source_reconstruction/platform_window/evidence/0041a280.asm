0041a280  55                                 push       ebp
0041a281  8b ec                              mov        ebp, esp
0041a283  83 ec 08                           sub        esp, 8
0041a286  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041a289  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041a28c  50                                 push       eax
0041a28d  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041a290  e8 6b f9 ff ff                     call       0x419c00
0041a295  85 c0                              test       eax, eax
0041a297  75 14                              jne        0x41a2ad
0041a299  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041a29c  8b 51 08                           mov        edx, dword ptr [ecx + 8]
0041a29f  23 55 08                           and        edx, dword ptr [ebp + 8]
0041a2a2  75 09                              jne        0x41a2ad
0041a2a4  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
0041a2ab  eb 07                              jmp        0x41a2b4
0041a2ad  c7 45 f8 01 00 00 00               mov        dword ptr [ebp - 8], 1
0041a2b4  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0041a2b7  8b e5                              mov        esp, ebp
0041a2b9  5d                                 pop        ebp
0041a2ba  c2 04 00                           ret        4
