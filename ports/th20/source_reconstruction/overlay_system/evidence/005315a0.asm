005315a0 55                       push       ebp
005315a1 8bec                     mov        ebp, esp
005315a3 51                       push       ecx
005315a4 8b4508                   mov        eax, dword ptr [ebp + 8]
005315a7 50                       push       eax
005315a8 8d4dff                   lea        ecx, [ebp - 1]
005315ab e810010000               call       0x5316c0
005315b0 8be5                     mov        esp, ebp
005315b2 5d                       pop        ebp
005315b3 c3                       ret        
005315b4 cc                       int3       
005315b5 cc                       int3       
005315b6 cc                       int3       
005315b7 cc                       int3       
005315b8 cc                       int3       
005315b9 cc                       int3       
005315ba cc                       int3       
005315bb cc                       int3       
005315bc cc                       int3       
005315bd cc                       int3       
005315be cc                       int3       
005315bf cc                       int3       
