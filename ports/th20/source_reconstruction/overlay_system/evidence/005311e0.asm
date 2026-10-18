005311e0 55                       push       ebp
005311e1 8bec                     mov        ebp, esp
005311e3 51                       push       ecx
005311e4 894dfc                   mov        dword ptr [ebp - 4], ecx
005311e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005311ea 83c138                   add        ecx, 0x38
005311ed e8deeaf1ff               call       0x44fcd0
005311f2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005311f5 e8261d0000               call       0x532f20
005311fa 8be5                     mov        esp, ebp
005311fc 5d                       pop        ebp
005311fd c3                       ret        
005311fe cc                       int3       
005311ff cc                       int3       
