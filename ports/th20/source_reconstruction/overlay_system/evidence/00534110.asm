00534110 55                       push       ebp
00534111 8bec                     mov        ebp, esp
00534113 8b4508                   mov        eax, dword ptr [ebp + 8]
00534116 50                       push       eax
00534117 b968a55b00               mov        ecx, 0x5ba568
0053411c e89f7aedff               call       0x40bbc0
00534121 8bc8                     mov        ecx, eax
00534123 e86892f8ff               call       0x4bd390
00534128 90                       nop        
00534129 5d                       pop        ebp
0053412a c3                       ret        
0053412b cc                       int3       
0053412c cc                       int3       
0053412d cc                       int3       
0053412e cc                       int3       
0053412f cc                       int3       
