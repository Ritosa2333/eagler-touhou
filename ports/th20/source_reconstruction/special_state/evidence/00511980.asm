00511980 55                       push       ebp
00511981 8bec                     mov        ebp, esp
00511983 51                       push       ecx
00511984 894dfc                   mov        dword ptr [ebp - 4], ecx
00511987 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051198a e8c170f0ff               call       0x418a50
0051198f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511992 83c118                   add        ecx, 0x18
00511995 e8f613f1ff               call       0x422d90
0051199a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051199d 83c128                   add        ecx, 0x28
005119a0 e8eb13f1ff               call       0x422d90
005119a5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005119a8 83c138                   add        ecx, 0x38
005119ab e83060f3ff               call       0x4479e0
005119b0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005119b3 83c164                   add        ecx, 0x64
005119b6 e875ffffff               call       0x511930
005119bb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005119be 81c184000000             add        ecx, 0x84
005119c4 e81760f3ff               call       0x4479e0
005119c9 8b45fc                   mov        eax, dword ptr [ebp - 4]
005119cc c680b000000000           mov        byte ptr [eax + 0xb0], 0
005119d3 8b45fc                   mov        eax, dword ptr [ebp - 4]
005119d6 8be5                     mov        esp, ebp
005119d8 5d                       pop        ebp
005119d9 c3                       ret        
005119da cc                       int3       
005119db cc                       int3       
005119dc cc                       int3       
005119dd cc                       int3       
005119de cc                       int3       
005119df cc                       int3       
