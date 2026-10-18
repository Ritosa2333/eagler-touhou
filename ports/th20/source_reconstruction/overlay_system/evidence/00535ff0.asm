00535ff0 55                       push       ebp
00535ff1 8bec                     mov        ebp, esp
00535ff3 83ec0c                   sub        esp, 0xc
00535ff6 894dfc                   mov        dword ptr [ebp - 4], ecx
00535ff9 8b45fc                   mov        eax, dword ptr [ebp - 4]
00535ffc c6403400                 mov        byte ptr [eax + 0x34], 0
00536000 6a00                     push       0
00536002 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00536005 83c114                   add        ecx, 0x14
00536008 e813d5eeff               call       0x423520
0053600d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00536010 c7411001000000           mov        dword ptr [ecx + 0x10], 1
00536017 6a00                     push       0
00536019 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053601c 83c124                   add        ecx, 0x24
0053601f e8fcd4eeff               call       0x423520
00536024 90                       nop        
00536025 6a00                     push       0
00536027 8d4df8                   lea        ecx, [ebp - 8]
0053602a e871e2edff               call       0x4142a0
0053602f 50                       push       eax
00536030 b984695c00               mov        ecx, 0x5c6984
00536035 e866f7ffff               call       0x5357a0
0053603a 90                       nop        
0053603b 6a00                     push       0
0053603d 8d4df4                   lea        ecx, [ebp - 0xc]
00536040 e85be2edff               call       0x4142a0
00536045 50                       push       eax
00536046 b998695c00               mov        ecx, 0x5c6998
0053604b e850f7ffff               call       0x5357a0
00536050 90                       nop        
00536051 6a00                     push       0
00536053 b994695c00               mov        ecx, 0x5c6994
00536058 e843b7edff               call       0x4117a0
0053605d 6a00                     push       0
0053605f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00536062 83c138                   add        ecx, 0x38
00536065 e8b6d4eeff               call       0x423520
0053606a 6a00                     push       0
0053606c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053606f 83c148                   add        ecx, 0x48
00536072 e8a9d4eeff               call       0x423520
00536077 90                       nop        
00536078 8be5                     mov        esp, ebp
0053607a 5d                       pop        ebp
0053607b c3                       ret        
0053607c cc                       int3       
0053607d cc                       int3       
0053607e cc                       int3       
0053607f cc                       int3       
