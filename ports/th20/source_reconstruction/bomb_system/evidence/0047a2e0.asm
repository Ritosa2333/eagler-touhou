0047a2e0 55                       push       ebp
0047a2e1 8bec                     mov        ebp, esp
0047a2e3 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0047a2e6 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047a2e9 f30f1000                 movss      xmm0, dword ptr [eax]
0047a2ed f30f5c01                 subss      xmm0, dword ptr [ecx]
0047a2f1 51                       push       ecx
0047a2f2 f30f110424               movss      dword ptr [esp], xmm0
0047a2f7 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0047a2fa 8b4508                   mov        eax, dword ptr [ebp + 8]
0047a2fd f30f104204               movss      xmm0, dword ptr [edx + 4]
0047a302 f30f5c4004               subss      xmm0, dword ptr [eax + 4]
0047a307 51                       push       ecx
0047a308 f30f110424               movss      dword ptr [esp], xmm0
0047a30d e86eeffdff               call       0x459280
0047a312 83c408                   add        esp, 8
0047a315 5d                       pop        ebp
0047a316 c3                       ret        
0047a317 cc                       int3       
0047a318 cc                       int3       
0047a319 cc                       int3       
0047a31a cc                       int3       
0047a31b cc                       int3       
0047a31c cc                       int3       
0047a31d cc                       int3       
0047a31e cc                       int3       
0047a31f cc                       int3       
