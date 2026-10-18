00478040 55                       push       ebp
00478041 8bec                     mov        ebp, esp
00478043 8b4508                   mov        eax, dword ptr [ebp + 8]
00478046 50                       push       eax
00478047 b968a55b00               mov        ecx, 0x5ba568
0047804c e86f3bf9ff               call       0x40bbc0
00478051 8bc8                     mov        ecx, eax
00478053 e8b8bffaff               call       0x424010
00478058 5d                       pop        ebp
00478059 c3                       ret        
0047805a cc                       int3       
0047805b cc                       int3       
0047805c cc                       int3       
0047805d cc                       int3       
0047805e cc                       int3       
0047805f cc                       int3       
