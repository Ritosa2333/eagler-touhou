004dda60  55                                 push       ebp
004dda61  8b ec                              mov        ebp, esp
004dda63  83 ec 0c                           sub        esp, 0xc
004dda66  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004dda69  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dda6c  83 b8 b8 0d 00 00 00               cmp        dword ptr [eax + 0xdb8], 0
004dda73  74 41                              je         0x4ddab6
004dda75  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dda7b  e8 40 7b f6 ff                     call       0x4455c0
004dda80  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dda83  c7 81 b8 0d 00 00 00 00 00 00      mov        dword ptr [ecx + 0xdb8], 0
004dda8d  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dda90  8b 42 08                           mov        eax, dword ptr [edx + 8]
004dda93  89 45 f8                           mov        dword ptr [ebp - 8], eax
004dda96  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dda99  8b 51 08                           mov        edx, dword ptr [ecx + 8]
004dda9c  8b 02                              mov        eax, dword ptr [edx]
004dda9e  8b 88 e4 00 00 00                  mov        ecx, dword ptr [eax + 0xe4]
004ddaa4  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
004ddaa7  6a 00                              push       0
004ddaa9  6a 1c                              push       0x1c
004ddaab  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004ddaae  52                                 push       edx
004ddaaf  ff 55 f4                           call       dword ptr [ebp - 0xc]
004ddab2  eb 04                              jmp        0x4ddab8
004ddab6  33 c0                              xor        eax, eax
004ddab8  8b e5                              mov        esp, ebp
004ddaba  5d                                 pop        ebp
004ddabb  c3                                 ret        
