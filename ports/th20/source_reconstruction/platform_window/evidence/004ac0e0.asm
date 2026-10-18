004ac0e0  55                                 push       ebp
004ac0e1  8b ec                              mov        ebp, esp
004ac0e3  83 ec 0c                           sub        esp, 0xc
004ac0e6  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004ac0e9  8d 45 f4                           lea        eax, [ebp - 0xc]
004ac0ec  50                                 push       eax
004ac0ed  e8 7e 00 00 00                     call       0x4ac170
004ac0f2  83 c4 04                           add        esp, 4
004ac0f5  8b 08                              mov        ecx, dword ptr [eax]
004ac0f7  8b 50 04                           mov        edx, dword ptr [eax + 4]
004ac0fa  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ac0fd  89 88 d0 00 00 00                  mov        dword ptr [eax + 0xd0], ecx
004ac103  89 90 d4 00 00 00                  mov        dword ptr [eax + 0xd4], edx
004ac109  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004ac10c  81 c1 d0 00 00 00                  add        ecx, 0xd0
004ac112  51                                 push       ecx
004ac113  e8 e8 00 00 00                     call       0x4ac200
004ac118  83 c4 04                           add        esp, 4
004ac11b  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004ac11e  89 81 d8 00 00 00                  mov        dword ptr [ecx + 0xd8], eax
004ac124  89 91 dc 00 00 00                  mov        dword ptr [ecx + 0xdc], edx
004ac12a  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004ac12d  81 c2 d8 00 00 00                  add        edx, 0xd8
004ac133  52                                 push       edx
004ac134  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ac137  05 e0 00 00 00                     add        eax, 0xe0
004ac13c  50                                 push       eax
004ac13d  e8 2e 01 00 00                     call       0x4ac270
004ac142  83 c4 08                           add        esp, 8
004ac145  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ac148  05 d0 00 00 00                     add        eax, 0xd0
004ac14d  8b e5                              mov        esp, ebp
004ac14f  5d                                 pop        ebp
004ac150  c3                                 ret        
