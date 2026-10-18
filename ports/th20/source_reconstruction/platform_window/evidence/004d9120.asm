004d9120  55                                 push       ebp
004d9121  8b ec                              mov        ebp, esp
004d9123  83 ec 18                           sub        esp, 0x18
004d9126  83 3d dc 4e 5c 00 00               cmp        dword ptr [0x5c4edc], 0
004d912d  0f 84 d2 00 00 00                  je         0x4d9205
004d9133  83 3d 30 4d 5c 00 00               cmp        dword ptr [0x5c4d30], 0
004d913a  0f 85 be 00 00 00                  jne        0x4d91fe
004d9140  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d9145  e8 e6 95 f3 ff                     call       0x412730
004d914a  89 45 fc                           mov        dword ptr [ebp - 4], eax
004d914d  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d9150  8b 08                              mov        ecx, dword ptr [eax]
004d9152  8b 91 e4 00 00 00                  mov        edx, dword ptr [ecx + 0xe4]
004d9158  89 55 f0                           mov        dword ptr [ebp - 0x10], edx
004d915b  6a 01                              push       1
004d915d  68 ab 00 00 00                     push       0xab
004d9162  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d9165  50                                 push       eax
004d9166  ff 55 f0                           call       dword ptr [ebp - 0x10]
004d9169  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d916e  e8 bd 95 f3 ff                     call       0x412730
004d9173  89 45 f8                           mov        dword ptr [ebp - 8], eax
004d9176  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004d9179  8b 11                              mov        edx, dword ptr [ecx]
004d917b  8b 82 e4 00 00 00                  mov        eax, dword ptr [edx + 0xe4]
004d9181  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
004d9184  6a 00                              push       0
004d9186  6a 0f                              push       0xf
004d9188  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004d918b  51                                 push       ecx
004d918c  ff 55 ec                           call       dword ptr [ebp - 0x14]
004d918f  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d9194  e8 e7 4d 00 00                     call       0x4ddf80
004d9199  6a 03                              push       3
004d919b  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d91a0  e8 3b 4b f4 ff                     call       0x41dce0
004d91a5  8b 0d ec 4e 5c 00                  mov        ecx, dword ptr [0x5c4eec]
004d91ab  e8 c0 33 f7 ff                     call       0x44c570
004d91b0  6a ff                              push       -1
004d91b2  8b 0d ec 4e 5c 00                  mov        ecx, dword ptr [0x5c4eec]
004d91b8  e8 53 68 f7 ff                     call       0x44fa10
004d91bd  6a 29                              push       0x29
004d91bf  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004d91c5  e8 76 0c f7 ff                     call       0x449e40
004d91ca  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004d91d0  e8 eb c3 f6 ff                     call       0x4455c0
004d91d5  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d91da  e8 51 95 f3 ff                     call       0x412730
004d91df  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
004d91e2  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
004d91e5  8b 02                              mov        eax, dword ptr [edx]
004d91e7  8b 88 e4 00 00 00                  mov        ecx, dword ptr [eax + 0xe4]
004d91ed  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
004d91f0  6a 01                              push       1
004d91f2  6a 0f                              push       0xf
004d91f4  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
004d91f7  52                                 push       edx
004d91f8  ff 55 e8                           call       dword ptr [ebp - 0x18]
004d91fb  90                                 nop        
004d91fc  eb 07                              jmp        0x4d9205
004d91fe  ff 15 30 4d 5c 00                  call       dword ptr [0x5c4d30]
004d9204  90                                 nop        
004d9205  b8 01 00 00 00                     mov        eax, 1
004d920a  8b e5                              mov        esp, ebp
004d920c  5d                                 pop        ebp
004d920d  c3                                 ret        
