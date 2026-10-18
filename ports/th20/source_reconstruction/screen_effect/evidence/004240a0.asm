004240a0 55                       push       ebp
004240a1 8bec                     mov        ebp, esp
004240a3 51                       push       ecx
004240a4 894dfc                   mov        dword ptr [ebp - 4], ecx
004240a7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004240aa c6800f0e0006ff           mov        byte ptr [eax + 0x6000e0f], 0xff
004240b1 8be5                     mov        esp, ebp
004240b3 5d                       pop        ebp
004240b4 c3                       ret        
004240b5 cc                       int3       
004240b6 cc                       int3       
004240b7 cc                       int3       
004240b8 cc                       int3       
004240b9 cc                       int3       
004240ba cc                       int3       
004240bb cc                       int3       
004240bc cc                       int3       
004240bd cc                       int3       
004240be cc                       int3       
004240bf cc                       int3       
