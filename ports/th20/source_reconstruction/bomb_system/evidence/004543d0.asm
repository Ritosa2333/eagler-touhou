004543d0 55                       push       ebp
004543d1 8bec                     mov        ebp, esp
004543d3 83ec0c                   sub        esp, 0xc
004543d6 894dfc                   mov        dword ptr [ebp - 4], ecx
004543d9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004543dc f30f1000                 movss      xmm0, dword ptr [eax]
004543e0 f30f59054ce05600         mulss      xmm0, dword ptr [0x56e04c]
004543e8 51                       push       ecx
004543e9 f30f110424               movss      dword ptr [esp], xmm0
004543ee e8bd4e0000               call       0x4592b0
004543f3 83c404                   add        esp, 4
004543f6 d95df8                   fstp       dword ptr [ebp - 8]
004543f9 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
004543fe f30f5e054ce05600         divss      xmm0, dword ptr [0x56e04c]
00454406 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00454409 f30f1101                 movss      dword ptr [ecx], xmm0
0045440d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00454410 f30f104204               movss      xmm0, dword ptr [edx + 4]
00454415 f30f59054ce05600         mulss      xmm0, dword ptr [0x56e04c]
0045441d 51                       push       ecx
0045441e f30f110424               movss      dword ptr [esp], xmm0
00454423 e8884e0000               call       0x4592b0
00454428 83c404                   add        esp, 4
0045442b d95df4                   fstp       dword ptr [ebp - 0xc]
0045442e f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00454433 f30f5e054ce05600         divss      xmm0, dword ptr [0x56e04c]
0045443b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045443e f30f114004               movss      dword ptr [eax + 4], xmm0
00454443 8be5                     mov        esp, ebp
00454445 5d                       pop        ebp
00454446 c3                       ret        
00454447 cc                       int3       
00454448 cc                       int3       
00454449 cc                       int3       
0045444a cc                       int3       
0045444b cc                       int3       
0045444c cc                       int3       
0045444d cc                       int3       
0045444e cc                       int3       
0045444f cc                       int3       
