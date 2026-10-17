004d8e90  55                                 push       ebp
004d8e91  8b ec                              mov        ebp, esp
004d8e93  83 ec 18                           sub        esp, 0x18
004d8e96  83 3d dc 4e 5c 00 00               cmp        dword ptr [0x5c4edc], 0
004d8e9d  0f 84 c5 00 00 00                  je         0x4d8f68
004d8ea3  83 3d 34 4d 5c 00 00               cmp        dword ptr [0x5c4d34], 0
004d8eaa  0f 85 b1 00 00 00                  jne        0x4d8f61
004d8eb0  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d8eb5  e8 76 98 f3 ff                     call       0x412730
004d8eba  89 45 fc                           mov        dword ptr [ebp - 4], eax
004d8ebd  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d8ec0  8b 08                              mov        ecx, dword ptr [eax]
004d8ec2  8b 91 e4 00 00 00                  mov        edx, dword ptr [ecx + 0xe4]
004d8ec8  89 55 f0                           mov        dword ptr [ebp - 0x10], edx
004d8ecb  6a 01                              push       1
004d8ecd  68 ab 00 00 00                     push       0xab
004d8ed2  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d8ed5  50                                 push       eax
004d8ed6  ff 55 f0                           call       dword ptr [ebp - 0x10]
004d8ed9  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d8ede  e8 4d 98 f3 ff                     call       0x412730
004d8ee3  89 45 f8                           mov        dword ptr [ebp - 8], eax
004d8ee6  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004d8ee9  8b 11                              mov        edx, dword ptr [ecx]
004d8eeb  8b 82 e4 00 00 00                  mov        eax, dword ptr [edx + 0xe4]
004d8ef1  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
004d8ef4  6a 00                              push       0
004d8ef6  6a 0f                              push       0xf
004d8ef8  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004d8efb  51                                 push       ecx
004d8efc  ff 55 ec                           call       dword ptr [ebp - 0x14]
004d8eff  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d8f04  e8 77 50 00 00                     call       0x4ddf80
004d8f09  6a 03                              push       3
004d8f0b  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d8f10  e8 cb 4d f4 ff                     call       0x41dce0
004d8f15  8b 0d f0 4e 5c 00                  mov        ecx, dword ptr [0x5c4ef0]
004d8f1b  e8 50 36 f7 ff                     call       0x44c570
004d8f20  6a ff                              push       -1
004d8f22  8b 0d f0 4e 5c 00                  mov        ecx, dword ptr [0x5c4ef0]
004d8f28  e8 e3 6a f7 ff                     call       0x44fa10
004d8f2d  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004d8f33  e8 88 c6 f6 ff                     call       0x4455c0
004d8f38  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d8f3d  e8 ee 97 f3 ff                     call       0x412730
004d8f42  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
004d8f45  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
004d8f48  8b 02                              mov        eax, dword ptr [edx]
004d8f4a  8b 88 e4 00 00 00                  mov        ecx, dword ptr [eax + 0xe4]
004d8f50  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
004d8f53  6a 01                              push       1
004d8f55  6a 0f                              push       0xf
004d8f57  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
004d8f5a  52                                 push       edx
004d8f5b  ff 55 e8                           call       dword ptr [ebp - 0x18]
004d8f5e  90                                 nop        
004d8f5f  eb 07                              jmp        0x4d8f68
004d8f61  ff 15 34 4d 5c 00                  call       dword ptr [0x5c4d34]
004d8f67  90                                 nop        
004d8f68  b8 01 00 00 00                     mov        eax, 1
004d8f6d  8b e5                              mov        esp, ebp
004d8f6f  5d                                 pop        ebp
004d8f70  c3                                 ret        
