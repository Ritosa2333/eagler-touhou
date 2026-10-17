004e69f0 55                       push       ebp
004e69f1 8bec                     mov        ebp, esp
004e69f3 51                       push       ecx
004e69f4 894dfc                   mov        dword ptr [ebp - 4], ecx
004e69f7 51                       push       ecx
004e69f8 f30f104508               movss      xmm0, dword ptr [ebp + 8]
004e69fd f30f110424               movss      dword ptr [esp], xmm0
004e6a02 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e6a05 81c108a20100             add        ecx, 0x1a208
004e6a0b e810c5f6ff               call       0x452f20
004e6a10 90                       nop        
004e6a11 8be5                     mov        esp, ebp
004e6a13 5d                       pop        ebp
004e6a14 c20400                   ret        4
004e6a17 cc                       int3       
004e6a18 cc                       int3       
004e6a19 cc                       int3       
004e6a1a cc                       int3       
004e6a1b cc                       int3       
004e6a1c cc                       int3       
004e6a1d cc                       int3       
004e6a1e cc                       int3       
004e6a1f cc                       int3       
