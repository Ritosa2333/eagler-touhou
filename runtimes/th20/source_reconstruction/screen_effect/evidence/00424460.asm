00424460 55                       push       ebp
00424461 8bec                     mov        ebp, esp
00424463 83ec08                   sub        esp, 8
00424466 8b4508                   mov        eax, dword ptr [ebp + 8]
00424469 c74018ff000000           mov        dword ptr [eax + 0x18], 0xff
00424470 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424473 83c130                   add        ecx, 0x30
00424476 894dfc                   mov        dword ptr [ebp - 4], ecx
00424479 8b5508                   mov        edx, dword ptr [ebp + 8]
0042447c 8b421c                   mov        eax, dword ptr [edx + 0x1c]
0042447f 50                       push       eax
00424480 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00424483 e838f1ffff               call       0x4235c0
00424488 0fb6c8                   movzx      ecx, al
0042448b 85c9                     test       ecx, ecx
0042448d 7407                     je         0x424496
0042448f b807000000               mov        eax, 7
00424494 eb18                     jmp        0x4244ae
00424496 8b5508                   mov        edx, dword ptr [ebp + 8]
00424499 83c230                   add        edx, 0x30
0042449c 8955f8                   mov        dword ptr [ebp - 8], edx
0042449f 6a00                     push       0
004244a1 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004244a4 e897f0ffff               call       0x423540
004244a9 b801000000               mov        eax, 1
004244ae 8be5                     mov        esp, ebp
004244b0 5d                       pop        ebp
004244b1 c3                       ret        
004244b2 cc                       int3       
004244b3 cc                       int3       
004244b4 cc                       int3       
004244b5 cc                       int3       
004244b6 cc                       int3       
004244b7 cc                       int3       
004244b8 cc                       int3       
004244b9 cc                       int3       
004244ba cc                       int3       
004244bb cc                       int3       
004244bc cc                       int3       
004244bd cc                       int3       
004244be cc                       int3       
004244bf cc                       int3       
