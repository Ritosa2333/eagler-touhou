00465960 55                       push       ebp
00465961 8bec                     mov        ebp, esp
00465963 51                       push       ecx
00465964 8b4508                   mov        eax, dword ptr [ebp + 8]
00465967 50                       push       eax
00465968 68b0f05600               push       0x56f0b0
0046596d 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00465973 e8c8ecffff               call       0x464640
00465978 8945fc                   mov        dword ptr [ebp - 4], eax
0046597b 6a34                     push       0x34
0046597d 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00465980 e8cba9feff               call       0x450350
00465985 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
00465988 51                       push       ecx
00465989 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0046598c 52                       push       edx
0046598d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00465990 e83bfeffff               call       0x4657d0
00465995 8be5                     mov        esp, ebp
00465997 5d                       pop        ebp
00465998 c3                       ret        
00465999 cc                       int3       
0046599a cc                       int3       
0046599b cc                       int3       
0046599c cc                       int3       
0046599d cc                       int3       
0046599e cc                       int3       
0046599f cc                       int3       
