004c9170 55                       push       ebp
004c9171 8bec                     mov        ebp, esp
004c9173 51                       push       ecx
004c9174 894dfc                   mov        dword ptr [ebp - 4], ecx
004c9177 8b4508                   mov        eax, dword ptr [ebp + 8]
004c917a 50                       push       eax
004c917b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c917e e8ed0cf6ff               call       0x429e70
004c9183 90                       nop        
004c9184 8be5                     mov        esp, ebp
004c9186 5d                       pop        ebp
004c9187 c20400                   ret        4
004c918a cc                       int3       
004c918b cc                       int3       
004c918c cc                       int3       
004c918d cc                       int3       
004c918e cc                       int3       
004c918f cc                       int3       
