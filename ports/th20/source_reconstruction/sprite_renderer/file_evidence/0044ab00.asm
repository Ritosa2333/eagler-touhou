0044ab00 55                       push       ebp
0044ab01 8bec                     mov        ebp, esp
0044ab03 83ec08                   sub        esp, 8
0044ab06 894dfc                   mov        dword ptr [ebp - 4], ecx
0044ab09 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044ab0c 83783c00                 cmp        dword ptr [eax + 0x3c], 0
0044ab10 0f8402010000             je         0x44ac18
0044ab16 6a00                     push       0
0044ab18 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044ab1b 51                       push       ecx
0044ab1c 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0044ab22 e809520000               call       0x44fd30
0044ab27 90                       nop        
0044ab28 c745f800000000           mov        dword ptr [ebp - 8], 0
0044ab2f eb09                     jmp        0x44ab3a
0044ab31 8b55f8                   mov        edx, dword ptr [ebp - 8]
0044ab34 83c201                   add        edx, 1
0044ab37 8955f8                   mov        dword ptr [ebp - 8], edx
0044ab3a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044ab3d 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0044ab40 3b4844                   cmp        ecx, dword ptr [eax + 0x44]
0044ab43 7d19                     jge        0x44ab5e
0044ab45 6b55f818                 imul       edx, dword ptr [ebp - 8], 0x18
0044ab49 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044ab4c 035058                   add        edx, dword ptr [eax + 0x58]
0044ab4f 52                       push       edx
0044ab50 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0044ab56 e8b5190000               call       0x44c510
0044ab5b 90                       nop        
0044ab5c ebd3                     jmp        0x44ab31
0044ab5e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044ab61 8b5158                   mov        edx, dword ptr [ecx + 0x58]
0044ab64 52                       push       edx
0044ab65 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0044ab6b e8d02cfcff               call       0x40d840
0044ab70 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044ab73 c7405800000000           mov        dword ptr [eax + 0x58], 0
0044ab7a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044ab7d 8b5150                   mov        edx, dword ptr [ecx + 0x50]
0044ab80 52                       push       edx
0044ab81 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0044ab87 e8b42cfcff               call       0x40d840
0044ab8c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044ab8f c7405000000000           mov        dword ptr [eax + 0x50], 0
0044ab96 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044ab99 8b5154                   mov        edx, dword ptr [ecx + 0x54]
0044ab9c 52                       push       edx
0044ab9d 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0044aba3 e8982cfcff               call       0x40d840
0044aba8 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044abab c7405400000000           mov        dword ptr [eax + 0x54], 0
0044abb2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044abb5 83796c00                 cmp        dword ptr [ecx + 0x6c], 0
0044abb9 741c                     je         0x44abd7
0044abbb 8b55fc                   mov        edx, dword ptr [ebp - 4]
0044abbe 8b426c                   mov        eax, dword ptr [edx + 0x6c]
0044abc1 50                       push       eax
0044abc2 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0044abc8 e8a34afdff               call       0x41f670
0044abcd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044abd0 c7416c00000000           mov        dword ptr [ecx + 0x6c], 0
0044abd7 8b55fc                   mov        edx, dword ptr [ebp - 4]
0044abda 837a3c00                 cmp        dword ptr [edx + 0x3c], 0
0044abde 741c                     je         0x44abfc
0044abe0 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044abe3 8b483c                   mov        ecx, dword ptr [eax + 0x3c]
0044abe6 51                       push       ecx
0044abe7 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0044abed e87e4afdff               call       0x41f670
0044abf2 8b55fc                   mov        edx, dword ptr [ebp - 4]
0044abf5 c7423c00000000           mov        dword ptr [edx + 0x3c], 0
0044abfc 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044abff 8b4840                   mov        ecx, dword ptr [eax + 0x40]
0044ac02 51                       push       ecx
0044ac03 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0044ac09 e8b2c6ffff               call       0x4472c0
0044ac0e 8b55fc                   mov        edx, dword ptr [ebp - 4]
0044ac11 c7424000000000           mov        dword ptr [edx + 0x40], 0
0044ac18 8be5                     mov        esp, ebp
0044ac1a 5d                       pop        ebp
0044ac1b c3                       ret        
0044ac1c cc                       int3       
0044ac1d cc                       int3       
0044ac1e cc                       int3       
0044ac1f cc                       int3       
