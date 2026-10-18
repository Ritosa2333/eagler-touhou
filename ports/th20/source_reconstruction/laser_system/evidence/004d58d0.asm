004d58d0 55                       push       ebp
004d58d1 8bec                     mov        ebp, esp
004d58d3 83ec4c                   sub        esp, 0x4c
004d58d6 56                       push       esi
004d58d7 894dfc                   mov        dword ptr [ebp - 4], ecx
004d58da 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d58dd 0590000000               add        eax, 0x90
004d58e2 8945f8                   mov        dword ptr [ebp - 8], eax
004d58e5 6a01                     push       1
004d58e7 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004d58ea e8916dfaff               call       0x47c680
004d58ef 8945ec                   mov        dword ptr [ebp - 0x14], eax
004d58f2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d58f5 81c190000000             add        ecx, 0x90
004d58fb 894df4                   mov        dword ptr [ebp - 0xc], ecx
004d58fe 6a01                     push       1
004d5900 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004d5903 e8786dfaff               call       0x47c680
004d5908 8b5030                   mov        edx, dword ptr [eax + 0x30]
004d590b 8955f0                   mov        dword ptr [ebp - 0x10], edx
004d590e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004d5911 50                       push       eax
004d5912 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d5915 e8a6dcf4ff               call       0x4235c0
004d591a 0fb6c8                   movzx      ecx, al
004d591d 85c9                     test       ecx, ecx
004d591f 7424                     je         0x4d5945
004d5921 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d5924 8b8298060000             mov        eax, dword ptr [edx + 0x698]
004d592a 83e0fb                   and        eax, 0xfffffffb
004d592d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d5930 898198060000             mov        dword ptr [ecx + 0x698], eax
004d5936 b801000000               mov        eax, 1
004d593b e941010000               jmp        0x4d5a81
004d5940 e918010000               jmp        0x4d5a5d
004d5945 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d5948 81c290000000             add        edx, 0x90
004d594e 8955e8                   mov        dword ptr [ebp - 0x18], edx
004d5951 6a01                     push       1
004d5953 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d5956 e8256dfaff               call       0x47c680
004d595b 8bf0                     mov        esi, eax
004d595d b9e4ef5a00               mov        ecx, 0x5aefe4
004d5962 e87939f5ff               call       0x4292e0
004d5967 d95de4                   fstp       dword ptr [ebp - 0x1c]
004d596a f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
004d596f f30f594610               mulss      xmm0, dword ptr [esi + 0x10]
004d5974 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
004d5979 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d597c f30f10407c               movss      xmm0, dword ptr [eax + 0x7c]
004d5981 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
004d5986 f30f1045e0               movss      xmm0, dword ptr [ebp - 0x20]
004d598b f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
004d5990 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d5993 f30f11417c               movss      dword ptr [ecx + 0x7c], xmm0
004d5998 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d599b 81c290000000             add        edx, 0x90
004d59a1 8955d8                   mov        dword ptr [ebp - 0x28], edx
004d59a4 6a01                     push       1
004d59a6 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004d59a9 e8d26cfaff               call       0x47c680
004d59ae 83c024                   add        eax, 0x24
004d59b1 8945d0                   mov        dword ptr [ebp - 0x30], eax
004d59b4 b9e4ef5a00               mov        ecx, 0x5aefe4
004d59b9 e82239f5ff               call       0x4292e0
004d59be d95dd4                   fstp       dword ptr [ebp - 0x2c]
004d59c1 51                       push       ecx
004d59c2 f30f1045d4               movss      xmm0, dword ptr [ebp - 0x2c]
004d59c7 f30f110424               movss      dword ptr [esp], xmm0
004d59cc 8d45b4                   lea        eax, [ebp - 0x4c]
004d59cf 50                       push       eax
004d59d0 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004d59d3 e81839f5ff               call       0x4292f0
004d59d8 50                       push       eax
004d59d9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d59dc 83c164                   add        ecx, 0x64
004d59df e8fc3cf5ff               call       0x4296e0
004d59e4 90                       nop        
004d59e5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d59e8 51                       push       ecx
004d59e9 f30f104164               movss      xmm0, dword ptr [ecx + 0x64]
004d59ee f30f110424               movss      dword ptr [esp], xmm0
004d59f3 e8384cfaff               call       0x47a630
004d59f8 83c404                   add        esp, 4
004d59fb d95dcc                   fstp       dword ptr [ebp - 0x34]
004d59fe f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
004d5a03 0f2f0530fd5600           comiss     xmm0, dword ptr [0x56fd30]
004d5a0a 7727                     ja         0x4d5a33
004d5a0c 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d5a0f 51                       push       ecx
004d5a10 f30f104268               movss      xmm0, dword ptr [edx + 0x68]
004d5a15 f30f110424               movss      dword ptr [esp], xmm0
004d5a1a e8114cfaff               call       0x47a630
004d5a1f 83c404                   add        esp, 4
004d5a22 d95dc8                   fstp       dword ptr [ebp - 0x38]
004d5a25 f30f1045c8               movss      xmm0, dword ptr [ebp - 0x38]
004d5a2a 0f2f0530fd5600           comiss     xmm0, dword ptr [0x56fd30]
004d5a31 762a                     jbe        0x4d5a5d
004d5a33 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d5a36 51                       push       ecx
004d5a37 f30f104064               movss      xmm0, dword ptr [eax + 0x64]
004d5a3c f30f110424               movss      dword ptr [esp], xmm0
004d5a41 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d5a44 51                       push       ecx
004d5a45 f30f104168               movss      xmm0, dword ptr [ecx + 0x68]
004d5a4a f30f110424               movss      dword ptr [esp], xmm0
004d5a4f e82c38f8ff               call       0x459280
004d5a54 83c408                   add        esp, 8
004d5a57 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d5a5a d95a70                   fstp       dword ptr [edx + 0x70]
004d5a5d 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d5a60 0590000000               add        eax, 0x90
004d5a65 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004d5a68 6a01                     push       1
004d5a6a 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004d5a6d e80e6cfaff               call       0x47c680
004d5a72 8945c0                   mov        dword ptr [ebp - 0x40], eax
004d5a75 6a00                     push       0
004d5a77 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004d5a7a e8c1daf4ff               call       0x423540
004d5a7f 33c0                     xor        eax, eax
004d5a81 5e                       pop        esi
004d5a82 8be5                     mov        esp, ebp
004d5a84 5d                       pop        ebp
004d5a85 c3                       ret        
004d5a86 cc                       int3       
004d5a87 cc                       int3       
004d5a88 cc                       int3       
004d5a89 cc                       int3       
004d5a8a cc                       int3       
004d5a8b cc                       int3       
004d5a8c cc                       int3       
004d5a8d cc                       int3       
004d5a8e cc                       int3       
004d5a8f cc                       int3       
