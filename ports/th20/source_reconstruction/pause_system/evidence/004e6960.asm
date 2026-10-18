004e6960 55                       push       ebp
004e6961 8bec                     mov        ebp, esp
004e6963 51                       push       ecx
004e6964 894dfc                   mov        dword ptr [ebp - 4], ecx
004e6967 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e696a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004e696d 89883ca20100             mov        dword ptr [eax + 0x1a23c], ecx
004e6973 8be5                     mov        esp, ebp
004e6975 5d                       pop        ebp
004e6976 c20400                   ret        4
004e6979 cc                       int3       
004e697a cc                       int3       
004e697b cc                       int3       
004e697c cc                       int3       
004e697d cc                       int3       
004e697e cc                       int3       
004e697f cc                       int3       
