00478060 55                       push       ebp
00478061 8bec                     mov        ebp, esp
00478063 8b4508                   mov        eax, dword ptr [ebp + 8]
00478066 50                       push       eax
00478067 b968a55b00               mov        ecx, 0x5ba568
0047806c e84f3bf9ff               call       0x40bbc0
00478071 8bc8                     mov        ecx, eax
00478073 e8b8a6f9ff               call       0x412730
00478078 5d                       pop        ebp
00478079 c3                       ret        
0047807a cc                       int3       
0047807b cc                       int3       
0047807c cc                       int3       
0047807d cc                       int3       
0047807e cc                       int3       
0047807f cc                       int3       
