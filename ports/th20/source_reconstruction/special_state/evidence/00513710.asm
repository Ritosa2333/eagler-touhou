00513710 55                       push       ebp
00513711 8bec                     mov        ebp, esp
00513713 83ec24                   sub        esp, 0x24
00513716 894dfc                   mov        dword ptr [ebp - 4], ecx
00513719 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051371c 83781800                 cmp        dword ptr [eax + 0x18], 0
00513720 7e71                     jle        0x513793
00513722 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00513725 83c108                   add        ecx, 8
00513728 e8d35cf1ff               call       0x429400
0051372d 90                       nop        
0051372e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00513731 83c108                   add        ecx, 8
00513734 894df4                   mov        dword ptr [ebp - 0xc], ecx
00513737 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051373a 8b4218                   mov        eax, dword ptr [edx + 0x18]
0051373d 50                       push       eax
0051373e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00513741 e87afef0ff               call       0x4235c0
00513746 0fb6c8                   movzx      ecx, al
00513749 85c9                     test       ecx, ecx
0051374b 7444                     je         0x513791
0051374d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00513750 8b4218                   mov        eax, dword ptr [edx + 0x18]
00513753 50                       push       eax
00513754 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00513757 83c108                   add        ecx, 8
0051375a e8c1fdf0ff               call       0x423520
0051375f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00513762 e8f98af8ff               call       0x49c260
00513767 90                       nop        
00513768 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051376b 83791c07                 cmp        dword ptr [ecx + 0x1c], 7
0051376f 7416                     je         0x513787
00513771 8b55fc                   mov        edx, dword ptr [ebp - 4]
00513774 837a1c11                 cmp        dword ptr [edx + 0x1c], 0x11
00513778 740d                     je         0x513787
0051377a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051377d 8a4001                   mov        al, byte ptr [eax + 1]
00513780 e925020000               jmp        0x5139aa
00513785 eb0a                     jmp        0x513791
00513787 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051378a 8a01                     mov        al, byte ptr [ecx]
0051378c e919020000               jmp        0x5139aa
00513791 eb32                     jmp        0x5137c5
00513793 8b55fc                   mov        edx, dword ptr [ebp - 4]
00513796 837a1800                 cmp        dword ptr [edx + 0x18], 0
0051379a 7529                     jne        0x5137c5
0051379c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051379f 83781c07                 cmp        dword ptr [eax + 0x1c], 7
005137a3 7416                     je         0x5137bb
005137a5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005137a8 83791c11                 cmp        dword ptr [ecx + 0x1c], 0x11
005137ac 740d                     je         0x5137bb
005137ae 8b55fc                   mov        edx, dword ptr [ebp - 4]
005137b1 8a4201                   mov        al, byte ptr [edx + 1]
005137b4 e9f1010000               jmp        0x5139aa
005137b9 eb0a                     jmp        0x5137c5
005137bb 8b45fc                   mov        eax, dword ptr [ebp - 4]
005137be 8a00                     mov        al, byte ptr [eax]
005137c0 e9e5010000               jmp        0x5139aa
005137c5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005137c8 83791c07                 cmp        dword ptr [ecx + 0x1c], 7
005137cc 7524                     jne        0x5137f2
005137ce 8b55fc                   mov        edx, dword ptr [ebp - 4]
005137d1 0fb602                   movzx      eax, byte ptr [edx]
005137d4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005137d7 0fb65101                 movzx      edx, byte ptr [ecx + 1]
005137db 03c2                     add        eax, edx
005137dd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005137e0 8801                     mov        byte ptr [ecx], al
005137e2 8b55fc                   mov        edx, dword ptr [ebp - 4]
005137e5 8b45fc                   mov        eax, dword ptr [ebp - 4]
005137e8 8a08                     mov        cl, byte ptr [eax]
005137ea 884a04                   mov        byte ptr [edx + 4], cl
005137ed e9b2010000               jmp        0x5139a4
005137f2 8b55fc                   mov        edx, dword ptr [ebp - 4]
005137f5 837a1c11                 cmp        dword ptr [edx + 0x1c], 0x11
005137f9 753a                     jne        0x513835
005137fb 8b45fc                   mov        eax, dword ptr [ebp - 4]
005137fe 0fb608                   movzx      ecx, byte ptr [eax]
00513801 8b55fc                   mov        edx, dword ptr [ebp - 4]
00513804 0fb64203                 movzx      eax, byte ptr [edx + 3]
00513808 03c8                     add        ecx, eax
0051380a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051380d 880a                     mov        byte ptr [edx], cl
0051380f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00513812 0fb64803                 movzx      ecx, byte ptr [eax + 3]
00513816 8b55fc                   mov        edx, dword ptr [ebp - 4]
00513819 0fb64201                 movzx      eax, byte ptr [edx + 1]
0051381d 03c8                     add        ecx, eax
0051381f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00513822 884a03                   mov        byte ptr [edx + 3], cl
00513825 8b45fc                   mov        eax, dword ptr [ebp - 4]
00513828 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051382b 8a11                     mov        dl, byte ptr [ecx]
0051382d 885004                   mov        byte ptr [eax + 4], dl
00513830 e96f010000               jmp        0x5139a4
00513835 8b45fc                   mov        eax, dword ptr [ebp - 4]
00513838 83781c08                 cmp        dword ptr [eax + 0x1c], 8
0051383c 0f8527010000             jne        0x513969
00513842 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00513845 83c108                   add        ecx, 8
00513848 e88303f1ff               call       0x423bd0
0051384d d95df0                   fstp       dword ptr [ebp - 0x10]
00513850 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00513855 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00513858 f30f2a4918               cvtsi2ss   xmm1, dword ptr [ecx + 0x18]
0051385d f30f5ec1                 divss      xmm0, xmm1
00513861 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00513866 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
0051386b f30f5c05ccc85600         subss      xmm0, dword ptr [0x56c8cc]
00513873 f30f104df8               movss      xmm1, dword ptr [ebp - 8]
00513878 f30f5c0dccc85600         subss      xmm1, dword ptr [0x56c8cc]
00513880 f30f59c1                 mulss      xmm0, xmm1
00513884 f30f100dd0c85600         movss      xmm1, dword ptr [0x56c8d0]
0051388c f30f594df8               mulss      xmm1, dword ptr [ebp - 8]
00513891 f30f580dccc85600         addss      xmm1, dword ptr [0x56c8cc]
00513899 f30f59c1                 mulss      xmm0, xmm1
0051389d f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
005138a2 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
005138a7 f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
005138ac f30f100dd0c85600         movss      xmm1, dword ptr [0x56c8d0]
005138b4 f30f594df8               mulss      xmm1, dword ptr [ebp - 8]
005138b9 f30f1015d8c85600         movss      xmm2, dword ptr [0x56c8d8]
005138c1 f30f5cd1                 subss      xmm2, xmm1
005138c5 f30f59c2                 mulss      xmm0, xmm2
005138c9 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
005138ce f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
005138d6 f30f5c45f8               subss      xmm0, dword ptr [ebp - 8]
005138db f30f100dccc85600         movss      xmm1, dword ptr [0x56c8cc]
005138e3 f30f5c4df8               subss      xmm1, dword ptr [ebp - 8]
005138e8 f30f59c1                 mulss      xmm0, xmm1
005138ec f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
005138f1 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
005138f6 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
005138fb f30f5c05ccc85600         subss      xmm0, dword ptr [0x56c8cc]
00513903 f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
00513908 f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
0051390d f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
00513912 8b55fc                   mov        edx, dword ptr [ebp - 4]
00513915 0fb602                   movzx      eax, byte ptr [edx]
00513918 f30f2ac0                 cvtsi2ss   xmm0, eax
0051391c f30f5945ec               mulss      xmm0, dword ptr [ebp - 0x14]
00513921 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00513924 0fb65101                 movzx      edx, byte ptr [ecx + 1]
00513928 f30f2aca                 cvtsi2ss   xmm1, edx
0051392c f30f594de8               mulss      xmm1, dword ptr [ebp - 0x18]
00513931 f30f58c1                 addss      xmm0, xmm1
00513935 8b45fc                   mov        eax, dword ptr [ebp - 4]
00513938 0fb64802                 movzx      ecx, byte ptr [eax + 2]
0051393c f30f2ac9                 cvtsi2ss   xmm1, ecx
00513940 f30f594de4               mulss      xmm1, dword ptr [ebp - 0x1c]
00513945 f30f58c1                 addss      xmm0, xmm1
00513949 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051394c 0fb64203                 movzx      eax, byte ptr [edx + 3]
00513950 f30f2ac8                 cvtsi2ss   xmm1, eax
00513954 f30f594de0               mulss      xmm1, dword ptr [ebp - 0x20]
00513959 f30f58c1                 addss      xmm0, xmm1
0051395d f30f2cc8                 cvttss2si  ecx, xmm0
00513961 8b55fc                   mov        edx, dword ptr [ebp - 4]
00513964 884a04                   mov        byte ptr [edx + 4], cl
00513967 eb3b                     jmp        0x5139a4
00513969 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051396c e86f030000               call       0x513ce0
00513971 d95ddc                   fstp       dword ptr [ebp - 0x24]
00513974 8b45fc                   mov        eax, dword ptr [ebp - 4]
00513977 0fb64801                 movzx      ecx, byte ptr [eax + 1]
0051397b 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051397e 0fb602                   movzx      eax, byte ptr [edx]
00513981 2bc8                     sub        ecx, eax
00513983 f30f2ac1                 cvtsi2ss   xmm0, ecx
00513987 f30f5945dc               mulss      xmm0, dword ptr [ebp - 0x24]
0051398c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051398f 0fb611                   movzx      edx, byte ptr [ecx]
00513992 f30f2aca                 cvtsi2ss   xmm1, edx
00513996 f30f58c1                 addss      xmm0, xmm1
0051399a f30f2cc0                 cvttss2si  eax, xmm0
0051399e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005139a1 884104                   mov        byte ptr [ecx + 4], al
005139a4 8b55fc                   mov        edx, dword ptr [ebp - 4]
005139a7 8a4204                   mov        al, byte ptr [edx + 4]
005139aa 8be5                     mov        esp, ebp
005139ac 5d                       pop        ebp
005139ad c3                       ret        
005139ae cc                       int3       
005139af cc                       int3       
