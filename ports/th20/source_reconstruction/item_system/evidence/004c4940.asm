004c4940 55                       push       ebp
004c4941 8bec                     mov        ebp, esp
004c4943 51                       push       ecx
004c4944 894dfc                   mov        dword ptr [ebp - 4], ecx
004c4947 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c494a d98080200000             fld        dword ptr [eax + 0x2080]
004c4950 8be5                     mov        esp, ebp
004c4952 5d                       pop        ebp
004c4953 c3                       ret        
004c4954 cc                       int3       
004c4955 cc                       int3       
004c4956 cc                       int3       
004c4957 cc                       int3       
004c4958 cc                       int3       
004c4959 cc                       int3       
004c495a cc                       int3       
004c495b cc                       int3       
004c495c cc                       int3       
004c495d cc                       int3       
004c495e cc                       int3       
004c495f cc                       int3       
