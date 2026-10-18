004dd490  55                                 push       ebp
004dd491  8b ec                              mov        ebp, esp
004dd493  68 dc 1a 57 00                     push       0x571adc ; string='shutdown MotherInf'
004dd498  e8 13 f2 f2 ff                     call       0x40c6b0
004dd49d  83 c4 04                           add        esp, 4
004dd4a0  b9 30 a8 5b 00                     mov        ecx, 0x5ba830
004dd4a5  e8 46 a3 f4 ff                     call       0x4277f0
004dd4aa  85 c0                              test       eax, eax
004dd4ac  74 02                              je         0x4dd4b0
004dd4ae  eb f0                              jmp        0x4dd4a0
004dd4b0  b9 e0 5a 5c 00                     mov        ecx, 0x5c5ae0
004dd4b5  e8 36 e8 f2 ff                     call       0x40bcf0
004dd4ba  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dd4bf  e8 6c c9 ff ff                     call       0x4d9e30
004dd4c4  90                                 nop        
004dd4c5  83 3d 04 5b 5c 00 00               cmp        dword ptr [0x5c5b04], 0
004dd4cc  74 2e                              je         0x4dd4fc
004dd4ce  83 3d 04 5b 5c 00 00               cmp        dword ptr [0x5c5b04], 0
004dd4d5  74 1b                              je         0x4dd4f2
004dd4d7  a1 04 5b 5c 00                     mov        eax, dword ptr [0x5c5b04]
004dd4dc  50                                 push       eax
004dd4dd  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004dd4e3  e8 88 21 f4 ff                     call       0x41f670
004dd4e8  c7 05 04 5b 5c 00 00 00 00 00      mov        dword ptr [0x5c5b04], 0
004dd4f2  c7 05 04 5b 5c 00 00 00 00 00      mov        dword ptr [0x5c5b04], 0
004dd4fc  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dd501  e8 2a 02 00 00                     call       0x4dd730
004dd506  8b 0d 00 4a 5c 00                  mov        ecx, dword ptr [0x5c4a00]
004dd50c  51                                 push       ecx
004dd50d  e8 ae 42 f4 ff                     call       0x4217c0
004dd512  83 c4 04                           add        esp, 4
004dd515  8b 15 98 88 5b 00                  mov        edx, dword ptr [0x5b8898]
004dd51b  52                                 push       edx
004dd51c  e8 9f 42 f4 ff                     call       0x4217c0
004dd521  83 c4 04                           add        esp, 4
004dd524  68 f0 1a 57 00                     push       0x571af0 ; string='VertexBuffer delete'
004dd529  e8 82 f1 f2 ff                     call       0x40c6b0
004dd52e  83 c4 04                           add        esp, 4
004dd531  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dd537  e8 04 d6 ff ff                     call       0x4dab40
004dd53c  68 08 1b 57 00                     push       0x571b08 ; string='Sound Stop'
004dd541  e8 6a f1 f2 ff                     call       0x40c6b0
004dd546  83 c4 04                           add        esp, 4
004dd549  68 14 1b 57 00                     push       0x571b14 ; string='dummy'
004dd54e  6a 00                              push       0
004dd550  6a 04                              push       4
004dd552  b9 30 a8 5b 00                     mov        ecx, 0x5ba830
004dd557  e8 34 b7 f4 ff                     call       0x428c90
004dd55c  b9 c8 66 5b 00                     mov        ecx, 0x5b66c8
004dd561  e8 0a cc 05 00                     call       0x53a170
004dd566  a1 ec 4e 5c 00                     mov        eax, dword ptr [0x5c4eec]
004dd56b  50                                 push       eax
004dd56c  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004dd572  e8 99 9e f6 ff                     call       0x447410
004dd577  c7 05 ec 4e 5c 00 00 00 00 00      mov        dword ptr [0x5c4eec], 0
004dd581  8b 0d f0 4e 5c 00                  mov        ecx, dword ptr [0x5c4ef0]
004dd587  51                                 push       ecx
004dd588  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004dd58e  e8 7d 9e f6 ff                     call       0x447410
004dd593  c7 05 f0 4e 5c 00 00 00 00 00      mov        dword ptr [0x5c4ef0], 0
004dd59d  8b 15 f4 4e 5c 00                  mov        edx, dword ptr [0x5c4ef4]
004dd5a3  52                                 push       edx
004dd5a4  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004dd5aa  e8 61 9e f6 ff                     call       0x447410
004dd5af  c7 05 f4 4e 5c 00 00 00 00 00      mov        dword ptr [0x5c4ef4], 0
004dd5b9  a1 f8 4e 5c 00                     mov        eax, dword ptr [0x5c4ef8]
004dd5be  50                                 push       eax
004dd5bf  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004dd5c5  e8 46 9e f6 ff                     call       0x447410
004dd5ca  c7 05 f8 4e 5c 00 00 00 00 00      mov        dword ptr [0x5c4ef8], 0
004dd5d4  8b 0d fc 4e 5c 00                  mov        ecx, dword ptr [0x5c4efc]
004dd5da  51                                 push       ecx
004dd5db  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004dd5e1  e8 2a 9e f6 ff                     call       0x447410
004dd5e6  c7 05 fc 4e 5c 00 00 00 00 00      mov        dword ptr [0x5c4efc], 0
004dd5f0  33 c0                              xor        eax, eax
004dd5f2  5d                                 pop        ebp
004dd5f3  c3                                 ret        
