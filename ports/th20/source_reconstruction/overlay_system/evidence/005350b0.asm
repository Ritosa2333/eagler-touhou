005350b0 55                       push       ebp
005350b1 8bec                     mov        ebp, esp
005350b3 83ec0c                   sub        esp, 0xc
005350b6 894dfc                   mov        dword ptr [ebp - 4], ecx
005350b9 8b45fc                   mov        eax, dword ptr [ebp - 4]
005350bc c6403400                 mov        byte ptr [eax + 0x34], 0
005350c0 6a00                     push       0
005350c2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005350c5 83c114                   add        ecx, 0x14
005350c8 e853e4eeff               call       0x423520
005350cd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005350d0 c7411001000000           mov        dword ptr [ecx + 0x10], 1
005350d7 6a00                     push       0
005350d9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005350dc 83c124                   add        ecx, 0x24
005350df e83ce4eeff               call       0x423520
005350e4 6a00                     push       0
005350e6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005350e9 83c138                   add        ecx, 0x38
005350ec e82fe4eeff               call       0x423520
005350f1 90                       nop        
005350f2 6a00                     push       0
005350f4 8d4df8                   lea        ecx, [ebp - 8]
005350f7 e8a4f1edff               call       0x4142a0
005350fc 50                       push       eax
005350fd b960695c00               mov        ecx, 0x5c6960
00535102 e899060000               call       0x5357a0
00535107 90                       nop        
00535108 6a00                     push       0
0053510a 8d4df4                   lea        ecx, [ebp - 0xc]
0053510d e88ef1edff               call       0x4142a0
00535112 50                       push       eax
00535113 b974695c00               mov        ecx, 0x5c6974
00535118 e883060000               call       0x5357a0
0053511d 90                       nop        
0053511e 6a00                     push       0
00535120 b970695c00               mov        ecx, 0x5c6970
00535125 e876c6edff               call       0x4117a0
0053512a 90                       nop        
0053512b 8be5                     mov        esp, ebp
0053512d 5d                       pop        ebp
0053512e c3                       ret        
0053512f cc                       int3       
