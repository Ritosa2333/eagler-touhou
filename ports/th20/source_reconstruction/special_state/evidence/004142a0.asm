004142a0 55                       push       ebp
004142a1 8bec                     mov        ebp, esp
004142a3 51                       push       ecx
004142a4 894dfc                   mov        dword ptr [ebp - 4], ecx
004142a7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004142aa 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004142ad 8908                     mov        dword ptr [eax], ecx
004142af 8b45fc                   mov        eax, dword ptr [ebp - 4]
004142b2 8be5                     mov        esp, ebp
004142b4 5d                       pop        ebp
004142b5 c20400                   ret        4
004142b8 cc                       int3       
004142b9 cc                       int3       
004142ba cc                       int3       
004142bb cc                       int3       
004142bc cc                       int3       
004142bd cc                       int3       
004142be cc                       int3       
004142bf cc                       int3       
