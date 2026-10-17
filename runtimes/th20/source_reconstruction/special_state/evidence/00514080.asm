00514080 55                       push       ebp
00514081 8bec                     mov        ebp, esp
00514083 83ec08                   sub        esp, 8
00514086 894dfc                   mov        dword ptr [ebp - 4], ecx
00514089 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051408c 83c014                   add        eax, 0x14
0051408f 8945f8                   mov        dword ptr [ebp - 8], eax
00514092 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00514095 8b5124                   mov        edx, dword ptr [ecx + 0x24]
00514098 52                       push       edx
00514099 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0051409c e8bff4f0ff               call       0x423560
005140a1 8be5                     mov        esp, ebp
005140a3 5d                       pop        ebp
005140a4 c3                       ret        
005140a5 cc                       int3       
005140a6 cc                       int3       
005140a7 cc                       int3       
005140a8 cc                       int3       
005140a9 cc                       int3       
005140aa cc                       int3       
005140ab cc                       int3       
005140ac cc                       int3       
005140ad cc                       int3       
005140ae cc                       int3       
005140af cc                       int3       
