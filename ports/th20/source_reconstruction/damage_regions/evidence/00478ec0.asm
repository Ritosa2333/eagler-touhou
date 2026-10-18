00478ec0 55                       push       ebp
00478ec1 8bec                     mov        ebp, esp
00478ec3 8b4508                   mov        eax, dword ptr [ebp + 8]
00478ec6 50                       push       eax
00478ec7 b968a55b00               mov        ecx, 0x5ba568
00478ecc e8ef2cf9ff               call       0x40bbc0
00478ed1 8bc8                     mov        ecx, eax
00478ed3 e83898f9ff               call       0x412710
00478ed8 5d                       pop        ebp
00478ed9 c3                       ret        
00478eda cc                       int3       
00478edb cc                       int3       
00478edc cc                       int3       
00478edd cc                       int3       
00478ede cc                       int3       
00478edf cc                       int3       
