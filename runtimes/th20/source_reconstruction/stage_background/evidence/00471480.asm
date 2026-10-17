00471480 55                       push       ebp
00471481 8bec                     mov        ebp, esp
00471483 6aff                     push       -1
00471485 6800765600               push       0x567600
0047148a 64a100000000             mov        eax, dword ptr fs:[0]
00471490 50                       push       eax
00471491 51                       push       ecx
00471492 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00471497 33c5                     xor        eax, ebp
00471499 50                       push       eax
0047149a 8d45f4                   lea        eax, [ebp - 0xc]
0047149d 64a300000000             mov        dword ptr fs:[0], eax
004714a3 894df0                   mov        dword ptr [ebp - 0x10], ecx
004714a6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004714a9 e872e8faff               call       0x41fd20
004714ae 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004714b1 c7000cf85600             mov        dword ptr [eax], 0x56f80c
004714b7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004714ba 83c110                   add        ecx, 0x10
004714bd e8fe000000               call       0x4715c0
004714c2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004714c5 c7812033000000000000     mov        dword ptr [ecx + 0x3320], 0
004714cf 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004714d2 c7822433000000000000     mov        dword ptr [edx + 0x3324], 0
004714dc 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004714df c7802833000000000000     mov        dword ptr [eax + 0x3328], 0
004714e9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004714ec c7812c33000000000000     mov        dword ptr [ecx + 0x332c], 0
004714f6 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004714f9 c7823033000000000000     mov        dword ptr [edx + 0x3330], 0
00471503 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00471506 c7803433000000000000     mov        dword ptr [eax + 0x3334], 0
00471510 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00471513 c7813833000000000000     mov        dword ptr [ecx + 0x3338], 0
0047151d 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00471520 c7823c33000000000000     mov        dword ptr [edx + 0x333c], 0
0047152a 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047152d c7804033000000000000     mov        dword ptr [eax + 0x3340], 0
00471537 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047153a 81c144330000             add        ecx, 0x3344
00471540 e87bfeffff               call       0x4713c0
00471545 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00471548 81c148330000             add        ecx, 0x3348
0047154e e83d18fbff               call       0x422d90
00471553 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00471556 c7815833000000000000     mov        dword ptr [ecx + 0x3358], 0
00471560 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00471563 c7825c33000000000000     mov        dword ptr [edx + 0x335c], 0
0047156d 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00471570 c7806033000000000000     mov        dword ptr [eax + 0x3360], 0
0047157a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047157d c7816433000000000000     mov        dword ptr [ecx + 0x3364], 0
00471587 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0047158a c7826833000000000000     mov        dword ptr [edx + 0x3368], 0
00471594 6818f85600               push       0x56f818
00471599 e812b1f9ff               call       0x40c6b0
0047159e 83c404                   add        esp, 4
004715a1 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004715a4 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004715a7 64890d00000000           mov        dword ptr fs:[0], ecx
004715ae 59                       pop        ecx
004715af 8be5                     mov        esp, ebp
004715b1 5d                       pop        ebp
004715b2 c3                       ret        
004715b3 cc                       int3       
004715b4 cc                       int3       
004715b5 cc                       int3       
004715b6 cc                       int3       
004715b7 cc                       int3       
004715b8 cc                       int3       
004715b9 cc                       int3       
004715ba cc                       int3       
004715bb cc                       int3       
004715bc cc                       int3       
004715bd cc                       int3       
004715be cc                       int3       
004715bf cc                       int3       
