00486390 55                       push       ebp
00486391 8bec                     mov        ebp, esp
00486393 83ec08                   sub        esp, 8
00486396 894dfc                   mov        dword ptr [ebp - 4], ecx
00486399 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048639c 05084e2800               add        eax, 0x284e08
004863a1 8945f8                   mov        dword ptr [ebp - 8], eax
004863a4 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004863a7 51                       push       ecx
004863a8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004863ab e8d0e1f8ff               call       0x414580
004863b0 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004863b3 8910                     mov        dword ptr [eax], edx
004863b5 8be5                     mov        esp, ebp
004863b7 5d                       pop        ebp
004863b8 c20800                   ret        8
004863bb cc                       int3       
004863bc cc                       int3       
004863bd cc                       int3       
004863be cc                       int3       
004863bf cc                       int3       
