00538620 55                       push       ebp
00538621 8bec                     mov        ebp, esp
00538623 81ec8c000000             sub        esp, 0x8c
00538629 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0053862e 33c5                     xor        eax, ebp
00538630 8945fc                   mov        dword ptr [ebp - 4], eax
00538633 898d7cffffff             mov        dword ptr [ebp - 0x84], ecx
00538639 c745b400000000           mov        dword ptr [ebp - 0x4c], 0
00538640 eb09                     jmp        0x53864b
00538642 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
00538645 83c001                   add        eax, 1
00538648 8945b4                   mov        dword ptr [ebp - 0x4c], eax
0053864b 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
0053864e 3b4d08                   cmp        ecx, dword ptr [ebp + 8]
00538651 0f8d08020000             jge        0x53885f
00538657 6a00                     push       0
00538659 e8d281f2ff               call       0x460830
0053865e 83c404                   add        esp, 4
00538661 8945ac                   mov        dword ptr [ebp - 0x54], eax
00538664 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
00538667 52                       push       edx
00538668 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
0053866b e850d1ffff               call       0x5357c0
00538670 8bc8                     mov        ecx, eax
00538672 e8f992eeff               call       0x421970
00538677 8b08                     mov        ecx, dword ptr [eax]
00538679 894df0                   mov        dword ptr [ebp - 0x10], ecx
0053867c 8b5004                   mov        edx, dword ptr [eax + 4]
0053867f 8955f4                   mov        dword ptr [ebp - 0xc], edx
00538682 8b4008                   mov        eax, dword ptr [eax + 8]
00538685 8945f8                   mov        dword ptr [ebp - 8], eax
00538688 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
0053868b 51                       push       ecx
0053868c b9cc695c00               mov        ecx, 0x5c69cc
00538691 e8eabeedff               call       0x414580
00538696 8945a8                   mov        dword ptr [ebp - 0x58], eax
00538699 6a00                     push       0
0053869b 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0053869e e88d0df1ff               call       0x449430
005386a3 0fb6d0                   movzx      edx, al
005386a6 85d2                     test       edx, edx
005386a8 0f8471010000             je         0x53881f
005386ae 8d4db8                   lea        ecx, [ebp - 0x48]
005386b1 e87a33f4ff               call       0x47ba30
005386b6 6a00                     push       0
005386b8 e87381f2ff               call       0x460830
005386bd 83c404                   add        esp, 4
005386c0 8bc8                     mov        ecx, eax
005386c2 e88981f2ff               call       0x460850
005386c7 8b08                     mov        ecx, dword ptr [eax]
005386c9 894db8                   mov        dword ptr [ebp - 0x48], ecx
005386cc 8b5004                   mov        edx, dword ptr [eax + 4]
005386cf 8955bc                   mov        dword ptr [ebp - 0x44], edx
005386d2 8b4008                   mov        eax, dword ptr [eax + 8]
005386d5 8945c0                   mov        dword ptr [ebp - 0x40], eax
005386d8 6a00                     push       0
005386da e841eeefff               call       0x437520
005386df 83c404                   add        esp, 4
005386e2 8945a4                   mov        dword ptr [ebp - 0x5c], eax
005386e5 6a00                     push       0
005386e7 8d4db8                   lea        ecx, [ebp - 0x48]
005386ea 51                       push       ecx
005386eb 6a07                     push       7
005386ed 8d9578ffffff             lea        edx, [ebp - 0x88]
005386f3 52                       push       edx
005386f4 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
005386f7 e87454f6ff               call       0x49db70
005386fc 8b00                     mov        eax, dword ptr [eax]
005386fe 8945a0                   mov        dword ptr [ebp - 0x60], eax
00538701 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00538704 51                       push       ecx
00538705 b9cc695c00               mov        ecx, 0x5c69cc
0053870a e871beedff               call       0x414580
0053870f 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00538712 8910                     mov        dword ptr [eax], edx
00538714 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
00538717 50                       push       eax
00538718 b9cc695c00               mov        ecx, 0x5c69cc
0053871d e85ebeedff               call       0x414580
00538722 89459c                   mov        dword ptr [ebp - 0x64], eax
00538725 51                       push       ecx
00538726 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
0053872e f30f110424               movss      dword ptr [esp], xmm0
00538733 51                       push       ecx
00538734 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
0053873c f30f110424               movss      dword ptr [esp], xmm0
00538741 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
00538744 e8677ff1ff               call       0x4506b0
00538749 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
0053874c 51                       push       ecx
0053874d b9e0695c00               mov        ecx, 0x5c69e0
00538752 e829beedff               call       0x414580
00538757 8bc8                     mov        ecx, eax
00538759 e8a297f8ff               call       0x4c1f00
0053875e 90                       nop        
0053875f 6a00                     push       0
00538761 e85a07f4ff               call       0x478ec0
00538766 83c404                   add        esp, 4
00538769 894598                   mov        dword ptr [ebp - 0x68], eax
0053876c 6a02                     push       2
0053876e 687f969800               push       0x98967f
00538773 51                       push       ecx
00538774 0f57c0                   xorps      xmm0, xmm0
00538777 f30f110424               movss      dword ptr [esp], xmm0
0053877c 51                       push       ecx
0053877d f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
00538785 f30f110424               movss      dword ptr [esp], xmm0
0053878a 8d55f0                   lea        edx, [ebp - 0x10]
0053878d 52                       push       edx
0053878e 8d8574ffffff             lea        eax, [ebp - 0x8c]
00538794 50                       push       eax
00538795 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00538798 e8e393f8ff               call       0x4c1b80
0053879d 8b08                     mov        ecx, dword ptr [eax]
0053879f 894d94                   mov        dword ptr [ebp - 0x6c], ecx
005387a2 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
005387a5 52                       push       edx
005387a6 b9e0695c00               mov        ecx, 0x5c69e0
005387ab e8d0bdedff               call       0x414580
005387b0 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
005387b3 8908                     mov        dword ptr [eax], ecx
005387b5 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
005387b8 52                       push       edx
005387b9 b9e0695c00               mov        ecx, 0x5c69e0
005387be e8bdbdedff               call       0x414580
005387c3 8bc8                     mov        ecx, eax
005387c5 e84686f8ff               call       0x4c0e10
005387ca 894590                   mov        dword ptr [ebp - 0x70], eax
005387cd 6a03                     push       3
005387cf 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
005387d2 e8091df4ff               call       0x47a4e0
005387d7 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
005387da 50                       push       eax
005387db b9e0695c00               mov        ecx, 0x5c69e0
005387e0 e89bbdedff               call       0x414580
005387e5 8bc8                     mov        ecx, eax
005387e7 e82486f8ff               call       0x4c0e10
005387ec 89458c                   mov        dword ptr [ebp - 0x74], eax
005387ef 687f969800               push       0x98967f
005387f4 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
005387f7 e854eff3ff               call       0x477750
005387fc 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
005387ff 51                       push       ecx
00538800 b9e0695c00               mov        ecx, 0x5c69e0
00538805 e876bdedff               call       0x414580
0053880a 8bc8                     mov        ecx, eax
0053880c e8ff85f8ff               call       0x4c0e10
00538811 894588                   mov        dword ptr [ebp - 0x78], eax
00538814 6a01                     push       1
00538816 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00538819 e8d2e4fcff               call       0x506cf0
0053881e 90                       nop        
0053881f 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
00538822 52                       push       edx
00538823 b9cc695c00               mov        ecx, 0x5c69cc
00538828 e853bdedff               call       0x414580
0053882d 894584                   mov        dword ptr [ebp - 0x7c], eax
00538830 8d45f0                   lea        eax, [ebp - 0x10]
00538833 50                       push       eax
00538834 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
00538837 e8a4bdf2ff               call       0x4645e0
0053883c 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
0053883f 51                       push       ecx
00538840 b9e0695c00               mov        ecx, 0x5c69e0
00538845 e836bdedff               call       0x414580
0053884a 894580                   mov        dword ptr [ebp - 0x80], eax
0053884d 8d55f0                   lea        edx, [ebp - 0x10]
00538850 52                       push       edx
00538851 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00538854 e89797f8ff               call       0x4c1ff0
00538859 90                       nop        
0053885a e9e3fdffff               jmp        0x538642
0053885f 8b4508                   mov        eax, dword ptr [ebp + 8]
00538862 8945b0                   mov        dword ptr [ebp - 0x50], eax
00538865 eb09                     jmp        0x538870
00538867 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0053886a 83c101                   add        ecx, 1
0053886d 894db0                   mov        dword ptr [ebp - 0x50], ecx
00538870 837db004                 cmp        dword ptr [ebp - 0x50], 4
00538874 7d2d                     jge        0x5388a3
00538876 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
00538879 52                       push       edx
0053887a b9cc695c00               mov        ecx, 0x5c69cc
0053887f e8fcbcedff               call       0x414580
00538884 8bc8                     mov        ecx, eax
00538886 e84574f1ff               call       0x44fcd0
0053888b 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
0053888e 50                       push       eax
0053888f b9e0695c00               mov        ecx, 0x5c69e0
00538894 e8e7bcedff               call       0x414580
00538899 8bc8                     mov        ecx, eax
0053889b e86096f8ff               call       0x4c1f00
005388a0 90                       nop        
005388a1 ebc4                     jmp        0x538867
005388a3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005388a6 33cd                     xor        ecx, ebp
005388a8 e841a10000               call       0x5429ee
005388ad 8be5                     mov        esp, ebp
005388af 5d                       pop        ebp
005388b0 c20400                   ret        4
005388b3 cc                       int3       
005388b4 cc                       int3       
005388b5 cc                       int3       
005388b6 cc                       int3       
005388b7 cc                       int3       
005388b8 cc                       int3       
005388b9 cc                       int3       
005388ba cc                       int3       
005388bb cc                       int3       
005388bc cc                       int3       
005388bd cc                       int3       
005388be cc                       int3       
005388bf cc                       int3       
