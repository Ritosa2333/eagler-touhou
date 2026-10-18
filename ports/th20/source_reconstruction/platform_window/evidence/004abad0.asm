004abad0  55                                 push       ebp
004abad1  8b ec                              mov        ebp, esp
004abad3  83 ec 0c                           sub        esp, 0xc
004abad6  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
004abad9  68 08 01 00 00                     push       0x108
004abade  e8 7a 6c 09 00                     call       0x54275d
004abae3  83 c4 04                           add        esp, 4
004abae6  89 45 fc                           mov        dword ptr [ebp - 4], eax
004abae9  83 7d fc 00                        cmp        dword ptr [ebp - 4], 0
004abaed  74 1a                              je         0x4abb09
004abaef  68 08 01 00 00                     push       0x108
004abaf4  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004abaf7  e8 84 05 f6 ff                     call       0x40c080
004abafc  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004abaff  e8 7c 00 00 00                     call       0x4abb80
004abb04  89 45 f8                           mov        dword ptr [ebp - 8], eax
004abb07  eb 07                              jmp        0x4abb10
004abb09  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
004abb10  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004abb13  8b e5                              mov        esp, ebp
004abb15  5d                                 pop        ebp
004abb16  c2 04 00                           ret        4
