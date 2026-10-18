004e6980 55                       push       ebp
004e6981 8bec                     mov        ebp, esp
004e6983 51                       push       ecx
004e6984 894dfc                   mov        dword ptr [ebp - 4], ecx
004e6987 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e698a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004e698d 8988d8a10100             mov        dword ptr [eax + 0x1a1d8], ecx
004e6993 8be5                     mov        esp, ebp
004e6995 5d                       pop        ebp
004e6996 c20400                   ret        4
004e6999 cc                       int3       
004e699a cc                       int3       
004e699b cc                       int3       
004e699c cc                       int3       
004e699d cc                       int3       
004e699e cc                       int3       
004e699f cc                       int3       
