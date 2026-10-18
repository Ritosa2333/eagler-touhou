00423620 55                       push       ebp
00423621 8bec                     mov        ebp, esp
00423623 51                       push       ecx
00423624 894dfc                   mov        dword ptr [ebp - 4], ecx
00423627 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0042362a e871feffff               call       0x4234a0
0042362f 8b4508                   mov        eax, dword ptr [ebp + 8]
00423632 83e001                   and        eax, 1
00423635 740e                     je         0x423645
00423637 6a44                     push       0x44
00423639 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0042363c 51                       push       ecx
0042363d e84bf11100               call       0x54278d
00423642 83c408                   add        esp, 8
00423645 8b45fc                   mov        eax, dword ptr [ebp - 4]
00423648 8be5                     mov        esp, ebp
0042364a 5d                       pop        ebp
0042364b c20400                   ret        4
0042364e cc                       int3       
0042364f cc                       int3       
