00513cc0 55                       push       ebp
00513cc1 8bec                     mov        ebp, esp
00513cc3 a118615c00               mov        eax, dword ptr [0x5c6118]
00513cc8 50                       push       eax
00513cc9 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00513ccf e89cdaffff               call       0x511770
00513cd4 c70518615c0000000000     mov        dword ptr [0x5c6118], 0
00513cde 5d                       pop        ebp
00513cdf c3                       ret        
