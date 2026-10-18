004a9e60 55                       push       ebp
004a9e61 8bec                     mov        ebp, esp
004a9e63 51                       push       ecx
004a9e64 894dfc                   mov        dword ptr [ebp - 4], ecx
004a9e67 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a9e6a 8b4870                   mov        ecx, dword ptr [eax + 0x70]
004a9e6d 034d08                   add        ecx, dword ptr [ebp + 8]
004a9e70 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a9e73 894a70                   mov        dword ptr [edx + 0x70], ecx
004a9e76 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9e79 e822110000               call       0x4aafa0
004a9e7e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9e81 394170                   cmp        dword ptr [ecx + 0x70], eax
004a9e84 7c27                     jl         0x4a9ead
004a9e86 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9e89 e812110000               call       0x4aafa0
004a9e8e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a9e91 894270                   mov        dword ptr [edx + 0x70], eax
004a9e94 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a9e97 83b89800000000           cmp        dword ptr [eax + 0x98], 0
004a9e9e 7d0d                     jge        0x4a9ead
004a9ea0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9ea3 c7819800000003000000     mov        dword ptr [ecx + 0x98], 3
004a9ead 8be5                     mov        esp, ebp
004a9eaf 5d                       pop        ebp
004a9eb0 c20400                   ret        4
004a9eb3 cc                       int3       
004a9eb4 cc                       int3       
004a9eb5 cc                       int3       
004a9eb6 cc                       int3       
004a9eb7 cc                       int3       
004a9eb8 cc                       int3       
004a9eb9 cc                       int3       
004a9eba cc                       int3       
004a9ebb cc                       int3       
004a9ebc cc                       int3       
004a9ebd cc                       int3       
004a9ebe cc                       int3       
004a9ebf cc                       int3       
