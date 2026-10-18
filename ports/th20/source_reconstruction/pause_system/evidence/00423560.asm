00423560 55                       push       ebp
00423561 8bec                     mov        ebp, esp
00423563 83ec08                   sub        esp, 8
00423566 894df8                   mov        dword ptr [ebp - 8], ecx
00423569 8b45f8                   mov        eax, dword ptr [ebp - 8]
0042356c 8b4804                   mov        ecx, dword ptr [eax + 4]
0042356f 3b4d08                   cmp        ecx, dword ptr [ebp + 8]
00423572 7d09                     jge        0x42357d
00423574 c745fc01000000           mov        dword ptr [ebp - 4], 1
0042357b eb07                     jmp        0x423584
0042357d c745fc00000000           mov        dword ptr [ebp - 4], 0
00423584 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
00423588 8be5                     mov        esp, ebp
0042358a 5d                       pop        ebp
0042358b c20400                   ret        4
0042358e cc                       int3       
0042358f cc                       int3       
