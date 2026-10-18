004779b0 55                       push       ebp
004779b1 8bec                     mov        ebp, esp
004779b3 6aff                     push       -1
004779b5 6800765600               push       0x567600
004779ba 64a100000000             mov        eax, dword ptr fs:[0]
004779c0 50                       push       eax
004779c1 51                       push       ecx
004779c2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004779c7 33c5                     xor        eax, ebp
004779c9 50                       push       eax
004779ca 8d45f4                   lea        eax, [ebp - 0xc]
004779cd 64a300000000             mov        dword ptr fs:[0], eax
004779d3 894df0                   mov        dword ptr [ebp - 0x10], ecx
004779d6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004779d9 e84283faff               call       0x41fd20
004779de 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004779e1 c70094fa5600             mov        dword ptr [eax], 0x56fa94
004779e7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004779ea c7411000000000           mov        dword ptr [ecx + 0x10], 0
004779f1 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004779f4 c7421400000000           mov        dword ptr [edx + 0x14], 0
004779fb 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004779fe c7401800000000           mov        dword ptr [eax + 0x18], 0
00477a05 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00477a08 83c11c                   add        ecx, 0x1c
00477a0b e880b3faff               call       0x422d90
00477a10 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00477a13 c7412c00000000           mov        dword ptr [ecx + 0x2c], 0
00477a1a 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00477a1d c7423000000000           mov        dword ptr [edx + 0x30], 0
00477a24 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00477a27 c7403400000000           mov        dword ptr [eax + 0x34], 0
00477a2e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00477a31 c7413800000000           mov        dword ptr [ecx + 0x38], 0
00477a38 68a0fa5600               push       0x56faa0
00477a3d e86e4cf9ff               call       0x40c6b0
00477a42 83c404                   add        esp, 4
00477a45 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00477a48 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00477a4b 64890d00000000           mov        dword ptr fs:[0], ecx
00477a52 59                       pop        ecx
00477a53 8be5                     mov        esp, ebp
00477a55 5d                       pop        ebp
00477a56 c3                       ret        
00477a57 cc                       int3       
00477a58 cc                       int3       
00477a59 cc                       int3       
00477a5a cc                       int3       
00477a5b cc                       int3       
00477a5c cc                       int3       
00477a5d cc                       int3       
00477a5e cc                       int3       
00477a5f cc                       int3       
