004c5e20 55                       push       ebp
004c5e21 8bec                     mov        ebp, esp
004c5e23 51                       push       ecx
004c5e24 894dfc                   mov        dword ptr [ebp - 4], ecx
004c5e27 8b4508                   mov        eax, dword ptr [ebp + 8]
004c5e2a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c5e2d 699481242e0000d4030000   imul       edx, dword ptr [ecx + eax*4 + 0x2e24], 0x3d4
004c5e38 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c5e3b 8d441020                 lea        eax, [eax + edx + 0x20]
004c5e3f 8be5                     mov        esp, ebp
004c5e41 5d                       pop        ebp
004c5e42 c20400                   ret        4
004c5e45 cc                       int3       
004c5e46 cc                       int3       
004c5e47 cc                       int3       
004c5e48 cc                       int3       
004c5e49 cc                       int3       
004c5e4a cc                       int3       
004c5e4b cc                       int3       
004c5e4c cc                       int3       
004c5e4d cc                       int3       
004c5e4e cc                       int3       
004c5e4f cc                       int3       
