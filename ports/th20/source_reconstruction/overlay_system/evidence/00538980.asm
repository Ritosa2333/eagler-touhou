00538980 55                       push       ebp
00538981 8bec                     mov        ebp, esp
00538983 51                       push       ecx
00538984 894dfc                   mov        dword ptr [ebp - 4], ecx
00538987 b9dc695c00               mov        ecx, 0x5c69dc
0053898c e8ef68f1ff               call       0x44f280
00538991 b9dc695c00               mov        ecx, 0x5c69dc
00538996 e88577f1ff               call       0x450120
0053899b 33c0                     xor        eax, eax
0053899d 8be5                     mov        esp, ebp
0053899f 5d                       pop        ebp
005389a0 c3                       ret        
005389a1 cc                       int3       
005389a2 cc                       int3       
005389a3 cc                       int3       
005389a4 cc                       int3       
005389a5 cc                       int3       
005389a6 cc                       int3       
005389a7 cc                       int3       
005389a8 cc                       int3       
005389a9 cc                       int3       
005389aa cc                       int3       
005389ab cc                       int3       
005389ac cc                       int3       
005389ad cc                       int3       
005389ae cc                       int3       
005389af cc                       int3       
