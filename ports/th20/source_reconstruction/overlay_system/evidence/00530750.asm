00530750 55                       push       ebp
00530751 8bec                     mov        ebp, esp
00530753 83ec14                   sub        esp, 0x14
00530756 894dfc                   mov        dword ptr [ebp - 4], ecx
00530759 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053075c 0fb64834                 movzx      ecx, byte ptr [eax + 0x34]
00530760 85c9                     test       ecx, ecx
00530762 7538                     jne        0x53079c
00530764 6a00                     push       0
00530766 e8c500f3ff               call       0x460830
0053076b 83c404                   add        esp, 4
0053076e 8bc8                     mov        ecx, eax
00530770 e83beffcff               call       0x4ff6b0
00530775 8945f4                   mov        dword ptr [ebp - 0xc], eax
00530778 8b55fc                   mov        edx, dword ptr [ebp - 4]
0053077b 6b420414                 imul       eax, dword ptr [edx + 4], 0x14
0053077f 034510                   add        eax, dword ptr [ebp + 0x10]
00530782 8945f8                   mov        dword ptr [ebp - 8], eax
00530785 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00530788 51                       push       ecx
00530789 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0053078c 52                       push       edx
0053078d 8b4508                   mov        eax, dword ptr [ebp + 8]
00530790 50                       push       eax
00530791 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00530794 e8a745fdff               call       0x504d40
00530799 90                       nop        
0053079a eb3a                     jmp        0x5307d6
0053079c 6a00                     push       0
0053079e e88d00f3ff               call       0x460830
005307a3 83c404                   add        esp, 4
005307a6 8bc8                     mov        ecx, eax
005307a8 e803effcff               call       0x4ff6b0
005307ad 8945ec                   mov        dword ptr [ebp - 0x14], eax
005307b0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005307b3 6b510414                 imul       edx, dword ptr [ecx + 4], 0x14
005307b7 8b4510                   mov        eax, dword ptr [ebp + 0x10]
005307ba 8d4c020f                 lea        ecx, [edx + eax + 0xf]
005307be 894df0                   mov        dword ptr [ebp - 0x10], ecx
005307c1 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
005307c4 52                       push       edx
005307c5 8b450c                   mov        eax, dword ptr [ebp + 0xc]
005307c8 50                       push       eax
005307c9 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005307cc 51                       push       ecx
005307cd 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
005307d0 e86b45fdff               call       0x504d40
005307d5 90                       nop        
005307d6 8be5                     mov        esp, ebp
005307d8 5d                       pop        ebp
005307d9 c20c00                   ret        0xc
005307dc cc                       int3       
005307dd cc                       int3       
005307de cc                       int3       
005307df cc                       int3       
