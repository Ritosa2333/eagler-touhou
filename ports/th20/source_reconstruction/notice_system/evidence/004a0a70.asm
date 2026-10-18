004a0a70 55                       push       ebp
004a0a71 8bec                     mov        ebp, esp
004a0a73 51                       push       ecx
004a0a74 894dfc                   mov        dword ptr [ebp - 4], ecx
004a0a77 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a0a7a 8b4808                   mov        ecx, dword ptr [eax + 8]
004a0a7d e8fe22f7ff               call       0x412d80
004a0a82 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a0a85 8b490c                   mov        ecx, dword ptr [ecx + 0xc]
004a0a88 e8f322f7ff               call       0x412d80
004a0a8d 90                       nop        
004a0a8e 8be5                     mov        esp, ebp
004a0a90 5d                       pop        ebp
004a0a91 c3                       ret        
004a0a92 cc                       int3       
004a0a93 cc                       int3       
004a0a94 cc                       int3       
004a0a95 cc                       int3       
004a0a96 cc                       int3       
004a0a97 cc                       int3       
004a0a98 cc                       int3       
004a0a99 cc                       int3       
004a0a9a cc                       int3       
004a0a9b cc                       int3       
004a0a9c cc                       int3       
004a0a9d cc                       int3       
004a0a9e cc                       int3       
004a0a9f cc                       int3       
