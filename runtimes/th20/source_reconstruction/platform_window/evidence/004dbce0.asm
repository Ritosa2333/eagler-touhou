004dbce0  55                                 push       ebp
004dbce1  8b ec                              mov        ebp, esp
004dbce3  83 ec 0c                           sub        esp, 0xc
004dbce6  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
004dbce9  a1 10 88 5b 00                     mov        eax, dword ptr [0x5b8810]
004dbcee  2b 05 f8 87 5b 00                  sub        eax, dword ptr [0x5b87f8]
004dbcf4  f3 0f 2a c0                        cvtsi2ss   xmm0, eax
004dbcf8  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
004dbd00  f3 0f 11 45 f8                     movss      dword ptr [ebp - 8], xmm0
004dbd05  8b 0d 14 88 5b 00                  mov        ecx, dword ptr [0x5b8814]
004dbd0b  2b 0d fc 87 5b 00                  sub        ecx, dword ptr [0x5b87fc]
004dbd11  f3 0f 2a c1                        cvtsi2ss   xmm0, ecx
004dbd15  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
004dbd1d  f3 0f 11 45 fc                     movss      dword ptr [ebp - 4], xmm0
004dbd22  f3 0f 5a 45 fc                     cvtss2sd   xmm0, dword ptr [ebp - 4]
004dbd27  83 ec 08                           sub        esp, 8
004dbd2a  f2 0f 11 04 24                     movsd      qword ptr [esp], xmm0
004dbd2f  f3 0f 5a 45 f8                     cvtss2sd   xmm0, dword ptr [ebp - 8]
004dbd34  83 ec 08                           sub        esp, 8
004dbd37  f2 0f 11 04 24                     movsd      qword ptr [esp], xmm0
004dbd3c  68 5c 1e 57 00                     push       0x571e5c ; string='RenderOffet %f, %f'
004dbd41  e8 6a 09 f3 ff                     call       0x40c6b0
004dbd46  83 c4 14                           add        esp, 0x14
004dbd49  f3 0f 2c 55 fc                     cvttss2si  edx, dword ptr [ebp - 4]
004dbd4e  52                                 push       edx
004dbd4f  f3 0f 2c 45 f8                     cvttss2si  eax, dword ptr [ebp - 8]
004dbd54  50                                 push       eax
004dbd55  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dbd5a  e8 c1 1d 00 00                     call       0x4ddb20
004dbd5f  90                                 nop        
004dbd60  8b e5                              mov        esp, ebp
004dbd62  5d                                 pop        ebp
004dbd63  c3                                 ret        
