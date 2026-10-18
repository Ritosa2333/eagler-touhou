00483270 55                       push       ebp
00483271 8bec                     mov        ebp, esp
00483273 83ec50                   sub        esp, 0x50
00483276 56                       push       esi
00483277 894dfc                   mov        dword ptr [ebp - 4], ecx
0048327a c745f800000000           mov        dword ptr [ebp - 8], 0
00483281 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483284 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
00483287 e8444afbff               call       0x437cd0
0048328c f30f104048               movss      xmm0, dword ptr [eax + 0x48]
00483291 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00483296 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483299 8b491c                   mov        ecx, dword ptr [ecx + 0x1c]
0048329c e82f4afbff               call       0x437cd0
004832a1 f30f10404c               movss      xmm0, dword ptr [eax + 0x4c]
004832a6 f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
004832ab 51                       push       ecx
004832ac f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004832b1 f30f110424               movss      dword ptr [esp], xmm0
004832b6 51                       push       ecx
004832b7 f30f1045ec               movss      xmm0, dword ptr [ebp - 0x14]
004832bc f30f110424               movss      dword ptr [esp], xmm0
004832c1 8b55fc                   mov        edx, dword ptr [ebp - 4]
004832c4 83c264                   add        edx, 0x64
004832c7 52                       push       edx
004832c8 e803220000               call       0x4854d0
004832cd 83c40c                   add        esp, 0xc
004832d0 85c0                     test       eax, eax
004832d2 0f849e020000             je         0x483576
004832d8 8b45fc                   mov        eax, dword ptr [ebp - 4]
004832db 05a0000000               add        eax, 0xa0
004832e0 8945e8                   mov        dword ptr [ebp - 0x18], eax
004832e3 6a06                     push       6
004832e5 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004832e8 e89393ffff               call       0x47c680
004832ed 8b4838                   mov        ecx, dword ptr [eax + 0x38]
004832f0 83e101                   and        ecx, 1
004832f3 745e                     je         0x483353
004832f5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004832f8 f30f5a4268               cvtss2sd   xmm0, dword ptr [edx + 0x68]
004832fd 0f57c9                   xorps      xmm1, xmm1
00483300 660f2fc8                 comisd     xmm1, xmm0
00483304 764d                     jbe        0x483353
00483306 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483309 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
0048330c e8bf49fbff               call       0x437cd0
00483311 f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
00483319 f30f584048               addss      xmm0, dword ptr [eax + 0x48]
0048331e f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
00483323 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483326 f30f104168               movss      xmm0, dword ptr [ecx + 0x68]
0048332b f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00483330 f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00483335 f30f5845e0               addss      xmm0, dword ptr [ebp - 0x20]
0048333a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048333d f30f114268               movss      dword ptr [edx + 0x68], xmm0
00483342 c745f801000000           mov        dword ptr [ebp - 8], 1
00483349 e98e010000               jmp        0x4834dc
0048334e e989010000               jmp        0x4834dc
00483353 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483356 05a0000000               add        eax, 0xa0
0048335b 8945dc                   mov        dword ptr [ebp - 0x24], eax
0048335e 6a06                     push       6
00483360 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00483363 e81893ffff               call       0x47c680
00483368 8b4838                   mov        ecx, dword ptr [eax + 0x38]
0048336b 83e102                   and        ecx, 2
0048336e 745e                     je         0x4833ce
00483370 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483373 f30f104268               movss      xmm0, dword ptr [edx + 0x68]
00483378 0f2f05c0d75600           comiss     xmm0, dword ptr [0x56d7c0]
0048337f 764d                     jbe        0x4833ce
00483381 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483384 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
00483387 e84449fbff               call       0x437cd0
0048338c f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
00483394 f30f584048               addss      xmm0, dword ptr [eax + 0x48]
00483399 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
0048339e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004833a1 f30f104168               movss      xmm0, dword ptr [ecx + 0x68]
004833a6 f30f1145d8               movss      dword ptr [ebp - 0x28], xmm0
004833ab f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
004833b0 f30f5c45d4               subss      xmm0, dword ptr [ebp - 0x2c]
004833b5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004833b8 f30f114268               movss      dword ptr [edx + 0x68], xmm0
004833bd c745f801000000           mov        dword ptr [ebp - 8], 1
004833c4 e913010000               jmp        0x4834dc
004833c9 e90e010000               jmp        0x4834dc
004833ce 8b45fc                   mov        eax, dword ptr [ebp - 4]
004833d1 05a0000000               add        eax, 0xa0
004833d6 8945d0                   mov        dword ptr [ebp - 0x30], eax
004833d9 6a06                     push       6
004833db 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004833de e89d92ffff               call       0x47c680
004833e3 8b4838                   mov        ecx, dword ptr [eax + 0x38]
004833e6 83e104                   and        ecx, 4
004833e9 7466                     je         0x483451
004833eb f30f10056cfd5600         movss      xmm0, dword ptr [0x56fd6c]
004833f3 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004833fb 8b55fc                   mov        edx, dword ptr [ebp - 4]
004833fe 0f2f4264                 comiss     xmm0, dword ptr [edx + 0x64]
00483402 764d                     jbe        0x483451
00483404 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483407 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
0048340a e8c148fbff               call       0x437cd0
0048340f f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
00483417 f30f58404c               addss      xmm0, dword ptr [eax + 0x4c]
0048341c f30f1145c8               movss      dword ptr [ebp - 0x38], xmm0
00483421 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483424 f30f104164               movss      xmm0, dword ptr [ecx + 0x64]
00483429 f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
0048342e f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
00483433 f30f5845c8               addss      xmm0, dword ptr [ebp - 0x38]
00483438 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048343b f30f114264               movss      dword ptr [edx + 0x64], xmm0
00483440 c745f801000000           mov        dword ptr [ebp - 8], 1
00483447 e990000000               jmp        0x4834dc
0048344c e98b000000               jmp        0x4834dc
00483451 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483454 05a0000000               add        eax, 0xa0
00483459 8945c4                   mov        dword ptr [ebp - 0x3c], eax
0048345c 6a06                     push       6
0048345e 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00483461 e81a92ffff               call       0x47c680
00483466 8b4838                   mov        ecx, dword ptr [eax + 0x38]
00483469 83e108                   and        ecx, 8
0048346c 7464                     je         0x4834d2
0048346e f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
00483476 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0048347e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483481 f30f104a64               movss      xmm1, dword ptr [edx + 0x64]
00483486 0f2fc8                   comiss     xmm1, xmm0
00483489 7647                     jbe        0x4834d2
0048348b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048348e 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
00483491 e83a48fbff               call       0x437cd0
00483496 f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
0048349e f30f58404c               addss      xmm0, dword ptr [eax + 0x4c]
004834a3 f30f1145bc               movss      dword ptr [ebp - 0x44], xmm0
004834a8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004834ab f30f104164               movss      xmm0, dword ptr [ecx + 0x64]
004834b0 f30f1145c0               movss      dword ptr [ebp - 0x40], xmm0
004834b5 f30f1045c0               movss      xmm0, dword ptr [ebp - 0x40]
004834ba f30f5c45bc               subss      xmm0, dword ptr [ebp - 0x44]
004834bf 8b55fc                   mov        edx, dword ptr [ebp - 4]
004834c2 f30f114264               movss      dword ptr [edx + 0x64], xmm0
004834c7 c745f801000000           mov        dword ptr [ebp - 8], 1
004834ce eb0c                     jmp        0x4834dc
004834d0 eb0a                     jmp        0x4834dc
004834d2 837df800                 cmp        dword ptr [ebp - 8], 0
004834d6 0f849a000000             je         0x483576
004834dc 8b45fc                   mov        eax, dword ptr [ebp - 4]
004834df 05a0000000               add        eax, 0xa0
004834e4 8945b8                   mov        dword ptr [ebp - 0x48], eax
004834e7 6a06                     push       6
004834e9 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004834ec e88f91ffff               call       0x47c680
004834f1 83c030                   add        eax, 0x30
004834f4 8945f4                   mov        dword ptr [ebp - 0xc], eax
004834f7 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004834fa 8b11                     mov        edx, dword ptr [ecx]
004834fc 83c201                   add        edx, 1
004834ff 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00483502 8910                     mov        dword ptr [eax], edx
00483504 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483507 83794000                 cmp        dword ptr [ecx + 0x40], 0
0048350b 7c14                     jl         0x483521
0048350d 6a00                     push       0
0048350f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483512 8b4240                   mov        eax, dword ptr [edx + 0x40]
00483515 50                       push       eax
00483516 b930a85b00               mov        ecx, 0x5ba830
0048351b e85038faff               call       0x426d70
00483520 90                       nop        
00483521 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483524 81c1a0000000             add        ecx, 0xa0
0048352a 894db0                   mov        dword ptr [ebp - 0x50], ecx
0048352d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483530 81c2a0000000             add        edx, 0xa0
00483536 8955b4                   mov        dword ptr [ebp - 0x4c], edx
00483539 6a06                     push       6
0048353b 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
0048353e e83d91ffff               call       0x47c680
00483543 8bf0                     mov        esi, eax
00483545 6a06                     push       6
00483547 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0048354a e83191ffff               call       0x47c680
0048354f 8b4030                   mov        eax, dword ptr [eax + 0x30]
00483552 3b4634                   cmp        eax, dword ptr [esi + 0x34]
00483555 7c1f                     jl         0x483576
00483557 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048355a 8b9190000000             mov        edx, dword ptr [ecx + 0x90]
00483560 81e2ffefffff             and        edx, 0xffffefff
00483566 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483569 899090000000             mov        dword ptr [eax + 0x90], edx
0048356f b801000000               mov        eax, 1
00483574 eb02                     jmp        0x483578
00483576 33c0                     xor        eax, eax
00483578 5e                       pop        esi
00483579 8be5                     mov        esp, ebp
0048357b 5d                       pop        ebp
0048357c c3                       ret        
0048357d cc                       int3       
0048357e cc                       int3       
0048357f cc                       int3       
