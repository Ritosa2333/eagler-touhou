00536080 55                       push       ebp
00536081 8bec                     mov        ebp, esp
00536083 83ec24                   sub        esp, 0x24
00536086 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00536089 c745f484695c00           mov        dword ptr [ebp - 0xc], 0x5c6984
00536090 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00536093 e8085dedff               call       0x40bda0
00536098 8945fc                   mov        dword ptr [ebp - 4], eax
0053609b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0053609e e8cdb8eeff               call       0x421970
005360a3 8945e8                   mov        dword ptr [ebp - 0x18], eax
005360a6 eb09                     jmp        0x5360b1
005360a8 8b45fc                   mov        eax, dword ptr [ebp - 4]
005360ab 83c004                   add        eax, 4
005360ae 8945fc                   mov        dword ptr [ebp - 4], eax
005360b1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005360b4 3b4de8                   cmp        ecx, dword ptr [ebp - 0x18]
005360b7 741b                     je         0x5360d4
005360b9 8b55fc                   mov        edx, dword ptr [ebp - 4]
005360bc 8955f0                   mov        dword ptr [ebp - 0x10], edx
005360bf 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005360c2 e8099cf1ff               call       0x44fcd0
005360c7 6a00                     push       0
005360c9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005360cc e8cfb6edff               call       0x4117a0
005360d1 90                       nop        
005360d2 ebd4                     jmp        0x5360a8
005360d4 c745ec98695c00           mov        dword ptr [ebp - 0x14], 0x5c6998
005360db 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
005360de e8bd5cedff               call       0x40bda0
005360e3 8945f8                   mov        dword ptr [ebp - 8], eax
005360e6 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
005360e9 e882b8eeff               call       0x421970
005360ee 8945e4                   mov        dword ptr [ebp - 0x1c], eax
005360f1 eb09                     jmp        0x5360fc
005360f3 8b45f8                   mov        eax, dword ptr [ebp - 8]
005360f6 83c004                   add        eax, 4
005360f9 8945f8                   mov        dword ptr [ebp - 8], eax
005360fc 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005360ff 3b4de4                   cmp        ecx, dword ptr [ebp - 0x1c]
00536102 7411                     je         0x536115
00536104 8b55f8                   mov        edx, dword ptr [ebp - 8]
00536107 8955e0                   mov        dword ptr [ebp - 0x20], edx
0053610a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0053610d e8eebdf8ff               call       0x4c1f00
00536112 90                       nop        
00536113 ebde                     jmp        0x5360f3
00536115 8be5                     mov        esp, ebp
00536117 5d                       pop        ebp
00536118 c3                       ret        
00536119 cc                       int3       
0053611a cc                       int3       
0053611b cc                       int3       
0053611c cc                       int3       
0053611d cc                       int3       
0053611e cc                       int3       
0053611f cc                       int3       
