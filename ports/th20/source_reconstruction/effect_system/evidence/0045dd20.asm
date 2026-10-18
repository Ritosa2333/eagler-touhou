0045dd20 55                       push       ebp
0045dd21 8bec                     mov        ebp, esp
0045dd23 51                       push       ecx
0045dd24 894dfc                   mov        dword ptr [ebp - 4], ecx
0045dd27 8b4508                   mov        eax, dword ptr [ebp + 8]
0045dd2a 50                       push       eax
0045dd2b 6a3c                     push       0x3c
0045dd2d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045dd30 51                       push       ecx
0045dd31 e8faf6ffff               call       0x45d430
0045dd36 83c40c                   add        esp, 0xc
0045dd39 8be5                     mov        esp, ebp
0045dd3b 5d                       pop        ebp
0045dd3c c20400                   ret        4
0045dd3f cc                       int3       
