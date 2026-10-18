004145d0  55                                 push       ebp
004145d1  8b ec                              mov        ebp, esp
004145d3  83 ec 2c                           sub        esp, 0x2c
004145d6  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004145db  33 c5                              xor        eax, ebp
004145dd  89 45 fc                           mov        dword ptr [ebp - 4], eax
004145e0  89 4d ec                           mov        dword ptr [ebp - 0x14], ecx
004145e3  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
004145e6  8b 48 04                           mov        ecx, dword ptr [eax + 4]
004145e9  89 4d dc                           mov        dword ptr [ebp - 0x24], ecx
004145ec  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
004145ef  8b 42 04                           mov        eax, dword ptr [edx + 4]
004145f2  8b 08                              mov        ecx, dword ptr [eax]
004145f4  8b 51 48                           mov        edx, dword ptr [ecx + 0x48]
004145f7  89 55 d8                           mov        dword ptr [ebp - 0x28], edx
004145fa  8d 45 f8                           lea        eax, [ebp - 8]
004145fd  50                                 push       eax
004145fe  6a 00                              push       0
00414600  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
00414603  51                                 push       ecx
00414604  ff 55 d8                           call       dword ptr [ebp - 0x28]
00414607  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0041460a  8b 02                              mov        eax, dword ptr [edx]
0041460c  8b 48 34                           mov        ecx, dword ptr [eax + 0x34]
0041460f  89 4d d4                           mov        dword ptr [ebp - 0x2c], ecx
00414612  6a 00                              push       0
00414614  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
00414617  83 c2 08                           add        edx, 8
0041461a  52                                 push       edx
0041461b  8d 45 f0                           lea        eax, [ebp - 0x10]
0041461e  50                                 push       eax
0041461f  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
00414622  51                                 push       ecx
00414623  ff 55 d4                           call       dword ptr [ebp - 0x2c]
00414626  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
00414629  89 55 e0                           mov        dword ptr [ebp - 0x20], edx
0041462c  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
0041462f  8b 08                              mov        ecx, dword ptr [eax]
00414631  e8 6a 10 00 00                     call       0x4156a0
00414636  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
00414639  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
0041463c  8b 09                              mov        ecx, dword ptr [ecx]
0041463e  e8 7d 10 00 00                     call       0x4156c0
00414643  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
00414646  0f af 42 1c                        imul       eax, dword ptr [edx + 0x1c]
0041464a  03 45 e8                           add        eax, dword ptr [ebp - 0x18]
0041464d  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
00414650  c7 45 e4 00 00 00 00               mov        dword ptr [ebp - 0x1c], 0
00414657  eb 09                              jmp        0x414662
00414659  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
0041465c  83 c0 01                           add        eax, 1
0041465f  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
00414662  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
00414665  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
00414668  8b 41 14                           mov        eax, dword ptr [ecx + 0x14]
0041466b  2b 42 0c                           sub        eax, dword ptr [edx + 0xc]
0041466e  39 45 e4                           cmp        dword ptr [ebp - 0x1c], eax
00414671  7d 41                              jge        0x4146b4
00414673  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
00414676  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
00414679  8b 41 20                           mov        eax, dword ptr [ecx + 0x20]
0041467c  2b 42 18                           sub        eax, dword ptr [edx + 0x18]
0041467f  c1 e0 02                           shl        eax, 2
00414682  50                                 push       eax
00414683  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
00414686  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
00414689  03 51 18                           add        edx, dword ptr [ecx + 0x18]
0041468c  52                                 push       edx
0041468d  8b 45 e0                           mov        eax, dword ptr [ebp - 0x20]
00414690  50                                 push       eax
00414691  e8 8a f7 12 00                     call       0x543e20
00414696  83 c4 0c                           add        esp, 0xc
00414699  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
0041469c  03 4d f0                           add        ecx, dword ptr [ebp - 0x10]
0041469f  89 4d e0                           mov        dword ptr [ebp - 0x20], ecx
004146a2  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
004146a5  8b 0a                              mov        ecx, dword ptr [edx]
004146a7  e8 14 10 00 00                     call       0x4156c0
004146ac  03 45 e8                           add        eax, dword ptr [ebp - 0x18]
004146af  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
004146b2  eb a5                              jmp        0x414659
004146b4  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004146b7  8b 08                              mov        ecx, dword ptr [eax]
004146b9  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004146bc  52                                 push       edx
004146bd  8b 41 38                           mov        eax, dword ptr [ecx + 0x38]
004146c0  ff d0                              call       eax
004146c2  90                                 nop        
004146c3  83 7d f8 00                        cmp        dword ptr [ebp - 8], 0
004146c7  74 15                              je         0x4146de
004146c9  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004146cc  8b 11                              mov        edx, dword ptr [ecx]
004146ce  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004146d1  50                                 push       eax
004146d2  8b 4a 08                           mov        ecx, dword ptr [edx + 8]
004146d5  ff d1                              call       ecx
004146d7  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
004146de  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
004146e1  8b 0a                              mov        ecx, dword ptr [edx]
004146e3  e8 78 1c 00 00                     call       0x416360
004146e8  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
004146eb  8b 08                              mov        ecx, dword ptr [eax]
004146ed  51                                 push       ecx
004146ee  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004146f4  e8 e7 f4 ff ff                     call       0x413be0
004146f9  90                                 nop        
004146fa  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
004146fd  83 7a 28 00                        cmp        dword ptr [edx + 0x28], 0
00414701  74 09                              je         0x41470c
00414703  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
00414706  8b 48 28                           mov        ecx, dword ptr [eax + 0x28]
00414709  c6 01 01                           mov        byte ptr [ecx], 1
0041470c  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
0041470f  83 c1 30                           add        ecx, 0x30
00414712  e8 89 fe ff ff                     call       0x4145a0
00414717  0f b6 d0                           movzx      edx, al
0041471a  85 d2                              test       edx, edx
0041471c  74 0c                              je         0x41472a
0041471e  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
00414721  83 c1 30                           add        ecx, 0x30
00414724  e8 17 00 00 00                     call       0x414740
00414729  90                                 nop        
0041472a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041472d  33 cd                              xor        ecx, ebp
0041472f  e8 ba e2 12 00                     call       0x5429ee
00414734  8b e5                              mov        esp, ebp
00414736  5d                                 pop        ebp
00414737  c3                                 ret        
