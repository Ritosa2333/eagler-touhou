00456920 55                       push       ebp
00456921 8bec                     mov        ebp, esp
00456923 83ec08                   sub        esp, 8
00456926 f30f104508               movss      xmm0, dword ptr [ebp + 8]
0045692b f30f5c4510               subss      xmm0, dword ptr [ebp + 0x10]
00456930 f30f104d1c               movss      xmm1, dword ptr [ebp + 0x1c]
00456935 f30f5c4d0c               subss      xmm1, dword ptr [ebp + 0xc]
0045693a f30f59c1                 mulss      xmm0, xmm1
0045693e f30f104d0c               movss      xmm1, dword ptr [ebp + 0xc]
00456943 f30f5c4d14               subss      xmm1, dword ptr [ebp + 0x14]
00456948 f30f105508               movss      xmm2, dword ptr [ebp + 8]
0045694d f30f5c5518               subss      xmm2, dword ptr [ebp + 0x18]
00456952 f30f59ca                 mulss      xmm1, xmm2
00456956 f30f58c1                 addss      xmm0, xmm1
0045695a f30f1145fc               movss      dword ptr [ebp - 4], xmm0
0045695f f30f104508               movss      xmm0, dword ptr [ebp + 8]
00456964 f30f5c4510               subss      xmm0, dword ptr [ebp + 0x10]
00456969 f30f104d24               movss      xmm1, dword ptr [ebp + 0x24]
0045696e f30f5c4d0c               subss      xmm1, dword ptr [ebp + 0xc]
00456973 f30f59c1                 mulss      xmm0, xmm1
00456977 f30f104d0c               movss      xmm1, dword ptr [ebp + 0xc]
0045697c f30f5c4d14               subss      xmm1, dword ptr [ebp + 0x14]
00456981 f30f105508               movss      xmm2, dword ptr [ebp + 8]
00456986 f30f5c5520               subss      xmm2, dword ptr [ebp + 0x20]
0045698b f30f59ca                 mulss      xmm1, xmm2
0045698f f30f58c1                 addss      xmm0, xmm1
00456993 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00456998 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
0045699d f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
004569a2 0f2f05e8e05600           comiss     xmm0, dword ptr [0x56e0e8]
004569a9 7607                     jbe        0x4569b2
004569ab 32c0                     xor        al, al
004569ad e97f010000               jmp        0x456b31
004569b2 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
004569b7 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
004569be 9f                       lahf       
004569bf f6c444                   test       ah, 0x44
004569c2 0f8ade000000             jp         0x456aa6
004569c8 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
004569cd 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
004569d4 9f                       lahf       
004569d5 f6c444                   test       ah, 0x44
004569d8 0f8ac8000000             jp         0x456aa6
004569de f30f104508               movss      xmm0, dword ptr [ebp + 8]
004569e3 0f2f4510                 comiss     xmm0, dword ptr [ebp + 0x10]
004569e7 763c                     jbe        0x456a25
004569e9 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004569ee f30f1145fc               movss      dword ptr [ebp - 4], xmm0
004569f3 f30f104508               movss      xmm0, dword ptr [ebp + 8]
004569f8 f30f114510               movss      dword ptr [ebp + 0x10], xmm0
004569fd f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00456a02 f30f114508               movss      dword ptr [ebp + 8], xmm0
00456a07 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00456a0c f30f1145fc               movss      dword ptr [ebp - 4], xmm0
00456a11 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00456a16 f30f114514               movss      dword ptr [ebp + 0x14], xmm0
00456a1b f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00456a20 f30f11450c               movss      dword ptr [ebp + 0xc], xmm0
00456a25 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00456a2a 0f2f4520                 comiss     xmm0, dword ptr [ebp + 0x20]
00456a2e 763c                     jbe        0x456a6c
00456a30 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00456a35 f30f1145fc               movss      dword ptr [ebp - 4], xmm0
00456a3a f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00456a3f f30f114520               movss      dword ptr [ebp + 0x20], xmm0
00456a44 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00456a49 f30f114518               movss      dword ptr [ebp + 0x18], xmm0
00456a4e f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00456a53 f30f1145fc               movss      dword ptr [ebp - 4], xmm0
00456a58 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00456a5d f30f114524               movss      dword ptr [ebp + 0x24], xmm0
00456a62 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00456a67 f30f11451c               movss      dword ptr [ebp + 0x1c], xmm0
00456a6c f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00456a71 0f2f4508                 comiss     xmm0, dword ptr [ebp + 8]
00456a75 7228                     jb         0x456a9f
00456a77 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00456a7c 0f2f450c                 comiss     xmm0, dword ptr [ebp + 0xc]
00456a80 721d                     jb         0x456a9f
00456a82 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00456a87 0f2f4518                 comiss     xmm0, dword ptr [ebp + 0x18]
00456a8b 7212                     jb         0x456a9f
00456a8d f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00456a92 0f2f451c                 comiss     xmm0, dword ptr [ebp + 0x1c]
00456a96 7207                     jb         0x456a9f
00456a98 b001                     mov        al, 1
00456a9a e992000000               jmp        0x456b31
00456a9f 32c0                     xor        al, al
00456aa1 e98b000000               jmp        0x456b31
00456aa6 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00456aab f30f5c4520               subss      xmm0, dword ptr [ebp + 0x20]
00456ab0 f30f104d0c               movss      xmm1, dword ptr [ebp + 0xc]
00456ab5 f30f5c4d1c               subss      xmm1, dword ptr [ebp + 0x1c]
00456aba f30f59c1                 mulss      xmm0, xmm1
00456abe f30f104d1c               movss      xmm1, dword ptr [ebp + 0x1c]
00456ac3 f30f5c4d24               subss      xmm1, dword ptr [ebp + 0x24]
00456ac8 f30f105518               movss      xmm2, dword ptr [ebp + 0x18]
00456acd f30f5c5508               subss      xmm2, dword ptr [ebp + 8]
00456ad2 f30f59ca                 mulss      xmm1, xmm2
00456ad6 f30f58c1                 addss      xmm0, xmm1
00456ada f30f1145fc               movss      dword ptr [ebp - 4], xmm0
00456adf f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00456ae4 f30f5c4520               subss      xmm0, dword ptr [ebp + 0x20]
00456ae9 f30f104d14               movss      xmm1, dword ptr [ebp + 0x14]
00456aee f30f5c4d1c               subss      xmm1, dword ptr [ebp + 0x1c]
00456af3 f30f59c1                 mulss      xmm0, xmm1
00456af7 f30f104d1c               movss      xmm1, dword ptr [ebp + 0x1c]
00456afc f30f5c4d24               subss      xmm1, dword ptr [ebp + 0x24]
00456b01 f30f105518               movss      xmm2, dword ptr [ebp + 0x18]
00456b06 f30f5c5510               subss      xmm2, dword ptr [ebp + 0x10]
00456b0b f30f59ca                 mulss      xmm1, xmm2
00456b0f f30f58c1                 addss      xmm0, xmm1
00456b13 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00456b18 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00456b1d f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
00456b22 0f2f05e8e05600           comiss     xmm0, dword ptr [0x56e0e8]
00456b29 7604                     jbe        0x456b2f
00456b2b 32c0                     xor        al, al
00456b2d eb02                     jmp        0x456b31
00456b2f b001                     mov        al, 1
00456b31 8be5                     mov        esp, ebp
00456b33 5d                       pop        ebp
00456b34 c3                       ret        
00456b35 cc                       int3       
00456b36 cc                       int3       
00456b37 cc                       int3       
00456b38 cc                       int3       
00456b39 cc                       int3       
00456b3a cc                       int3       
00456b3b cc                       int3       
00456b3c cc                       int3       
00456b3d cc                       int3       
00456b3e cc                       int3       
00456b3f cc                       int3       
