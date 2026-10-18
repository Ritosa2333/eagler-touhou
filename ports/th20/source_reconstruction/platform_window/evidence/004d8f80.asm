004d8f80  55                                 push       ebp
004d8f81  8b ec                              mov        ebp, esp
004d8f83  83 ec 18                           sub        esp, 0x18
004d8f86  83 3d dc 4e 5c 00 00               cmp        dword ptr [0x5c4edc], 0
004d8f8d  0f 84 a3 00 00 00                  je         0x4d9036
004d8f93  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d8f98  e8 93 97 f3 ff                     call       0x412730
004d8f9d  89 45 fc                           mov        dword ptr [ebp - 4], eax
004d8fa0  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d8fa3  8b 08                              mov        ecx, dword ptr [eax]
004d8fa5  8b 91 e4 00 00 00                  mov        edx, dword ptr [ecx + 0xe4]
004d8fab  89 55 f0                           mov        dword ptr [ebp - 0x10], edx
004d8fae  6a 01                              push       1
004d8fb0  68 ab 00 00 00                     push       0xab
004d8fb5  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d8fb8  50                                 push       eax
004d8fb9  ff 55 f0                           call       dword ptr [ebp - 0x10]
004d8fbc  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d8fc1  e8 6a 97 f3 ff                     call       0x412730
004d8fc6  89 45 f8                           mov        dword ptr [ebp - 8], eax
004d8fc9  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004d8fcc  8b 11                              mov        edx, dword ptr [ecx]
004d8fce  8b 82 e4 00 00 00                  mov        eax, dword ptr [edx + 0xe4]
004d8fd4  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
004d8fd7  6a 00                              push       0
004d8fd9  6a 0f                              push       0xf
004d8fdb  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004d8fde  51                                 push       ecx
004d8fdf  ff 55 ec                           call       dword ptr [ebp - 0x14]
004d8fe2  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d8fe7  e8 94 4f 00 00                     call       0x4ddf80
004d8fec  8b 0d f4 4e 5c 00                  mov        ecx, dword ptr [0x5c4ef4]
004d8ff2  e8 79 35 f7 ff                     call       0x44c570
004d8ff7  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004d8ffd  e8 be c5 f6 ff                     call       0x4455c0
004d9002  6a ff                              push       -1
004d9004  8b 0d f4 4e 5c 00                  mov        ecx, dword ptr [0x5c4ef4]
004d900a  e8 01 6a f7 ff                     call       0x44fa10
004d900f  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d9014  e8 17 97 f3 ff                     call       0x412730
004d9019  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
004d901c  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
004d901f  8b 02                              mov        eax, dword ptr [edx]
004d9021  8b 88 e4 00 00 00                  mov        ecx, dword ptr [eax + 0xe4]
004d9027  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
004d902a  6a 01                              push       1
004d902c  6a 0f                              push       0xf
004d902e  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
004d9031  52                                 push       edx
004d9032  ff 55 e8                           call       dword ptr [ebp - 0x18]
004d9035  90                                 nop        
004d9036  b8 01 00 00 00                     mov        eax, 1
004d903b  8b e5                              mov        esp, ebp
004d903d  5d                                 pop        ebp
004d903e  c3                                 ret        
