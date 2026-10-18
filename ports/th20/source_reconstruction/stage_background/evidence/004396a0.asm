004396a0 55                       push       ebp
004396a1 8bec                     mov        ebp, esp
004396a3 51                       push       ecx
004396a4 894dfc                   mov        dword ptr [ebp - 4], ecx
004396a7 6a00                     push       0
004396a9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004396ac 83c13c                   add        ecx, 0x3c
004396af e86c9efeff               call       0x423520
004396b4 90                       nop        
004396b5 8be5                     mov        esp, ebp
004396b7 5d                       pop        ebp
004396b8 c3                       ret        
004396b9 cc                       int3       
004396ba cc                       int3       
004396bb cc                       int3       
004396bc cc                       int3       
004396bd cc                       int3       
004396be cc                       int3       
004396bf cc                       int3       
