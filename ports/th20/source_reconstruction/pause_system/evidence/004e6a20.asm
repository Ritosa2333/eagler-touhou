004e6a20 55                       push       ebp
004e6a21 8bec                     mov        ebp, esp
004e6a23 51                       push       ecx
004e6a24 894dfc                   mov        dword ptr [ebp - 4], ecx
004e6a27 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e6a2a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004e6a2d 8988dca10100             mov        dword ptr [eax + 0x1a1dc], ecx
004e6a33 8be5                     mov        esp, ebp
004e6a35 5d                       pop        ebp
004e6a36 c20400                   ret        4
004e6a39 cc                       int3       
004e6a3a cc                       int3       
004e6a3b cc                       int3       
004e6a3c cc                       int3       
004e6a3d cc                       int3       
004e6a3e cc                       int3       
004e6a3f cc                       int3       
