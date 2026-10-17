004bd5a0 55                       push       ebp
004bd5a1 8bec                     mov        ebp, esp
004bd5a3 8b4508                   mov        eax, dword ptr [ebp + 8]
004bd5a6 50                       push       eax
004bd5a7 b968a55b00               mov        ecx, 0x5ba568
004bd5ac e80fe6f4ff               call       0x40bbc0
004bd5b1 8bc8                     mov        ecx, eax
004bd5b3 e85842f6ff               call       0x421810
004bd5b8 5d                       pop        ebp
004bd5b9 c3                       ret        
004bd5ba cc                       int3       
004bd5bb cc                       int3       
004bd5bc cc                       int3       
004bd5bd cc                       int3       
004bd5be cc                       int3       
004bd5bf cc                       int3       
