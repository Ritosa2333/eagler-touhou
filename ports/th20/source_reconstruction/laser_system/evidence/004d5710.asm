004d5710 55                       push       ebp
004d5711 8bec                     mov        ebp, esp
004d5713 83ec4c                   sub        esp, 0x4c
004d5716 56                       push       esi
004d5717 894dfc                   mov        dword ptr [ebp - 4], ecx
004d571a 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d571d 0590000000               add        eax, 0x90
004d5722 8945f8                   mov        dword ptr [ebp - 8], eax
004d5725 6a0a                     push       0xa
004d5727 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004d572a e8516ffaff               call       0x47c680
004d572f 8945ec                   mov        dword ptr [ebp - 0x14], eax
004d5732 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d5735 81c190000000             add        ecx, 0x90
004d573b 894df4                   mov        dword ptr [ebp - 0xc], ecx
004d573e 6a0a                     push       0xa
004d5740 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004d5743 e8386ffaff               call       0x47c680
004d5748 8b5030                   mov        edx, dword ptr [eax + 0x30]
004d574b 8955f0                   mov        dword ptr [ebp - 0x10], edx
004d574e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004d5751 50                       push       eax
004d5752 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d5755 e866def4ff               call       0x4235c0
004d575a 0fb6c8                   movzx      ecx, al
004d575d 85c9                     test       ecx, ecx
004d575f 7426                     je         0x4d5787
004d5761 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d5764 8b8298060000             mov        eax, dword ptr [edx + 0x698]
004d576a 25ffffdfff               and        eax, 0xffdfffff
004d576f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d5772 898198060000             mov        dword ptr [ecx + 0x698], eax
004d5778 b801000000               mov        eax, 1
004d577d e941010000               jmp        0x4d58c3
004d5782 e918010000               jmp        0x4d589f
004d5787 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d578a 81c290000000             add        edx, 0x90
004d5790 8955e8                   mov        dword ptr [ebp - 0x18], edx
004d5793 6a0a                     push       0xa
004d5795 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d5798 e8e36efaff               call       0x47c680
004d579d 8bf0                     mov        esi, eax
004d579f b9e4ef5a00               mov        ecx, 0x5aefe4
004d57a4 e8373bf5ff               call       0x4292e0
004d57a9 d95de4                   fstp       dword ptr [ebp - 0x1c]
004d57ac f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
004d57b1 f30f594610               mulss      xmm0, dword ptr [esi + 0x10]
004d57b6 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
004d57bb 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d57be f30f10407c               movss      xmm0, dword ptr [eax + 0x7c]
004d57c3 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
004d57c8 f30f1045e0               movss      xmm0, dword ptr [ebp - 0x20]
004d57cd f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
004d57d2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d57d5 f30f11417c               movss      dword ptr [ecx + 0x7c], xmm0
004d57da 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d57dd 81c290000000             add        edx, 0x90
004d57e3 8955d8                   mov        dword ptr [ebp - 0x28], edx
004d57e6 6a0a                     push       0xa
004d57e8 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004d57eb e8906efaff               call       0x47c680
004d57f0 83c024                   add        eax, 0x24
004d57f3 8945d0                   mov        dword ptr [ebp - 0x30], eax
004d57f6 b9e4ef5a00               mov        ecx, 0x5aefe4
004d57fb e8e03af5ff               call       0x4292e0
004d5800 d95dd4                   fstp       dword ptr [ebp - 0x2c]
004d5803 51                       push       ecx
004d5804 f30f1045d4               movss      xmm0, dword ptr [ebp - 0x2c]
004d5809 f30f110424               movss      dword ptr [esp], xmm0
004d580e 8d45b4                   lea        eax, [ebp - 0x4c]
004d5811 50                       push       eax
004d5812 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004d5815 e8d63af5ff               call       0x4292f0
004d581a 50                       push       eax
004d581b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d581e 83c164                   add        ecx, 0x64
004d5821 e8ba3ef5ff               call       0x4296e0
004d5826 90                       nop        
004d5827 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d582a 51                       push       ecx
004d582b f30f104164               movss      xmm0, dword ptr [ecx + 0x64]
004d5830 f30f110424               movss      dword ptr [esp], xmm0
004d5835 e8f64dfaff               call       0x47a630
004d583a 83c404                   add        esp, 4
004d583d d95dcc                   fstp       dword ptr [ebp - 0x34]
004d5840 f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
004d5845 0f2f0530fd5600           comiss     xmm0, dword ptr [0x56fd30]
004d584c 7727                     ja         0x4d5875
004d584e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d5851 51                       push       ecx
004d5852 f30f104268               movss      xmm0, dword ptr [edx + 0x68]
004d5857 f30f110424               movss      dword ptr [esp], xmm0
004d585c e8cf4dfaff               call       0x47a630
004d5861 83c404                   add        esp, 4
004d5864 d95dc8                   fstp       dword ptr [ebp - 0x38]
004d5867 f30f1045c8               movss      xmm0, dword ptr [ebp - 0x38]
004d586c 0f2f0530fd5600           comiss     xmm0, dword ptr [0x56fd30]
004d5873 762a                     jbe        0x4d589f
004d5875 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d5878 51                       push       ecx
004d5879 f30f104064               movss      xmm0, dword ptr [eax + 0x64]
004d587e f30f110424               movss      dword ptr [esp], xmm0
004d5883 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d5886 51                       push       ecx
004d5887 f30f104168               movss      xmm0, dword ptr [ecx + 0x68]
004d588c f30f110424               movss      dword ptr [esp], xmm0
004d5891 e8ea39f8ff               call       0x459280
004d5896 83c408                   add        esp, 8
004d5899 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d589c d95a70                   fstp       dword ptr [edx + 0x70]
004d589f 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d58a2 0590000000               add        eax, 0x90
004d58a7 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004d58aa 6a0a                     push       0xa
004d58ac 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004d58af e8cc6dfaff               call       0x47c680
004d58b4 8945c0                   mov        dword ptr [ebp - 0x40], eax
004d58b7 6a00                     push       0
004d58b9 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004d58bc e87fdcf4ff               call       0x423540
004d58c1 33c0                     xor        eax, eax
004d58c3 5e                       pop        esi
004d58c4 8be5                     mov        esp, ebp
004d58c6 5d                       pop        ebp
004d58c7 c3                       ret        
004d58c8 cc                       int3       
004d58c9 cc                       int3       
004d58ca cc                       int3       
004d58cb cc                       int3       
004d58cc cc                       int3       
004d58cd cc                       int3       
004d58ce cc                       int3       
004d58cf cc                       int3       
