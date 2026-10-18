004c1c10 55                       push       ebp
004c1c11 8bec                     mov        ebp, esp
004c1c13 51                       push       ecx
004c1c14 894dfc                   mov        dword ptr [ebp - 4], ecx
004c1c17 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1c1a e8f1f1ffff               call       0x4c0e10
004c1c1f 85c0                     test       eax, eax
004c1c21 7410                     je         0x4c1c33
004c1c23 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1c26 e8e5f1ffff               call       0x4c0e10
004c1c2b 8bc8                     mov        ecx, eax
004c1c2d e86e88fbff               call       0x47a4a0
004c1c32 90                       nop        
004c1c33 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1c36 8b08                     mov        ecx, dword ptr [eax]
004c1c38 8b5508                   mov        edx, dword ptr [ebp + 8]
004c1c3b 890a                     mov        dword ptr [edx], ecx
004c1c3d 8b4508                   mov        eax, dword ptr [ebp + 8]
004c1c40 8be5                     mov        esp, ebp
004c1c42 5d                       pop        ebp
004c1c43 c20400                   ret        4
004c1c46 cc                       int3       
004c1c47 cc                       int3       
004c1c48 cc                       int3       
004c1c49 cc                       int3       
004c1c4a cc                       int3       
004c1c4b cc                       int3       
004c1c4c cc                       int3       
004c1c4d cc                       int3       
004c1c4e cc                       int3       
004c1c4f cc                       int3       
