00511cf0 55                       push       ebp
00511cf1 8bec                     mov        ebp, esp
00511cf3 83ec20                   sub        esp, 0x20
00511cf6 894dfc                   mov        dword ptr [ebp - 4], ecx
00511cf9 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511cfc 83782807                 cmp        dword ptr [eax + 0x28], 7
00511d00 7526                     jne        0x511d28
00511d02 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511d05 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511d08 f30f1001                 movss      xmm0, dword ptr [ecx]
00511d0c f30f584204               addss      xmm0, dword ptr [edx + 4]
00511d11 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511d14 f30f1100                 movss      dword ptr [eax], xmm0
00511d18 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511d1b 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511d1e 8b02                     mov        eax, dword ptr [edx]
00511d20 894110                   mov        dword ptr [ecx + 0x10], eax
00511d23 e99b010000               jmp        0x511ec3
00511d28 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511d2b 83792811                 cmp        dword ptr [ecx + 0x28], 0x11
00511d2f 753e                     jne        0x511d6f
00511d31 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511d34 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511d37 f30f1002                 movss      xmm0, dword ptr [edx]
00511d3b f30f58400c               addss      xmm0, dword ptr [eax + 0xc]
00511d40 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511d43 f30f1101                 movss      dword ptr [ecx], xmm0
00511d47 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511d4a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511d4d f30f10420c               movss      xmm0, dword ptr [edx + 0xc]
00511d52 f30f584004               addss      xmm0, dword ptr [eax + 4]
00511d57 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511d5a f30f11410c               movss      dword ptr [ecx + 0xc], xmm0
00511d5f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511d62 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511d65 8b08                     mov        ecx, dword ptr [eax]
00511d67 894a10                   mov        dword ptr [edx + 0x10], ecx
00511d6a e954010000               jmp        0x511ec3
00511d6f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511d72 837a2808                 cmp        dword ptr [edx + 0x28], 8
00511d76 0f8519010000             jne        0x511e95
00511d7c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511d7f 83c114                   add        ecx, 0x14
00511d82 e8491ef1ff               call       0x423bd0
00511d87 d95df4                   fstp       dword ptr [ebp - 0xc]
00511d8a f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00511d8f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511d92 f30f2a4824               cvtsi2ss   xmm1, dword ptr [eax + 0x24]
00511d97 f30f5ec1                 divss      xmm0, xmm1
00511d9b f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00511da0 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00511da5 f30f5c05ccc85600         subss      xmm0, dword ptr [0x56c8cc]
00511dad f30f104df8               movss      xmm1, dword ptr [ebp - 8]
00511db2 f30f5c0dccc85600         subss      xmm1, dword ptr [0x56c8cc]
00511dba f30f59c1                 mulss      xmm0, xmm1
00511dbe f30f100dd0c85600         movss      xmm1, dword ptr [0x56c8d0]
00511dc6 f30f594df8               mulss      xmm1, dword ptr [ebp - 8]
00511dcb f30f580dccc85600         addss      xmm1, dword ptr [0x56c8cc]
00511dd3 f30f59c1                 mulss      xmm0, xmm1
00511dd7 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00511ddc f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00511de1 f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
00511de6 f30f100dd0c85600         movss      xmm1, dword ptr [0x56c8d0]
00511dee f30f594df8               mulss      xmm1, dword ptr [ebp - 8]
00511df3 f30f1015d8c85600         movss      xmm2, dword ptr [0x56c8d8]
00511dfb f30f5cd1                 subss      xmm2, xmm1
00511dff f30f59c2                 mulss      xmm0, xmm2
00511e03 f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
00511e08 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00511e10 f30f5c45f8               subss      xmm0, dword ptr [ebp - 8]
00511e15 f30f100dccc85600         movss      xmm1, dword ptr [0x56c8cc]
00511e1d f30f5c4df8               subss      xmm1, dword ptr [ebp - 8]
00511e22 f30f59c1                 mulss      xmm0, xmm1
00511e26 f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
00511e2b f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
00511e30 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00511e35 f30f5c05ccc85600         subss      xmm0, dword ptr [0x56c8cc]
00511e3d f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
00511e42 f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
00511e47 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00511e4c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511e4f f30f1001                 movss      xmm0, dword ptr [ecx]
00511e53 f30f5945f0               mulss      xmm0, dword ptr [ebp - 0x10]
00511e58 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511e5b f30f104a04               movss      xmm1, dword ptr [edx + 4]
00511e60 f30f594dec               mulss      xmm1, dword ptr [ebp - 0x14]
00511e65 f30f58c1                 addss      xmm0, xmm1
00511e69 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511e6c f30f104808               movss      xmm1, dword ptr [eax + 8]
00511e71 f30f594de8               mulss      xmm1, dword ptr [ebp - 0x18]
00511e76 f30f58c1                 addss      xmm0, xmm1
00511e7a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511e7d f30f10490c               movss      xmm1, dword ptr [ecx + 0xc]
00511e82 f30f594de4               mulss      xmm1, dword ptr [ebp - 0x1c]
00511e87 f30f58c1                 addss      xmm0, xmm1
00511e8b 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511e8e f30f114210               movss      dword ptr [edx + 0x10], xmm0
00511e93 eb2e                     jmp        0x511ec3
00511e95 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511e98 e8a356f2ff               call       0x437540
00511e9d d95de0                   fstp       dword ptr [ebp - 0x20]
00511ea0 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511ea3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511ea6 f30f104004               movss      xmm0, dword ptr [eax + 4]
00511eab f30f5c01                 subss      xmm0, dword ptr [ecx]
00511eaf f30f5945e0               mulss      xmm0, dword ptr [ebp - 0x20]
00511eb4 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511eb7 f30f5802                 addss      xmm0, dword ptr [edx]
00511ebb 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511ebe f30f114010               movss      dword ptr [eax + 0x10], xmm0
00511ec3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511ec6 d94110                   fld        dword ptr [ecx + 0x10]
00511ec9 8be5                     mov        esp, ebp
00511ecb 5d                       pop        ebp
00511ecc c3                       ret        
00511ecd cc                       int3       
00511ece cc                       int3       
00511ecf cc                       int3       
