004d85c0  55                                 push       ebp
004d85c1  8b ec                              mov        ebp, esp
004d85c3  51                                 push       ecx
004d85c4  68 c8 18 57 00                     push       0x5718c8 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\loading.cpp:239 LoadingInf'
004d85c9  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004d85cf  e8 cc f8 ff ff                     call       0x4d7ea0
004d85d4  89 45 fc                           mov        dword ptr [ebp - 4], eax
004d85d7  83 7d fc 00                        cmp        dword ptr [ebp - 4], 0
004d85db  75 04                              jne        0x4d85e1
004d85dd  33 c0                              xor        eax, eax
004d85df  eb 33                              jmp        0x4d8614
004d85e1  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d85e4  e8 37 fc ff ff                     call       0x4d8220
004d85e9  85 c0                              test       eax, eax
004d85eb  74 04                              je         0x4d85f1
004d85ed  eb 07                              jmp        0x4d85f6
004d85f1  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d85f4  eb 1e                              jmp        0x4d8614
004d85f6  83 7d fc 00                        cmp        dword ptr [ebp - 4], 0
004d85fa  74 16                              je         0x4d8612
004d85fc  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d85ff  50                                 push       eax
004d8600  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004d8606  e8 b5 71 f4 ff                     call       0x41f7c0
004d860b  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
004d8612  33 c0                              xor        eax, eax
004d8614  8b e5                              mov        esp, ebp
004d8616  5d                                 pop        ebp
004d8617  c3                                 ret        
