004199a0  55                                 push       ebp
004199a1  8b ec                              mov        ebp, esp
004199a3  83 ec 14                           sub        esp, 0x14
004199a6  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
004199a9  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004199ac  e8 af fd ff ff                     call       0x419760
004199b1  90                                 nop        
004199b2  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004199b7  e8 74 8d ff ff                     call       0x412730
004199bc  89 45 fc                           mov        dword ptr [ebp - 4], eax
004199bf  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004199c2  8b 08                              mov        ecx, dword ptr [eax]
004199c4  8b 51 44                           mov        edx, dword ptr [ecx + 0x44]
004199c7  89 55 f0                           mov        dword ptr [ebp - 0x10], edx
004199ca  6a 00                              push       0
004199cc  6a 00                              push       0
004199ce  6a 00                              push       0
004199d0  6a 00                              push       0
004199d2  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004199d5  50                                 push       eax
004199d6  ff 55 f0                           call       dword ptr [ebp - 0x10]
004199d9  85 c0                              test       eax, eax
004199db  7d 5d                              jge        0x419a3a
004199dd  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004199e2  e8 59 3e 0c 00                     call       0x4dd840
004199e7  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004199ed  e8 ee 41 00 00                     call       0x41dbe0
004199f2  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004199f7  e8 34 8d ff ff                     call       0x412730
004199fc  89 45 f8                           mov        dword ptr [ebp - 8], eax
004199ff  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
00419a02  8b 11                              mov        edx, dword ptr [ecx]
00419a04  8b 42 40                           mov        eax, dword ptr [edx + 0x40]
00419a07  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
00419a0a  68 24 4e 5c 00                     push       0x5c4e24
00419a0f  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
00419a12  51                                 push       ecx
00419a13  ff 55 ec                           call       dword ptr [ebp - 0x14]
00419a16  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
00419a1c  e8 cf 3f 00 00                     call       0x41d9f0
00419a21  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419a26  e8 45 23 0c 00                     call       0x4dbd70
00419a2b  e8 90 08 00 00                     call       0x41a2c0
00419a30  c7 05 6c 58 5c 00 02 00 00 00      mov        dword ptr [0x5c586c], 2
00419a3a  e8 81 f9 ff ff                     call       0x4193c0
00419a3f  90                                 nop        
00419a40  8b e5                              mov        esp, ebp
00419a42  5d                                 pop        ebp
00419a43  c3                                 ret        
