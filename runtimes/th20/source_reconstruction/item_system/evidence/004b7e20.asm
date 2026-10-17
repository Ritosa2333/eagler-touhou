004b7e20 55                       push       ebp
004b7e21 8bec                     mov        ebp, esp
004b7e23 83ec0c                   sub        esp, 0xc
004b7e26 894dfc                   mov        dword ptr [ebp - 4], ecx
004b7e29 c745f807000000           mov        dword ptr [ebp - 8], 7
004b7e30 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004b7e37 8d45f8                   lea        eax, [ebp - 8]
004b7e3a 50                       push       eax
004b7e3b 8d4df4                   lea        ecx, [ebp - 0xc]
004b7e3e 51                       push       ecx
004b7e3f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b7e42 81c2d8000000             add        edx, 0xd8
004b7e48 52                       push       edx
004b7e49 e82293fbff               call       0x471170
004b7e4e 83c40c                   add        esp, 0xc
004b7e51 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b7e54 8b10                     mov        edx, dword ptr [eax]
004b7e56 8991d8000000             mov        dword ptr [ecx + 0xd8], edx
004b7e5c 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b7e5f 8b80d8000000             mov        eax, dword ptr [eax + 0xd8]
004b7e65 8be5                     mov        esp, ebp
004b7e67 5d                       pop        ebp
004b7e68 c3                       ret        
004b7e69 cc                       int3       
004b7e6a cc                       int3       
004b7e6b cc                       int3       
004b7e6c cc                       int3       
004b7e6d cc                       int3       
004b7e6e cc                       int3       
004b7e6f cc                       int3       
