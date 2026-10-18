00471f50 55                       push       ebp
00471f51 8bec                     mov        ebp, esp
00471f53 51                       push       ecx
00471f54 56                       push       esi
00471f55 894dfc                   mov        dword ptr [ebp - 4], ecx
00471f58 b804000000               mov        eax, 4
00471f5d 6bc803                   imul       ecx, eax, 3
00471f60 ba04000000               mov        edx, 4
00471f65 6bc203                   imul       eax, edx, 3
00471f68 8b55fc                   mov        edx, dword ptr [ebp - 4]
00471f6b 8b750c                   mov        esi, dword ptr [ebp + 0xc]
00471f6e f30f10440a08             movss      xmm0, dword ptr [edx + ecx + 8]
00471f74 f30f58440608             addss      xmm0, dword ptr [esi + eax + 8]
00471f7a 51                       push       ecx
00471f7b f30f110424               movss      dword ptr [esp], xmm0
00471f80 b804000000               mov        eax, 4
00471f85 d1e0                     shl        eax, 1
00471f87 b904000000               mov        ecx, 4
00471f8c d1e1                     shl        ecx, 1
00471f8e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00471f91 8b750c                   mov        esi, dword ptr [ebp + 0xc]
00471f94 f30f10440208             movss      xmm0, dword ptr [edx + eax + 8]
00471f9a f30f58440e08             addss      xmm0, dword ptr [esi + ecx + 8]
00471fa0 51                       push       ecx
00471fa1 f30f110424               movss      dword ptr [esp], xmm0
00471fa6 b804000000               mov        eax, 4
00471fab c1e000                   shl        eax, 0
00471fae b904000000               mov        ecx, 4
00471fb3 c1e100                   shl        ecx, 0
00471fb6 8b55fc                   mov        edx, dword ptr [ebp - 4]
00471fb9 8b750c                   mov        esi, dword ptr [ebp + 0xc]
00471fbc f30f10440208             movss      xmm0, dword ptr [edx + eax + 8]
00471fc2 f30f58440e08             addss      xmm0, dword ptr [esi + ecx + 8]
00471fc8 51                       push       ecx
00471fc9 f30f110424               movss      dword ptr [esp], xmm0
00471fce b804000000               mov        eax, 4
00471fd3 6bc800                   imul       ecx, eax, 0
00471fd6 ba04000000               mov        edx, 4
00471fdb 6bc200                   imul       eax, edx, 0
00471fde 8b55fc                   mov        edx, dword ptr [ebp - 4]
00471fe1 8b750c                   mov        esi, dword ptr [ebp + 0xc]
00471fe4 f30f10440a08             movss      xmm0, dword ptr [edx + ecx + 8]
00471fea f30f58440608             addss      xmm0, dword ptr [esi + eax + 8]
00471ff0 51                       push       ecx
00471ff1 f30f110424               movss      dword ptr [esp], xmm0
00471ff6 8b45fc                   mov        eax, dword ptr [ebp - 4]
00471ff9 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00471ffc f30f104004               movss      xmm0, dword ptr [eax + 4]
00472001 f30f584104               addss      xmm0, dword ptr [ecx + 4]
00472006 51                       push       ecx
00472007 f30f110424               movss      dword ptr [esp], xmm0
0047200c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047200f 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00472012 f30f1002                 movss      xmm0, dword ptr [edx]
00472016 f30f5800                 addss      xmm0, dword ptr [eax]
0047201a 51                       push       ecx
0047201b f30f110424               movss      dword ptr [esp], xmm0
00472020 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00472023 e8a8f8ffff               call       0x4718d0
00472028 8b4508                   mov        eax, dword ptr [ebp + 8]
0047202b 5e                       pop        esi
0047202c 8be5                     mov        esp, ebp
0047202e 5d                       pop        ebp
0047202f c20800                   ret        8
00472032 cc                       int3       
00472033 cc                       int3       
00472034 cc                       int3       
00472035 cc                       int3       
00472036 cc                       int3       
00472037 cc                       int3       
00472038 cc                       int3       
00472039 cc                       int3       
0047203a cc                       int3       
0047203b cc                       int3       
0047203c cc                       int3       
0047203d cc                       int3       
0047203e cc                       int3       
0047203f cc                       int3       
