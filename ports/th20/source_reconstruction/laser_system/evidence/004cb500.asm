004cb500 55                       push       ebp
004cb501 8bec                     mov        ebp, esp
004cb503 51                       push       ecx
004cb504 894dfc                   mov        dword ptr [ebp - 4], ecx
004cb507 33c0                     xor        eax, eax
004cb509 8be5                     mov        esp, ebp
004cb50b 5d                       pop        ebp
004cb50c c21800                   ret        0x18
004cb50f cc                       int3       
