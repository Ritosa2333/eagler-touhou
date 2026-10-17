0041c320  55                                 push       ebp
0041c321  8b ec                              mov        ebp, esp
0041c323  81 ec e0 00 00 00                  sub        esp, 0xe0
0041c329  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0041c32e  33 c5                              xor        eax, ebp
0041c330  89 45 fc                           mov        dword ptr [ebp - 4], eax
0041c333  6a 20                              push       0x20
0041c335  e8 32 43 12 00                     call       0x54066c
0041c33a  50                                 push       eax
0041c33b  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c340  e8 2b 52 ff ff                     call       0x411570
0041c345  90                                 nop        
0041c346  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c34b  e8 40 3c ff ff                     call       0x40ff90
0041c350  85 c0                              test       eax, eax
0041c352  75 19                              jne        0x41c36d
0041c354  68 cc ca 56 00                     push       0x56cacc ; string='Direct3D '
0041c359  68 78 06 5c 00                     push       0x5c0678
0041c35e  e8 cd 7e 03 00                     call       0x454230
0041c363  83 c4 08                           add        esp, 8
0041c366  b8 01 00 00 00                     mov        eax, 1
0041c36b  eb 59                              jmp        0x41c3c6
0041c36d  68 dc 00 00 00                     push       0xdc
0041c372  6a 00                              push       0
0041c374  8d 85 20 ff ff ff                  lea        eax, [ebp - 0xe0]
0041c37a  50                                 push       eax
0041c37b  e8 e0 83 12 00                     call       0x544760
0041c380  83 c4 0c                           add        esp, 0xc
0041c383  c7 45 cc 00 00 00 00               mov        dword ptr [ebp - 0x34], 0
0041c38a  8d 8d 20 ff ff ff                  lea        ecx, [ebp - 0xe0]
0041c390  51                                 push       ecx
0041c391  6a ff                              push       -1
0041c393  6a 00                              push       0
0041c395  ff 15 80 c2 56 00                  call       dword ptr [0x56c280] ; USER32.dll!EnumDisplaySettingsW
0041c39b  8b 55 cc                           mov        edx, dword ptr [ebp - 0x34]
0041c39e  89 15 08 88 5b 00                  mov        dword ptr [0x5b8808], edx
0041c3a4  8b 45 d0                           mov        eax, dword ptr [ebp - 0x30]
0041c3a7  a3 0c 88 5b 00                     mov        dword ptr [0x5b880c], eax
0041c3ac  8b 0d 08 88 5b 00                  mov        ecx, dword ptr [0x5b8808]
0041c3b2  89 0d 00 88 5b 00                  mov        dword ptr [0x5b8800], ecx
0041c3b8  8b 15 0c 88 5b 00                  mov        edx, dword ptr [0x5b880c]
0041c3be  89 15 04 88 5b 00                  mov        dword ptr [0x5b8804], edx
0041c3c4  33 c0                              xor        eax, eax
0041c3c6  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041c3c9  33 cd                              xor        ecx, ebp
0041c3cb  e8 1e 66 12 00                     call       0x5429ee
0041c3d0  8b e5                              mov        esp, ebp
0041c3d2  5d                                 pop        ebp
0041c3d3  c3                                 ret        
