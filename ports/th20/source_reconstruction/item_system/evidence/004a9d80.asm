004a9d80 55                       push       ebp
004a9d81 8bec                     mov        ebp, esp
004a9d83 51                       push       ecx
004a9d84 894dfc                   mov        dword ptr [ebp - 4], ecx
004a9d87 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a9d8a 8b4868                   mov        ecx, dword ptr [eax + 0x68]
004a9d8d 034d08                   add        ecx, dword ptr [ebp + 8]
004a9d90 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a9d93 894a68                   mov        dword ptr [edx + 0x68], ecx
004a9d96 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9d99 e802120000               call       0x4aafa0
004a9d9e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9da1 394168                   cmp        dword ptr [ecx + 0x68], eax
004a9da4 7c27                     jl         0x4a9dcd
004a9da6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9da9 e8f2110000               call       0x4aafa0
004a9dae 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a9db1 894268                   mov        dword ptr [edx + 0x68], eax
004a9db4 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a9db7 83b89800000000           cmp        dword ptr [eax + 0x98], 0
004a9dbe 7d0d                     jge        0x4a9dcd
004a9dc0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9dc3 c7819800000001000000     mov        dword ptr [ecx + 0x98], 1
004a9dcd 8be5                     mov        esp, ebp
004a9dcf 5d                       pop        ebp
004a9dd0 c20400                   ret        4
004a9dd3 cc                       int3       
004a9dd4 cc                       int3       
004a9dd5 cc                       int3       
004a9dd6 cc                       int3       
004a9dd7 cc                       int3       
004a9dd8 cc                       int3       
004a9dd9 cc                       int3       
004a9dda cc                       int3       
004a9ddb cc                       int3       
004a9ddc cc                       int3       
004a9ddd cc                       int3       
004a9dde cc                       int3       
004a9ddf cc                       int3       
