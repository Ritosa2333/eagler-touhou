004ceba0 55                       push       ebp
004ceba1 8bec                     mov        ebp, esp
004ceba3 51                       push       ecx
004ceba4 894dfc                   mov        dword ptr [ebp - 4], ecx
004ceba7 33c0                     xor        eax, eax
004ceba9 8be5                     mov        esp, ebp
004cebab 5d                       pop        ebp
004cebac c21c00                   ret        0x1c
004cebaf cc                       int3       
