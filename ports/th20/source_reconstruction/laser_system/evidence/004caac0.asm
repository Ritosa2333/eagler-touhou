004caac0 55                       push       ebp
004caac1 8bec                     mov        ebp, esp
004caac3 51                       push       ecx
004caac4 894dfc                   mov        dword ptr [ebp - 4], ecx
004caac7 33c0                     xor        eax, eax
004caac9 8be5                     mov        esp, ebp
004caacb 5d                       pop        ebp
004caacc c21000                   ret        0x10
004caacf cc                       int3       
