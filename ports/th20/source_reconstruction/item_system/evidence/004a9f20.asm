004a9f20 55                       push       ebp
004a9f21 8bec                     mov        ebp, esp
004a9f23 51                       push       ecx
004a9f24 894dfc                   mov        dword ptr [ebp - 4], ecx
004a9f27 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a9f2a 8b486c                   mov        ecx, dword ptr [eax + 0x6c]
004a9f2d 034d08                   add        ecx, dword ptr [ebp + 8]
004a9f30 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a9f33 894a6c                   mov        dword ptr [edx + 0x6c], ecx
004a9f36 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9f39 e862100000               call       0x4aafa0
004a9f3e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9f41 39416c                   cmp        dword ptr [ecx + 0x6c], eax
004a9f44 7c27                     jl         0x4a9f6d
004a9f46 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9f49 e852100000               call       0x4aafa0
004a9f4e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a9f51 89426c                   mov        dword ptr [edx + 0x6c], eax
004a9f54 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a9f57 83b89800000000           cmp        dword ptr [eax + 0x98], 0
004a9f5e 7d0d                     jge        0x4a9f6d
004a9f60 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9f63 c7819800000002000000     mov        dword ptr [ecx + 0x98], 2
004a9f6d 8be5                     mov        esp, ebp
004a9f6f 5d                       pop        ebp
004a9f70 c20400                   ret        4
004a9f73 cc                       int3       
004a9f74 cc                       int3       
004a9f75 cc                       int3       
004a9f76 cc                       int3       
004a9f77 cc                       int3       
004a9f78 cc                       int3       
004a9f79 cc                       int3       
004a9f7a cc                       int3       
004a9f7b cc                       int3       
004a9f7c cc                       int3       
004a9f7d cc                       int3       
004a9f7e cc                       int3       
004a9f7f cc                       int3       
