004d9db0  55                                 push       ebp
004d9db1  8b ec                              mov        ebp, esp
004d9db3  83 ec 0c                           sub        esp, 0xc
004d9db6  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004d9db9  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004d9dbf  e8 fc b7 f6 ff                     call       0x4455c0
004d9dc4  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d9dc7  8b 48 08                           mov        ecx, dword ptr [eax + 8]
004d9dca  89 4d f8                           mov        dword ptr [ebp - 8], ecx
004d9dcd  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d9dd0  8b 42 08                           mov        eax, dword ptr [edx + 8]
004d9dd3  8b 08                              mov        ecx, dword ptr [eax]
004d9dd5  8b 91 e4 00 00 00                  mov        edx, dword ptr [ecx + 0xe4]
004d9ddb  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
004d9dde  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
004d9de1  50                                 push       eax
004d9de2  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004d9de5  51                                 push       ecx
004d9de6  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004d9de9  52                                 push       edx
004d9dea  ff 55 f4                           call       dword ptr [ebp - 0xc]
004d9ded  8b e5                              mov        esp, ebp
004d9def  5d                                 pop        ebp
004d9df0  c2 08 00                           ret        8
