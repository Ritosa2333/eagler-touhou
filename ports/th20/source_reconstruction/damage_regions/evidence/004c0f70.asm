004c0f70 55                       push       ebp
004c0f71 8bec                     mov        ebp, esp
004c0f73 83ec0c                   sub        esp, 0xc
004c0f76 894dfc                   mov        dword ptr [ebp - 4], ecx
004c0f79 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0f7c e85f000000               call       0x4c0fe0
004c0f81 8bc8                     mov        ecx, eax
004c0f83 e808f0f4ff               call       0x40ff90
004c0f88 8945f4                   mov        dword ptr [ebp - 0xc], eax
004c0f8b 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c0f8e 8b8890000000             mov        ecx, dword ptr [eax + 0x90]
004c0f94 894df8                   mov        dword ptr [ebp - 8], ecx
004c0f97 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c0f9a 52                       push       edx
004c0f9b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c0f9e e80d000000               call       0x4c0fb0
004c0fa3 8be5                     mov        esp, ebp
004c0fa5 5d                       pop        ebp
004c0fa6 c3                       ret        
004c0fa7 cc                       int3       
004c0fa8 cc                       int3       
004c0fa9 cc                       int3       
004c0faa cc                       int3       
004c0fab cc                       int3       
004c0fac cc                       int3       
004c0fad cc                       int3       
004c0fae cc                       int3       
004c0faf cc                       int3       
