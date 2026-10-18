004d8350  55                                 push       ebp
004d8351  8b ec                              mov        ebp, esp
004d8353  51                                 push       ecx
004d8354  68 d0 07 00 00                     push       0x7d0
004d8359  ff 15 6c c1 56 00                  call       dword ptr [0x56c16c] ; KERNEL32.dll!Sleep
004d835f  68 bc 17 57 00                     push       0x5717bc ; string='LoadThread 1'
004d8364  e8 47 43 f3 ff                     call       0x40c6b0
004d8369  83 c4 04                           add        esp, 4
004d836c  a1 2c 4d 5c 00                     mov        eax, dword ptr [0x5c4d2c]
004d8371  89 45 08                           mov        dword ptr [ebp + 8], eax
004d8374  68 cc 17 57 00                     push       0x5717cc ; string='sig.anm'
004d8379  6a 01                              push       1
004d837b  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004d8381  e8 aa 67 f7 ff                     call       0x44eb30
004d8386  89 45 fc                           mov        dword ptr [ebp - 4], eax
004d8389  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004d838c  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d838f  89 91 0c 06 00 00                  mov        dword ptr [ecx + 0x60c], edx
004d8395  83 7d fc 00                        cmp        dword ptr [ebp - 4], 0
004d8399  75 0a                              jne        0x4d83a5
004d839b  e9 9f 01 00 00                     jmp        0x4d853f
004d83a5  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004d83a8  8b 48 0c                           mov        ecx, dword ptr [eax + 0xc]
004d83ab  e8 d0 a9 f3 ff                     call       0x412d80
004d83b0  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004d83b3  c7 81 10 06 00 00 01 00 00 00      mov        dword ptr [ecx + 0x610], 1
004d83bd  68 d4 17 57 00                     push       0x5717d4 ; string='LoadThread 2'
004d83c2  e8 e9 42 f3 ff                     call       0x40c6b0
004d83c7  83 c4 04                           add        esp, 4
004d83ca  68 e4 17 57 00                     push       0x5717e4 ; string='text.anm'
004d83cf  6a 00                              push       0
004d83d1  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004d83d7  e8 54 67 f7 ff                     call       0x44eb30
004d83dc  a3 80 58 5c 00                     mov        dword ptr [0x5c5880], eax
004d83e1  83 3d 80 58 5c 00 00               cmp        dword ptr [0x5c5880], 0
004d83e8  75 0a                              jne        0x4d83f4
004d83ea  e9 50 01 00 00                     jmp        0x4d853f
004d83f4  68 f0 17 57 00                     push       0x5717f0 ; string='LoadThread 3'
004d83f9  e8 b2 42 f3 ff                     call       0x40c6b0
004d83fe  83 c4 04                           add        esp, 4
004d8401  e8 5a 8c f9 ff                     call       0x471060
004d8406  85 c0                              test       eax, eax
004d8408  75 1c                              jne        0x4d8426
004d840a  68 00 18 57 00                     push       0x571800 ; string='error : '
004d840f  68 78 06 5c 00                     push       0x5c0678
004d8414  e8 37 bd f7 ff                     call       0x454150
004d8419  83 c4 08                           add        esp, 8
004d841c  e9 1e 01 00 00                     jmp        0x4d853f
004d8426  8b 55 08                           mov        edx, dword ptr [ebp + 8]
004d8429  c7 82 14 06 00 00 01 00 00 00      mov        dword ptr [edx + 0x614], 1
004d8433  68 f0 17 57 00                     push       0x5717f0 ; string='LoadThread 3'
004d8438  e8 73 42 f3 ff                     call       0x40c6b0
004d843d  83 c4 04                           add        esp, 4
004d8440  68 28 18 57 00                     push       0x571828 ; string='../../bgm/thbgm.fmt'
004d8445  b9 30 a8 5b 00                     mov        ecx, 0x5ba830
004d844a  e8 f1 e3 f4 ff                     call       0x426840
004d844f  85 c0                              test       eax, eax
004d8451  74 12                              je         0x4d8465
004d8453  68 3c 18 57 00                     push       0x57183c ; string='error : BGM '
004d8458  68 78 06 5c 00                     push       0x5c0678
004d845d  e8 ee bc f7 ff                     call       0x454150
004d8462  83 c4 08                           add        esp, 8
004d8465  68 64 18 57 00                     push       0x571864 ; string='LoadThread 5'
004d846a  e8 41 42 f3 ff                     call       0x40c6b0
004d846f  83 c4 04                           add        esp, 4
004d8472  b9 58 67 5b 00                     mov        ecx, 0x5b6758
004d8477  e8 84 3e f3 ff                     call       0x40c300
004d847c  50                                 push       eax
004d847d  b9 30 a8 5b 00                     mov        ecx, 0x5ba830
004d8482  e8 59 df f4 ff                     call       0x4263e0
004d8487  b9 30 a8 5b 00                     mov        ecx, 0x5ba830
004d848c  e8 cf 00 f5 ff                     call       0x428560
004d8491  68 74 18 57 00                     push       0x571874 ; string='LoadThread 6'
004d8496  e8 15 42 f3 ff                     call       0x40c6b0
004d849b  83 c4 04                           add        esp, 4
004d849e  68 84 18 57 00                     push       0x571884 ; string='thbgm.dat'
004d84a3  e8 f8 82 f3 ff                     call       0x4107a0
004d84a8  83 c4 04                           add        esp, 4
004d84ab  0f b6 c0                           movzx      eax, al
004d84ae  85 c0                              test       eax, eax
004d84b0  74 34                              je         0x4d84e6
004d84b2  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d84b7  e8 e4 0a f5 ff                     call       0x428fa0
004d84bc  85 c0                              test       eax, eax
004d84be  75 12                              jne        0x4d84d2
004d84c0  68 84 18 57 00                     push       0x571884 ; string='thbgm.dat'
004d84c5  b9 30 a8 5b 00                     mov        ecx, 0x5ba830
004d84ca  e8 e1 01 f5 ff                     call       0x4286b0
004d84cf  90                                 nop        
004d84d0  eb 14                              jmp        0x4d84e6
004d84d2  68 84 18 57 00                     push       0x571884 ; string='thbgm.dat'
004d84d7  6a 20                              push       0x20
004d84d9  68 f4 fe 5b 00                     push       0x5bfef4
004d84de  e8 2d 01 07 00                     call       0x548610
004d84e3  83 c4 0c                           add        esp, 0xc
004d84e6  68 90 18 57 00                     push       0x571890 ; string='LoadThread 7'
004d84eb  e8 c0 41 f3 ff                     call       0x40c6b0
004d84f0  83 c4 04                           add        esp, 4
004d84f3  e8 c8 fd ff ff                     call       0x4d82c0
004d84f8  90                                 nop        
004d84f9  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004d84ff  e8 7c 53 f7 ff                     call       0x44d880
004d8504  0f b6 c8                           movzx      ecx, al
004d8507  85 c9                              test       ecx, ecx
004d8509  75 0b                              jne        0x4d8516
004d850b  6a 01                              push       1
004d850d  ff 15 6c c1 56 00                  call       dword ptr [0x56c16c] ; KERNEL32.dll!Sleep
004d8513  90                                 nop        
004d8514  eb e3                              jmp        0x4d84f9
004d8516  68 a0 18 57 00                     push       0x5718a0 ; string='loading finish'
004d851b  e8 90 41 f3 ff                     call       0x40c6b0
004d8520  83 c4 04                           add        esp, 4
004d8523  8b 55 08                           mov        edx, dword ptr [ebp + 8]
004d8526  8b 4a 08                           mov        ecx, dword ptr [edx + 8]
004d8529  e8 52 a8 f3 ff                     call       0x412d80
004d852e  6a 00                              push       0
004d8530  8b 0d 00 4a 5c 00                  mov        ecx, dword ptr [0x5c4a00]
004d8536  e8 a5 90 f7 ff                     call       0x4515e0
004d853b  33 c0                              xor        eax, eax
004d853d  eb 19                              jmp        0x4d8558
004d853f  6a 03                              push       3
004d8541  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004d8546  e8 05 5d fe ff                     call       0x4be250
004d854b  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004d854e  8b 48 08                           mov        ecx, dword ptr [eax + 8]
004d8551  e8 2a a8 f3 ff                     call       0x412d80
004d8556  33 c0                              xor        eax, eax
004d8558  8b e5                              mov        esp, ebp
004d855a  5d                                 pop        ebp
004d855b  c3                                 ret        
