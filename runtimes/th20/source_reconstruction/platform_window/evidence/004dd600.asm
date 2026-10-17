004dd600  55                                 push       ebp
004dd601  8b ec                              mov        ebp, esp
004dd603  e8 98 c8 ff ff                     call       0x4d9ea0
004dd608  51                                 push       ecx
004dd609  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
004dd611  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dd616  b9 e4 ef 5a 00                     mov        ecx, 0x5aefe4
004dd61b  e8 80 bc f4 ff                     call       0x4292a0
004dd620  c7 05 20 5b 5c 00 00 00 00 ff      mov        dword ptr [0x5c5b20], 0xff000000
004dd62a  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dd62f  e8 6c d5 ff ff                     call       0x4daba0
004dd634  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dd639  e8 a2 e6 ff ff                     call       0x4dbce0
004dd63e  ff 15 e8 c2 56 00                  call       dword ptr [0x56c2e8] ; WINMM.dll!timeGetTime
004dd644  a3 8c 58 5c 00                     mov        dword ptr [0x5c588c], eax
004dd649  a1 8c 58 5c 00                     mov        eax, dword ptr [0x5c588c]
004dd64e  50                                 push       eax
004dd64f  b9 a8 a4 5b 00                     mov        ecx, 0x5ba4a8
004dd654  e8 e7 c2 ff ff                     call       0x4d9940
004dd659  8b 0d 8c 58 5c 00                  mov        ecx, dword ptr [0x5c588c]
004dd65f  51                                 push       ecx
004dd660  b9 c4 a4 5b 00                     mov        ecx, 0x5ba4c4
004dd665  e8 d6 c2 ff ff                     call       0x4d9940
004dd66a  8b 15 8c 58 5c 00                  mov        edx, dword ptr [0x5c588c]
004dd670  d1 ea                              shr        edx, 1
004dd672  52                                 push       edx
004dd673  b9 e0 a4 5b 00                     mov        ecx, 0x5ba4e0
004dd678  e8 c3 c2 ff ff                     call       0x4d9940
004dd67d  a1 8c 58 5c 00                     mov        eax, dword ptr [0x5c588c]
004dd682  33 d2                              xor        edx, edx
004dd684  b9 03 00 00 00                     mov        ecx, 3
004dd689  f7 f1                              div        ecx
004dd68b  50                                 push       eax
004dd68c  b9 fc a4 5b 00                     mov        ecx, 0x5ba4fc
004dd691  e8 aa c2 ff ff                     call       0x4d9940
004dd696  e8 05 eb fc ff                     call       0x4ac1a0
004dd69b  e8 60 54 f4 ff                     call       0x422b00
004dd6a0  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dd6a6  e8 75 f9 f6 ff                     call       0x44d020
004dd6ab  68 9c 19 57 00                     push       0x57199c ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\mother.cpp:233 SprtInf'
004dd6b0  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004dd6b6  e8 45 a1 f6 ff                     call       0x447800
004dd6bb  a3 ec 4e 5c 00                     mov        dword ptr [0x5c4eec], eax
004dd6c0  68 dc 19 57 00                     push       0x5719dc ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\mother.cpp:234 SprtInf'
004dd6c5  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004dd6cb  e8 30 a1 f6 ff                     call       0x447800
004dd6d0  a3 f0 4e 5c 00                     mov        dword ptr [0x5c4ef0], eax
004dd6d5  68 1c 1a 57 00                     push       0x571a1c ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\mother.cpp:235 SprtInf'
004dd6da  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004dd6e0  e8 1b a1 f6 ff                     call       0x447800
004dd6e5  a3 f4 4e 5c 00                     mov        dword ptr [0x5c4ef4], eax
004dd6ea  68 5c 1a 57 00                     push       0x571a5c ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\mother.cpp:236 SprtInf'
004dd6ef  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004dd6f5  e8 06 a1 f6 ff                     call       0x447800
004dd6fa  a3 f8 4e 5c 00                     mov        dword ptr [0x5c4ef8], eax
004dd6ff  68 9c 1a 57 00                     push       0x571a9c ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\mother.cpp:237 SprtInf'
004dd704  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004dd70a  e8 f1 a0 f6 ff                     call       0x447800
004dd70f  a3 fc 4e 5c 00                     mov        dword ptr [0x5c4efc], eax
004dd714  c7 05 30 4d 5c 00 00 00 00 00      mov        dword ptr [0x5c4d30], 0
004dd71e  c7 05 34 4d 5c 00 00 00 00 00      mov        dword ptr [0x5c4d34], 0
004dd728  33 c0                              xor        eax, eax
004dd72a  5d                                 pop        ebp
004dd72b  c3                                 ret        
