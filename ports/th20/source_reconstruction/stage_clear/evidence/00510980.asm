00510980 55                       push       ebp
00510981 8bec                     mov        ebp, esp
00510983 6aff                     push       -1
00510985 6800765600               push       0x567600
0051098a 64a100000000             mov        eax, dword ptr fs:[0]
00510990 50                       push       eax
00510991 51                       push       ecx
00510992 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00510997 33c5                     xor        eax, ebp
00510999 50                       push       eax
0051099a 8d45f4                   lea        eax, [ebp - 0xc]
0051099d 64a300000000             mov        dword ptr fs:[0], eax
005109a3 894df0                   mov        dword ptr [ebp - 0x10], ecx
005109a6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005109a9 e872f3f0ff               call       0x41fd20
005109ae 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
005109b1 c700c43e5700             mov        dword ptr [eax], 0x573ec4
005109b7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005109ba c7411000000000           mov        dword ptr [ecx + 0x10], 0
005109c1 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005109c4 83c114                   add        ecx, 0x14
005109c7 e8f452f1ff               call       0x425cc0
005109cc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005109cf 83c118                   add        ecx, 0x18
005109d2 e8e952f1ff               call       0x425cc0
005109d7 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
005109da c7421c00000000           mov        dword ptr [edx + 0x1c], 0
005109e1 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
005109e4 c7402000000000           mov        dword ptr [eax + 0x20], 0
005109eb 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005109ee c7412400000000           mov        dword ptr [ecx + 0x24], 0
005109f5 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
005109f8 c7422800000000           mov        dword ptr [edx + 0x28], 0
005109ff 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00510a02 83c12c                   add        ecx, 0x2c
00510a05 e88623f1ff               call       0x422d90
00510a0a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00510a0d 83c13c                   add        ecx, 0x3c
00510a10 e87b23f1ff               call       0x422d90
00510a15 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00510a18 c7404c00000000           mov        dword ptr [eax + 0x4c], 0
00510a1f 6a40                     push       0x40
00510a21 6a00                     push       0
00510a23 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00510a26 83c150                   add        ecx, 0x50
00510a29 51                       push       ecx
00510a2a e8313d0300               call       0x544760
00510a2f 83c40c                   add        esp, 0xc
00510a32 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00510a35 c7829000000000000000     mov        dword ptr [edx + 0x90], 0
00510a3f 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00510a42 c7809400000000000000     mov        dword ptr [eax + 0x94], 0
00510a4c 68d03e5700               push       0x573ed0
00510a51 e85abcefff               call       0x40c6b0
00510a56 83c404                   add        esp, 4
00510a59 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00510a5c 890d14615c00             mov        dword ptr [0x5c6114], ecx
00510a62 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00510a65 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00510a68 64890d00000000           mov        dword ptr fs:[0], ecx
00510a6f 59                       pop        ecx
00510a70 8be5                     mov        esp, ebp
00510a72 5d                       pop        ebp
00510a73 c3                       ret        
00510a74 cc                       int3       
00510a75 cc                       int3       
00510a76 cc                       int3       
00510a77 cc                       int3       
00510a78 cc                       int3       
00510a79 cc                       int3       
00510a7a cc                       int3       
00510a7b cc                       int3       
00510a7c cc                       int3       
00510a7d cc                       int3       
00510a7e cc                       int3       
00510a7f cc                       int3       
