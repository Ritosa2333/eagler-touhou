004ac1a0  55                                 push       ebp
004ac1a1  8b ec                              mov        ebp, esp
004ac1a3  51                                 push       ecx
004ac1a4  68 d8 05 57 00                     push       0x5705d8 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\fps.cpp:61 FPSInf'
004ac1a9  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004ac1af  e8 1c f9 ff ff                     call       0x4abad0
004ac1b4  89 45 fc                           mov        dword ptr [ebp - 4], eax
004ac1b7  83 7d fc 00                        cmp        dword ptr [ebp - 4], 0
004ac1bb  75 04                              jne        0x4ac1c1
004ac1bd  33 c0                              xor        eax, eax
004ac1bf  eb 33                              jmp        0x4ac1f4
004ac1c1  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004ac1c4  e8 b7 fe ff ff                     call       0x4ac080
004ac1c9  85 c0                              test       eax, eax
004ac1cb  74 04                              je         0x4ac1d1
004ac1cd  eb 07                              jmp        0x4ac1d6
004ac1d1  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ac1d4  eb 1e                              jmp        0x4ac1f4
004ac1d6  83 7d fc 00                        cmp        dword ptr [ebp - 4], 0
004ac1da  74 16                              je         0x4ac1f2
004ac1dc  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ac1df  50                                 push       eax
004ac1e0  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004ac1e6  e8 d5 35 f7 ff                     call       0x41f7c0
004ac1eb  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
004ac1f2  33 c0                              xor        eax, eax
004ac1f4  8b e5                              mov        esp, ebp
004ac1f6  5d                                 pop        ebp
004ac1f7  c3                                 ret        
