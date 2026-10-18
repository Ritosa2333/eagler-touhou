004a0f10 55                       push       ebp
004a0f11 8bec                     mov        ebp, esp
004a0f13 51                       push       ecx
004a0f14 894dfc                   mov        dword ptr [ebp - 4], ecx
004a0f17 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a0f1a 8b4020                   mov        eax, dword ptr [eax + 0x20]
004a0f1d c1e802                   shr        eax, 2
004a0f20 83e001                   and        eax, 1
004a0f23 8be5                     mov        esp, ebp
004a0f25 5d                       pop        ebp
004a0f26 c3                       ret        
004a0f27 cc                       int3       
004a0f28 cc                       int3       
004a0f29 cc                       int3       
004a0f2a cc                       int3       
004a0f2b cc                       int3       
004a0f2c cc                       int3       
004a0f2d cc                       int3       
004a0f2e cc                       int3       
004a0f2f cc                       int3       
