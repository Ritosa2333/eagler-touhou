004c3880 55                       push       ebp
004c3881 8bec                     mov        ebp, esp
004c3883 83ec08                   sub        esp, 8
004c3886 894dfc                   mov        dword ptr [ebp - 4], ecx
004c3889 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c388c c780240c000000000000     mov        dword ptr [eax + 0xc24], 0
004c3896 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c3899 81c1e00b0000             add        ecx, 0xbe0
004c389f e82cc4f8ff               call       0x44fcd0
004c38a4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c38a7 e834e4f4ff               call       0x411ce0
004c38ac 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c38af 8b5114                   mov        edx, dword ptr [ecx + 0x14]
004c38b2 8955f8                   mov        dword ptr [ebp - 8], edx
004c38b5 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c38b8 50                       push       eax
004c38b9 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c38bc e8ef6df8ff               call       0x44a6b0
004c38c1 90                       nop        
004c38c2 8be5                     mov        esp, ebp
004c38c4 5d                       pop        ebp
004c38c5 c3                       ret        
004c38c6 cc                       int3       
004c38c7 cc                       int3       
004c38c8 cc                       int3       
004c38c9 cc                       int3       
004c38ca cc                       int3       
004c38cb cc                       int3       
004c38cc cc                       int3       
004c38cd cc                       int3       
004c38ce cc                       int3       
004c38cf cc                       int3       
