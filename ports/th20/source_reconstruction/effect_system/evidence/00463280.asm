00463280 55                       push       ebp
00463281 8bec                     mov        ebp, esp
00463283 51                       push       ecx
00463284 8b4508                   mov        eax, dword ptr [ebp + 8]
00463287 50                       push       eax
00463288 6838f05600               push       0x56f038
0046328d 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00463293 e898e2ffff               call       0x461530
00463298 8945fc                   mov        dword ptr [ebp - 4], eax
0046329b 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
0046329e 51                       push       ecx
0046329f 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004632a2 52                       push       edx
004632a3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004632a6 e8d5f9ffff               call       0x462c80
004632ab 8be5                     mov        esp, ebp
004632ad 5d                       pop        ebp
004632ae c3                       ret        
004632af cc                       int3       
