00452fc0 55                       push       ebp
00452fc1 8bec                     mov        ebp, esp
00452fc3 51                       push       ecx
00452fc4 894dfc                   mov        dword ptr [ebp - 4], ecx
00452fc7 8b45fc                   mov        eax, dword ptr [ebp - 4]
00452fca f30f1000                 movss      xmm0, dword ptr [eax]
00452fce f30f58450c               addss      xmm0, dword ptr [ebp + 0xc]
00452fd3 51                       push       ecx
00452fd4 f30f110424               movss      dword ptr [esp], xmm0
00452fd9 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00452fdc e82f62fdff               call       0x429210
00452fe1 8b4508                   mov        eax, dword ptr [ebp + 8]
00452fe4 8be5                     mov        esp, ebp
00452fe6 5d                       pop        ebp
00452fe7 c20800                   ret        8
00452fea cc                       int3       
00452feb cc                       int3       
00452fec cc                       int3       
00452fed cc                       int3       
00452fee cc                       int3       
00452fef cc                       int3       
