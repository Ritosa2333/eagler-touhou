004e66d0 55                       push       ebp
004e66d1 8bec                     mov        ebp, esp
004e66d3 51                       push       ecx
004e66d4 894dfc                   mov        dword ptr [ebp - 4], ecx
004e66d7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e66da 8b4078                   mov        eax, dword ptr [eax + 0x78]
004e66dd 8be5                     mov        esp, ebp
004e66df 5d                       pop        ebp
004e66e0 c3                       ret        
004e66e1 cc                       int3       
004e66e2 cc                       int3       
004e66e3 cc                       int3       
004e66e4 cc                       int3       
004e66e5 cc                       int3       
004e66e6 cc                       int3       
004e66e7 cc                       int3       
004e66e8 cc                       int3       
004e66e9 cc                       int3       
004e66ea cc                       int3       
004e66eb cc                       int3       
004e66ec cc                       int3       
004e66ed cc                       int3       
004e66ee cc                       int3       
004e66ef cc                       int3       
