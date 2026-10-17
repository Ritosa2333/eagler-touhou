00515b50 55                       push       ebp
00515b51 8bec                     mov        ebp, esp
00515b53 51                       push       ecx
00515b54 894dfc                   mov        dword ptr [ebp - 4], ecx
00515b57 51                       push       ecx
00515b58 0f57c0                   xorps      xmm0, xmm0
00515b5b f30f110424               movss      dword ptr [esp], xmm0
00515b60 51                       push       ecx
00515b61 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00515b66 f30f110424               movss      dword ptr [esp], xmm0
00515b6b 51                       push       ecx
00515b6c f30f104508               movss      xmm0, dword ptr [ebp + 8]
00515b71 f30f110424               movss      dword ptr [esp], xmm0
00515b76 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515b79 e852d2f0ff               call       0x422dd0
00515b7e 90                       nop        
00515b7f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00515b82 8be5                     mov        esp, ebp
00515b84 5d                       pop        ebp
00515b85 c20800                   ret        8
00515b88 cc                       int3       
00515b89 cc                       int3       
00515b8a cc                       int3       
00515b8b cc                       int3       
00515b8c cc                       int3       
00515b8d cc                       int3       
00515b8e cc                       int3       
00515b8f cc                       int3       
