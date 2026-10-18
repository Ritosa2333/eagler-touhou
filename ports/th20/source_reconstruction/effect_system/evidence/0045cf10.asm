0045cf10 55                       push       ebp
0045cf11 8bec                     mov        ebp, esp
0045cf13 83ec08                   sub        esp, 8
0045cf16 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0045cf19 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0045cf1c f30f1000                 movss      xmm0, dword ptr [eax]
0045cf20 f30f5901                 mulss      xmm0, dword ptr [ecx]
0045cf24 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0045cf27 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0045cf2a f30f104a04               movss      xmm1, dword ptr [edx + 4]
0045cf2f f30f594804               mulss      xmm1, dword ptr [eax + 4]
0045cf34 f30f58c1                 addss      xmm0, xmm1
0045cf38 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0045cf3b 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0045cf3e f30f104908               movss      xmm1, dword ptr [ecx + 8]
0045cf43 f30f594a08               mulss      xmm1, dword ptr [edx + 8]
0045cf48 f30f58c1                 addss      xmm0, xmm1
0045cf4c 51                       push       ecx
0045cf4d f30f110424               movss      dword ptr [esp], xmm0
0045cf52 e8d99bfeff               call       0x446b30
0045cf57 83c404                   add        esp, 4
0045cf5a d95dfc                   fstp       dword ptr [ebp - 4]
0045cf5d 51                       push       ecx
0045cf5e f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
0045cf63 f30f110424               movss      dword ptr [esp], xmm0
0045cf68 e81387feff               call       0x445680
0045cf6d 83c404                   add        esp, 4
0045cf70 d95df8                   fstp       dword ptr [ebp - 8]
0045cf73 f30f1005a0e15600         movss      xmm0, dword ptr [0x56e1a0]
0045cf7b 0f2f45f8                 comiss     xmm0, dword ptr [ebp - 8]
0045cf7f 7618                     jbe        0x45cf99
0045cf81 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0045cf84 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0045cf87 8b10                     mov        edx, dword ptr [eax]
0045cf89 8911                     mov        dword ptr [ecx], edx
0045cf8b 8b5004                   mov        edx, dword ptr [eax + 4]
0045cf8e 895104                   mov        dword ptr [ecx + 4], edx
0045cf91 8b4008                   mov        eax, dword ptr [eax + 8]
0045cf94 894108                   mov        dword ptr [ecx + 8], eax
0045cf97 eb3d                     jmp        0x45cfd6
0045cf99 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0045cf9c f30f1001                 movss      xmm0, dword ptr [ecx]
0045cfa0 f30f5e45fc               divss      xmm0, dword ptr [ebp - 4]
0045cfa5 8b5508                   mov        edx, dword ptr [ebp + 8]
0045cfa8 f30f1102                 movss      dword ptr [edx], xmm0
0045cfac 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0045cfaf f30f104004               movss      xmm0, dword ptr [eax + 4]
0045cfb4 f30f5e45fc               divss      xmm0, dword ptr [ebp - 4]
0045cfb9 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0045cfbc f30f114104               movss      dword ptr [ecx + 4], xmm0
0045cfc1 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0045cfc4 f30f104208               movss      xmm0, dword ptr [edx + 8]
0045cfc9 f30f5e45fc               divss      xmm0, dword ptr [ebp - 4]
0045cfce 8b4508                   mov        eax, dword ptr [ebp + 8]
0045cfd1 f30f114008               movss      dword ptr [eax + 8], xmm0
0045cfd6 8be5                     mov        esp, ebp
0045cfd8 5d                       pop        ebp
0045cfd9 c3                       ret        
0045cfda cc                       int3       
0045cfdb cc                       int3       
0045cfdc cc                       int3       
0045cfdd cc                       int3       
0045cfde cc                       int3       
0045cfdf cc                       int3       
