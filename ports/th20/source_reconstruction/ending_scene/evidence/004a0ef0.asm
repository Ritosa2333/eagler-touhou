004a0ef0 55                       push       ebp
004a0ef1 8bec                     mov        ebp, esp
004a0ef3 51                       push       ecx
004a0ef4 894dfc                   mov        dword ptr [ebp - 4], ecx
004a0ef7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a0efa 8b407c                   mov        eax, dword ptr [eax + 0x7c]
004a0efd d1e8                     shr        eax, 1
004a0eff 83e001                   and        eax, 1
004a0f02 8be5                     mov        esp, ebp
004a0f04 5d                       pop        ebp
004a0f05 c3                       ret        
004a0f06 cc                       int3       
004a0f07 cc                       int3       
004a0f08 cc                       int3       
004a0f09 cc                       int3       
004a0f0a cc                       int3       
004a0f0b cc                       int3       
004a0f0c cc                       int3       
004a0f0d cc                       int3       
004a0f0e cc                       int3       
004a0f0f cc                       int3       
