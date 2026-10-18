004ddf80  55                                 push       ebp
004ddf81  8b ec                              mov        ebp, esp
004ddf83  83 ec 0c                           sub        esp, 0xc
004ddf86  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004ddf89  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ddf8c  83 b8 bc 0d 00 00 00               cmp        dword ptr [eax + 0xdbc], 0
004ddf93  74 41                              je         0x4ddfd6
004ddf95  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004ddf9b  e8 20 76 f6 ff                     call       0x4455c0
004ddfa0  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004ddfa3  c7 81 bc 0d 00 00 00 00 00 00      mov        dword ptr [ecx + 0xdbc], 0
004ddfad  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004ddfb0  8b 42 08                           mov        eax, dword ptr [edx + 8]
004ddfb3  89 45 f8                           mov        dword ptr [ebp - 8], eax
004ddfb6  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004ddfb9  8b 51 08                           mov        edx, dword ptr [ecx + 8]
004ddfbc  8b 02                              mov        eax, dword ptr [edx]
004ddfbe  8b 88 e4 00 00 00                  mov        ecx, dword ptr [eax + 0xe4]
004ddfc4  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
004ddfc7  6a 00                              push       0
004ddfc9  6a 0e                              push       0xe
004ddfcb  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004ddfce  52                                 push       edx
004ddfcf  ff 55 f4                           call       dword ptr [ebp - 0xc]
004ddfd2  eb 04                              jmp        0x4ddfd8
004ddfd6  33 c0                              xor        eax, eax
004ddfd8  8b e5                              mov        esp, ebp
004ddfda  5d                                 pop        ebp
004ddfdb  c3                                 ret        
