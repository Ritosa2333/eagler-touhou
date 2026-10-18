00458670 55                       push       ebp
00458671 8bec                     mov        ebp, esp
00458673 83ec44                   sub        esp, 0x44
00458676 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0045867b 33c5                     xor        eax, ebp
0045867d 8945fc                   mov        dword ptr [ebp - 4], eax
00458680 51                       push       ecx
00458681 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00458686 f30f110424               movss      dword ptr [esp], xmm0
0045868b 51                       push       ecx
0045868c f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00458691 f30f110424               movss      dword ptr [esp], xmm0
00458696 51                       push       ecx
00458697 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
0045869c f30f110424               movss      dword ptr [esp], xmm0
004586a1 51                       push       ecx
004586a2 f30f104508               movss      xmm0, dword ptr [ebp + 8]
004586a7 f30f110424               movss      dword ptr [esp], xmm0
004586ac 51                       push       ecx
004586ad f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
004586b2 f30f110424               movss      dword ptr [esp], xmm0
004586b7 51                       push       ecx
004586b8 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
004586bd f30f110424               movss      dword ptr [esp], xmm0
004586c2 e839ecffff               call       0x457300
004586c7 83c418                   add        esp, 0x18
004586ca 0fb6c0                   movzx      eax, al
004586cd 85c0                     test       eax, eax
004586cf 7407                     je         0x4586d8
004586d1 b001                     mov        al, 1
004586d3 e96c040000               jmp        0x458b44
004586d8 8b4d30                   mov        ecx, dword ptr [ebp + 0x30]
004586db 51                       push       ecx
004586dc 51                       push       ecx
004586dd f30f10452c               movss      xmm0, dword ptr [ebp + 0x2c]
004586e2 f30f110424               movss      dword ptr [esp], xmm0
004586e7 51                       push       ecx
004586e8 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
004586ed f30f110424               movss      dword ptr [esp], xmm0
004586f2 51                       push       ecx
004586f3 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
004586f8 f30f110424               movss      dword ptr [esp], xmm0
004586fd 51                       push       ecx
004586fe f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00458703 f30f110424               movss      dword ptr [esp], xmm0
00458708 51                       push       ecx
00458709 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
0045870e f30f110424               movss      dword ptr [esp], xmm0
00458713 51                       push       ecx
00458714 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00458719 f30f110424               movss      dword ptr [esp], xmm0
0045871e 51                       push       ecx
0045871f f30f104508               movss      xmm0, dword ptr [ebp + 8]
00458724 f30f110424               movss      dword ptr [esp], xmm0
00458729 e852ecffff               call       0x457380
0045872e 83c420                   add        esp, 0x20
00458731 0fb6d0                   movzx      edx, al
00458734 85d2                     test       edx, edx
00458736 7407                     je         0x45873f
00458738 b001                     mov        al, 1
0045873a e905040000               jmp        0x458b44
0045873f 68102e4200               push       0x422e10
00458744 6a04                     push       4
00458746 6a0c                     push       0xc
00458748 8d45cc                   lea        eax, [ebp - 0x34]
0045874b 50                       push       eax
0045874c e8cf34fbff               call       0x40bc20
00458751 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00458756 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
0045875d f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458765 b90c000000               mov        ecx, 0xc
0045876a 6bd100                   imul       edx, ecx, 0
0045876d f30f114415cc             movss      dword ptr [ebp + edx - 0x34], xmm0
00458773 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00458778 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
0045877f f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458787 b80c000000               mov        eax, 0xc
0045878c 6bc800                   imul       ecx, eax, 0
0045878f f30f11440dd0             movss      dword ptr [ebp + ecx - 0x30], xmm0
00458795 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
0045879a 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
004587a1 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004587a9 ba0c000000               mov        edx, 0xc
004587ae c1e200                   shl        edx, 0
004587b1 f30f114415cc             movss      dword ptr [ebp + edx - 0x34], xmm0
004587b7 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004587bc f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004587c4 b80c000000               mov        eax, 0xc
004587c9 c1e000                   shl        eax, 0
004587cc f30f114405d0             movss      dword ptr [ebp + eax - 0x30], xmm0
004587d2 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004587d7 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004587df b90c000000               mov        ecx, 0xc
004587e4 d1e1                     shl        ecx, 1
004587e6 f30f11440dcc             movss      dword ptr [ebp + ecx - 0x34], xmm0
004587ec f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004587f1 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004587f9 ba0c000000               mov        edx, 0xc
004587fe d1e2                     shl        edx, 1
00458800 f30f114415d0             movss      dword ptr [ebp + edx - 0x30], xmm0
00458806 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
0045880b f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458813 b80c000000               mov        eax, 0xc
00458818 6bc803                   imul       ecx, eax, 3
0045881b f30f11440dcc             movss      dword ptr [ebp + ecx - 0x34], xmm0
00458821 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00458826 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
0045882d f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458835 ba0c000000               mov        edx, 0xc
0045883a 6bc203                   imul       eax, edx, 3
0045883d f30f114405d0             movss      dword ptr [ebp + eax - 0x30], xmm0
00458843 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00458848 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
0045884f 9f                       lahf       
00458850 f6c444                   test       ah, 0x44
00458853 7b1d                     jnp        0x458872
00458855 6a04                     push       4
00458857 51                       push       ecx
00458858 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
0045885d f30f110424               movss      dword ptr [esp], xmm0
00458862 8d4dcc                   lea        ecx, [ebp - 0x34]
00458865 51                       push       ecx
00458866 8d55cc                   lea        edx, [ebp - 0x34]
00458869 52                       push       edx
0045886a e871080000               call       0x4590e0
0045886f 83c410                   add        esp, 0x10
00458872 c745c800000000           mov        dword ptr [ebp - 0x38], 0
00458879 eb09                     jmp        0x458884
0045887b 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0045887e 83c001                   add        eax, 1
00458881 8945c8                   mov        dword ptr [ebp - 0x38], eax
00458884 837dc804                 cmp        dword ptr [ebp - 0x38], 4
00458888 7d3b                     jge        0x4588c5
0045888a 51                       push       ecx
0045888b 0f57c0                   xorps      xmm0, xmm0
0045888e f30f110424               movss      dword ptr [esp], xmm0
00458893 51                       push       ecx
00458894 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00458899 f30f110424               movss      dword ptr [esp], xmm0
0045889e 51                       push       ecx
0045889f f30f104508               movss      xmm0, dword ptr [ebp + 8]
004588a4 f30f110424               movss      dword ptr [esp], xmm0
004588a9 8d4dbc                   lea        ecx, [ebp - 0x44]
004588ac e81fa5fcff               call       0x422dd0
004588b1 8d4dbc                   lea        ecx, [ebp - 0x44]
004588b4 51                       push       ecx
004588b5 6b55c80c                 imul       edx, dword ptr [ebp - 0x38], 0xc
004588b9 8d4c15cc                 lea        ecx, [ebp + edx - 0x34]
004588bd e81e0efdff               call       0x4296e0
004588c2 90                       nop        
004588c3 ebb6                     jmp        0x45887b
004588c5 8b4530                   mov        eax, dword ptr [ebp + 0x30]
004588c8 50                       push       eax
004588c9 51                       push       ecx
004588ca f30f10452c               movss      xmm0, dword ptr [ebp + 0x2c]
004588cf f30f110424               movss      dword ptr [esp], xmm0
004588d4 51                       push       ecx
004588d5 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
004588da f30f110424               movss      dword ptr [esp], xmm0
004588df 51                       push       ecx
004588e0 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
004588e5 f30f110424               movss      dword ptr [esp], xmm0
004588ea 51                       push       ecx
004588eb f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
004588f0 f30f110424               movss      dword ptr [esp], xmm0
004588f5 51                       push       ecx
004588f6 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
004588fb f30f110424               movss      dword ptr [esp], xmm0
00458900 b90c000000               mov        ecx, 0xc
00458905 c1e100                   shl        ecx, 0
00458908 51                       push       ecx
00458909 f30f10440dd0             movss      xmm0, dword ptr [ebp + ecx - 0x30]
0045890f f30f110424               movss      dword ptr [esp], xmm0
00458914 ba0c000000               mov        edx, 0xc
00458919 c1e200                   shl        edx, 0
0045891c 51                       push       ecx
0045891d f30f104415cc             movss      xmm0, dword ptr [ebp + edx - 0x34]
00458923 f30f110424               movss      dword ptr [esp], xmm0
00458928 b80c000000               mov        eax, 0xc
0045892d 6bc800                   imul       ecx, eax, 0
00458930 51                       push       ecx
00458931 f30f10440dd0             movss      xmm0, dword ptr [ebp + ecx - 0x30]
00458937 f30f110424               movss      dword ptr [esp], xmm0
0045893c ba0c000000               mov        edx, 0xc
00458941 6bc200                   imul       eax, edx, 0
00458944 51                       push       ecx
00458945 f30f104405cc             movss      xmm0, dword ptr [ebp + eax - 0x34]
0045894b f30f110424               movss      dword ptr [esp], xmm0
00458950 e8fbe3ffff               call       0x456d50
00458955 83c428                   add        esp, 0x28
00458958 0fb6c8                   movzx      ecx, al
0045895b 85c9                     test       ecx, ecx
0045895d 7407                     je         0x458966
0045895f b001                     mov        al, 1
00458961 e9de010000               jmp        0x458b44
00458966 8b5530                   mov        edx, dword ptr [ebp + 0x30]
00458969 52                       push       edx
0045896a 51                       push       ecx
0045896b f30f10452c               movss      xmm0, dword ptr [ebp + 0x2c]
00458970 f30f110424               movss      dword ptr [esp], xmm0
00458975 51                       push       ecx
00458976 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
0045897b f30f110424               movss      dword ptr [esp], xmm0
00458980 51                       push       ecx
00458981 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00458986 f30f110424               movss      dword ptr [esp], xmm0
0045898b 51                       push       ecx
0045898c f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00458991 f30f110424               movss      dword ptr [esp], xmm0
00458996 51                       push       ecx
00458997 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
0045899c f30f110424               movss      dword ptr [esp], xmm0
004589a1 b80c000000               mov        eax, 0xc
004589a6 d1e0                     shl        eax, 1
004589a8 51                       push       ecx
004589a9 f30f104405d0             movss      xmm0, dword ptr [ebp + eax - 0x30]
004589af f30f110424               movss      dword ptr [esp], xmm0
004589b4 b90c000000               mov        ecx, 0xc
004589b9 d1e1                     shl        ecx, 1
004589bb 51                       push       ecx
004589bc f30f10440dcc             movss      xmm0, dword ptr [ebp + ecx - 0x34]
004589c2 f30f110424               movss      dword ptr [esp], xmm0
004589c7 ba0c000000               mov        edx, 0xc
004589cc c1e200                   shl        edx, 0
004589cf 51                       push       ecx
004589d0 f30f104415d0             movss      xmm0, dword ptr [ebp + edx - 0x30]
004589d6 f30f110424               movss      dword ptr [esp], xmm0
004589db b80c000000               mov        eax, 0xc
004589e0 c1e000                   shl        eax, 0
004589e3 51                       push       ecx
004589e4 f30f104405cc             movss      xmm0, dword ptr [ebp + eax - 0x34]
004589ea f30f110424               movss      dword ptr [esp], xmm0
004589ef e85ce3ffff               call       0x456d50
004589f4 83c428                   add        esp, 0x28
004589f7 0fb6c8                   movzx      ecx, al
004589fa 85c9                     test       ecx, ecx
004589fc 7407                     je         0x458a05
004589fe b001                     mov        al, 1
00458a00 e93f010000               jmp        0x458b44
00458a05 8b5530                   mov        edx, dword ptr [ebp + 0x30]
00458a08 52                       push       edx
00458a09 51                       push       ecx
00458a0a f30f10452c               movss      xmm0, dword ptr [ebp + 0x2c]
00458a0f f30f110424               movss      dword ptr [esp], xmm0
00458a14 51                       push       ecx
00458a15 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
00458a1a f30f110424               movss      dword ptr [esp], xmm0
00458a1f 51                       push       ecx
00458a20 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00458a25 f30f110424               movss      dword ptr [esp], xmm0
00458a2a 51                       push       ecx
00458a2b f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00458a30 f30f110424               movss      dword ptr [esp], xmm0
00458a35 51                       push       ecx
00458a36 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00458a3b f30f110424               movss      dword ptr [esp], xmm0
00458a40 b80c000000               mov        eax, 0xc
00458a45 6bc803                   imul       ecx, eax, 3
00458a48 51                       push       ecx
00458a49 f30f10440dd0             movss      xmm0, dword ptr [ebp + ecx - 0x30]
00458a4f f30f110424               movss      dword ptr [esp], xmm0
00458a54 ba0c000000               mov        edx, 0xc
00458a59 6bc203                   imul       eax, edx, 3
00458a5c 51                       push       ecx
00458a5d f30f104405cc             movss      xmm0, dword ptr [ebp + eax - 0x34]
00458a63 f30f110424               movss      dword ptr [esp], xmm0
00458a68 b90c000000               mov        ecx, 0xc
00458a6d d1e1                     shl        ecx, 1
00458a6f 51                       push       ecx
00458a70 f30f10440dd0             movss      xmm0, dword ptr [ebp + ecx - 0x30]
00458a76 f30f110424               movss      dword ptr [esp], xmm0
00458a7b ba0c000000               mov        edx, 0xc
00458a80 d1e2                     shl        edx, 1
00458a82 51                       push       ecx
00458a83 f30f104415cc             movss      xmm0, dword ptr [ebp + edx - 0x34]
00458a89 f30f110424               movss      dword ptr [esp], xmm0
00458a8e e8bde2ffff               call       0x456d50
00458a93 83c428                   add        esp, 0x28
00458a96 0fb6c0                   movzx      eax, al
00458a99 85c0                     test       eax, eax
00458a9b 7407                     je         0x458aa4
00458a9d b001                     mov        al, 1
00458a9f e9a0000000               jmp        0x458b44
00458aa4 8b4d30                   mov        ecx, dword ptr [ebp + 0x30]
00458aa7 51                       push       ecx
00458aa8 51                       push       ecx
00458aa9 f30f10452c               movss      xmm0, dword ptr [ebp + 0x2c]
00458aae f30f110424               movss      dword ptr [esp], xmm0
00458ab3 51                       push       ecx
00458ab4 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
00458ab9 f30f110424               movss      dword ptr [esp], xmm0
00458abe 51                       push       ecx
00458abf f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00458ac4 f30f110424               movss      dword ptr [esp], xmm0
00458ac9 51                       push       ecx
00458aca f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00458acf f30f110424               movss      dword ptr [esp], xmm0
00458ad4 51                       push       ecx
00458ad5 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00458ada f30f110424               movss      dword ptr [esp], xmm0
00458adf ba0c000000               mov        edx, 0xc
00458ae4 6bc200                   imul       eax, edx, 0
00458ae7 51                       push       ecx
00458ae8 f30f104405d0             movss      xmm0, dword ptr [ebp + eax - 0x30]
00458aee f30f110424               movss      dword ptr [esp], xmm0
00458af3 b90c000000               mov        ecx, 0xc
00458af8 6bd100                   imul       edx, ecx, 0
00458afb 51                       push       ecx
00458afc f30f104415cc             movss      xmm0, dword ptr [ebp + edx - 0x34]
00458b02 f30f110424               movss      dword ptr [esp], xmm0
00458b07 b80c000000               mov        eax, 0xc
00458b0c 6bc803                   imul       ecx, eax, 3
00458b0f 51                       push       ecx
00458b10 f30f10440dd0             movss      xmm0, dword ptr [ebp + ecx - 0x30]
00458b16 f30f110424               movss      dword ptr [esp], xmm0
00458b1b ba0c000000               mov        edx, 0xc
00458b20 6bc203                   imul       eax, edx, 3
00458b23 51                       push       ecx
00458b24 f30f104405cc             movss      xmm0, dword ptr [ebp + eax - 0x34]
00458b2a f30f110424               movss      dword ptr [esp], xmm0
00458b2f e81ce2ffff               call       0x456d50
00458b34 83c428                   add        esp, 0x28
00458b37 0fb6c8                   movzx      ecx, al
00458b3a 85c9                     test       ecx, ecx
00458b3c 7404                     je         0x458b42
00458b3e b001                     mov        al, 1
00458b40 eb02                     jmp        0x458b44
00458b42 32c0                     xor        al, al
00458b44 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00458b47 33cd                     xor        ecx, ebp
00458b49 e8a09e0e00               call       0x5429ee
00458b4e 8be5                     mov        esp, ebp
00458b50 5d                       pop        ebp
00458b51 c3                       ret        
00458b52 cc                       int3       
00458b53 cc                       int3       
00458b54 cc                       int3       
00458b55 cc                       int3       
00458b56 cc                       int3       
00458b57 cc                       int3       
00458b58 cc                       int3       
00458b59 cc                       int3       
00458b5a cc                       int3       
00458b5b cc                       int3       
00458b5c cc                       int3       
00458b5d cc                       int3       
00458b5e cc                       int3       
00458b5f cc                       int3       
