00478300 55                       push       ebp
00478301 8bec                     mov        ebp, esp
00478303 83ec08                   sub        esp, 8
00478306 68ccfa5600               push       0x56facc
0047830b 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00478311 e84af6ffff               call       0x477960
00478316 8945fc                   mov        dword ptr [ebp - 4], eax
00478319 837dfc00                 cmp        dword ptr [ebp - 4], 0
0047831d 7504                     jne        0x478323
0047831f 33c0                     xor        eax, eax
00478321 eb54                     jmp        0x478377
00478323 8b4508                   mov        eax, dword ptr [ebp + 8]
00478326 50                       push       eax
00478327 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047832a e891faffff               call       0x477dc0
0047832f 85c0                     test       eax, eax
00478331 7404                     je         0x478337
00478333 eb24                     jmp        0x478359
00478335 eb22                     jmp        0x478359
00478337 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047833a 51                       push       ecx
0047833b b968a55b00               mov        ecx, 0x5ba568
00478340 e87b38f9ff               call       0x40bbc0
00478345 8945f8                   mov        dword ptr [ebp - 8], eax
00478348 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047834b 52                       push       edx
0047834c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0047834f e83c5bfaff               call       0x41de90
00478354 8b45fc                   mov        eax, dword ptr [ebp - 4]
00478357 eb1e                     jmp        0x478377
00478359 837dfc00                 cmp        dword ptr [ebp - 4], 0
0047835d 7416                     je         0x478375
0047835f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00478362 50                       push       eax
00478363 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00478369 e85274faff               call       0x41f7c0
0047836e c745fc00000000           mov        dword ptr [ebp - 4], 0
00478375 33c0                     xor        eax, eax
00478377 8be5                     mov        esp, ebp
00478379 5d                       pop        ebp
0047837a c3                       ret        
0047837b cc                       int3       
0047837c cc                       int3       
0047837d cc                       int3       
0047837e cc                       int3       
0047837f cc                       int3       
