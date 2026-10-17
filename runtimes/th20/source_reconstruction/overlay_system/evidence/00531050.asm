00531050 55                       push       ebp
00531051 8bec                     mov        ebp, esp
00531053 51                       push       ecx
00531054 8b4508                   mov        eax, dword ptr [ebp + 8]
00531057 50                       push       eax
00531058 8d4dff                   lea        ecx, [ebp - 1]
0053105b e8f0000000               call       0x531150
00531060 8be5                     mov        esp, ebp
00531062 5d                       pop        ebp
00531063 c3                       ret        
00531064 cc                       int3       
00531065 cc                       int3       
00531066 cc                       int3       
00531067 cc                       int3       
00531068 cc                       int3       
00531069 cc                       int3       
0053106a cc                       int3       
0053106b cc                       int3       
0053106c cc                       int3       
0053106d cc                       int3       
0053106e cc                       int3       
0053106f cc                       int3       
