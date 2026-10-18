004383a0 55                       push       ebp
004383a1 8bec                     mov        ebp, esp
004383a3 83ec08                   sub        esp, 8
004383a6 894df8                   mov        dword ptr [ebp - 8], ecx
004383a9 8b45f8                   mov        eax, dword ptr [ebp - 8]
004383ac 8b4854                   mov        ecx, dword ptr [eax + 0x54]
004383af 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004383b2 833c9100                 cmp        dword ptr [ecx + edx*4], 0
004383b6 7421                     je         0x4383d9
004383b8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004383bb 83c15c                   add        ecx, 0x5c
004383be e8fd0effff               call       0x4292c0
004383c3 85c0                     test       eax, eax
004383c5 7406                     je         0x4383cd
004383c7 c645ff01                 mov        byte ptr [ebp - 1], 1
004383cb eb04                     jmp        0x4383d1
004383cd c645ff00                 mov        byte ptr [ebp - 1], 0
004383d1 0fb645ff                 movzx      eax, byte ptr [ebp - 1]
004383d5 85c0                     test       eax, eax
004383d7 7418                     je         0x4383f1
004383d9 68e4050000               push       0x5e4
004383de 6a00                     push       0
004383e0 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004383e3 51                       push       ecx
004383e4 e877c31000               call       0x544760
004383e9 83c40c                   add        esp, 0xc
004383ec e9fb000000               jmp        0x4384ec
004383f1 8b5508                   mov        edx, dword ptr [ebp + 8]
004383f4 668b450c                 mov        ax, word ptr [ebp + 0xc]
004383f8 66898240040000           mov        word ptr [edx + 0x440], ax
004383ff 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00438402 8b55f8                   mov        edx, dword ptr [ebp - 8]
00438405 8b02                     mov        eax, dword ptr [edx]
00438407 894118                   mov        dword ptr [ecx + 0x18], eax
0043840a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0043840d 8b55f8                   mov        edx, dword ptr [ebp - 8]
00438410 8b02                     mov        eax, dword ptr [edx]
00438412 89411c                   mov        dword ptr [ecx + 0x1c], eax
00438415 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00438418 8a91a0040000             mov        dl, byte ptr [ecx + 0x4a0]
0043841e 80e23f                   and        dl, 0x3f
00438421 8b4508                   mov        eax, dword ptr [ebp + 8]
00438424 8890a0040000             mov        byte ptr [eax + 0x4a0], dl
0043842a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0043842d 8a91a0040000             mov        dl, byte ptr [ecx + 0x4a0]
00438433 80e2fc                   and        dl, 0xfc
00438436 80ca02                   or         dl, 2
00438439 8b4508                   mov        eax, dword ptr [ebp + 8]
0043843c 8890a0040000             mov        byte ptr [eax + 0x4a0], dl
00438442 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00438445 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00438448 895124                   mov        dword ptr [ecx + 0x24], edx
0043844b 8b4508                   mov        eax, dword ptr [ebp + 8]
0043844e c7402800000000           mov        dword ptr [eax + 0x28], 0
00438455 6a00                     push       0
00438457 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0043845a 81c1d8040000             add        ecx, 0x4d8
00438460 e8bbb0feff               call       0x423520
00438465 6a00                     push       0
00438467 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0043846a 81c1c8040000             add        ecx, 0x4c8
00438470 e8abb0feff               call       0x423520
00438475 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00438478 8a919a040000             mov        dl, byte ptr [ecx + 0x49a]
0043847e 80e2fe                   and        dl, 0xfe
00438481 8b4508                   mov        eax, dword ptr [ebp + 8]
00438484 88909a040000             mov        byte ptr [eax + 0x49a], dl
0043848a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0043848d c781dc05000000000000     mov        dword ptr [ecx + 0x5dc], 0
00438497 8b5508                   mov        edx, dword ptr [ebp + 8]
0043849a 52                       push       edx
0043849b 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004384a1 e85ad0ffff               call       0x435500
004384a6 a128005c00               mov        eax, dword ptr [0x5c0028]
004384ab 8b88b8000000             mov        ecx, dword ptr [eax + 0xb8]
004384b1 83c101                   add        ecx, 1
004384b4 8b1528005c00             mov        edx, dword ptr [0x5c0028]
004384ba 898ab8000000             mov        dword ptr [edx + 0xb8], ecx
004384c0 8b4508                   mov        eax, dword ptr [ebp + 8]
004384c3 8a88a0040000             mov        cl, byte ptr [eax + 0x4a0]
004384c9 80e103                   and        cl, 3
004384cc 0fb6d1                   movzx      edx, cl
004384cf 83fa02                   cmp        edx, 2
004384d2 7518                     jne        0x4384ec
004384d4 8b4508                   mov        eax, dword ptr [ebp + 8]
004384d7 8a88a0040000             mov        cl, byte ptr [eax + 0x4a0]
004384dd 80e1fc                   and        cl, 0xfc
004384e0 80c901                   or         cl, 1
004384e3 8b5508                   mov        edx, dword ptr [ebp + 8]
004384e6 888aa0040000             mov        byte ptr [edx + 0x4a0], cl
004384ec 8be5                     mov        esp, ebp
004384ee 5d                       pop        ebp
004384ef c20800                   ret        8
004384f2 cc                       int3       
004384f3 cc                       int3       
004384f4 cc                       int3       
004384f5 cc                       int3       
004384f6 cc                       int3       
004384f7 cc                       int3       
004384f8 cc                       int3       
004384f9 cc                       int3       
004384fa cc                       int3       
004384fb cc                       int3       
004384fc cc                       int3       
004384fd cc                       int3       
004384fe cc                       int3       
004384ff cc                       int3       
