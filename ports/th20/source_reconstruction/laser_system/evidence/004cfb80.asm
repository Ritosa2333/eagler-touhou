004cfb80 55                       push       ebp
004cfb81 8bec                     mov        ebp, esp
004cfb83 51                       push       ecx
004cfb84 894dfc                   mov        dword ptr [ebp - 4], ecx
004cfb87 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004cfb8a 83c108                   add        ecx, 8
004cfb8d e84e21f4ff               call       0x411ce0
004cfb92 8b45fc                   mov        eax, dword ptr [ebp - 4]
004cfb95 8b4828                   mov        ecx, dword ptr [eax + 0x28]
004cfb98 83e901                   sub        ecx, 1
004cfb9b 8b55fc                   mov        edx, dword ptr [ebp - 4]
004cfb9e 894a28                   mov        dword ptr [edx + 0x28], ecx
004cfba1 8be5                     mov        esp, ebp
004cfba3 5d                       pop        ebp
004cfba4 c20400                   ret        4
004cfba7 cc                       int3       
004cfba8 cc                       int3       
004cfba9 cc                       int3       
004cfbaa cc                       int3       
004cfbab cc                       int3       
004cfbac cc                       int3       
004cfbad cc                       int3       
004cfbae cc                       int3       
004cfbaf cc                       int3       
