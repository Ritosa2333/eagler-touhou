0047a1b0 55                       push       ebp
0047a1b1 8bec                     mov        ebp, esp
0047a1b3 51                       push       ecx
0047a1b4 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a1b7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a1ba 81c1b8000000             add        ecx, 0xb8
0047a1c0 e8cbfdffff               call       0x479f90
0047a1c5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a1c8 83c174                   add        ecx, 0x74
0047a1cb e870eeffff               call       0x479040
0047a1d0 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a1d3 50                       push       eax
0047a1d4 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0047a1da e8e155faff               call       0x41f7c0
0047a1df 33c0                     xor        eax, eax
0047a1e1 8be5                     mov        esp, ebp
0047a1e3 5d                       pop        ebp
0047a1e4 c3                       ret        
0047a1e5 cc                       int3       
0047a1e6 cc                       int3       
0047a1e7 cc                       int3       
0047a1e8 cc                       int3       
0047a1e9 cc                       int3       
0047a1ea cc                       int3       
0047a1eb cc                       int3       
0047a1ec cc                       int3       
0047a1ed cc                       int3       
0047a1ee cc                       int3       
0047a1ef cc                       int3       
