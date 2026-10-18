004b8150 55                       push       ebp
004b8151 8bec                     mov        ebp, esp
004b8153 83ec0c                   sub        esp, 0xc
004b8156 894dfc                   mov        dword ptr [ebp - 4], ecx
004b8159 c745f80a000000           mov        dword ptr [ebp - 8], 0xa
004b8160 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004b8167 8d45f8                   lea        eax, [ebp - 8]
004b816a 50                       push       eax
004b816b 8d4df4                   lea        ecx, [ebp - 0xc]
004b816e 51                       push       ecx
004b816f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b8172 81c2c0000000             add        edx, 0xc0
004b8178 52                       push       edx
004b8179 e8f28ffbff               call       0x471170
004b817e 83c40c                   add        esp, 0xc
004b8181 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b8184 8b10                     mov        edx, dword ptr [eax]
004b8186 8991c0000000             mov        dword ptr [ecx + 0xc0], edx
004b818c 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b818f 8b80c0000000             mov        eax, dword ptr [eax + 0xc0]
004b8195 8be5                     mov        esp, ebp
004b8197 5d                       pop        ebp
004b8198 c3                       ret        
004b8199 cc                       int3       
004b819a cc                       int3       
004b819b cc                       int3       
004b819c cc                       int3       
004b819d cc                       int3       
004b819e cc                       int3       
004b819f cc                       int3       
