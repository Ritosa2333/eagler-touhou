0043cb70 55                       push       ebp
0043cb71 8bec                     mov        ebp, esp
0043cb73 83ec20                   sub        esp, 0x20
0043cb76 894dfc                   mov        dword ptr [ebp - 4], ecx
0043cb79 8b4508                   mov        eax, dword ptr [ebp + 8]
0043cb7c 83c001                   add        eax, 1
0043cb7f 6bc814                   imul       ecx, eax, 0x14
0043cb82 8b55fc                   mov        edx, dword ptr [ebp - 4]
0043cb85 038a800ed407             add        ecx, dword ptr [edx + 0x7d40e80]
0043cb8b b814000000               mov        eax, 0x14
0043cb90 c1e010                   shl        eax, 0x10
0043cb93 8b55fc                   mov        edx, dword ptr [ebp - 4]
0043cb96 8d8402800ec007           lea        eax, [edx + eax + 0x7c00e80]
0043cb9d 3bc8                     cmp        ecx, eax
0043cb9f 7207                     jb         0x43cba8
0043cba1 33c0                     xor        eax, eax
0043cba3 e989010000               jmp        0x43cd31
0043cba8 837d0803                 cmp        dword ptr [ebp + 8], 3
0043cbac 7d07                     jge        0x43cbb5
0043cbae 33c0                     xor        eax, eax
0043cbb0 e97c010000               jmp        0x43cd31
0043cbb5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0043cbb8 8b91800ed407             mov        edx, dword ptr [ecx + 0x7d40e80]
0043cbbe 8955f8                   mov        dword ptr [ebp - 8], edx
0043cbc1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0043cbc4 e8f7890000               call       0x4455c0
0043cbc9 90                       nop        
0043cbca c745f400000000           mov        dword ptr [ebp - 0xc], 0
0043cbd1 eb09                     jmp        0x43cbdc
0043cbd3 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0043cbd6 83c001                   add        eax, 1
0043cbd9 8945f4                   mov        dword ptr [ebp - 0xc], eax
0043cbdc 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0043cbdf 3b4d08                   cmp        ecx, dword ptr [ebp + 8]
0043cbe2 0f8da7000000             jge        0x43cc8f
0043cbe8 8b5510                   mov        edx, dword ptr [ebp + 0x10]
0043cbeb 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0043cbee f30f1002                 movss      xmm0, dword ptr [edx]
0043cbf2 f30f5800                 addss      xmm0, dword ptr [eax]
0043cbf6 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0043cbf9 f30f1101                 movss      dword ptr [ecx], xmm0
0043cbfd 8b5510                   mov        edx, dword ptr [ebp + 0x10]
0043cc00 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0043cc03 f30f104204               movss      xmm0, dword ptr [edx + 4]
0043cc08 f30f584004               addss      xmm0, dword ptr [eax + 4]
0043cc0d 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0043cc10 f30f114104               movss      dword ptr [ecx + 4], xmm0
0043cc15 8b55f8                   mov        edx, dword ptr [ebp - 8]
0043cc18 0f57c0                   xorps      xmm0, xmm0
0043cc1b f30f114208               movss      dword ptr [edx + 8], xmm0
0043cc20 8b45f8                   mov        eax, dword ptr [ebp - 8]
0043cc23 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
0043cc2b f30f11400c               movss      dword ptr [eax + 0xc], xmm0
0043cc30 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0043cc33 8b5514                   mov        edx, dword ptr [ebp + 0x14]
0043cc36 8b02                     mov        eax, dword ptr [edx]
0043cc38 894110                   mov        dword ptr [ecx + 0x10], eax
0043cc3b 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0043cc3e 8b55fc                   mov        edx, dword ptr [ebp - 4]
0043cc41 f30f1001                 movss      xmm0, dword ptr [ecx]
0043cc45 f30f5882d0000000         addss      xmm0, dword ptr [edx + 0xd0]
0043cc4d 8b45f8                   mov        eax, dword ptr [ebp - 8]
0043cc50 f30f1100                 movss      dword ptr [eax], xmm0
0043cc54 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0043cc57 8b55fc                   mov        edx, dword ptr [ebp - 4]
0043cc5a f30f104104               movss      xmm0, dword ptr [ecx + 4]
0043cc5f f30f5882d4000000         addss      xmm0, dword ptr [edx + 0xd4]
0043cc67 8b45f8                   mov        eax, dword ptr [ebp - 8]
0043cc6a f30f114004               movss      dword ptr [eax + 4], xmm0
0043cc6f 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
0043cc72 83c108                   add        ecx, 8
0043cc75 894d10                   mov        dword ptr [ebp + 0x10], ecx
0043cc78 8b5514                   mov        edx, dword ptr [ebp + 0x14]
0043cc7b 83c204                   add        edx, 4
0043cc7e 895514                   mov        dword ptr [ebp + 0x14], edx
0043cc81 8b45f8                   mov        eax, dword ptr [ebp - 8]
0043cc84 83c014                   add        eax, 0x14
0043cc87 8945f8                   mov        dword ptr [ebp - 8], eax
0043cc8a e944ffffff               jmp        0x43cbd3
0043cc8f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0043cc92 c6810e0e000601           mov        byte ptr [ecx + 0x6000e0e], 1
0043cc99 6a02                     push       2
0043cc9b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0043cc9e e8ed990000               call       0x446690
0043cca3 b9404d5c00               mov        ecx, 0x5c4d40
0043cca8 e8835afdff               call       0x412730
0043ccad 8945f0                   mov        dword ptr [ebp - 0x10], eax
0043ccb0 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0043ccb3 8b02                     mov        eax, dword ptr [edx]
0043ccb5 8b8864010000             mov        ecx, dword ptr [eax + 0x164]
0043ccbb 894de8                   mov        dword ptr [ebp - 0x18], ecx
0043ccbe 6a44                     push       0x44
0043ccc0 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0043ccc3 52                       push       edx
0043ccc4 ff55e8                   call       dword ptr [ebp - 0x18]
0043ccc7 b9404d5c00               mov        ecx, 0x5c4d40
0043cccc e85f5afdff               call       0x412730
0043ccd1 8945ec                   mov        dword ptr [ebp - 0x14], eax
0043ccd4 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0043ccd7 8b08                     mov        ecx, dword ptr [eax]
0043ccd9 8b914c010000             mov        edx, dword ptr [ecx + 0x14c]
0043ccdf 8955e0                   mov        dword ptr [ebp - 0x20], edx
0043cce2 8b45fc                   mov        eax, dword ptr [ebp - 4]
0043cce5 8b88800ed407             mov        ecx, dword ptr [eax + 0x7d40e80]
0043cceb 894de4                   mov        dword ptr [ebp - 0x1c], ecx
0043ccee 6a14                     push       0x14
0043ccf0 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0043ccf3 52                       push       edx
0043ccf4 8b4508                   mov        eax, dword ptr [ebp + 8]
0043ccf7 83e802                   sub        eax, 2
0043ccfa 50                       push       eax
0043ccfb 6a06                     push       6
0043ccfd 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0043cd00 51                       push       ecx
0043cd01 ff55e0                   call       dword ptr [ebp - 0x20]
0043cd04 6b550814                 imul       edx, dword ptr [ebp + 8], 0x14
0043cd08 8b45fc                   mov        eax, dword ptr [ebp - 4]
0043cd0b 0390800ed407             add        edx, dword ptr [eax + 0x7d40e80]
0043cd11 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0043cd14 8991800ed407             mov        dword ptr [ecx + 0x7d40e80], edx
0043cd1a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0043cd1d 8b82c4000000             mov        eax, dword ptr [edx + 0xc4]
0043cd23 83c001                   add        eax, 1
0043cd26 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0043cd29 8981c4000000             mov        dword ptr [ecx + 0xc4], eax
0043cd2f 33c0                     xor        eax, eax
0043cd31 8be5                     mov        esp, ebp
0043cd33 5d                       pop        ebp
0043cd34 c21000                   ret        0x10
0043cd37 cc                       int3       
0043cd38 cc                       int3       
0043cd39 cc                       int3       
0043cd3a cc                       int3       
0043cd3b cc                       int3       
0043cd3c cc                       int3       
0043cd3d cc                       int3       
0043cd3e cc                       int3       
0043cd3f cc                       int3       
