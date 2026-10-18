00488990 55                       push       ebp
00488991 8bec                     mov        ebp, esp
00488993 51                       push       ecx
00488994 894dfc                   mov        dword ptr [ebp - 4], ecx
00488997 8b4508                   mov        eax, dword ptr [ebp + 8]
0048899a 83c042                   add        eax, 0x42
0048899d 99                       cdq        
0048899e b9e8030000               mov        ecx, 0x3e8
004889a3 f7f9                     idiv       ecx
004889a5 6bd264                   imul       edx, edx, 0x64
004889a8 8b45fc                   mov        eax, dword ptr [ebp - 4]
004889ab 8990a8000000             mov        dword ptr [eax + 0xa8], edx
004889b1 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004889b4 83c021                   add        eax, 0x21
004889b7 99                       cdq        
004889b8 b964000000               mov        ecx, 0x64
004889bd f7f9                     idiv       ecx
004889bf 8b45fc                   mov        eax, dword ptr [ebp - 4]
004889c2 0390a8000000             add        edx, dword ptr [eax + 0xa8]
004889c8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004889cb 8991a8000000             mov        dword ptr [ecx + 0xa8], edx
004889d1 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004889d4 8b4508                   mov        eax, dword ptr [ebp + 8]
004889d7 8d4c1016                 lea        ecx, [eax + edx + 0x16]
004889db 69d1a0860100             imul       edx, ecx, 0x186a0
004889e1 8b45fc                   mov        eax, dword ptr [ebp - 4]
004889e4 0390a8000000             add        edx, dword ptr [eax + 0xa8]
004889ea 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004889ed 8991a8000000             mov        dword ptr [ecx + 0xa8], edx
004889f3 8be5                     mov        esp, ebp
004889f5 5d                       pop        ebp
004889f6 c20800                   ret        8
004889f9 cc                       int3       
004889fa cc                       int3       
004889fb cc                       int3       
004889fc cc                       int3       
004889fd cc                       int3       
004889fe cc                       int3       
004889ff cc                       int3       
