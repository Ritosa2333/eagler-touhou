0047c5f0 55                       push       ebp
0047c5f1 8bec                     mov        ebp, esp
0047c5f3 51                       push       ecx
0047c5f4 894dfc                   mov        dword ptr [ebp - 4], ecx
0047c5f7 51                       push       ecx
0047c5f8 f30f104508               movss      xmm0, dword ptr [ebp + 8]
0047c5fd f30f110424               movss      dword ptr [esp], xmm0
0047c602 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c605 e8e67b0000               call       0x4841f0
0047c60a 90                       nop        
0047c60b 8be5                     mov        esp, ebp
0047c60d 5d                       pop        ebp
0047c60e c20400                   ret        4
0047c611 cc                       int3       
0047c612 cc                       int3       
0047c613 cc                       int3       
0047c614 cc                       int3       
0047c615 cc                       int3       
0047c616 cc                       int3       
0047c617 cc                       int3       
0047c618 cc                       int3       
0047c619 cc                       int3       
0047c61a cc                       int3       
0047c61b cc                       int3       
0047c61c cc                       int3       
0047c61d cc                       int3       
0047c61e cc                       int3       
0047c61f cc                       int3       
