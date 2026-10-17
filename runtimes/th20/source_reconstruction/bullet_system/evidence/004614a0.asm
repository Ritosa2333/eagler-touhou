004614a0 55                       push       ebp
004614a1 8bec                     mov        ebp, esp
004614a3 83ec08                   sub        esp, 8
004614a6 894dfc                   mov        dword ptr [ebp - 4], ecx
004614a9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004614ac 058c000000               add        eax, 0x8c
004614b1 8945f8                   mov        dword ptr [ebp - 8], eax
004614b4 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
004614b7 51                       push       ecx
004614b8 8b5510                   mov        edx, dword ptr [ebp + 0x10]
004614bb 52                       push       edx
004614bc 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004614bf 50                       push       eax
004614c0 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004614c3 51                       push       ecx
004614c4 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004614c7 e80481fdff               call       0x4395d0
004614cc 90                       nop        
004614cd 8be5                     mov        esp, ebp
004614cf 5d                       pop        ebp
004614d0 c21000                   ret        0x10
004614d3 cc                       int3       
004614d4 cc                       int3       
004614d5 cc                       int3       
004614d6 cc                       int3       
004614d7 cc                       int3       
004614d8 cc                       int3       
004614d9 cc                       int3       
004614da cc                       int3       
004614db cc                       int3       
004614dc cc                       int3       
004614dd cc                       int3       
004614de cc                       int3       
004614df cc                       int3       
