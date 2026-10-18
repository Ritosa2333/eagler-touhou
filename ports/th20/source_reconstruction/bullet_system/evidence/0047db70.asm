0047db70 55                       push       ebp
0047db71 8bec                     mov        ebp, esp
0047db73 83ec14                   sub        esp, 0x14
0047db76 894df8                   mov        dword ptr [ebp - 8], ecx
0047db79 8b4508                   mov        eax, dword ptr [ebp + 8]
0047db7c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0047db7f 8b548114                 mov        edx, dword ptr [ecx + eax*4 + 0x14]
0047db83 8955fc                   mov        dword ptr [ebp - 4], edx
0047db86 837dfc00                 cmp        dword ptr [ebp - 4], 0
0047db8a 0f84e6000000             je         0x47dc76
0047db90 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047db93 83c064                   add        eax, 0x64
0047db96 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047db99 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
0047db9c 81c2bc050000             add        edx, 0x5bc
0047dba2 8b08                     mov        ecx, dword ptr [eax]
0047dba4 890a                     mov        dword ptr [edx], ecx
0047dba6 8b4804                   mov        ecx, dword ptr [eax + 4]
0047dba9 894a04                   mov        dword ptr [edx + 4], ecx
0047dbac 8b4008                   mov        eax, dword ptr [eax + 8]
0047dbaf 894208                   mov        dword ptr [edx + 8], eax
0047dbb2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047dbb5 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
0047dbb8 8a82a2040000             mov        al, byte ptr [edx + 0x4a2]
0047dbbe c0e805                   shr        al, 5
0047dbc1 2407                     and        al, 7
0047dbc3 0fb6c8                   movzx      ecx, al
0047dbc6 85c9                     test       ecx, ecx
0047dbc8 7450                     je         0x47dc1a
0047dbca 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047dbcd 8b421c                   mov        eax, dword ptr [edx + 0x1c]
0047dbd0 8945f0                   mov        dword ptr [ebp - 0x10], eax
0047dbd3 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
0047dbdb f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0047dbe3 51                       push       ecx
0047dbe4 f30f110424               movss      dword ptr [esp], xmm0
0047dbe9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047dbec 81c184000000             add        ecx, 0x84
0047dbf2 e8e9b6faff               call       0x4292e0
0047dbf7 51                       push       ecx
0047dbf8 d91c24                   fstp       dword ptr [esp]
0047dbfb e800aafbff               call       0x438600
0047dc00 83c408                   add        esp, 8
0047dc03 d95df4                   fstp       dword ptr [ebp - 0xc]
0047dc06 51                       push       ecx
0047dc07 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
0047dc0c f30f110424               movss      dword ptr [esp], xmm0
0047dc11 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047dc14 e87729fdff               call       0x450590
0047dc19 90                       nop        
0047dc1a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047dc1d 8b5114                   mov        edx, dword ptr [ecx + 0x14]
0047dc20 c1ea06                   shr        edx, 6
0047dc23 83e201                   and        edx, 1
0047dc26 742e                     je         0x47dc56
0047dc28 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047dc2b 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
0047dc2e 894dec                   mov        dword ptr [ebp - 0x14], ecx
0047dc31 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047dc34 51                       push       ecx
0047dc35 f30f104248               movss      xmm0, dword ptr [edx + 0x48]
0047dc3a f30f110424               movss      dword ptr [esp], xmm0
0047dc3f 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047dc42 51                       push       ecx
0047dc43 f30f104048               movss      xmm0, dword ptr [eax + 0x48]
0047dc48 f30f110424               movss      dword ptr [esp], xmm0
0047dc4d 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047dc50 e89b880000               call       0x4864f0
0047dc55 90                       nop        
0047dc56 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047dc59 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
0047dc5c 52                       push       edx
0047dc5d 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0047dc63 e8185cfcff               call       0x443880
0047dc68 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047dc6b 8b4860                   mov        ecx, dword ptr [eax + 0x60]
0047dc6e 894dfc                   mov        dword ptr [ebp - 4], ecx
0047dc71 e910ffffff               jmp        0x47db86
0047dc76 b801000000               mov        eax, 1
0047dc7b 8be5                     mov        esp, ebp
0047dc7d 5d                       pop        ebp
0047dc7e c20400                   ret        4
0047dc81 cc                       int3       
0047dc82 cc                       int3       
0047dc83 cc                       int3       
0047dc84 cc                       int3       
0047dc85 cc                       int3       
0047dc86 cc                       int3       
0047dc87 cc                       int3       
0047dc88 cc                       int3       
0047dc89 cc                       int3       
0047dc8a cc                       int3       
0047dc8b cc                       int3       
0047dc8c cc                       int3       
0047dc8d cc                       int3       
0047dc8e cc                       int3       
0047dc8f cc                       int3       
