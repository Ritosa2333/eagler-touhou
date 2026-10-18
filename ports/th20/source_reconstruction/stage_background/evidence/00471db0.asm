00471db0 55                       push       ebp
00471db1 8bec                     mov        ebp, esp
00471db3 51                       push       ecx
00471db4 894dfc                   mov        dword ptr [ebp - 4], ecx
00471db7 b804000000               mov        eax, 4
00471dbc 6bc803                   imul       ecx, eax, 3
00471dbf 8b55fc                   mov        edx, dword ptr [ebp - 4]
00471dc2 f30f10440a08             movss      xmm0, dword ptr [edx + ecx + 8]
00471dc8 f30f59450c               mulss      xmm0, dword ptr [ebp + 0xc]
00471dcd 51                       push       ecx
00471dce f30f110424               movss      dword ptr [esp], xmm0
00471dd3 b804000000               mov        eax, 4
00471dd8 d1e0                     shl        eax, 1
00471dda 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00471ddd f30f10440108             movss      xmm0, dword ptr [ecx + eax + 8]
00471de3 f30f59450c               mulss      xmm0, dword ptr [ebp + 0xc]
00471de8 51                       push       ecx
00471de9 f30f110424               movss      dword ptr [esp], xmm0
00471dee ba04000000               mov        edx, 4
00471df3 c1e200                   shl        edx, 0
00471df6 8b45fc                   mov        eax, dword ptr [ebp - 4]
00471df9 f30f10441008             movss      xmm0, dword ptr [eax + edx + 8]
00471dff f30f59450c               mulss      xmm0, dword ptr [ebp + 0xc]
00471e04 51                       push       ecx
00471e05 f30f110424               movss      dword ptr [esp], xmm0
00471e0a b904000000               mov        ecx, 4
00471e0f 6bd100                   imul       edx, ecx, 0
00471e12 8b45fc                   mov        eax, dword ptr [ebp - 4]
00471e15 f30f10441008             movss      xmm0, dword ptr [eax + edx + 8]
00471e1b f30f59450c               mulss      xmm0, dword ptr [ebp + 0xc]
00471e20 51                       push       ecx
00471e21 f30f110424               movss      dword ptr [esp], xmm0
00471e26 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00471e29 f30f104104               movss      xmm0, dword ptr [ecx + 4]
00471e2e f30f59450c               mulss      xmm0, dword ptr [ebp + 0xc]
00471e33 51                       push       ecx
00471e34 f30f110424               movss      dword ptr [esp], xmm0
00471e39 8b55fc                   mov        edx, dword ptr [ebp - 4]
00471e3c f30f1002                 movss      xmm0, dword ptr [edx]
00471e40 f30f59450c               mulss      xmm0, dword ptr [ebp + 0xc]
00471e45 51                       push       ecx
00471e46 f30f110424               movss      dword ptr [esp], xmm0
00471e4b 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00471e4e e87dfaffff               call       0x4718d0
00471e53 8b4508                   mov        eax, dword ptr [ebp + 8]
00471e56 8be5                     mov        esp, ebp
00471e58 5d                       pop        ebp
00471e59 c20800                   ret        8
00471e5c cc                       int3       
00471e5d cc                       int3       
00471e5e cc                       int3       
00471e5f cc                       int3       
