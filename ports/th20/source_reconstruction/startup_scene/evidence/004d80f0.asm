004d80f0  55                                 push       ebp
004d80f1  8b ec                              mov        ebp, esp
004d80f3  51                                 push       ecx
004d80f4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004d80f7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d80fa  8b 48 04                           mov        ecx, dword ptr [eax + 4]
004d80fd  d1 e9                              shr        ecx, 1
004d80ff  83 e1 01                           and        ecx, 1
004d8102  74 51                              je         0x4d8155
004d8104  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d8109  e8 62 3c 00 00                     call       0x4dbd70
004d810e  c7 05 58 88 5b 00 01 00 00 00      mov        dword ptr [0x5b8858], 1
004d8118  8b 15 98 06 5c 00                  mov        edx, dword ptr [0x5c0698]
004d811e  8b 02                              mov        eax, dword ptr [edx]
004d8120  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004d8126  8b 50 04                           mov        edx, dword ptr [eax + 4]
004d8129  ff d2                              call       edx
004d812b  a1 88 58 5c 00                     mov        eax, dword ptr [0x5c5888]
004d8130  25 ff fd ff ff                     and        eax, 0xfffffdff
004d8135  a3 88 58 5c 00                     mov        dword ptr [0x5c5888], eax
004d813a  6a 04                              push       4
004d813c  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d8141  e8 0a 61 fe ff                     call       0x4be250
004d8146  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d8149  8b 51 04                           mov        edx, dword ptr [ecx + 4]
004d814c  83 e2 fd                           and        edx, 0xfffffffd
004d814f  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d8152  89 50 04                           mov        dword ptr [eax + 4], edx
004d8155  b8 01 00 00 00                     mov        eax, 1
004d815a  8b e5                              mov        esp, ebp
004d815c  5d                                 pop        ebp
004d815d  c3                                 ret        
