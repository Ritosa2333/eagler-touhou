00483f00 55                       push       ebp
00483f01 8bec                     mov        ebp, esp
00483f03 83ec4c                   sub        esp, 0x4c
00483f06 56                       push       esi
00483f07 894dfc                   mov        dword ptr [ebp - 4], ecx
00483f0a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483f0d 05a0000000               add        eax, 0xa0
00483f12 8945f8                   mov        dword ptr [ebp - 8], eax
00483f15 6a01                     push       1
00483f17 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00483f1a e86187ffff               call       0x47c680
00483f1f 8945ec                   mov        dword ptr [ebp - 0x14], eax
00483f22 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483f25 81c1a0000000             add        ecx, 0xa0
00483f2b 894df4                   mov        dword ptr [ebp - 0xc], ecx
00483f2e 6a01                     push       1
00483f30 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00483f33 e84887ffff               call       0x47c680
00483f38 8b5030                   mov        edx, dword ptr [eax + 0x30]
00483f3b 8955f0                   mov        dword ptr [ebp - 0x10], edx
00483f3e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00483f41 50                       push       eax
00483f42 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00483f45 e876f6f9ff               call       0x4235c0
00483f4a 0fb6c8                   movzx      ecx, al
00483f4d 85c9                     test       ecx, ecx
00483f4f 7424                     je         0x483f75
00483f51 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483f54 8b8290000000             mov        eax, dword ptr [edx + 0x90]
00483f5a 83e0fb                   and        eax, 0xfffffffb
00483f5d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483f60 898190000000             mov        dword ptr [ecx + 0x90], eax
00483f66 b801000000               mov        eax, 1
00483f6b e962010000               jmp        0x4840d2
00483f70 e938010000               jmp        0x4840ad
00483f75 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483f78 81c2a0000000             add        edx, 0xa0
00483f7e 8955e8                   mov        dword ptr [ebp - 0x18], edx
00483f81 6a01                     push       1
00483f83 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00483f86 e8f586ffff               call       0x47c680
00483f8b 8bf0                     mov        esi, eax
00483f8d b9e4ef5a00               mov        ecx, 0x5aefe4
00483f92 e84953faff               call       0x4292e0
00483f97 d95de4                   fstp       dword ptr [ebp - 0x1c]
00483f9a f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00483f9f f30f594610               mulss      xmm0, dword ptr [esi + 0x10]
00483fa4 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00483fa9 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483fac f30f104020               movss      xmm0, dword ptr [eax + 0x20]
00483fb1 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
00483fb6 f30f1045e0               movss      xmm0, dword ptr [ebp - 0x20]
00483fbb f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
00483fc0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483fc3 f30f114120               movss      dword ptr [ecx + 0x20], xmm0
00483fc8 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483fcb 81c2a0000000             add        edx, 0xa0
00483fd1 8955d8                   mov        dword ptr [ebp - 0x28], edx
00483fd4 6a01                     push       1
00483fd6 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00483fd9 e8a286ffff               call       0x47c680
00483fde 83c024                   add        eax, 0x24
00483fe1 8945d0                   mov        dword ptr [ebp - 0x30], eax
00483fe4 b9e4ef5a00               mov        ecx, 0x5aefe4
00483fe9 e8f252faff               call       0x4292e0
00483fee d95dd4                   fstp       dword ptr [ebp - 0x2c]
00483ff1 51                       push       ecx
00483ff2 f30f1045d4               movss      xmm0, dword ptr [ebp - 0x2c]
00483ff7 f30f110424               movss      dword ptr [esp], xmm0
00483ffc 8d45b4                   lea        eax, [ebp - 0x4c]
00483fff 50                       push       eax
00484000 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00484003 e8e852faff               call       0x4292f0
00484008 50                       push       eax
00484009 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048400c 83c170                   add        ecx, 0x70
0048400f e8cc56faff               call       0x4296e0
00484014 90                       nop        
00484015 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00484018 51                       push       ecx
00484019 f30f104170               movss      xmm0, dword ptr [ecx + 0x70]
0048401e f30f110424               movss      dword ptr [esp], xmm0
00484023 e80866ffff               call       0x47a630
00484028 83c404                   add        esp, 4
0048402b d95dcc                   fstp       dword ptr [ebp - 0x34]
0048402e f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
00484033 0f2f0530fd5600           comiss     xmm0, dword ptr [0x56fd30]
0048403a 7727                     ja         0x484063
0048403c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048403f 51                       push       ecx
00484040 f30f104274               movss      xmm0, dword ptr [edx + 0x74]
00484045 f30f110424               movss      dword ptr [esp], xmm0
0048404a e8e165ffff               call       0x47a630
0048404f 83c404                   add        esp, 4
00484052 d95dc8                   fstp       dword ptr [ebp - 0x38]
00484055 f30f1045c8               movss      xmm0, dword ptr [ebp - 0x38]
0048405a 0f2f0530fd5600           comiss     xmm0, dword ptr [0x56fd30]
00484061 764a                     jbe        0x4840ad
00484063 8b45fc                   mov        eax, dword ptr [ebp - 4]
00484066 51                       push       ecx
00484067 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
0048406c f30f110424               movss      dword ptr [esp], xmm0
00484071 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00484074 51                       push       ecx
00484075 f30f104174               movss      xmm0, dword ptr [ecx + 0x74]
0048407a f30f110424               movss      dword ptr [esp], xmm0
0048407f e8fc51fdff               call       0x459280
00484084 83c404                   add        esp, 4
00484087 d91c24                   fstp       dword ptr [esp]
0048408a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048408d 81c184000000             add        ecx, 0x84
00484093 e888eefcff               call       0x452f20
00484098 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048409b 83c270                   add        edx, 0x70
0048409e 52                       push       edx
0048409f e8fc61ffff               call       0x47a2a0
004840a4 83c404                   add        esp, 4
004840a7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004840aa d95820                   fstp       dword ptr [eax + 0x20]
004840ad 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004840b0 81c1a0000000             add        ecx, 0xa0
004840b6 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
004840b9 6a01                     push       1
004840bb 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004840be e8bd85ffff               call       0x47c680
004840c3 8945c0                   mov        dword ptr [ebp - 0x40], eax
004840c6 6a00                     push       0
004840c8 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004840cb e870f4f9ff               call       0x423540
004840d0 33c0                     xor        eax, eax
004840d2 5e                       pop        esi
004840d3 8be5                     mov        esp, ebp
004840d5 5d                       pop        ebp
004840d6 c3                       ret        
004840d7 cc                       int3       
004840d8 cc                       int3       
004840d9 cc                       int3       
004840da cc                       int3       
004840db cc                       int3       
004840dc cc                       int3       
004840dd cc                       int3       
004840de cc                       int3       
004840df cc                       int3       
