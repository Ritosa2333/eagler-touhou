004c1f30 55                       push       ebp
004c1f31 8bec                     mov        ebp, esp
004c1f33 83ec0c                   sub        esp, 0xc
004c1f36 894dfc                   mov        dword ptr [ebp - 4], ecx
004c1f39 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1f3c 81c18c000000             add        ecx, 0x8c
004c1f42 e8b9a3f4ff               call       0x40c300
004c1f47 85c0                     test       eax, eax
004c1f49 7502                     jne        0x4c1f4d
004c1f4b eb69                     jmp        0x4c1fb6
004c1f4d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1f50 81c18c000000             add        ecx, 0x8c
004c1f56 e8a5a3f4ff               call       0x40c300
004c1f5b 8945f4                   mov        dword ptr [ebp - 0xc], eax
004c1f5e 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1f61 8b88c0000000             mov        ecx, dword ptr [eax + 0xc0]
004c1f67 e8a407f5ff               call       0x412710
004c1f6c 8945f8                   mov        dword ptr [ebp - 8], eax
004c1f6f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1f72 51                       push       ecx
004c1f73 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c1f76 e835ffffff               call       0x4c1eb0
004c1f7b 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1f7e 8b4214                   mov        eax, dword ptr [edx + 0x14]
004c1f81 83e0fe                   and        eax, 0xfffffffe
004c1f84 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1f87 894114                   mov        dword ptr [ecx + 0x14], eax
004c1f8a 6a00                     push       0
004c1f8c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1f8f 81c18c000000             add        ecx, 0x8c
004c1f95 e806f8f4ff               call       0x4117a0
004c1f9a 90                       nop        
004c1f9b 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004c1f9e 81e200000001             and        edx, 0x1000000
004c1fa4 7410                     je         0x4c1fb6
004c1fa6 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1fa9 50                       push       eax
004c1faa 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004c1fb0 e8bb1bf5ff               call       0x413b70
004c1fb5 90                       nop        
004c1fb6 8be5                     mov        esp, ebp
004c1fb8 5d                       pop        ebp
004c1fb9 c3                       ret        
004c1fba cc                       int3       
004c1fbb cc                       int3       
004c1fbc cc                       int3       
004c1fbd cc                       int3       
004c1fbe cc                       int3       
004c1fbf cc                       int3       
