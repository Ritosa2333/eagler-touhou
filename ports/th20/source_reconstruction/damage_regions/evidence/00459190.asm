00459190 55                       push       ebp
00459191 8bec                     mov        ebp, esp
00459193 83ec08                   sub        esp, 8
00459196 51                       push       ecx
00459197 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
0045919c f30f110424               movss      dword ptr [esp], xmm0
004591a1 e82ae0fdff               call       0x4371d0
004591a6 83c404                   add        esp, 4
004591a9 d95dfc                   fstp       dword ptr [ebp - 4]
004591ac f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
004591b1 f30f594510               mulss      xmm0, dword ptr [ebp + 0x10]
004591b6 8b4508                   mov        eax, dword ptr [ebp + 8]
004591b9 f30f1100                 movss      dword ptr [eax], xmm0
004591bd 51                       push       ecx
004591be f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004591c3 f30f110424               movss      dword ptr [esp], xmm0
004591c8 e86303feff               call       0x439530
004591cd 83c404                   add        esp, 4
004591d0 d95df8                   fstp       dword ptr [ebp - 8]
004591d3 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
004591d8 f30f594514               mulss      xmm0, dword ptr [ebp + 0x14]
004591dd 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004591e0 f30f114104               movss      dword ptr [ecx + 4], xmm0
004591e5 8be5                     mov        esp, ebp
004591e7 5d                       pop        ebp
004591e8 c3                       ret        
004591e9 cc                       int3       
004591ea cc                       int3       
004591eb cc                       int3       
004591ec cc                       int3       
004591ed cc                       int3       
004591ee cc                       int3       
004591ef cc                       int3       
