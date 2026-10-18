00425070 55                       push       ebp
00425071 8bec                     mov        ebp, esp
00425073 8b4508                   mov        eax, dword ptr [ebp + 8]
00425076 50                       push       eax
00425077 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0042507d e83ea7ffff               call       0x41f7c0
00425082 c7450800000000           mov        dword ptr [ebp + 8], 0
00425089 33c0                     xor        eax, eax
0042508b 5d                       pop        ebp
0042508c c3                       ret        
0042508d cc                       int3       
0042508e cc                       int3       
0042508f cc                       int3       
