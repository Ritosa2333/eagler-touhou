004456e0 55                       push       ebp
004456e1 8bec                     mov        ebp, esp
004456e3 51                       push       ecx
004456e4 894dfc                   mov        dword ptr [ebp - 4], ecx
004456e7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004456ea 8b8090040000             mov        eax, dword ptr [eax + 0x490]
004456f0 8be5                     mov        esp, ebp
004456f2 5d                       pop        ebp
004456f3 c3                       ret        
004456f4 cc                       int3       
004456f5 cc                       int3       
004456f6 cc                       int3       
004456f7 cc                       int3       
004456f8 cc                       int3       
004456f9 cc                       int3       
004456fa cc                       int3       
004456fb cc                       int3       
004456fc cc                       int3       
004456fd cc                       int3       
004456fe cc                       int3       
004456ff cc                       int3       
