004a9fb0 55                       push       ebp
004a9fb1 8bec                     mov        ebp, esp
004a9fb3 83ec0c                   sub        esp, 0xc
004a9fb6 894dfc                   mov        dword ptr [ebp - 4], ecx
004a9fb9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a9fbc 8b4844                   mov        ecx, dword ptr [eax + 0x44]
004a9fbf 034d08                   add        ecx, dword ptr [ebp + 8]
004a9fc2 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a9fc5 894a44                   mov        dword ptr [edx + 0x44], ecx
004a9fc8 c745f840420f00           mov        dword ptr [ebp - 8], 0xf4240
004a9fcf c745f400000000           mov        dword ptr [ebp - 0xc], 0
004a9fd6 8d45f8                   lea        eax, [ebp - 8]
004a9fd9 50                       push       eax
004a9fda 8d4df4                   lea        ecx, [ebp - 0xc]
004a9fdd 51                       push       ecx
004a9fde 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a9fe1 83c244                   add        edx, 0x44
004a9fe4 52                       push       edx
004a9fe5 e88671fcff               call       0x471170
004a9fea 83c40c                   add        esp, 0xc
004a9fed 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a9ff0 8b10                     mov        edx, dword ptr [eax]
004a9ff2 895144                   mov        dword ptr [ecx + 0x44], edx
004a9ff5 8be5                     mov        esp, ebp
004a9ff7 5d                       pop        ebp
004a9ff8 c20400                   ret        4
004a9ffb cc                       int3       
004a9ffc cc                       int3       
004a9ffd cc                       int3       
004a9ffe cc                       int3       
004a9fff cc                       int3       
