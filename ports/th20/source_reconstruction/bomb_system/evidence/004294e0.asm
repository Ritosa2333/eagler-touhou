004294e0 55                       push       ebp
004294e1 8bec                     mov        ebp, esp
004294e3 51                       push       ecx
004294e4 894dfc                   mov        dword ptr [ebp - 4], ecx
004294e7 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004294ea 51                       push       ecx
004294eb f30f1000                 movss      xmm0, dword ptr [eax]
004294ef f30f110424               movss      dword ptr [esp], xmm0
004294f4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004294f7 51                       push       ecx
004294f8 f30f1001                 movss      xmm0, dword ptr [ecx]
004294fc f30f110424               movss      dword ptr [esp], xmm0
00429501 e8ba010100               call       0x4396c0
00429506 83c404                   add        esp, 4
00429509 d91c24                   fstp       dword ptr [esp]
0042950c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0042950f e8fcfcffff               call       0x429210
00429514 8b4508                   mov        eax, dword ptr [ebp + 8]
00429517 8be5                     mov        esp, ebp
00429519 5d                       pop        ebp
0042951a c20800                   ret        8
0042951d cc                       int3       
0042951e cc                       int3       
0042951f cc                       int3       
