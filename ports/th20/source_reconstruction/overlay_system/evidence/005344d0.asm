005344d0 55                       push       ebp
005344d1 8bec                     mov        ebp, esp
005344d3 83ec1c                   sub        esp, 0x1c
005344d6 894dfc                   mov        dword ptr [ebp - 4], ecx
005344d9 8b45fc                   mov        eax, dword ptr [ebp - 4]
005344dc 8b482c                   mov        ecx, dword ptr [eax + 0x2c]
005344df 51                       push       ecx
005344e0 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
005344e6 e855e2ffff               call       0x532740
005344eb 8b55fc                   mov        edx, dword ptr [ebp - 4]
005344ee c7422c00000000           mov        dword ptr [edx + 0x2c], 0
005344f5 6a00                     push       0
005344f7 e884fbf2ff               call       0x464080
005344fc 83c404                   add        esp, 4
005344ff 8945f8                   mov        dword ptr [ebp - 8], eax
00534502 6a00                     push       0
00534504 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00534507 e8a4000000               call       0x5345b0
0053450c 90                       nop        
0053450d 837d0c08                 cmp        dword ptr [ebp + 0xc], 8
00534511 7532                     jne        0x534545
00534513 6a00                     push       0
00534515 e866fbf2ff               call       0x464080
0053451a 83c404                   add        esp, 4
0053451d 8945f4                   mov        dword ptr [ebp - 0xc], eax
00534520 6a00                     push       0
00534522 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00534525 e8565dfdff               call       0x50a280
0053452a 89450c                   mov        dword ptr [ebp + 0xc], eax
0053452d 6a00                     push       0
0053452f e84cfbf2ff               call       0x464080
00534534 83c404                   add        esp, 4
00534537 8945f0                   mov        dword ptr [ebp - 0x10], eax
0053453a 6a01                     push       1
0053453c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0053453f e86c000000               call       0x5345b0
00534544 90                       nop        
00534545 6a00                     push       0
00534547 e834fbf2ff               call       0x464080
0053454c 83c404                   add        esp, 4
0053454f 8945ec                   mov        dword ptr [ebp - 0x14], eax
00534552 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00534555 50                       push       eax
00534556 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00534559 e842e8edff               call       0x412da0
0053455e 6b4d0824                 imul       ecx, dword ptr [ebp + 8], 0x24
00534562 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00534565 8b8491b00a5b00           mov        eax, dword ptr [ecx + edx*4 + 0x5b0ab0]
0053456c ffd0                     call       eax
0053456e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534571 89412c                   mov        dword ptr [ecx + 0x2c], eax
00534574 8b55fc                   mov        edx, dword ptr [ebp - 4]
00534577 8b422c                   mov        eax, dword ptr [edx + 0x2c]
0053457a 8945e8                   mov        dword ptr [ebp - 0x18], eax
0053457d 6a02                     push       2
0053457f 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00534582 e839e8edff               call       0x412dc0
00534587 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053458a 8b512c                   mov        edx, dword ptr [ecx + 0x2c]
0053458d 8955e4                   mov        dword ptr [ebp - 0x1c], edx
00534590 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00534593 50                       push       eax
00534594 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00534597 e8d4cfedff               call       0x411570
0053459c 90                       nop        
0053459d 8be5                     mov        esp, ebp
0053459f 5d                       pop        ebp
005345a0 c20800                   ret        8
005345a3 cc                       int3       
005345a4 cc                       int3       
005345a5 cc                       int3       
005345a6 cc                       int3       
005345a7 cc                       int3       
005345a8 cc                       int3       
005345a9 cc                       int3       
005345aa cc                       int3       
005345ab cc                       int3       
005345ac cc                       int3       
005345ad cc                       int3       
005345ae cc                       int3       
005345af cc                       int3       
