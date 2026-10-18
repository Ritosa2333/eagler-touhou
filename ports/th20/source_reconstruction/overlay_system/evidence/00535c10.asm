00535c10 55                       push       ebp
00535c11 8bec                     mov        ebp, esp
00535c13 51                       push       ecx
00535c14 894dfc                   mov        dword ptr [ebp - 4], ecx
00535c17 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00535c1a e8b19effff               call       0x52fad0
00535c1f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00535c22 c70038645700             mov        dword ptr [eax], 0x576438
00535c28 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00535c2b 83c138                   add        ecx, 0x38
00535c2e e85dd1eeff               call       0x422d90
00535c33 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00535c36 83c148                   add        ecx, 0x48
00535c39 e852d1eeff               call       0x422d90
00535c3e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00535c41 c7415800000000           mov        dword ptr [ecx + 0x58], 0
00535c48 8b45fc                   mov        eax, dword ptr [ebp - 4]
00535c4b 8be5                     mov        esp, ebp
00535c4d 5d                       pop        ebp
00535c4e c3                       ret        
00535c4f cc                       int3       
