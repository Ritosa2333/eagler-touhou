00464420 55                       push       ebp
00464421 8bec                     mov        ebp, esp
00464423 83ec08                   sub        esp, 8
00464426 894dfc                   mov        dword ptr [ebp - 4], ecx
00464429 8b4508                   mov        eax, dword ptr [ebp + 8]
0046442c 8945f8                   mov        dword ptr [ebp - 8], eax
0046442f 837df803                 cmp        dword ptr [ebp - 8], 3
00464433 770a                     ja         0x46443f
00464435 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00464438 ff248d6c444600           jmp        dword ptr [ecx*4 + 0x46446c]
0046443f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00464442 e879feffff               call       0x4642c0
00464447 eb1c                     jmp        0x464465
00464449 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0046444c e87f000000               call       0x4644d0
00464451 eb12                     jmp        0x464465
00464453 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00464456 e8d5feffff               call       0x464330
0046445b eb08                     jmp        0x464465
0046445d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00464460 e89bffffff               call       0x464400
00464465 8be5                     mov        esp, ebp
00464467 5d                       pop        ebp
00464468 c20400                   ret        4
0046446b 90                       nop        
0046446c 3f                       aas        
0046446d 44                       inc        esp
0046446e 46                       inc        esi
0046446f 004944                   add        byte ptr [ecx + 0x44], cl
00464472 46                       inc        esi
00464473 005344                   add        byte ptr [ebx + 0x44], dl
00464476 46                       inc        esi
00464477 005d44                   add        byte ptr [ebp + 0x44], bl
0046447a 46                       inc        esi
0046447b 00cc                     add        ah, cl
0046447d cc                       int3       
0046447e cc                       int3       
0046447f cc                       int3       
