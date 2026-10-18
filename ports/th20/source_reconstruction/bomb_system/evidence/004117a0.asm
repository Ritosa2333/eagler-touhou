004117a0 55                       push       ebp
004117a1 8bec                     mov        ebp, esp
004117a3 51                       push       ecx
004117a4 894dfc                   mov        dword ptr [ebp - 4], ecx
004117a7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004117aa 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004117ad 8908                     mov        dword ptr [eax], ecx
004117af 8be5                     mov        esp, ebp
004117b1 5d                       pop        ebp
004117b2 c20400                   ret        4
004117b5 cc                       int3       
004117b6 cc                       int3       
004117b7 cc                       int3       
004117b8 cc                       int3       
004117b9 cc                       int3       
004117ba cc                       int3       
004117bb cc                       int3       
004117bc cc                       int3       
004117bd cc                       int3       
004117be cc                       int3       
004117bf cc                       int3       
