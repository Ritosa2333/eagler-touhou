00485920 55                       push       ebp
00485921 8bec                     mov        ebp, esp
00485923 83ec0c                   sub        esp, 0xc
00485926 894dfc                   mov        dword ptr [ebp - 4], ecx
00485929 c745f864000000           mov        dword ptr [ebp - 8], 0x64
00485930 c745f400000000           mov        dword ptr [ebp - 0xc], 0
00485937 8d45f8                   lea        eax, [ebp - 8]
0048593a 50                       push       eax
0048593b 8d4df4                   lea        ecx, [ebp - 0xc]
0048593e 51                       push       ecx
0048593f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485942 81c2b4000000             add        edx, 0xb4
00485948 52                       push       edx
00485949 e822b8feff               call       0x471170
0048594e 83c40c                   add        esp, 0xc
00485951 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485954 8b10                     mov        edx, dword ptr [eax]
00485956 8991b4000000             mov        dword ptr [ecx + 0xb4], edx
0048595c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048595f 8b80b4000000             mov        eax, dword ptr [eax + 0xb4]
00485965 8be5                     mov        esp, ebp
00485967 5d                       pop        ebp
00485968 c3                       ret        
00485969 cc                       int3       
0048596a cc                       int3       
0048596b cc                       int3       
0048596c cc                       int3       
0048596d cc                       int3       
0048596e cc                       int3       
0048596f cc                       int3       
