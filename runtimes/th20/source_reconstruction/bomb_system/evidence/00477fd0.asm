00477fd0 55                       push       ebp
00477fd1 8bec                     mov        ebp, esp
00477fd3 8b4508                   mov        eax, dword ptr [ebp + 8]
00477fd6 50                       push       eax
00477fd7 b968a55b00               mov        ecx, 0x5ba568
00477fdc e8df3bf9ff               call       0x40bbc0
00477fe1 8bc8                     mov        ecx, eax
00477fe3 e8c8510400               call       0x4bd1b0
00477fe8 90                       nop        
00477fe9 5d                       pop        ebp
00477fea c3                       ret        
00477feb cc                       int3       
00477fec cc                       int3       
00477fed cc                       int3       
00477fee cc                       int3       
00477fef cc                       int3       
