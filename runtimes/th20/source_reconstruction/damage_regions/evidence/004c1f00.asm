004c1f00 55                       push       ebp
004c1f01 8bec                     mov        ebp, esp
004c1f03 51                       push       ecx
004c1f04 894dfc                   mov        dword ptr [ebp - 4], ecx
004c1f07 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1f0a e801efffff               call       0x4c0e10
004c1f0f 85c0                     test       eax, eax
004c1f11 7410                     je         0x4c1f23
004c1f13 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1f16 e8f5eeffff               call       0x4c0e10
004c1f1b 8bc8                     mov        ecx, eax
004c1f1d e80e000000               call       0x4c1f30
004c1f22 90                       nop        
004c1f23 8be5                     mov        esp, ebp
004c1f25 5d                       pop        ebp
004c1f26 c3                       ret        
004c1f27 cc                       int3       
004c1f28 cc                       int3       
004c1f29 cc                       int3       
004c1f2a cc                       int3       
004c1f2b cc                       int3       
004c1f2c cc                       int3       
004c1f2d cc                       int3       
004c1f2e cc                       int3       
004c1f2f cc                       int3       
