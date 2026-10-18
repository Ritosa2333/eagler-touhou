004dc510  55                                 push       ebp
004dc511  8b ec                              mov        ebp, esp
004dc513  51                                 push       ecx
004dc514  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004dc517  8b 88 48 0b 00 00                  mov        ecx, dword ptr [eax + 0xb48]
004dc51d  c1 e9 05                           shr        ecx, 5
004dc520  83 e1 03                           and        ecx, 3
004dc523  83 f9 01                           cmp        ecx, 1
004dc526  75 22                              jne        0x4dc54a
004dc528  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004dc52b  81 c1 90 0d 00 00                  add        ecx, 0xd90
004dc531  e8 4a fc ff ff                     call       0x4dc180
004dc536  0f b6 d0                           movzx      edx, al
004dc539  85 d2                              test       edx, edx
004dc53b  75 0d                              jne        0x4dc54a
004dc53d  6a 03                              push       3
004dc53f  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dc544  e8 07 1d fe ff                     call       0x4be250
004dc549  90                                 nop        
004dc54a  b9 30 a8 5b 00                     mov        ecx, 0x5ba830
004dc54f  e8 9c b2 f4 ff                     call       0x4277f0
004dc554  b9 30 a8 5b 00                     mov        ecx, 0x5ba830
004dc559  e8 b2 d1 ff ff                     call       0x4d9710
004dc55e  8b 0d 98 88 5b 00                  mov        ecx, dword ptr [0x5b8898]
004dc564  e8 17 39 f4 ff                     call       0x41fe80
004dc569  90                                 nop        
004dc56a  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dc570  e8 3b 1a f7 ff                     call       0x44dfb0
004dc575  85 c0                              test       eax, eax
004dc577  74 07                              je         0x4dc580
004dc579  b8 04 00 00 00                     mov        eax, 4
004dc57e  eb 66                              jmp        0x4dc5e6
004dc580  b9 58 67 5b 00                     mov        ecx, 0x5b6758
004dc585  e8 f6 e5 ff ff                     call       0x4dab80
004dc58a  85 c0                              test       eax, eax
004dc58c  74 0d                              je         0x4dc59b
004dc58e  6a ff                              push       -1
004dc590  b9 58 67 5b 00                     mov        ecx, 0x5b6758
004dc595  e8 d6 d8 ff ff                     call       0x4d9e70
004dc59a  90                                 nop        
004dc59b  b9 58 67 5b 00                     mov        ecx, 0x5b6758
004dc5a0  e8 db e5 ff ff                     call       0x4dab80
004dc5a5  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004dc5a8  83 b8 b0 0d 00 00 00               cmp        dword ptr [eax + 0xdb0], 0
004dc5af  74 1a                              je         0x4dc5cb
004dc5b1  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004dc5b4  83 b9 b0 0d 00 00 02               cmp        dword ptr [ecx + 0xdb0], 2
004dc5bb  75 07                              jne        0x4dc5c4
004dc5bd  b8 04 00 00 00                     mov        eax, 4
004dc5c2  eb 22                              jmp        0x4dc5e6
004dc5c4  b8 01 00 00 00                     mov        eax, 1
004dc5c9  eb 1b                              jmp        0x4dc5e6
004dc5cb  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004dc5ce  e8 6d df ff ff                     call       0x4da540
004dc5d3  89 45 fc                           mov        dword ptr [ebp - 4], eax
004dc5d6  83 7d fc 01                        cmp        dword ptr [ebp - 4], 1
004dc5da  74 05                              je         0x4dc5e1
004dc5dc  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dc5df  eb 05                              jmp        0x4dc5e6
004dc5e1  b8 01 00 00 00                     mov        eax, 1
004dc5e6  8b e5                              mov        esp, ebp
004dc5e8  5d                                 pop        ebp
004dc5e9  c3                                 ret        
