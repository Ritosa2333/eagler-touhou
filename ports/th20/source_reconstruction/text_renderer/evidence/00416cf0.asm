00416cf0  55                                 push       ebp
00416cf1  8b ec                              mov        ebp, esp
00416cf3  f3 0f 10 45 08                     movss      xmm0, dword ptr [ebp + 8]
00416cf8  f3 0f 11 05 20 e1 5a 00            movss      dword ptr [0x5ae120], xmm0
00416d00  5d                                 pop        ebp
00416d01  c3                                 ret        
