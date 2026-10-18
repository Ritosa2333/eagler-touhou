00510710 55                       push       ebp
00510711 8bec                     mov        ebp, esp
00510713 83ec0c                   sub        esp, 0xc
00510716 894df4                   mov        dword ptr [ebp - 0xc], ecx
00510719 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0051071c 8378140a                 cmp        dword ptr [eax + 0x14], 0xa
00510720 7c0a                     jl         0x51072c
00510722 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00510725 c7411400000000           mov        dword ptr [ecx + 0x14], 0
0051072c 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0051072f 6b421444                 imul       eax, dword ptr [edx + 0x14], 0x44
00510733 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00510736 8d940100060000           lea        edx, [ecx + eax + 0x600]
0051073d 8955f8                   mov        dword ptr [ebp - 8], edx
00510740 8b45f8                   mov        eax, dword ptr [ebp - 8]
00510743 c6403801                 mov        byte ptr [eax + 0x38], 1
00510747 c745fc00000000           mov        dword ptr [ebp - 4], 0
0051074e 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
00510752 7c34                     jl         0x510788
00510754 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
00510758 742c                     je         0x510786
0051075a 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0051075d 99                       cdq        
0051075e b90a000000               mov        ecx, 0xa
00510763 f7f9                     idiv       ecx
00510765 8b45f8                   mov        eax, dword ptr [ebp - 8]
00510768 0345fc                   add        eax, dword ptr [ebp - 4]
0051076b 8810                     mov        byte ptr [eax], dl
0051076d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00510770 83c101                   add        ecx, 1
00510773 894dfc                   mov        dword ptr [ebp - 4], ecx
00510776 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00510779 99                       cdq        
0051077a b90a000000               mov        ecx, 0xa
0051077f f7f9                     idiv       ecx
00510781 89450c                   mov        dword ptr [ebp + 0xc], eax
00510784 ebce                     jmp        0x510754
00510786 eb12                     jmp        0x51079a
00510788 8b55f8                   mov        edx, dword ptr [ebp - 8]
0051078b 0355fc                   add        edx, dword ptr [ebp - 4]
0051078e c6020a                   mov        byte ptr [edx], 0xa
00510791 8b45fc                   mov        eax, dword ptr [ebp - 4]
00510794 83c001                   add        eax, 1
00510797 8945fc                   mov        dword ptr [ebp - 4], eax
0051079a 837dfc00                 cmp        dword ptr [ebp - 4], 0
0051079e 7512                     jne        0x5107b2
005107a0 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005107a3 034dfc                   add        ecx, dword ptr [ebp - 4]
005107a6 c60100                   mov        byte ptr [ecx], 0
005107a9 8b55fc                   mov        edx, dword ptr [ebp - 4]
005107ac 83c201                   add        edx, 1
005107af 8955fc                   mov        dword ptr [ebp - 4], edx
005107b2 8b45f8                   mov        eax, dword ptr [ebp - 8]
005107b5 8a4dfc                   mov        cl, byte ptr [ebp - 4]
005107b8 884839                   mov        byte ptr [eax + 0x39], cl
005107bb 8b55f8                   mov        edx, dword ptr [ebp - 8]
005107be 8b4510                   mov        eax, dword ptr [ebp + 0x10]
005107c1 89421c                   mov        dword ptr [edx + 0x1c], eax
005107c4 6a00                     push       0
005107c6 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005107c9 83c120                   add        ecx, 0x20
005107cc e84f2df1ff               call       0x423520
005107d1 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005107d4 83c10c                   add        ecx, 0xc
005107d7 8b5508                   mov        edx, dword ptr [ebp + 8]
005107da 8b02                     mov        eax, dword ptr [edx]
005107dc 8901                     mov        dword ptr [ecx], eax
005107de 8b4204                   mov        eax, dword ptr [edx + 4]
005107e1 894104                   mov        dword ptr [ecx + 4], eax
005107e4 8b5208                   mov        edx, dword ptr [edx + 8]
005107e7 895108                   mov        dword ptr [ecx + 8], edx
005107ea 8b45f8                   mov        eax, dword ptr [ebp - 8]
005107ed f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
005107f5 f30f114018               movss      dword ptr [eax + 0x18], xmm0
005107fa 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005107fd 8b5114                   mov        edx, dword ptr [ecx + 0x14]
00510800 83c201                   add        edx, 1
00510803 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00510806 895014                   mov        dword ptr [eax + 0x14], edx
00510809 8be5                     mov        esp, ebp
0051080b 5d                       pop        ebp
0051080c c20c00                   ret        0xc
0051080f cc                       int3       
