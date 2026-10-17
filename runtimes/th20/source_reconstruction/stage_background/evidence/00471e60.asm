00471e60 55                       push       ebp
00471e61 8bec                     mov        ebp, esp
00471e63 51                       push       ecx
00471e64 56                       push       esi
00471e65 894dfc                   mov        dword ptr [ebp - 4], ecx
00471e68 b804000000               mov        eax, 4
00471e6d 6bc803                   imul       ecx, eax, 3
00471e70 ba04000000               mov        edx, 4
00471e75 6bc203                   imul       eax, edx, 3
00471e78 8b55fc                   mov        edx, dword ptr [ebp - 4]
00471e7b 8b750c                   mov        esi, dword ptr [ebp + 0xc]
00471e7e f30f10440a08             movss      xmm0, dword ptr [edx + ecx + 8]
00471e84 f30f5c440608             subss      xmm0, dword ptr [esi + eax + 8]
00471e8a 51                       push       ecx
00471e8b f30f110424               movss      dword ptr [esp], xmm0
00471e90 b804000000               mov        eax, 4
00471e95 d1e0                     shl        eax, 1
00471e97 b904000000               mov        ecx, 4
00471e9c d1e1                     shl        ecx, 1
00471e9e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00471ea1 8b750c                   mov        esi, dword ptr [ebp + 0xc]
00471ea4 f30f10440208             movss      xmm0, dword ptr [edx + eax + 8]
00471eaa f30f5c440e08             subss      xmm0, dword ptr [esi + ecx + 8]
00471eb0 51                       push       ecx
00471eb1 f30f110424               movss      dword ptr [esp], xmm0
00471eb6 b804000000               mov        eax, 4
00471ebb c1e000                   shl        eax, 0
00471ebe b904000000               mov        ecx, 4
00471ec3 c1e100                   shl        ecx, 0
00471ec6 8b55fc                   mov        edx, dword ptr [ebp - 4]
00471ec9 8b750c                   mov        esi, dword ptr [ebp + 0xc]
00471ecc f30f10440208             movss      xmm0, dword ptr [edx + eax + 8]
00471ed2 f30f5c440e08             subss      xmm0, dword ptr [esi + ecx + 8]
00471ed8 51                       push       ecx
00471ed9 f30f110424               movss      dword ptr [esp], xmm0
00471ede b804000000               mov        eax, 4
00471ee3 6bc800                   imul       ecx, eax, 0
00471ee6 ba04000000               mov        edx, 4
00471eeb 6bc200                   imul       eax, edx, 0
00471eee 8b55fc                   mov        edx, dword ptr [ebp - 4]
00471ef1 8b750c                   mov        esi, dword ptr [ebp + 0xc]
00471ef4 f30f10440a08             movss      xmm0, dword ptr [edx + ecx + 8]
00471efa f30f5c440608             subss      xmm0, dword ptr [esi + eax + 8]
00471f00 51                       push       ecx
00471f01 f30f110424               movss      dword ptr [esp], xmm0
00471f06 8b45fc                   mov        eax, dword ptr [ebp - 4]
00471f09 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00471f0c f30f104004               movss      xmm0, dword ptr [eax + 4]
00471f11 f30f5c4104               subss      xmm0, dword ptr [ecx + 4]
00471f16 51                       push       ecx
00471f17 f30f110424               movss      dword ptr [esp], xmm0
00471f1c 8b55fc                   mov        edx, dword ptr [ebp - 4]
00471f1f 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00471f22 f30f1002                 movss      xmm0, dword ptr [edx]
00471f26 f30f5c00                 subss      xmm0, dword ptr [eax]
00471f2a 51                       push       ecx
00471f2b f30f110424               movss      dword ptr [esp], xmm0
00471f30 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00471f33 e898f9ffff               call       0x4718d0
00471f38 8b4508                   mov        eax, dword ptr [ebp + 8]
00471f3b 5e                       pop        esi
00471f3c 8be5                     mov        esp, ebp
00471f3e 5d                       pop        ebp
00471f3f c20800                   ret        8
00471f42 cc                       int3       
00471f43 cc                       int3       
00471f44 cc                       int3       
00471f45 cc                       int3       
00471f46 cc                       int3       
00471f47 cc                       int3       
00471f48 cc                       int3       
00471f49 cc                       int3       
00471f4a cc                       int3       
00471f4b cc                       int3       
00471f4c cc                       int3       
00471f4d cc                       int3       
00471f4e cc                       int3       
00471f4f cc                       int3       
