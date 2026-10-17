00419760  55                                 push       ebp
00419761  8b ec                              mov        ebp, esp
00419763  6a ff                              push       -1
00419765  68 76 7f 56 00                     push       0x567f76
0041976a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
00419770  50                                 push       eax
00419771  81 ec 64 01 00 00                  sub        esp, 0x164
00419777  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0041977c  33 c5                              xor        eax, ebp
0041977e  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
00419781  50                                 push       eax
00419782  8d 45 f4                           lea        eax, [ebp - 0xc]
00419785  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0041978b  89 8d 90 fe ff ff                  mov        dword ptr [ebp - 0x170], ecx
00419791  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
00419797  e8 24 28 00 00                     call       0x41bfc0
0041979c  90                                 nop        
0041979d  33 c0                              xor        eax, eax
0041979f  75 3b                              jne        0x4197dc
004197a1  b9 04 00 00 00                     mov        ecx, 4
004197a6  d1 e1                              shl        ecx, 1
004197a8  83 b9 9c 88 5b 00 00               cmp        dword ptr [ecx + 0x5b889c], 0
004197af  74 2b                              je         0x4197dc
004197b1  ba 04 00 00 00                     mov        edx, 4
004197b6  d1 e2                              shl        edx, 1
004197b8  8b 82 9c 88 5b 00                  mov        eax, dword ptr [edx + 0x5b889c]
004197be  89 85 9c fe ff ff                  mov        dword ptr [ebp - 0x164], eax
004197c4  68 00 00 04 00                     push       0x40000
004197c9  8b 8d 9c fe ff ff                  mov        ecx, dword ptr [ebp - 0x164]
004197cf  e8 2c 04 00 00                     call       0x419c00
004197d4  89 85 a0 fe ff ff                  mov        dword ptr [ebp - 0x160], eax
004197da  eb 0a                              jmp        0x4197e6
004197dc  c7 85 a0 fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x160], 0
004197e6  83 bd a0 fe ff ff 00               cmp        dword ptr [ebp - 0x160], 0
004197ed  0f 84 92 01 00 00                  je         0x419985
004197f3  8b 0d e8 87 5b 00                  mov        ecx, dword ptr [0x5b87e8]
004197f9  c1 e9 06                           shr        ecx, 6
004197fc  83 e1 01                           and        ecx, 1
004197ff  0f 85 80 01 00 00                  jne        0x419985
00419805  6a 00                              push       0
00419807  68 e1 67 5b 00                     push       0x5b67e1
0041980c  8d 8d d8 fe ff ff                  lea        ecx, [ebp - 0x128]
00419812  e8 b9 e4 ff ff                     call       0x417cd0
00419817  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0041981e  68 ac ca 56 00                     push       0x56caac ; string='snapshot'
00419823  8d 8d d8 fe ff ff                  lea        ecx, [ebp - 0x128]
00419829  e8 12 e6 ff ff                     call       0x417e40
0041982e  8d 95 d8 fe ff ff                  lea        edx, [ebp - 0x128]
00419834  52                                 push       edx
00419835  e8 a6 2a 00 00                     call       0x41c2e0
0041983a  83 c4 04                           add        esp, 4
0041983d  c7 85 a4 fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x15c], 0
00419847  eb 0f                              jmp        0x419858
00419849  8b 85 a4 fe ff ff                  mov        eax, dword ptr [ebp - 0x15c]
0041984f  83 c0 01                           add        eax, 1
00419852  89 85 a4 fe ff ff                  mov        dword ptr [ebp - 0x15c], eax
00419858  81 bd a4 fe ff ff 10 27 00 00      cmp        dword ptr [ebp - 0x15c], 0x2710
00419862  0f 8d b6 00 00 00                  jge        0x41991e
00419868  8b 8d a4 fe ff ff                  mov        ecx, dword ptr [ebp - 0x15c]
0041986e  51                                 push       ecx
0041986f  68 bc ca 56 00                     push       0x56cabc ; string='th20_%.3d.png'
00419874  68 00 01 00 00                     push       0x100
00419879  8d 95 f0 fe ff ff                  lea        edx, [ebp - 0x110]
0041987f  52                                 push       edx
00419880  e8 9b 59 00 00                     call       0x41f220
00419885  83 c4 10                           add        esp, 0x10
00419888  68 e1 67 5b 00                     push       0x5b67e1
0041988d  8d 8d d8 fe ff ff                  lea        ecx, [ebp - 0x128]
00419893  e8 58 e5 ff ff                     call       0x417df0
00419898  68 ac ca 56 00                     push       0x56caac ; string='snapshot'
0041989d  8d 8d d8 fe ff ff                  lea        ecx, [ebp - 0x128]
004198a3  e8 98 e5 ff ff                     call       0x417e40
004198a8  8d 85 f0 fe ff ff                  lea        eax, [ebp - 0x110]
004198ae  50                                 push       eax
004198af  8d 8d d8 fe ff ff                  lea        ecx, [ebp - 0x128]
004198b5  e8 86 e5 ff ff                     call       0x417e40
004198ba  8d 8d c0 fe ff ff                  lea        ecx, [ebp - 0x140]
004198c0  51                                 push       ecx
004198c1  8d 8d d8 fe ff ff                  lea        ecx, [ebp - 0x128]
004198c7  e8 c4 4e 00 00                     call       0x41e790
004198cc  c6 45 fc 01                        mov        byte ptr [ebp - 4], 1
004198d0  8d 8d c0 fe ff ff                  lea        ecx, [ebp - 0x140]
004198d6  e8 55 66 ff ff                     call       0x40ff30
004198db  89 85 98 fe ff ff                  mov        dword ptr [ebp - 0x168], eax
004198e1  8b 95 98 fe ff ff                  mov        edx, dword ptr [ebp - 0x168]
004198e7  52                                 push       edx
004198e8  e8 b3 6e ff ff                     call       0x4107a0
004198ed  83 c4 04                           add        esp, 4
004198f0  0f b6 c0                           movzx      eax, al
004198f3  85 c0                              test       eax, eax
004198f5  75 12                              jne        0x419909
004198f7  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
004198fb  8d 8d c0 fe ff ff                  lea        ecx, [ebp - 0x140]
00419901  e8 9a 4c ff ff                     call       0x40e5a0
00419906  90                                 nop        
00419907  eb 15                              jmp        0x41991e
00419909  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
0041990d  8d 8d c0 fe ff ff                  lea        ecx, [ebp - 0x140]
00419913  e8 88 4c ff ff                     call       0x40e5a0
00419918  90                                 nop        
00419919  e9 2b ff ff ff                     jmp        0x419849
0041991e  81 bd a4 fe ff ff 10 27 00 00      cmp        dword ptr [ebp - 0x15c], 0x2710
00419928  7d 48                              jge        0x419972
0041992a  8d 8d a8 fe ff ff                  lea        ecx, [ebp - 0x158]
00419930  51                                 push       ecx
00419931  8d 8d d8 fe ff ff                  lea        ecx, [ebp - 0x128]
00419937  e8 54 4e 00 00                     call       0x41e790
0041993c  c6 45 fc 02                        mov        byte ptr [ebp - 4], 2
00419940  8d 8d a8 fe ff ff                  lea        ecx, [ebp - 0x158]
00419946  e8 e5 65 ff ff                     call       0x40ff30
0041994b  89 85 94 fe ff ff                  mov        dword ptr [ebp - 0x16c], eax
00419951  8b 95 94 fe ff ff                  mov        edx, dword ptr [ebp - 0x16c]
00419957  52                                 push       edx
00419958  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041995d  e8 de 46 0c 00                     call       0x4de040
00419962  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
00419966  8d 8d a8 fe ff ff                  lea        ecx, [ebp - 0x158]
0041996c  e8 2f 4c ff ff                     call       0x40e5a0
00419971  90                                 nop        
00419972  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
00419979  8d 8d d8 fe ff ff                  lea        ecx, [ebp - 0x128]
0041997f  e8 3c 4d ff ff                     call       0x40e6c0
00419984  90                                 nop        
00419985  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
00419988  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0041998f  59                                 pop        ecx
00419990  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419993  33 cd                              xor        ecx, ebp
00419995  e8 54 90 12 00                     call       0x5429ee
0041999a  8b e5                              mov        esp, ebp
0041999c  5d                                 pop        ebp
0041999d  c3                                 ret        
