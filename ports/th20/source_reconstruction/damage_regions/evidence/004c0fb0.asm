004c0fb0 55                       push       ebp
004c0fb1 8bec                     mov        ebp, esp
004c0fb3 83ec08                   sub        esp, 8
004c0fb6 894dfc                   mov        dword ptr [ebp - 4], ecx
004c0fb9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c0fbc 05b4220000               add        eax, 0x22b4
004c0fc1 8945f8                   mov        dword ptr [ebp - 8], eax
004c0fc4 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c0fc7 51                       push       ecx
004c0fc8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c0fcb e890530400               call       0x506360
004c0fd0 8be5                     mov        esp, ebp
004c0fd2 5d                       pop        ebp
004c0fd3 c20400                   ret        4
004c0fd6 cc                       int3       
004c0fd7 cc                       int3       
004c0fd8 cc                       int3       
004c0fd9 cc                       int3       
004c0fda cc                       int3       
004c0fdb cc                       int3       
004c0fdc cc                       int3       
004c0fdd cc                       int3       
004c0fde cc                       int3       
004c0fdf cc                       int3       
