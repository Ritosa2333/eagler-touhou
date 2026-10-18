00534210 55                       push       ebp
00534211 8bec                     mov        ebp, esp
00534213 51                       push       ecx
00534214 894dfc                   mov        dword ptr [ebp - 4], ecx
00534217 6a00                     push       0
00534219 e812c6f2ff               call       0x460830
0053421e 83c404                   add        esp, 4
00534221 8bc8                     mov        ecx, eax
00534223 e8b8b3fcff               call       0x4ff5e0
00534228 0fb6c0                   movzx      eax, al
0053422b 85c0                     test       eax, eax
0053422d 7417                     je         0x534246
0053422f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534232 8b512c                   mov        edx, dword ptr [ecx + 0x2c]
00534235 8b45fc                   mov        eax, dword ptr [ebp - 4]
00534238 8b12                     mov        edx, dword ptr [edx]
0053423a 8b482c                   mov        ecx, dword ptr [eax + 0x2c]
0053423d 8b4264                   mov        eax, dword ptr [edx + 0x64]
00534240 ffd0                     call       eax
00534242 eb15                     jmp        0x534259
00534244 eb13                     jmp        0x534259
00534246 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534249 8b5130                   mov        edx, dword ptr [ecx + 0x30]
0053424c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053424f 8b12                     mov        edx, dword ptr [edx]
00534251 8b4830                   mov        ecx, dword ptr [eax + 0x30]
00534254 8b4264                   mov        eax, dword ptr [edx + 0x64]
00534257 ffd0                     call       eax
00534259 8be5                     mov        esp, ebp
0053425b 5d                       pop        ebp
0053425c c3                       ret        
0053425d cc                       int3       
0053425e cc                       int3       
0053425f cc                       int3       
