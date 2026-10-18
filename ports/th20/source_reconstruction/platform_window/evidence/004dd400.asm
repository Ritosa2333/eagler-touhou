004dd400  55                                 push       ebp
004dd401  8b ec                              mov        ebp, esp
004dd403  83 ec 14                           sub        esp, 0x14
004dd406  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dd40c  e8 af 81 f6 ff                     call       0x4455c0
004dd411  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dd416  e8 15 53 f3 ff                     call       0x412730
004dd41b  89 45 fc                           mov        dword ptr [ebp - 4], eax
004dd41e  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dd421  8b 08                              mov        ecx, dword ptr [eax]
004dd423  8b 91 94 00 00 00                  mov        edx, dword ptr [ecx + 0x94]
004dd429  89 55 f0                           mov        dword ptr [ebp - 0x10], edx
004dd42c  a1 e4 4e 5c 00                     mov        eax, dword ptr [0x5c4ee4]
004dd431  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
004dd434  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004dd437  51                                 push       ecx
004dd438  6a 00                              push       0
004dd43a  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dd43d  52                                 push       edx
004dd43e  ff 55 f0                           call       dword ptr [ebp - 0x10]
004dd441  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dd446  e8 e5 52 f3 ff                     call       0x412730
004dd44b  89 45 f8                           mov        dword ptr [ebp - 8], eax
004dd44e  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004dd451  8b 08                              mov        ecx, dword ptr [eax]
004dd453  8b 91 ac 00 00 00                  mov        edx, dword ptr [ecx + 0xac]
004dd459  89 55 ec                           mov        dword ptr [ebp - 0x14], edx
004dd45c  6a 00                              push       0
004dd45e  51                                 push       ecx
004dd45f  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
004dd467  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dd46c  68 00 00 00 ff                     push       0xff000000
004dd471  6a 03                              push       3
004dd473  6a 00                              push       0
004dd475  6a 00                              push       0
004dd477  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004dd47a  50                                 push       eax
004dd47b  ff 55 ec                           call       dword ptr [ebp - 0x14]
004dd47e  b8 01 00 00 00                     mov        eax, 1
004dd483  8b e5                              mov        esp, ebp
004dd485  5d                                 pop        ebp
004dd486  c3                                 ret        
