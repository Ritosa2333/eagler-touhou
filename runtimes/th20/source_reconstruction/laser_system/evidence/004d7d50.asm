004d7d50 55                       push       ebp
004d7d51 8bec                     mov        ebp, esp
004d7d53 51                       push       ecx
004d7d54 894dfc                   mov        dword ptr [ebp - 4], ecx
004d7d57 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d7d5a 83c058                   add        eax, 0x58
004d7d5d 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004d7d60 8b11                     mov        edx, dword ptr [ecx]
004d7d62 8910                     mov        dword ptr [eax], edx
004d7d64 8b5104                   mov        edx, dword ptr [ecx + 4]
004d7d67 895004                   mov        dword ptr [eax + 4], edx
004d7d6a 8b4908                   mov        ecx, dword ptr [ecx + 8]
004d7d6d 894808                   mov        dword ptr [eax + 8], ecx
004d7d70 8be5                     mov        esp, ebp
004d7d72 5d                       pop        ebp
004d7d73 c20400                   ret        4
004d7d76 cc                       int3       
004d7d77 cc                       int3       
004d7d78 cc                       int3       
004d7d79 cc                       int3       
004d7d7a cc                       int3       
004d7d7b cc                       int3       
004d7d7c cc                       int3       
004d7d7d cc                       int3       
004d7d7e cc                       int3       
004d7d7f cc                       int3       
