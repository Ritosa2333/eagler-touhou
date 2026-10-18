004e6af0 55                       push       ebp
004e6af1 8bec                     mov        ebp, esp
004e6af3 51                       push       ecx
004e6af4 894dfc                   mov        dword ptr [ebp - 4], ecx
004e6af7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e6afa 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004e6afd 8988f0a10100             mov        dword ptr [eax + 0x1a1f0], ecx
004e6b03 8be5                     mov        esp, ebp
004e6b05 5d                       pop        ebp
004e6b06 c20400                   ret        4
004e6b09 cc                       int3       
004e6b0a cc                       int3       
004e6b0b cc                       int3       
004e6b0c cc                       int3       
004e6b0d cc                       int3       
004e6b0e cc                       int3       
004e6b0f cc                       int3       
