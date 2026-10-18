0052da70 55                       push       ebp
0052da71 8bec                     mov        ebp, esp
0052da73 6aff                     push       -1
0052da75 6800765600               push       0x567600
0052da7a 64a100000000             mov        eax, dword ptr fs:[0]
0052da80 50                       push       eax
0052da81 51                       push       ecx
0052da82 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052da87 33c5                     xor        eax, ebp
0052da89 50                       push       eax
0052da8a 8d45f4                   lea        eax, [ebp - 0xc]
0052da8d 64a300000000             mov        dword ptr fs:[0], eax
0052da93 894df0                   mov        dword ptr [ebp - 0x10], ecx
0052da96 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052da99 e88222efff               call       0x41fd20
0052da9e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0052daa1 c700dc565700             mov        dword ptr [eax], 0x5756dc
0052daa7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052daaa c7411000000000           mov        dword ptr [ecx + 0x10], 0
0052dab1 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052dab4 83c114                   add        ecx, 0x14
0052dab7 e804feffff               call       0x52d8c0
0052dabc 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0052dabf c7422c00000000           mov        dword ptr [edx + 0x2c], 0
0052dac6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0052dac9 c7403000000000           mov        dword ptr [eax + 0x30], 0
0052dad0 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052dad3 c7413400000000           mov        dword ptr [ecx + 0x34], 0
0052dada 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052dadd 83c138                   add        ecx, 0x38
0052dae0 e8ab52efff               call       0x422d90
0052dae5 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052dae8 83c148                   add        ecx, 0x48
0052daeb e8d081efff               call       0x425cc0
0052daf0 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052daf3 83c14c                   add        ecx, 0x4c
0052daf6 e8c581efff               call       0x425cc0
0052dafb 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052dafe 83c150                   add        ecx, 0x50
0052db01 e8ba81efff               call       0x425cc0
0052db06 68e8565700               push       0x5756e8
0052db0b e8a0ebedff               call       0x40c6b0
0052db10 83c404                   add        esp, 4
0052db13 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0052db16 891558685c00             mov        dword ptr [0x5c6858], edx
0052db1c 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0052db1f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052db22 64890d00000000           mov        dword ptr fs:[0], ecx
0052db29 59                       pop        ecx
0052db2a 8be5                     mov        esp, ebp
0052db2c 5d                       pop        ebp
0052db2d c3                       ret        
0052db2e cc                       int3       
0052db2f cc                       int3       
0052db30 cc                       int3       
0052db31 cc                       int3       
0052db32 cc                       int3       
0052db33 cc                       int3       
0052db34 cc                       int3       
0052db35 cc                       int3       
0052db36 cc                       int3       
0052db37 cc                       int3       
0052db38 cc                       int3       
0052db39 cc                       int3       
0052db3a cc                       int3       
0052db3b cc                       int3       
0052db3c cc                       int3       
0052db3d cc                       int3       
0052db3e cc                       int3       
0052db3f cc                       int3       
