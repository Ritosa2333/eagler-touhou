004e6920 55                       push       ebp
004e6921 8bec                     mov        ebp, esp
004e6923 51                       push       ecx
004e6924 894dfc                   mov        dword ptr [ebp - 4], ecx
004e6927 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e692a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004e692d 8988fca10100             mov        dword ptr [eax + 0x1a1fc], ecx
004e6933 8be5                     mov        esp, ebp
004e6935 5d                       pop        ebp
004e6936 c20400                   ret        4
004e6939 cc                       int3       
004e693a cc                       int3       
004e693b cc                       int3       
004e693c cc                       int3       
004e693d cc                       int3       
004e693e cc                       int3       
004e693f cc                       int3       
