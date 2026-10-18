0045e800 53                       push       ebx
0045e801 8bdc                     mov        ebx, esp
0045e803 83ec08                   sub        esp, 8
0045e806 83e4f8                   and        esp, 0xfffffff8
0045e809 83c404                   add        esp, 4
0045e80c 55                       push       ebp
0045e80d 8b6b04                   mov        ebp, dword ptr [ebx + 4]
0045e810 896c2404                 mov        dword ptr [esp + 4], ebp
0045e814 8bec                     mov        ebp, esp
0045e816 83ec58                   sub        esp, 0x58
0045e819 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0045e81e 33c5                     xor        eax, ebp
0045e820 8945fc                   mov        dword ptr [ebp - 4], eax
0045e823 894df0                   mov        dword ptr [ebp - 0x10], ecx
0045e826 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0045e829 05d0020000               add        eax, 0x2d0
0045e82e 8945e0                   mov        dword ptr [ebp - 0x20], eax
0045e831 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0045e834 e867d5faff               call       0x40bda0
0045e839 8945ec                   mov        dword ptr [ebp - 0x14], eax
0045e83c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0045e83f e85cf4ffff               call       0x45dca0
0045e844 8945d8                   mov        dword ptr [ebp - 0x28], eax
0045e847 eb09                     jmp        0x45e852
0045e849 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0045e84c 83c104                   add        ecx, 4
0045e84f 894dec                   mov        dword ptr [ebp - 0x14], ecx
0045e852 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0045e855 3b55d8                   cmp        edx, dword ptr [ebp - 0x28]
0045e858 0f849f000000             je         0x45e8fd
0045e85e 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0045e861 8945f4                   mov        dword ptr [ebp - 0xc], eax
0045e864 b9c4a45b00               mov        ecx, 0x5ba4c4
0045e869 e872b0fcff               call       0x4298e0
0045e86e d95dd4                   fstp       dword ptr [ebp - 0x2c]
0045e871 f30f1045d4               movss      xmm0, dword ptr [ebp - 0x2c]
0045e876 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
0045e87e f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
0045e883 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045e886 f30f1001                 movss      xmm0, dword ptr [ecx]
0045e88a f30f1145d0               movss      dword ptr [ebp - 0x30], xmm0
0045e88f f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
0045e894 f30f5845cc               addss      xmm0, dword ptr [ebp - 0x34]
0045e899 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0045e89c f30f1102                 movss      dword ptr [edx], xmm0
0045e8a0 f30f104308               movss      xmm0, dword ptr [ebx + 8]
0045e8a5 f30f5805e0c85600         addss      xmm0, dword ptr [0x56c8e0]
0045e8ad 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0045e8b0 f30f1008                 movss      xmm1, dword ptr [eax]
0045e8b4 0f2fc8                   comiss     xmm1, xmm0
0045e8b7 7616                     jbe        0x45e8cf
0045e8b9 f30f104308               movss      xmm0, dword ptr [ebx + 8]
0045e8be f30f5805e0c85600         addss      xmm0, dword ptr [0x56c8e0]
0045e8c6 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045e8c9 f30f1101                 movss      dword ptr [ecx], xmm0
0045e8cd eb29                     jmp        0x45e8f8
0045e8cf f30f104308               movss      xmm0, dword ptr [ebx + 8]
0045e8d4 f30f5c05e0c85600         subss      xmm0, dword ptr [0x56c8e0]
0045e8dc 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0045e8df 0f2f02                   comiss     xmm0, dword ptr [edx]
0045e8e2 7614                     jbe        0x45e8f8
0045e8e4 f30f104308               movss      xmm0, dword ptr [ebx + 8]
0045e8e9 f30f5c05e0c85600         subss      xmm0, dword ptr [0x56c8e0]
0045e8f1 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0045e8f4 f30f1100                 movss      dword ptr [eax], xmm0
0045e8f8 e94cffffff               jmp        0x45e849
0045e8fd c745e400000000           mov        dword ptr [ebp - 0x1c], 0
0045e904 51                       push       ecx
0045e905 0f57c0                   xorps      xmm0, xmm0
0045e908 f30f110424               movss      dword ptr [esp], xmm0
0045e90d 8d4df8                   lea        ecx, [ebp - 8]
0045e910 e8fba8fcff               call       0x429210
0045e915 90                       nop        
0045e916 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0045e919 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0045e91c 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0045e91f e87cd4faff               call       0x40bda0
0045e924 8945e8                   mov        dword ptr [ebp - 0x18], eax
0045e927 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0045e92a e891f3ffff               call       0x45dcc0
0045e92f 8945c8                   mov        dword ptr [ebp - 0x38], eax
0045e932 eb09                     jmp        0x45e93d
0045e934 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0045e937 83c208                   add        edx, 8
0045e93a 8955e8                   mov        dword ptr [ebp - 0x18], edx
0045e93d 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0045e940 3b45c8                   cmp        eax, dword ptr [ebp - 0x38]
0045e943 0f8488000000             je         0x45e9d1
0045e949 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0045e94c 894db8                   mov        dword ptr [ebp - 0x48], ecx
0045e94f 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0045e952 81c2d0020000             add        edx, 0x2d0
0045e958 8955c4                   mov        dword ptr [ebp - 0x3c], edx
0045e95b 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0045e95e 50                       push       eax
0045e95f 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
0045e962 e8195cfbff               call       0x414580
0045e967 f30f1000                 movss      xmm0, dword ptr [eax]
0045e96b f30f1145c0               movss      dword ptr [ebp - 0x40], xmm0
0045e970 8d4df8                   lea        ecx, [ebp - 8]
0045e973 e868a9fcff               call       0x4292e0
0045e978 d95dbc                   fstp       dword ptr [ebp - 0x44]
0045e97b 51                       push       ecx
0045e97c f30f1045c0               movss      xmm0, dword ptr [ebp - 0x40]
0045e981 f30f110424               movss      dword ptr [esp], xmm0
0045e986 51                       push       ecx
0045e987 f30f1045bc               movss      xmm0, dword ptr [ebp - 0x44]
0045e98c f30f110424               movss      dword ptr [esp], xmm0
0045e991 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
0045e994 51                       push       ecx
0045e995 e896a9fdff               call       0x439330
0045e99a 83c40c                   add        esp, 0xc
0045e99d f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
0045e9a5 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
0045e9ad f30f5e0548ee5600         divss      xmm0, dword ptr [0x56ee48]
0045e9b5 51                       push       ecx
0045e9b6 f30f110424               movss      dword ptr [esp], xmm0
0045e9bb 8d4df8                   lea        ecx, [ebp - 8]
0045e9be e8fd46ffff               call       0x4530c0
0045e9c3 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0045e9c6 83c201                   add        edx, 1
0045e9c9 8955e4                   mov        dword ptr [ebp - 0x1c], edx
0045e9cc e963ffffff               jmp        0x45e934
0045e9d1 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0045e9d4 8945b4                   mov        dword ptr [ebp - 0x4c], eax
0045e9d7 6a00                     push       0
0045e9d9 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
0045e9dc e88fecffff               call       0x45d670
0045e9e1 8b08                     mov        ecx, dword ptr [eax]
0045e9e3 8b5004                   mov        edx, dword ptr [eax + 4]
0045e9e6 894da8                   mov        dword ptr [ebp - 0x58], ecx
0045e9e9 8955ac                   mov        dword ptr [ebp - 0x54], edx
0045e9ec 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0045e9ef 8945b0                   mov        dword ptr [ebp - 0x50], eax
0045e9f2 6a3b                     push       0x3b
0045e9f4 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0045e9f7 e874ecffff               call       0x45d670
0045e9fc 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0045e9ff 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
0045ea02 8908                     mov        dword ptr [eax], ecx
0045ea04 895004                   mov        dword ptr [eax + 4], edx
0045ea07 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045ea0a 33cd                     xor        ecx, ebp
0045ea0c e8dd3f0e00               call       0x5429ee
0045ea11 8be5                     mov        esp, ebp
0045ea13 5d                       pop        ebp
0045ea14 8be3                     mov        esp, ebx
0045ea16 5b                       pop        ebx
0045ea17 c20400                   ret        4
0045ea1a cc                       int3       
0045ea1b cc                       int3       
0045ea1c cc                       int3       
0045ea1d cc                       int3       
0045ea1e cc                       int3       
0045ea1f cc                       int3       
