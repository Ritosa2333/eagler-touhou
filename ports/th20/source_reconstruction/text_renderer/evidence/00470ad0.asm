00470ad0  55                                 push       ebp
00470ad1  8b ec                              mov        ebp, esp
00470ad3  51                                 push       ecx
00470ad4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00470ad7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00470ada  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00470add  89 88 c0 a1 01 00                  mov        dword ptr [eax + 0x1a1c0], ecx
00470ae3  8b e5                              mov        esp, ebp
00470ae5  5d                                 pop        ebp
00470ae6  c2 04 00                           ret        4
