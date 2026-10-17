00453e40 55                       push       ebp
00453e41 8bec                     mov        ebp, esp
00453e43 83ec5c                   sub        esp, 0x5c
00453e46 894dfc                   mov        dword ptr [ebp - 4], ecx
00453e49 8b45fc                   mov        eax, dword ptr [ebp - 4]
00453e4c 8b4844                   mov        ecx, dword ptr [eax + 0x44]
00453e4f c1e905                   shr        ecx, 5
00453e52 83e101                   and        ecx, 1
00453e55 7405                     je         0x453e5c
00453e57 e996020000               jmp        0x4540f2
00453e5c 8b55fc                   mov        edx, dword ptr [ebp - 4]
00453e5f 8b4244                   mov        eax, dword ptr [edx + 0x44]
00453e62 83e00f                   and        eax, 0xf
00453e65 8945f4                   mov        dword ptr [ebp - 0xc], eax
00453e68 837df404                 cmp        dword ptr [ebp - 0xc], 4
00453e6c 0f8780020000             ja         0x4540f2
00453e72 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00453e75 ff248df8404500           jmp        dword ptr [ecx*4 + 0x4540f8]
00453e7c b9e4ef5a00               mov        ecx, 0x5aefe4
00453e81 e85a54fdff               call       0x4292e0
00453e86 d95df0                   fstp       dword ptr [ebp - 0x10]
00453e89 8b55fc                   mov        edx, dword ptr [ebp - 4]
00453e8c f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00453e91 f30f594218               mulss      xmm0, dword ptr [edx + 0x18]
00453e96 f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
00453e9b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00453e9e 83c11c                   add        ecx, 0x1c
00453ea1 e83a54fdff               call       0x4292e0
00453ea6 d95de8                   fstp       dword ptr [ebp - 0x18]
00453ea9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00453eac e80f240000               call       0x4562c0
00453eb1 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00453eb4 51                       push       ecx
00453eb5 f30f1045ec               movss      xmm0, dword ptr [ebp - 0x14]
00453eba f30f110424               movss      dword ptr [esp], xmm0
00453ebf 51                       push       ecx
00453ec0 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00453ec5 f30f110424               movss      dword ptr [esp], xmm0
00453eca 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
00453ecd 50                       push       eax
00453ece e85d54feff               call       0x439330
00453ed3 83c40c                   add        esp, 0xc
00453ed6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00453ed9 f30f104134               movss      xmm0, dword ptr [ecx + 0x34]
00453ede 0f2f05e8e05600           comiss     xmm0, dword ptr [0x56e0e8]
00453ee5 7630                     jbe        0x453f17
00453ee7 8b55fc                   mov        edx, dword ptr [ebp - 4]
00453eea 83c238                   add        edx, 0x38
00453eed 8955e0                   mov        dword ptr [ebp - 0x20], edx
00453ef0 8b45fc                   mov        eax, dword ptr [ebp - 4]
00453ef3 51                       push       ecx
00453ef4 f30f104034               movss      xmm0, dword ptr [eax + 0x34]
00453ef9 f30f110424               movss      dword ptr [esp], xmm0
00453efe 8d4da4                   lea        ecx, [ebp - 0x5c]
00453f01 51                       push       ecx
00453f02 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00453f05 e8e653fdff               call       0x4292f0
00453f0a 50                       push       eax
00453f0b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00453f0e 83c138                   add        ecx, 0x38
00453f11 e82a58fdff               call       0x429740
00453f16 90                       nop        
00453f17 8b55fc                   mov        edx, dword ptr [ebp - 4]
00453f1a 8b4244                   mov        eax, dword ptr [edx + 0x44]
00453f1d c1e804                   shr        eax, 4
00453f20 83e001                   and        eax, 1
00453f23 7464                     je         0x453f89
00453f25 b9e4ef5a00               mov        ecx, 0x5aefe4
00453f2a e8b153fdff               call       0x4292e0
00453f2f d95ddc                   fstp       dword ptr [ebp - 0x24]
00453f32 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00453f35 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00453f3a f30f594124               mulss      xmm0, dword ptr [ecx + 0x24]
00453f3f f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00453f44 8b55fc                   mov        edx, dword ptr [ebp - 4]
00453f47 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00453f4c f30f594234               mulss      xmm0, dword ptr [edx + 0x34]
00453f51 f30f104df8               movss      xmm1, dword ptr [ebp - 8]
00453f56 f30f5cc8                 subss      xmm1, xmm0
00453f5a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00453f5d 83c11c                   add        ecx, 0x1c
00453f60 f30f114dd8               movss      dword ptr [ebp - 0x28], xmm1
00453f65 e87653fdff               call       0x4292e0
00453f6a d95dd4                   fstp       dword ptr [ebp - 0x2c]
00453f6d f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
00453f72 f30f5845d4               addss      xmm0, dword ptr [ebp - 0x2c]
00453f77 51                       push       ecx
00453f78 f30f110424               movss      dword ptr [esp], xmm0
00453f7d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00453f80 83c11c                   add        ecx, 0x1c
00453f83 e898efffff               call       0x452f20
00453f88 90                       nop        
00453f89 e964010000               jmp        0x4540f2
00453f8e e95f010000               jmp        0x4540f2
00453f93 b9e4ef5a00               mov        ecx, 0x5aefe4
00453f98 e84353fdff               call       0x4292e0
00453f9d d95dd0                   fstp       dword ptr [ebp - 0x30]
00453fa0 8b45fc                   mov        eax, dword ptr [ebp - 4]
00453fa3 f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
00453fa8 f30f594024               mulss      xmm0, dword ptr [eax + 0x24]
00453fad f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00453fb2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00453fb5 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00453fba f30f594134               mulss      xmm0, dword ptr [ecx + 0x34]
00453fbf f30f104df8               movss      xmm1, dword ptr [ebp - 8]
00453fc4 f30f5cc8                 subss      xmm1, xmm0
00453fc8 8b55fc                   mov        edx, dword ptr [ebp - 4]
00453fcb f30f584a20               addss      xmm1, dword ptr [edx + 0x20]
00453fd0 8b45fc                   mov        eax, dword ptr [ebp - 4]
00453fd3 f30f114820               movss      dword ptr [eax + 0x20], xmm1
00453fd8 b9e4ef5a00               mov        ecx, 0x5aefe4
00453fdd e8fe52fdff               call       0x4292e0
00453fe2 d95dcc                   fstp       dword ptr [ebp - 0x34]
00453fe5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00453fe8 f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
00453fed f30f594118               mulss      xmm0, dword ptr [ecx + 0x18]
00453ff2 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00453ff7 8b55fc                   mov        edx, dword ptr [ebp - 4]
00453ffa f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00453fff f30f594234               mulss      xmm0, dword ptr [edx + 0x34]
00454004 f30f104df8               movss      xmm1, dword ptr [ebp - 8]
00454009 f30f5cc8                 subss      xmm1, xmm0
0045400d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00454010 83c11c                   add        ecx, 0x1c
00454013 f30f114dc8               movss      dword ptr [ebp - 0x38], xmm1
00454018 e8c352fdff               call       0x4292e0
0045401d d95dc4                   fstp       dword ptr [ebp - 0x3c]
00454020 f30f1045c8               movss      xmm0, dword ptr [ebp - 0x38]
00454025 f30f5845c4               addss      xmm0, dword ptr [ebp - 0x3c]
0045402a 51                       push       ecx
0045402b f30f110424               movss      dword ptr [esp], xmm0
00454030 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00454033 83c11c                   add        ecx, 0x1c
00454036 e8e5eeffff               call       0x452f20
0045403b 90                       nop        
0045403c e9b1000000               jmp        0x4540f2
00454041 b9e4ef5a00               mov        ecx, 0x5aefe4
00454046 e89552fdff               call       0x4292e0
0045404b d95dc0                   fstp       dword ptr [ebp - 0x40]
0045404e 8b45fc                   mov        eax, dword ptr [ebp - 4]
00454051 f30f1045c0               movss      xmm0, dword ptr [ebp - 0x40]
00454056 f30f594024               mulss      xmm0, dword ptr [eax + 0x24]
0045405b f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00454060 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00454063 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00454068 f30f594134               mulss      xmm0, dword ptr [ecx + 0x34]
0045406d f30f104df8               movss      xmm1, dword ptr [ebp - 8]
00454072 f30f5cc8                 subss      xmm1, xmm0
00454076 51                       push       ecx
00454077 f30f110c24               movss      dword ptr [esp], xmm1
0045407c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045407f 83c130                   add        ecx, 0x30
00454082 e839f0ffff               call       0x4530c0
00454087 b9e4ef5a00               mov        ecx, 0x5aefe4
0045408c e84f52fdff               call       0x4292e0
00454091 d95dbc                   fstp       dword ptr [ebp - 0x44]
00454094 8b55fc                   mov        edx, dword ptr [ebp - 4]
00454097 f30f1045bc               movss      xmm0, dword ptr [ebp - 0x44]
0045409c f30f594218               mulss      xmm0, dword ptr [edx + 0x18]
004540a1 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
004540a6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004540a9 83c128                   add        ecx, 0x28
004540ac e82f52fdff               call       0x4292e0
004540b1 d95db4                   fstp       dword ptr [ebp - 0x4c]
004540b4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004540b7 e804220000               call       0x4562c0
004540bc 8945b0                   mov        dword ptr [ebp - 0x50], eax
004540bf 51                       push       ecx
004540c0 f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
004540c5 f30f110424               movss      dword ptr [esp], xmm0
004540ca 51                       push       ecx
004540cb f30f1045b4               movss      xmm0, dword ptr [ebp - 0x4c]
004540d0 f30f110424               movss      dword ptr [esp], xmm0
004540d5 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
004540d8 50                       push       eax
004540d9 e85252feff               call       0x439330
004540de 83c408                   add        esp, 8
004540e1 0f57c0                   xorps      xmm0, xmm0
004540e4 f30f110424               movss      dword ptr [esp], xmm0
004540e9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004540ec e8ff500000               call       0x4591f0
004540f1 90                       nop        
004540f2 8be5                     mov        esp, ebp
004540f4 5d                       pop        ebp
004540f5 c3                       ret        
004540f6 6690                     nop        
004540f8 7c3e                     jl         0x454138
004540fa 45                       inc        ebp
004540fb 00f2                     add        dl, dh
004540fd 40                       inc        eax
004540fe 45                       inc        ebp
004540ff 00933f450093             add        byte ptr [ebx - 0x6cffbac1], dl
00454105 3f                       aas        
00454106 45                       inc        ebp
00454107 004140                   add        byte ptr [ecx + 0x40], al
0045410a 45                       inc        ebp
0045410b 00cc                     add        ah, cl
0045410d cc                       int3       
0045410e cc                       int3       
0045410f cc                       int3       
