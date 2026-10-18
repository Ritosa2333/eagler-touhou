00460850 55                       push       ebp
00460851 8bec                     mov        ebp, esp
00460853 51                       push       ecx
00460854 894dfc                   mov        dword ptr [ebp - 4], ecx
00460857 8b45fc                   mov        eax, dword ptr [ebp - 4]
0046085a 0514060000               add        eax, 0x614
0046085f 8be5                     mov        esp, ebp
00460861 5d                       pop        ebp
00460862 c3                       ret        
00460863 cc                       int3       
00460864 cc                       int3       
00460865 cc                       int3       
00460866 cc                       int3       
00460867 cc                       int3       
00460868 cc                       int3       
00460869 cc                       int3       
0046086a cc                       int3       
0046086b cc                       int3       
0046086c cc                       int3       
0046086d cc                       int3       
0046086e cc                       int3       
0046086f cc                       int3       
