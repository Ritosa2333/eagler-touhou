005347d0 55                       push       ebp
005347d1 8bec                     mov        ebp, esp
005347d3 83ec1c                   sub        esp, 0x1c
005347d6 894dfc                   mov        dword ptr [ebp - 4], ecx
005347d9 8b45fc                   mov        eax, dword ptr [ebp - 4]
005347dc 8b4830                   mov        ecx, dword ptr [eax + 0x30]
005347df 51                       push       ecx
005347e0 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
005347e6 e855dfffff               call       0x532740
005347eb 8b55fc                   mov        edx, dword ptr [ebp - 4]
005347ee c7423000000000           mov        dword ptr [edx + 0x30], 0
005347f5 6a00                     push       0
005347f7 e884f8f2ff               call       0x464080
005347fc 83c404                   add        esp, 4
005347ff 8945f8                   mov        dword ptr [ebp - 8], eax
00534802 6a00                     push       0
00534804 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00534807 e8a4000000               call       0x5348b0
0053480c 90                       nop        
0053480d 837d0c08                 cmp        dword ptr [ebp + 0xc], 8
00534811 7532                     jne        0x534845
00534813 6a00                     push       0
00534815 e866f8f2ff               call       0x464080
0053481a 83c404                   add        esp, 4
0053481d 8945f4                   mov        dword ptr [ebp - 0xc], eax
00534820 6a00                     push       0
00534822 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00534825 e8565afdff               call       0x50a280
0053482a 89450c                   mov        dword ptr [ebp + 0xc], eax
0053482d 6a00                     push       0
0053482f e84cf8f2ff               call       0x464080
00534834 83c404                   add        esp, 4
00534837 8945f0                   mov        dword ptr [ebp - 0x10], eax
0053483a 6a01                     push       1
0053483c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0053483f e86c000000               call       0x5348b0
00534844 90                       nop        
00534845 6a00                     push       0
00534847 e834f8f2ff               call       0x464080
0053484c 83c404                   add        esp, 4
0053484f 8945ec                   mov        dword ptr [ebp - 0x14], eax
00534852 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00534855 50                       push       eax
00534856 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00534859 e8f295eeff               call       0x41de50
0053485e 6b4d0824                 imul       ecx, dword ptr [ebp + 8], 0x24
00534862 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00534865 8b8491b00a5b00           mov        eax, dword ptr [ecx + edx*4 + 0x5b0ab0]
0053486c ffd0                     call       eax
0053486e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534871 894130                   mov        dword ptr [ecx + 0x30], eax
00534874 8b55fc                   mov        edx, dword ptr [ebp - 4]
00534877 8b4230                   mov        eax, dword ptr [edx + 0x30]
0053487a 8945e8                   mov        dword ptr [ebp - 0x18], eax
0053487d 6a01                     push       1
0053487f 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00534882 e839e5edff               call       0x412dc0
00534887 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053488a 8b5130                   mov        edx, dword ptr [ecx + 0x30]
0053488d 8955e4                   mov        dword ptr [ebp - 0x1c], edx
00534890 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00534893 50                       push       eax
00534894 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00534897 e8d4ccedff               call       0x411570
0053489c 90                       nop        
0053489d 8be5                     mov        esp, ebp
0053489f 5d                       pop        ebp
005348a0 c20800                   ret        8
005348a3 cc                       int3       
005348a4 cc                       int3       
005348a5 cc                       int3       
005348a6 cc                       int3       
005348a7 cc                       int3       
005348a8 cc                       int3       
005348a9 cc                       int3       
005348aa cc                       int3       
005348ab cc                       int3       
005348ac cc                       int3       
005348ad cc                       int3       
005348ae cc                       int3       
005348af cc                       int3       
