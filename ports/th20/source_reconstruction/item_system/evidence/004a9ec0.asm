004a9ec0 55                       push       ebp
004a9ec1 8bec                     mov        ebp, esp
004a9ec3 51                       push       ecx
004a9ec4 894dfc                   mov        dword ptr [ebp - 4], ecx
004a9ec7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a9eca 8b4864                   mov        ecx, dword ptr [eax + 0x64]
004a9ecd 034d08                   add        ecx, dword ptr [ebp + 8]
004a9ed0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a9ed3 894a64                   mov        dword ptr [edx + 0x64], ecx
004a9ed6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9ed9 e8c2100000               call       0x4aafa0
004a9ede 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9ee1 394164                   cmp        dword ptr [ecx + 0x64], eax
004a9ee4 7c27                     jl         0x4a9f0d
004a9ee6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9ee9 e8b2100000               call       0x4aafa0
004a9eee 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a9ef1 894264                   mov        dword ptr [edx + 0x64], eax
004a9ef4 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a9ef7 83b89800000000           cmp        dword ptr [eax + 0x98], 0
004a9efe 7d0d                     jge        0x4a9f0d
004a9f00 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9f03 c7819800000000000000     mov        dword ptr [ecx + 0x98], 0
004a9f0d 8be5                     mov        esp, ebp
004a9f0f 5d                       pop        ebp
004a9f10 c20400                   ret        4
004a9f13 cc                       int3       
004a9f14 cc                       int3       
004a9f15 cc                       int3       
004a9f16 cc                       int3       
004a9f17 cc                       int3       
004a9f18 cc                       int3       
004a9f19 cc                       int3       
004a9f1a cc                       int3       
004a9f1b cc                       int3       
004a9f1c cc                       int3       
004a9f1d cc                       int3       
004a9f1e cc                       int3       
004a9f1f cc                       int3       
