0045dd00 55                       push       ebp
0045dd01 8bec                     mov        ebp, esp
0045dd03 51                       push       ecx
0045dd04 894dfc                   mov        dword ptr [ebp - 4], ecx
0045dd07 8b4508                   mov        eax, dword ptr [ebp + 8]
0045dd0a 50                       push       eax
0045dd0b 6a3c                     push       0x3c
0045dd0d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045dd10 51                       push       ecx
0045dd11 e84af6ffff               call       0x45d360
0045dd16 83c40c                   add        esp, 0xc
0045dd19 8be5                     mov        esp, ebp
0045dd1b 5d                       pop        ebp
0045dd1c c20400                   ret        4
0045dd1f cc                       int3       
