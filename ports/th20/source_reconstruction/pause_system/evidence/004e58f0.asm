004e58f0 55                       push       ebp
004e58f1 8bec                     mov        ebp, esp
004e58f3 51                       push       ecx
004e58f4 894dfc                   mov        dword ptr [ebp - 4], ecx
004e58f7 b968a55b00               mov        ecx, 0x5ba568
004e58fc e8df7dfdff               call       0x4bd6e0
004e5901 a128a85b00               mov        eax, dword ptr [0x5ba828]
004e5906 8b88e8000000             mov        ecx, dword ptr [eax + 0xe8]
004e590c 83e1ef                   and        ecx, 0xffffffef
004e590f 8b1528a85b00             mov        edx, dword ptr [0x5ba828]
004e5915 898ae8000000             mov        dword ptr [edx + 0xe8], ecx
004e591b 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e591e 51                       push       ecx
004e591f f30f1080c4010000         movss      xmm0, dword ptr [eax + 0x1c4]
004e5927 f30f110424               movss      dword ptr [esp], xmm0
004e592c b9e4ef5a00               mov        ecx, 0x5aefe4
004e5931 e86a39f4ff               call       0x4292a0
004e5936 90                       nop        
004e5937 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e593d e81e28f9ff               call       0x478160
004e5942 85c0                     test       eax, eax
004e5944 7413                     je         0x4e5959
004e5946 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e594c e81f27fdff               call       0x4b8070
004e5951 8bc8                     mov        ecx, eax
004e5953 e8282ffdff               call       0x4b8880
004e5958 90                       nop        
004e5959 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e595f e88c2efdff               call       0x4b87f0
004e5964 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5967 8b91ec000000             mov        edx, dword ptr [ecx + 0xec]
004e596d 891558885b00             mov        dword ptr [0x5b8858], edx
004e5973 8be5                     mov        esp, ebp
004e5975 5d                       pop        ebp
004e5976 c3                       ret        
004e5977 cc                       int3       
004e5978 cc                       int3       
004e5979 cc                       int3       
004e597a cc                       int3       
004e597b cc                       int3       
004e597c cc                       int3       
004e597d cc                       int3       
004e597e cc                       int3       
004e597f cc                       int3       
