004c2030 55                       push       ebp
004c2031 8bec                     mov        ebp, esp
004c2033 83ec08                   sub        esp, 8
004c2036 894dfc                   mov        dword ptr [ebp - 4], ecx
004c2039 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c203c 83c034                   add        eax, 0x34
004c203f 8945f8                   mov        dword ptr [ebp - 8], eax
004c2042 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c2045 51                       push       ecx
004c2046 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c2049 e8a273f7ff               call       0x4393f0
004c204e 90                       nop        
004c204f 8be5                     mov        esp, ebp
004c2051 5d                       pop        ebp
004c2052 c20400                   ret        4
004c2055 cc                       int3       
004c2056 cc                       int3       
004c2057 cc                       int3       
004c2058 cc                       int3       
004c2059 cc                       int3       
004c205a cc                       int3       
004c205b cc                       int3       
004c205c cc                       int3       
004c205d cc                       int3       
004c205e cc                       int3       
004c205f cc                       int3       
