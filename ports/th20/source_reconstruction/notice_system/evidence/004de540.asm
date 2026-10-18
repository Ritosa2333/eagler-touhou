004de540 55                       push       ebp
004de541 8bec                     mov        ebp, esp
004de543 51                       push       ecx
004de544 894dfc                   mov        dword ptr [ebp - 4], ecx
004de547 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004de54a e851d8f2ff               call       0x40bda0
004de54f 8b45fc                   mov        eax, dword ptr [ebp - 4]
004de552 c70018205700             mov        dword ptr [eax], 0x572018
004de558 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004de55b 51                       push       ecx
004de55c e81fcdf2ff               call       0x40b280
004de561 83c404                   add        esp, 4
004de564 8b45fc                   mov        eax, dword ptr [ebp - 4]
004de567 8be5                     mov        esp, ebp
004de569 5d                       pop        ebp
004de56a c20400                   ret        4
004de56d cc                       int3       
004de56e cc                       int3       
004de56f cc                       int3       
