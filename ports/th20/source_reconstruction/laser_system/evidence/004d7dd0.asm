004d7dd0 55                       push       ebp
004d7dd1 8bec                     mov        ebp, esp
004d7dd3 51                       push       ecx
004d7dd4 894dfc                   mov        dword ptr [ebp - 4], ecx
004d7dd7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d7dda 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004d7ddd 8988ec060000             mov        dword ptr [eax + 0x6ec], ecx
004d7de3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d7de6 8b82ec060000             mov        eax, dword ptr [edx + 0x6ec]
004d7dec 50                       push       eax
004d7ded b968a55b00               mov        ecx, 0x5ba568
004d7df2 e8c93df3ff               call       0x40bbc0
004d7df7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d7dfa 8981f0060000             mov        dword ptr [ecx + 0x6f0], eax
004d7e00 8be5                     mov        esp, ebp
004d7e02 5d                       pop        ebp
004d7e03 c20400                   ret        4
004d7e06 cc                       int3       
004d7e07 cc                       int3       
004d7e08 cc                       int3       
004d7e09 cc                       int3       
004d7e0a cc                       int3       
004d7e0b cc                       int3       
004d7e0c cc                       int3       
004d7e0d cc                       int3       
004d7e0e cc                       int3       
004d7e0f cc                       int3       
