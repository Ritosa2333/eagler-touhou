004c2130 55                       push       ebp
004c2131 8bec                     mov        ebp, esp
004c2133 83ec0c                   sub        esp, 0xc
004c2136 894df4                   mov        dword ptr [ebp - 0xc], ecx
004c2139 689cc84900               push       0x49c89c
004c213e e81a060800               call       0x54275d
004c2143 83c404                   add        esp, 4
004c2146 8945fc                   mov        dword ptr [ebp - 4], eax
004c2149 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c214d 741a                     je         0x4c2169
004c214f 689cc84900               push       0x49c89c
004c2154 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c2157 e8249ff4ff               call       0x40c080
004c215c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c215f e87c010000               call       0x4c22e0
004c2164 8945f8                   mov        dword ptr [ebp - 8], eax
004c2167 eb07                     jmp        0x4c2170
004c2169 c745f800000000           mov        dword ptr [ebp - 8], 0
004c2170 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c2173 8be5                     mov        esp, ebp
004c2175 5d                       pop        ebp
004c2176 c20400                   ret        4
004c2179 cc                       int3       
004c217a cc                       int3       
004c217b cc                       int3       
004c217c cc                       int3       
004c217d cc                       int3       
004c217e cc                       int3       
004c217f cc                       int3       
