004c6210 55                       push       ebp
004c6211 8bec                     mov        ebp, esp
004c6213 51                       push       ecx
004c6214 894dfc                   mov        dword ptr [ebp - 4], ecx
004c6217 8b4508                   mov        eax, dword ptr [ebp + 8]
004c621a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c621d 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004c6220 899481242e0000           mov        dword ptr [ecx + eax*4 + 0x2e24], edx
004c6227 8be5                     mov        esp, ebp
004c6229 5d                       pop        ebp
004c622a c20800                   ret        8
004c622d cc                       int3       
004c622e cc                       int3       
004c622f cc                       int3       
