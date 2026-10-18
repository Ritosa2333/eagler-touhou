004b80b0 55                       push       ebp
004b80b1 8bec                     mov        ebp, esp
004b80b3 83ec0c                   sub        esp, 0xc
004b80b6 894dfc                   mov        dword ptr [ebp - 4], ecx
004b80b9 c745f807000000           mov        dword ptr [ebp - 8], 7
004b80c0 c745f4ffffffff           mov        dword ptr [ebp - 0xc], 0xffffffff
004b80c7 8d45f8                   lea        eax, [ebp - 8]
004b80ca 50                       push       eax
004b80cb 8d4df4                   lea        ecx, [ebp - 0xc]
004b80ce 51                       push       ecx
004b80cf 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b80d2 81c2b8000000             add        edx, 0xb8
004b80d8 52                       push       edx
004b80d9 e89290fbff               call       0x471170
004b80de 83c40c                   add        esp, 0xc
004b80e1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b80e4 8b10                     mov        edx, dword ptr [eax]
004b80e6 8991b8000000             mov        dword ptr [ecx + 0xb8], edx
004b80ec 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b80ef 8b80b8000000             mov        eax, dword ptr [eax + 0xb8]
004b80f5 8be5                     mov        esp, ebp
004b80f7 5d                       pop        ebp
004b80f8 c3                       ret        
004b80f9 cc                       int3       
004b80fa cc                       int3       
004b80fb cc                       int3       
004b80fc cc                       int3       
004b80fd cc                       int3       
004b80fe cc                       int3       
004b80ff cc                       int3       
