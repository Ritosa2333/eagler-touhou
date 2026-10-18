005394c0  55                                 push       ebp
005394c1  8b ec                              mov        ebp, esp
005394c3  51                                 push       ecx
005394c4  6b 45 08 0c                        imul       eax, dword ptr [ebp + 8], 0xc
005394c8  83 b8 38 8b 5c 00 00               cmp        dword ptr [eax + 0x5c8b38], 0
005394cf  75 02                              jne        0x5394d3
005394d1  eb 77                              jmp        0x53954a
005394d3  6b 4d 08 0c                        imul       ecx, dword ptr [ebp + 8], 0xc
005394d7  83 b9 40 8b 5c 00 00               cmp        dword ptr [ecx + 0x5c8b40], 0
005394de  75 19                              jne        0x5394f9
005394e0  6b 55 08 0c                        imul       edx, dword ptr [ebp + 8], 0xc
005394e4  8b 82 3c 8b 5c 00                  mov        eax, dword ptr [edx + 0x5c8b3c]
005394ea  50                                 push       eax
005394eb  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
005394ee  51                                 push       ecx
005394ef  e8 8c fc ff ff                     call       0x539180
005394f4  83 c4 08                           add        esp, 8
005394f7  eb 51                              jmp        0x53954a
005394f9  6b 55 08 0c                        imul       edx, dword ptr [ebp + 8], 0xc
005394fd  83 ba 3c 8b 5c 00 00               cmp        dword ptr [edx + 0x5c8b3c], 0
00539504  75 19                              jne        0x53951f
00539506  6b 45 08 0c                        imul       eax, dword ptr [ebp + 8], 0xc
0053950a  8b 88 40 8b 5c 00                  mov        ecx, dword ptr [eax + 0x5c8b40]
00539510  51                                 push       ecx
00539511  8b 55 08                           mov        edx, dword ptr [ebp + 8]
00539514  52                                 push       edx
00539515  e8 66 fc ff ff                     call       0x539180
0053951a  83 c4 08                           add        esp, 8
0053951d  eb 2b                              jmp        0x53954a
0053951f  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00539522  50                                 push       eax
00539523  e8 38 04 00 00                     call       0x539960
00539528  83 c4 04                           add        esp, 4
0053952b  89 45 fc                           mov        dword ptr [ebp - 4], eax
0053952e  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00539531  51                                 push       ecx
00539532  e8 89 ff ff ff                     call       0x5394c0
00539537  83 c4 04                           add        esp, 4
0053953a  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0053953d  52                                 push       edx
0053953e  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00539541  50                                 push       eax
00539542  e8 c9 04 00 00                     call       0x539a10
00539547  83 c4 08                           add        esp, 8
0053954a  8b e5                              mov        esp, ebp
0053954c  5d                                 pop        ebp
0053954d  c3                                 ret        
