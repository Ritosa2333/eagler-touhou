00534130 55                       push       ebp
00534131 8bec                     mov        ebp, esp
00534133 51                       push       ecx
00534134 894dfc                   mov        dword ptr [ebp - 4], ecx
00534137 6a00                     push       0
00534139 e8f2c6f2ff               call       0x460830
0053413e 83c404                   add        esp, 4
00534141 8bc8                     mov        ecx, eax
00534143 e898b4fcff               call       0x4ff5e0
00534148 0fb6c0                   movzx      eax, al
0053414b 85c0                     test       eax, eax
0053414d 7417                     je         0x534166
0053414f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534152 8b512c                   mov        edx, dword ptr [ecx + 0x2c]
00534155 8b45fc                   mov        eax, dword ptr [ebp - 4]
00534158 8b12                     mov        edx, dword ptr [edx]
0053415a 8b482c                   mov        ecx, dword ptr [eax + 0x2c]
0053415d 8b424c                   mov        eax, dword ptr [edx + 0x4c]
00534160 ffd0                     call       eax
00534162 eb15                     jmp        0x534179
00534164 eb13                     jmp        0x534179
00534166 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534169 8b5130                   mov        edx, dword ptr [ecx + 0x30]
0053416c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053416f 8b12                     mov        edx, dword ptr [edx]
00534171 8b4830                   mov        ecx, dword ptr [eax + 0x30]
00534174 8b424c                   mov        eax, dword ptr [edx + 0x4c]
00534177 ffd0                     call       eax
00534179 8be5                     mov        esp, ebp
0053417b 5d                       pop        ebp
0053417c c3                       ret        
0053417d cc                       int3       
0053417e cc                       int3       
0053417f cc                       int3       
