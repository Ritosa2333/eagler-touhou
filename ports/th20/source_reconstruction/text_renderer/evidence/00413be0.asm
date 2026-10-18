00413be0  55                                 push       ebp
00413be1  8b ec                              mov        ebp, esp
00413be3  83 ec 0c                           sub        esp, 0xc
00413be6  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
00413beb  33 c5                              xor        eax, ebp
00413bed  89 45 fc                           mov        dword ptr [ebp - 4], eax
00413bf0  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
00413bf3  83 7d 08 00                        cmp        dword ptr [ebp + 8], 0
00413bf7  75 02                              jne        0x413bfb
00413bf9  eb 36                              jmp        0x413c31
00413bfb  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00413bfe  50                                 push       eax
00413bff  e8 cc 00 00 00                     call       0x413cd0
00413c04  83 c4 04                           add        esp, 4
00413c07  6a 01                              push       1
00413c09  b9 40 02 5c 00                     mov        ecx, 0x5c0240
00413c0e  e8 fd 86 ff ff                     call       0x40c310
00413c13  50                                 push       eax
00413c14  8d 4d f8                           lea        ecx, [ebp - 8]
00413c17  e8 14 7b ff ff                     call       0x40b730
00413c1c  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00413c1f  51                                 push       ecx
00413c20  e8 d7 ed 12 00                     call       0x5429fc
00413c25  83 c4 04                           add        esp, 4
00413c28  8d 4d f8                           lea        ecx, [ebp - 8]
00413c2b  e8 d0 7c ff ff                     call       0x40b900
00413c30  90                                 nop        
00413c31  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00413c34  33 cd                              xor        ecx, ebp
00413c36  e8 b3 ed 12 00                     call       0x5429ee
00413c3b  8b e5                              mov        esp, ebp
00413c3d  5d                                 pop        ebp
00413c3e  c2 04 00                           ret        4
