004c85b0 55                       push       ebp
004c85b1 8bec                     mov        ebp, esp
004c85b3 6aff                     push       -1
004c85b5 6800765600               push       0x567600
004c85ba 64a100000000             mov        eax, dword ptr fs:[0]
004c85c0 50                       push       eax
004c85c1 51                       push       ecx
004c85c2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c85c7 33c5                     xor        eax, ebp
004c85c9 50                       push       eax
004c85ca 8d45f4                   lea        eax, [ebp - 0xc]
004c85cd 64a300000000             mov        dword ptr fs:[0], eax
004c85d3 894df0                   mov        dword ptr [ebp - 0x10], ecx
004c85d6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c85d9 c700bc105700             mov        dword ptr [eax], 0x5710bc
004c85df 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c85e2 83c108                   add        ecx, 8
004c85e5 e82604f5ff               call       0x418a10
004c85ea 33c9                     xor        ecx, ecx
004c85ec 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c85ef 83c21c                   add        edx, 0x1c
004c85f2 890a                     mov        dword ptr [edx], ecx
004c85f4 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c85f7 c7402000000000           mov        dword ptr [eax + 0x20], 0
004c85fe 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8601 c7412400000000           mov        dword ptr [ecx + 0x24], 0
004c8608 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c860b 83c128                   add        ecx, 0x28
004c860e e87da7f5ff               call       0x422d90
004c8613 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8616 83c138                   add        ecx, 0x38
004c8619 e872a7f5ff               call       0x422d90
004c861e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8621 83c148                   add        ecx, 0x48
004c8624 e867a7f5ff               call       0x422d90
004c8629 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c862c 83c158                   add        ecx, 0x58
004c862f e8dca7f5ff               call       0x422e10
004c8634 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8637 83c164                   add        ecx, 0x64
004c863a e8d1a7f5ff               call       0x422e10
004c863f 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c8642 0f57c0                   xorps      xmm0, xmm0
004c8645 f30f114270               movss      dword ptr [edx + 0x70], xmm0
004c864a 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c864d 0f57c0                   xorps      xmm0, xmm0
004c8650 f30f114074               movss      dword ptr [eax + 0x74], xmm0
004c8655 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8658 0f57c0                   xorps      xmm0, xmm0
004c865b f30f114178               movss      dword ptr [ecx + 0x78], xmm0
004c8660 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c8663 0f57c0                   xorps      xmm0, xmm0
004c8666 f30f11427c               movss      dword ptr [edx + 0x7c], xmm0
004c866b 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c866e 0f57c0                   xorps      xmm0, xmm0
004c8671 f30f118080000000         movss      dword ptr [eax + 0x80], xmm0
004c8679 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c867c 0f57c0                   xorps      xmm0, xmm0
004c867f f30f118184000000         movss      dword ptr [ecx + 0x84], xmm0
004c8687 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c868a 0f57c0                   xorps      xmm0, xmm0
004c868d f30f118288000000         movss      dword ptr [edx + 0x88], xmm0
004c8695 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c8698 c7808c00000000000000     mov        dword ptr [eax + 0x8c], 0
004c86a2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c86a5 81c190000000             add        ecx, 0x90
004c86ab e840fbffff               call       0x4c81f0
004c86b0 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c86b3 c7819006000000000000     mov        dword ptr [ecx + 0x690], 0
004c86bd 33d2                     xor        edx, edx
004c86bf 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c86c2 0598060000               add        eax, 0x698
004c86c7 8910                     mov        dword ptr [eax], edx
004c86c9 895004                   mov        dword ptr [eax + 4], edx
004c86cc 33c9                     xor        ecx, ecx
004c86ce 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c86d1 81c2a0060000             add        edx, 0x6a0
004c86d7 890a                     mov        dword ptr [edx], ecx
004c86d9 894a04                   mov        dword ptr [edx + 4], ecx
004c86dc 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c86df c780a806000000000000     mov        dword ptr [eax + 0x6a8], 0
004c86e9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c86ec 81c1ac060000             add        ecx, 0x6ac
004c86f2 e899a6f5ff               call       0x422d90
004c86f7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c86fa 81c1bc060000             add        ecx, 0x6bc
004c8700 e88ba6f5ff               call       0x422d90
004c8705 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8708 c781cc06000000000000     mov        dword ptr [ecx + 0x6cc], 0
004c8712 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c8715 c782d006000000000000     mov        dword ptr [edx + 0x6d0], 0
004c871f 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c8722 c780d406000000000000     mov        dword ptr [eax + 0x6d4], 0
004c872c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c872f c781d806000000000000     mov        dword ptr [ecx + 0x6d8], 0
004c8739 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c873c c782dc06000000000000     mov        dword ptr [edx + 0x6dc], 0
004c8746 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c8749 c780e006000000000000     mov        dword ptr [eax + 0x6e0], 0
004c8753 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8756 c781e406000000000000     mov        dword ptr [ecx + 0x6e4], 0
004c8760 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c8763 c782e8060000ffffffff     mov        dword ptr [edx + 0x6e8], 0xffffffff
004c876d 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c8770 c780ec06000000000000     mov        dword ptr [eax + 0x6ec], 0
004c877a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c877d c781f006000000000000     mov        dword ptr [ecx + 0x6f0], 0
004c8787 68f8060000               push       0x6f8
004c878c 6a00                     push       0
004c878e 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c8791 52                       push       edx
004c8792 e8c9bf0700               call       0x544760
004c8797 83c40c                   add        esp, 0xc
004c879a 6a00                     push       0
004c879c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c879f 83c128                   add        ecx, 0x28
004c87a2 e879adf5ff               call       0x423520
004c87a7 90                       nop        
004c87a8 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c87ab 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c87ae 64890d00000000           mov        dword ptr fs:[0], ecx
004c87b5 59                       pop        ecx
004c87b6 8be5                     mov        esp, ebp
004c87b8 5d                       pop        ebp
004c87b9 c3                       ret        
004c87ba cc                       int3       
004c87bb cc                       int3       
004c87bc cc                       int3       
004c87bd cc                       int3       
004c87be cc                       int3       
004c87bf cc                       int3       
