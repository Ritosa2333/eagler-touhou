00498fd0 55                       push       ebp
00498fd1 8bec                     mov        ebp, esp
00498fd3 8b4508                   mov        eax, dword ptr [ebp + 8]
00498fd6 50                       push       eax
00498fd7 b968a55b00               mov        ecx, 0x5ba568
00498fdc e8df2bf7ff               call       0x40bbc0
00498fe1 8bc8                     mov        ecx, eax
00498fe3 e8c83af8ff               call       0x41cab0
00498fe8 5d                       pop        ebp
00498fe9 c3                       ret        
00498fea cc                       int3       
00498feb cc                       int3       
00498fec cc                       int3       
00498fed cc                       int3       
00498fee cc                       int3       
00498fef cc                       int3       
