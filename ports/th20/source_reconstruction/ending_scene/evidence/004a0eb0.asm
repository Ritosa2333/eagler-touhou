004a0eb0 55                       push       ebp
004a0eb1 8bec                     mov        ebp, esp
004a0eb3 51                       push       ecx
004a0eb4 894dfc                   mov        dword ptr [ebp - 4], ecx
004a0eb7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a0eba 8b4020                   mov        eax, dword ptr [eax + 0x20]
004a0ebd d1e8                     shr        eax, 1
004a0ebf 83e001                   and        eax, 1
004a0ec2 8be5                     mov        esp, ebp
004a0ec4 5d                       pop        ebp
004a0ec5 c3                       ret        
004a0ec6 cc                       int3       
004a0ec7 cc                       int3       
004a0ec8 cc                       int3       
004a0ec9 cc                       int3       
004a0eca cc                       int3       
004a0ecb cc                       int3       
004a0ecc cc                       int3       
004a0ecd cc                       int3       
004a0ece cc                       int3       
004a0ecf cc                       int3       
