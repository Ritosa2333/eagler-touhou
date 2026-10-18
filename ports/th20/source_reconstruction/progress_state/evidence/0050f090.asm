0050f090  55                                 push       ebp
0050f091  8b ec                              mov        ebp, esp
0050f093  83 ec 10                           sub        esp, 0x10
0050f096  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0050f099  b8 53 54 00 00                     mov        eax, 0x5453
0050f09e  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050f0a1  66 89 01                           mov        word ptr [ecx], ax
0050f0a4  ba 02 00 00 00                     mov        edx, 2
0050f0a9  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050f0ac  66 89 50 02                        mov        word ptr [eax + 2], dx
0050f0b0  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050f0b3  c7 41 08 f8 01 00 00               mov        dword ptr [ecx + 8], 0x1f8
0050f0ba  68 ac 24 57 00                     push       0x5724ac ; string='        '
0050f0bf  6a 0a                              push       0xa
0050f0c1  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050f0c4  83 c2 0c                           add        edx, 0xc
0050f0c7  52                                 push       edx
0050f0c8  e8 43 95 03 00                     call       0x548610
0050f0cd  83 c4 0c                           add        esp, 0xc
0050f0d0  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
0050f0d7  eb 09                              jmp        0x50f0e2
0050f0d9  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0050f0dc  83 c0 01                           add        eax, 1
0050f0df  89 45 f8                           mov        dword ptr [ebp - 8], eax
0050f0e2  83 7d f8 20                        cmp        dword ptr [ebp - 8], 0x20
0050f0e6  7d 1d                              jge        0x50f105
0050f0e8  b9 c4 a4 5b 00                     mov        ecx, 0x5ba4c4
0050f0ed  e8 ee 4d f1 ff                     call       0x423ee0 ; candidate_rng_locked
0050f0f2  25 ff 00 00 00                     and        eax, 0xff
0050f0f7  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050f0fa  03 4d f8                           add        ecx, dword ptr [ebp - 8]
0050f0fd  88 81 b0 01 00 00                  mov        byte ptr [ecx + 0x1b0], al
0050f103  eb d4                              jmp        0x50f0d9
0050f105  c7 45 f4 00 00 00 00               mov        dword ptr [ebp - 0xc], 0
0050f10c  eb 09                              jmp        0x50f117
0050f10e  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
0050f111  83 c2 01                           add        edx, 1
0050f114  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
0050f117  83 7d f4 20                        cmp        dword ptr [ebp - 0xc], 0x20
0050f11b  7d 1d                              jge        0x50f13a
0050f11d  b9 c4 a4 5b 00                     mov        ecx, 0x5ba4c4
0050f122  e8 b9 4d f1 ff                     call       0x423ee0 ; candidate_rng_locked
0050f127  25 ff 00 00 00                     and        eax, 0xff
0050f12c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050f12f  03 4d f4                           add        ecx, dword ptr [ebp - 0xc]
0050f132  88 81 d2 01 00 00                  mov        byte ptr [ecx + 0x1d2], al
0050f138  eb d4                              jmp        0x50f10e
0050f13a  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050f13d  81 c2 68 01 00 00                  add        edx, 0x168
0050f143  89 55 f0                           mov        dword ptr [ebp - 0x10], edx
0050f146  6a 08                              push       8
0050f148  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050f14b  e8 30 df fa ff                     call       0x4bd080
0050f150  c7 00 09 00 00 00                  mov        dword ptr [eax], 9
0050f156  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050f159  e8 02 fa fa ff                     call       0x4beb60
0050f15e  90                                 nop        
0050f15f  8b e5                              mov        esp, ebp
0050f161  5d                                 pop        ebp
0050f162  c3                                 ret        
