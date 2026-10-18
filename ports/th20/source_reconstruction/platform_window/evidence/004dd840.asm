004dd840  55                                 push       ebp
004dd841  8b ec                              mov        ebp, esp
004dd843  51                                 push       ecx
004dd844  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004dd847  83 3d dc 4e 5c 00 00               cmp        dword ptr [0x5c4edc], 0
004dd84e  74 1d                              je         0x4dd86d
004dd850  a1 dc 4e 5c 00                     mov        eax, dword ptr [0x5c4edc]
004dd855  8b 08                              mov        ecx, dword ptr [eax]
004dd857  8b 15 dc 4e 5c 00                  mov        edx, dword ptr [0x5c4edc]
004dd85d  52                                 push       edx
004dd85e  8b 41 08                           mov        eax, dword ptr [ecx + 8]
004dd861  ff d0                              call       eax
004dd863  c7 05 dc 4e 5c 00 00 00 00 00      mov        dword ptr [0x5c4edc], 0
004dd86d  83 3d e0 4e 5c 00 00               cmp        dword ptr [0x5c4ee0], 0
004dd874  74 1d                              je         0x4dd893
004dd876  8b 0d e0 4e 5c 00                  mov        ecx, dword ptr [0x5c4ee0]
004dd87c  8b 11                              mov        edx, dword ptr [ecx]
004dd87e  a1 e0 4e 5c 00                     mov        eax, dword ptr [0x5c4ee0]
004dd883  50                                 push       eax
004dd884  8b 4a 08                           mov        ecx, dword ptr [edx + 8]
004dd887  ff d1                              call       ecx
004dd889  c7 05 e0 4e 5c 00 00 00 00 00      mov        dword ptr [0x5c4ee0], 0
004dd893  83 3d e4 4e 5c 00 00               cmp        dword ptr [0x5c4ee4], 0
004dd89a  74 1e                              je         0x4dd8ba
004dd89c  8b 15 e4 4e 5c 00                  mov        edx, dword ptr [0x5c4ee4]
004dd8a2  8b 02                              mov        eax, dword ptr [edx]
004dd8a4  8b 0d e4 4e 5c 00                  mov        ecx, dword ptr [0x5c4ee4]
004dd8aa  51                                 push       ecx
004dd8ab  8b 50 08                           mov        edx, dword ptr [eax + 8]
004dd8ae  ff d2                              call       edx
004dd8b0  c7 05 e4 4e 5c 00 00 00 00 00      mov        dword ptr [0x5c4ee4], 0
004dd8ba  c7 05 dc 4e 5c 00 00 00 00 00      mov        dword ptr [0x5c4edc], 0
004dd8c4  8b e5                              mov        esp, ebp
004dd8c6  5d                                 pop        ebp
004dd8c7  c3                                 ret        
