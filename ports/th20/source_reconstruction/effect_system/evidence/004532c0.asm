004532c0 55                       push       ebp
004532c1 8bec                     mov        ebp, esp
004532c3 83ec30                   sub        esp, 0x30
004532c6 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004532c9 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004532cc f30f1000                 movss      xmm0, dword ptr [eax]
004532d0 f30f5901                 mulss      xmm0, dword ptr [ecx]
004532d4 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004532d7 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004532da f30f104a04               movss      xmm1, dword ptr [edx + 4]
004532df f30f594804               mulss      xmm1, dword ptr [eax + 4]
004532e4 f30f58c1                 addss      xmm0, xmm1
004532e8 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004532eb 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004532ee f30f104908               movss      xmm1, dword ptr [ecx + 8]
004532f3 f30f594a08               mulss      xmm1, dword ptr [edx + 8]
004532f8 f30f58c1                 addss      xmm0, xmm1
004532fc 51                       push       ecx
004532fd f30f110424               movss      dword ptr [esp], xmm0
00453302 e82938ffff               call       0x446b30
00453307 83c404                   add        esp, 4
0045330a d95dfc                   fstp       dword ptr [ebp - 4]
0045330d 51                       push       ecx
0045330e f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00453313 f30f110424               movss      dword ptr [esp], xmm0
00453318 e86323ffff               call       0x445680
0045331d 83c404                   add        esp, 4
00453320 d95df8                   fstp       dword ptr [ebp - 8]
00453323 f30f1005a0e15600         movss      xmm0, dword ptr [0x56e1a0]
0045332b 0f2f45f8                 comiss     xmm0, dword ptr [ebp - 8]
0045332f 762c                     jbe        0x45335d
00453331 51                       push       ecx
00453332 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00453337 f30f110424               movss      dword ptr [esp], xmm0
0045333c 8d45e8                   lea        eax, [ebp - 0x18]
0045333f 50                       push       eax
00453340 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00453343 e8a85ffdff               call       0x4292f0
00453348 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0045334b 8b10                     mov        edx, dword ptr [eax]
0045334d 8911                     mov        dword ptr [ecx], edx
0045334f 8b5004                   mov        edx, dword ptr [eax + 4]
00453352 895104                   mov        dword ptr [ecx + 4], edx
00453355 8b4008                   mov        eax, dword ptr [eax + 8]
00453358 894108                   mov        dword ptr [ecx + 8], eax
0045335b eb44                     jmp        0x4533a1
0045335d 51                       push       ecx
0045335e f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00453363 f30f110424               movss      dword ptr [esp], xmm0
00453368 8d4ddc                   lea        ecx, [ebp - 0x24]
0045336b 51                       push       ecx
0045336c 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0045336f e87cfcffff               call       0x452ff0
00453374 8945f4                   mov        dword ptr [ebp - 0xc], eax
00453377 51                       push       ecx
00453378 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
0045337d f30f110424               movss      dword ptr [esp], xmm0
00453382 8d55d0                   lea        edx, [ebp - 0x30]
00453385 52                       push       edx
00453386 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00453389 e8625ffdff               call       0x4292f0
0045338e 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00453391 8b10                     mov        edx, dword ptr [eax]
00453393 8911                     mov        dword ptr [ecx], edx
00453395 8b5004                   mov        edx, dword ptr [eax + 4]
00453398 895104                   mov        dword ptr [ecx + 4], edx
0045339b 8b4008                   mov        eax, dword ptr [eax + 8]
0045339e 894108                   mov        dword ptr [ecx + 8], eax
004533a1 8be5                     mov        esp, ebp
004533a3 5d                       pop        ebp
004533a4 c3                       ret        
004533a5 cc                       int3       
004533a6 cc                       int3       
004533a7 cc                       int3       
004533a8 cc                       int3       
004533a9 cc                       int3       
004533aa cc                       int3       
004533ab cc                       int3       
004533ac cc                       int3       
004533ad cc                       int3       
004533ae cc                       int3       
004533af cc                       int3       
