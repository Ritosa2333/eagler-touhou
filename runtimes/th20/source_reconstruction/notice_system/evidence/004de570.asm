004de570 55                       push       ebp
004de571 8bec                     mov        ebp, esp
004de573 51                       push       ecx
004de574 894dfc                   mov        dword ptr [ebp - 4], ecx
004de577 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004de57a e821d8f2ff               call       0x40bda0
004de57f 8b45fc                   mov        eax, dword ptr [ebp - 4]
004de582 c70034205700             mov        dword ptr [eax], 0x572034
004de588 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004de58b 51                       push       ecx
004de58c e8efccf2ff               call       0x40b280
004de591 83c404                   add        esp, 4
004de594 8b45fc                   mov        eax, dword ptr [ebp - 4]
004de597 8be5                     mov        esp, ebp
004de599 5d                       pop        ebp
004de59a c20400                   ret        4
004de59d cc                       int3       
004de59e cc                       int3       
004de59f cc                       int3       
