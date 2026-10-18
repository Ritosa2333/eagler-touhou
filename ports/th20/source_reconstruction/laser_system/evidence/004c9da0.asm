004c9da0 55                       push       ebp
004c9da1 8bec                     mov        ebp, esp
004c9da3 51                       push       ecx
004c9da4 894dfc                   mov        dword ptr [ebp - 4], ecx
004c9da7 33c0                     xor        eax, eax
004c9da9 8be5                     mov        esp, ebp
004c9dab 5d                       pop        ebp
004c9dac c21400                   ret        0x14
004c9daf cc                       int3       
