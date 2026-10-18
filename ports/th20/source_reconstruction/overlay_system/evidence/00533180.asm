00533180 55                       push       ebp
00533181 8bec                     mov        ebp, esp
00533183 83ec14                   sub        esp, 0x14
00533186 894dfc                   mov        dword ptr [ebp - 4], ecx
00533189 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053318c 8b482c                   mov        ecx, dword ptr [eax + 0x2c]
0053318f 894df8                   mov        dword ptr [ebp - 8], ecx
00533192 8b55fc                   mov        edx, dword ptr [ebp - 4]
00533195 8b422c                   mov        eax, dword ptr [edx + 0x2c]
00533198 8b08                     mov        ecx, dword ptr [eax]
0053319a 8b5128                   mov        edx, dword ptr [ecx + 0x28]
0053319d 8955f4                   mov        dword ptr [ebp - 0xc], edx
005331a0 8b450c                   mov        eax, dword ptr [ebp + 0xc]
005331a3 50                       push       eax
005331a4 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005331a7 51                       push       ecx
005331a8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005331ab ff55f4                   call       dword ptr [ebp - 0xc]
005331ae 8b55fc                   mov        edx, dword ptr [ebp - 4]
005331b1 8b4230                   mov        eax, dword ptr [edx + 0x30]
005331b4 8945f0                   mov        dword ptr [ebp - 0x10], eax
005331b7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005331ba 8b5130                   mov        edx, dword ptr [ecx + 0x30]
005331bd 8b02                     mov        eax, dword ptr [edx]
005331bf 8b482c                   mov        ecx, dword ptr [eax + 0x2c]
005331c2 894dec                   mov        dword ptr [ebp - 0x14], ecx
005331c5 8b550c                   mov        edx, dword ptr [ebp + 0xc]
005331c8 52                       push       edx
005331c9 8b4508                   mov        eax, dword ptr [ebp + 8]
005331cc 50                       push       eax
005331cd 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005331d0 ff55ec                   call       dword ptr [ebp - 0x14]
005331d3 90                       nop        
005331d4 8be5                     mov        esp, ebp
005331d6 5d                       pop        ebp
005331d7 c20800                   ret        8
005331da cc                       int3       
005331db cc                       int3       
005331dc cc                       int3       
005331dd cc                       int3       
005331de cc                       int3       
005331df cc                       int3       
