0043e910 55                       push       ebp
0043e911 8bec                     mov        ebp, esp
0043e913 83ec08                   sub        esp, 8
0043e916 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0043e919 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0043e91c f30f1000                 movss      xmm0, dword ptr [eax]
0043e920 f30f5901                 mulss      xmm0, dword ptr [ecx]
0043e924 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0043e927 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0043e92a f30f104a04               movss      xmm1, dword ptr [edx + 4]
0043e92f f30f594804               mulss      xmm1, dword ptr [eax + 4]
0043e934 f30f58c1                 addss      xmm0, xmm1
0043e938 51                       push       ecx
0043e939 f30f110424               movss      dword ptr [esp], xmm0
0043e93e e8ed810000               call       0x446b30
0043e943 83c404                   add        esp, 4
0043e946 d95dfc                   fstp       dword ptr [ebp - 4]
0043e949 51                       push       ecx
0043e94a f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
0043e94f f30f110424               movss      dword ptr [esp], xmm0
0043e954 e8276d0000               call       0x445680
0043e959 83c404                   add        esp, 4
0043e95c d95df8                   fstp       dword ptr [ebp - 8]
0043e95f f30f1005a0e15600         movss      xmm0, dword ptr [0x56e1a0]
0043e967 0f2f45f8                 comiss     xmm0, dword ptr [ebp - 8]
0043e96b 7612                     jbe        0x43e97f
0043e96d 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0043e970 8b11                     mov        edx, dword ptr [ecx]
0043e972 8b4104                   mov        eax, dword ptr [ecx + 4]
0043e975 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0043e978 8911                     mov        dword ptr [ecx], edx
0043e97a 894104                   mov        dword ptr [ecx + 4], eax
0043e97d eb28                     jmp        0x43e9a7
0043e97f 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0043e982 f30f1002                 movss      xmm0, dword ptr [edx]
0043e986 f30f5e45fc               divss      xmm0, dword ptr [ebp - 4]
0043e98b 8b4508                   mov        eax, dword ptr [ebp + 8]
0043e98e f30f1100                 movss      dword ptr [eax], xmm0
0043e992 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0043e995 f30f104104               movss      xmm0, dword ptr [ecx + 4]
0043e99a f30f5e45fc               divss      xmm0, dword ptr [ebp - 4]
0043e99f 8b5508                   mov        edx, dword ptr [ebp + 8]
0043e9a2 f30f114204               movss      dword ptr [edx + 4], xmm0
0043e9a7 8be5                     mov        esp, ebp
0043e9a9 5d                       pop        ebp
0043e9aa c3                       ret        
0043e9ab cc                       int3       
0043e9ac cc                       int3       
0043e9ad cc                       int3       
0043e9ae cc                       int3       
0043e9af cc                       int3       
