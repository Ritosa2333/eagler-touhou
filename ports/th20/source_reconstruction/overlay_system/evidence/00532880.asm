00532880 55                       push       ebp
00532881 8bec                     mov        ebp, esp
00532883 51                       push       ecx
00532884 894dfc                   mov        dword ptr [ebp - 4], ecx
00532887 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053288a e891d4eeff               call       0x41fd20
0053288f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00532892 c70028625700             mov        dword ptr [eax], 0x576228
00532898 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053289b 83c110                   add        ecx, 0x10
0053289e e8ed04efff               call       0x422d90
005328a3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005328a6 83c120                   add        ecx, 0x20
005328a9 e81234efff               call       0x425cc0
005328ae 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005328b1 c7412400000000           mov        dword ptr [ecx + 0x24], 0
005328b8 8b55fc                   mov        edx, dword ptr [ebp - 4]
005328bb c7422800000000           mov        dword ptr [edx + 0x28], 0
005328c2 8b45fc                   mov        eax, dword ptr [ebp - 4]
005328c5 c7402c00000000           mov        dword ptr [eax + 0x2c], 0
005328cc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005328cf c7413000000000           mov        dword ptr [ecx + 0x30], 0
005328d6 8b55fc                   mov        edx, dword ptr [ebp - 4]
005328d9 c7423400000000           mov        dword ptr [edx + 0x34], 0
005328e0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005328e3 83c138                   add        ecx, 0x38
005328e6 e865ffffff               call       0x532850
005328eb 8b45fc                   mov        eax, dword ptr [ebp - 4]
005328ee c7404000000000           mov        dword ptr [eax + 0x40], 0
005328f5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005328f8 83c144                   add        ecx, 0x44
005328fb e89004efff               call       0x422d90
00532900 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00532903 c7415400000000           mov        dword ptr [ecx + 0x54], 0
0053290a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053290d 83c158                   add        ecx, 0x58
00532910 e8fb04efff               call       0x422e10
00532915 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00532918 83c164                   add        ecx, 0x64
0053291b e87004efff               call       0x422d90
00532920 8b55fc                   mov        edx, dword ptr [ebp - 4]
00532923 0f57c0                   xorps      xmm0, xmm0
00532926 f30f114274               movss      dword ptr [edx + 0x74], xmm0
0053292b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053292e c6407800                 mov        byte ptr [eax + 0x78], 0
00532932 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00532935 c7417c00000000           mov        dword ptr [ecx + 0x7c], 0
0053293c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0053293f c7828000000000000000     mov        dword ptr [edx + 0x80], 0
00532949 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053294c 8be5                     mov        esp, ebp
0053294e 5d                       pop        ebp
0053294f c3                       ret        
