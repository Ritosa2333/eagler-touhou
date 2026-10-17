0047a2a0 55                       push       ebp
0047a2a1 8bec                     mov        ebp, esp
0047a2a3 8b4508                   mov        eax, dword ptr [ebp + 8]
0047a2a6 50                       push       eax
0047a2a7 e884befdff               call       0x456130
0047a2ac d91c24                   fstp       dword ptr [esp]
0047a2af e87cc8fcff               call       0x446b30
0047a2b4 83c404                   add        esp, 4
0047a2b7 5d                       pop        ebp
0047a2b8 c3                       ret        
0047a2b9 cc                       int3       
0047a2ba cc                       int3       
0047a2bb cc                       int3       
0047a2bc cc                       int3       
0047a2bd cc                       int3       
0047a2be cc                       int3       
0047a2bf cc                       int3       
