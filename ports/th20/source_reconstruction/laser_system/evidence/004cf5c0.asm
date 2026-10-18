004cf5c0 55                       push       ebp
004cf5c1 8bec                     mov        ebp, esp
004cf5c3 51                       push       ecx
004cf5c4 8b4508                   mov        eax, dword ptr [ebp + 8]
004cf5c7 8b88c8050000             mov        ecx, dword ptr [eax + 0x5c8]
004cf5cd 894dfc                   mov        dword ptr [ebp - 4], ecx
004cf5d0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004cf5d3 8b82e8060000             mov        eax, dword ptr [edx + 0x6e8]
004cf5d9 050c020000               add        eax, 0x20c
004cf5de 89450c                   mov        dword ptr [ebp + 0xc], eax
004cf5e1 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004cf5e4 8be5                     mov        esp, ebp
004cf5e6 5d                       pop        ebp
004cf5e7 c3                       ret        
004cf5e8 cc                       int3       
004cf5e9 cc                       int3       
004cf5ea cc                       int3       
004cf5eb cc                       int3       
004cf5ec cc                       int3       
004cf5ed cc                       int3       
004cf5ee cc                       int3       
004cf5ef cc                       int3       
