004d9ea0  55                                 push       ebp
004d9ea1  8b ec                              mov        ebp, esp
004d9ea3  81 ec 88 00 00 00                  sub        esp, 0x88
004d9ea9  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004d9eae  33 c5                              xor        eax, ebp
004d9eb0  89 45 fc                           mov        dword ptr [ebp - 4], eax
004d9eb3  68 28 19 57 00                     push       0x571928 ; string='th20.dat'
004d9eb8  b9 c8 66 5b 00                     mov        ecx, 0x5b66c8
004d9ebd  e8 2e 08 06 00                     call       0x53a6f0
004d9ec2  0f b6 c0                           movzx      eax, al
004d9ec5  85 c0                              test       eax, eax
004d9ec7  74 6b                              je         0x4d9f34
004d9ec9  6a 61                              push       0x61
004d9ecb  68 00 01 00 00                     push       0x100
004d9ed0  68 34 19 57 00                     push       0x571934 ; string='th20_%.4x%c.ver'
004d9ed5  68 80 00 00 00                     push       0x80
004d9eda  8d 8d 7c ff ff ff                  lea        ecx, [ebp - 0x84]
004d9ee0  51                                 push       ecx
004d9ee1  e8 3a 53 f4 ff                     call       0x41f220
004d9ee6  83 c4 14                           add        esp, 0x14
004d9ee9  6a 00                              push       0
004d9eeb  8d 95 78 ff ff ff                  lea        edx, [ebp - 0x88]
004d9ef1  52                                 push       edx
004d9ef2  8d 85 7c ff ff ff                  lea        eax, [ebp - 0x84]
004d9ef8  50                                 push       eax
004d9ef9  e8 a2 6b f3 ff                     call       0x410aa0
004d9efe  83 c4 0c                           add        esp, 0xc
004d9f01  a3 04 5b 5c 00                     mov        dword ptr [0x5c5b04], eax
004d9f06  8b 8d 78 ff ff ff                  mov        ecx, dword ptr [ebp - 0x88]
004d9f0c  89 0d 00 5b 5c 00                  mov        dword ptr [0x5c5b00], ecx
004d9f12  83 3d 04 5b 5c 00 00               cmp        dword ptr [0x5c5b04], 0
004d9f19  75 17                              jne        0x4d9f32
004d9f1b  68 44 19 57 00                     push       0x571944 ; string='error : '
004d9f20  68 78 06 5c 00                     push       0x5c0678
004d9f25  e8 06 a3 f7 ff                     call       0x454230
004d9f2a  83 c4 08                           add        esp, 8
004d9f2d  83 c8 ff                           or         eax, 0xffffffff
004d9f30  eb 1b                              jmp        0x4d9f4d
004d9f32  eb 17                              jmp        0x4d9f4b
004d9f34  68 74 19 57 00                     push       0x571974 ; string='error : '
004d9f39  68 78 06 5c 00                     push       0x5c0678
004d9f3e  e8 ed a2 f7 ff                     call       0x454230
004d9f43  83 c4 08                           add        esp, 8
004d9f46  83 c8 ff                           or         eax, 0xffffffff
004d9f49  eb 02                              jmp        0x4d9f4d
004d9f4b  33 c0                              xor        eax, eax
004d9f4d  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d9f50  33 cd                              xor        ecx, ebp
004d9f52  e8 97 8a 06 00                     call       0x5429ee
004d9f57  8b e5                              mov        esp, ebp
004d9f59  5d                                 pop        ebp
004d9f5a  c3                                 ret        
