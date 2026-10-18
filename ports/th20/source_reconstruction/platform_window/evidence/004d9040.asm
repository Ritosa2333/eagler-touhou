004d9040  55                                 push       ebp
004d9041  8b ec                              mov        ebp, esp
004d9043  83 ec 10                           sub        esp, 0x10
004d9046  83 3d dc 4e 5c 00 00               cmp        dword ptr [0x5c4edc], 0
004d904d  0f 84 ba 00 00 00                  je         0x4d910d
004d9053  6a 00                              push       0
004d9055  6a 1f                              push       0x1f
004d9057  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004d905d  e8 6e 63 f7 ff                     call       0x44f3d0
004d9062  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d9067  e8 c4 96 f3 ff                     call       0x412730
004d906c  89 45 fc                           mov        dword ptr [ebp - 4], eax
004d906f  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d9072  8b 08                              mov        ecx, dword ptr [eax]
004d9074  8b 91 e4 00 00 00                  mov        edx, dword ptr [ecx + 0xe4]
004d907a  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
004d907d  6a 00                              push       0
004d907f  6a 0f                              push       0xf
004d9081  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d9084  50                                 push       eax
004d9085  ff 55 f4                           call       dword ptr [ebp - 0xc]
004d9088  90                                 nop        
004d9089  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d908e  e8 2d 30 fd ff                     call       0x4ac0c0
004d9093  83 f8 07                           cmp        eax, 7
004d9096  74 0f                              je         0x4d90a7
004d9098  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d909d  e8 1e 30 fd ff                     call       0x4ac0c0
004d90a2  83 f8 0c                           cmp        eax, 0xc
004d90a5  75 1b                              jne        0x4d90c2
004d90a7  8b 0d f8 4e 5c 00                  mov        ecx, dword ptr [0x5c4ef8]
004d90ad  e8 be 34 f7 ff                     call       0x44c570
004d90b2  6a ff                              push       -1
004d90b4  8b 0d f8 4e 5c 00                  mov        ecx, dword ptr [0x5c4ef8]
004d90ba  e8 51 69 f7 ff                     call       0x44fa10
004d90bf  90                                 nop        
004d90c0  eb 19                              jmp        0x4d90db
004d90c2  8b 0d fc 4e 5c 00                  mov        ecx, dword ptr [0x5c4efc]
004d90c8  e8 a3 34 f7 ff                     call       0x44c570
004d90cd  6a ff                              push       -1
004d90cf  8b 0d fc 4e 5c 00                  mov        ecx, dword ptr [0x5c4efc]
004d90d5  e8 36 69 f7 ff                     call       0x44fa10
004d90da  90                                 nop        
004d90db  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004d90e1  e8 da c4 f6 ff                     call       0x4455c0
004d90e6  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d90eb  e8 40 96 f3 ff                     call       0x412730
004d90f0  89 45 f8                           mov        dword ptr [ebp - 8], eax
004d90f3  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004d90f6  8b 11                              mov        edx, dword ptr [ecx]
004d90f8  8b 82 e4 00 00 00                  mov        eax, dword ptr [edx + 0xe4]
004d90fe  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
004d9101  6a 01                              push       1
004d9103  6a 0f                              push       0xf
004d9105  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004d9108  51                                 push       ecx
004d9109  ff 55 f0                           call       dword ptr [ebp - 0x10]
004d910c  90                                 nop        
004d910d  b8 01 00 00 00                     mov        eax, 1
004d9112  8b e5                              mov        esp, ebp
004d9114  5d                                 pop        ebp
004d9115  c3                                 ret        
