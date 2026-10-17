0051feb0 55                       push       ebp
0051feb1 8bec                     mov        ebp, esp
0051feb3 81ec30010000             sub        esp, 0x130
0051feb9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0051febe 33c5                     xor        eax, ebp
0051fec0 8945fc                   mov        dword ptr [ebp - 4], eax
0051fec3 898d20ffffff             mov        dword ptr [ebp - 0xe0], ecx
0051fec9 c78528ffffff00000000     mov        dword ptr [ebp - 0xd8], 0
0051fed3 c78510ffffff00000000     mov        dword ptr [ebp - 0xf0], 0
0051fedd eb0f                     jmp        0x51feee
0051fedf 8b8510ffffff             mov        eax, dword ptr [ebp - 0xf0]
0051fee5 83c001                   add        eax, 1
0051fee8 898510ffffff             mov        dword ptr [ebp - 0xf0], eax
0051feee 83bd10ffffff05           cmp        dword ptr [ebp - 0xf0], 5
0051fef5 7d12                     jge        0x51ff09
0051fef7 8b8d10ffffff             mov        ecx, dword ptr [ebp - 0xf0]
0051fefd 8b5510                   mov        edx, dword ptr [ebp + 0x10]
0051ff00 c7048afeffffff           mov        dword ptr [edx + ecx*4], 0xfffffffe
0051ff07 ebd6                     jmp        0x51fedf
0051ff09 8b8520ffffff             mov        eax, dword ptr [ebp - 0xe0]
0051ff0f 0554010000               add        eax, 0x154
0051ff14 8985f8feffff             mov        dword ptr [ebp - 0x108], eax
0051ff1a 6a02                     push       2
0051ff1c 8b8df8feffff             mov        ecx, dword ptr [ebp - 0x108]
0051ff22 e83936f0ff               call       0x423560
0051ff27 0fb6c8                   movzx      ecx, al
0051ff2a 85c9                     test       ecx, ecx
0051ff2c 0f84b6000000             je         0x51ffe8
0051ff32 c7850cffffff00000000     mov        dword ptr [ebp - 0xf4], 0
0051ff3c eb0f                     jmp        0x51ff4d
0051ff3e 8b950cffffff             mov        edx, dword ptr [ebp - 0xf4]
0051ff44 83c201                   add        edx, 1
0051ff47 89950cffffff             mov        dword ptr [ebp - 0xf4], edx
0051ff4d 83bd0cffffff07           cmp        dword ptr [ebp - 0xf4], 7
0051ff54 7d18                     jge        0x51ff6e
0051ff56 8b850cffffff             mov        eax, dword ptr [ebp - 0xf4]
0051ff5c 83c07f                   add        eax, 0x7f
0051ff5f 50                       push       eax
0051ff60 8b8d20ffffff             mov        ecx, dword ptr [ebp - 0xe0]
0051ff66 e885ce0000               call       0x52cdf0
0051ff6b 90                       nop        
0051ff6c ebd0                     jmp        0x51ff3e
0051ff6e 837d0806                 cmp        dword ptr [ebp + 8], 6
0051ff72 7453                     je         0x51ffc7
0051ff74 c78518ffffff00000000     mov        dword ptr [ebp - 0xe8], 0
0051ff7e eb0f                     jmp        0x51ff8f
0051ff80 8b8d18ffffff             mov        ecx, dword ptr [ebp - 0xe8]
0051ff86 83c101                   add        ecx, 1
0051ff89 898d18ffffff             mov        dword ptr [ebp - 0xe8], ecx
0051ff8f 83bd18ffffff04           cmp        dword ptr [ebp - 0xe8], 4
0051ff96 7d2d                     jge        0x51ffc5
0051ff98 8b9518ffffff             mov        edx, dword ptr [ebp - 0xe8]
0051ff9e 83c27f                   add        edx, 0x7f
0051ffa1 52                       push       edx
0051ffa2 8b8d20ffffff             mov        ecx, dword ptr [ebp - 0xe0]
0051ffa8 e8d3cf0000               call       0x52cf80
0051ffad 8b8518ffffff             mov        eax, dword ptr [ebp - 0xe8]
0051ffb3 83c07f                   add        eax, 0x7f
0051ffb6 50                       push       eax
0051ffb7 8b8d20ffffff             mov        ecx, dword ptr [ebp - 0xe0]
0051ffbd e8cecc0000               call       0x52cc90
0051ffc2 90                       nop        
0051ffc3 ebbb                     jmp        0x51ff80
0051ffc5 eb21                     jmp        0x51ffe8
0051ffc7 6883000000               push       0x83
0051ffcc 8b8d20ffffff             mov        ecx, dword ptr [ebp - 0xe0]
0051ffd2 e8a9cf0000               call       0x52cf80
0051ffd7 6883000000               push       0x83
0051ffdc 8b8d20ffffff             mov        ecx, dword ptr [ebp - 0xe0]
0051ffe2 e8a9cc0000               call       0x52cc90
0051ffe7 90                       nop        
0051ffe8 c7851cffffff00000000     mov        dword ptr [ebp - 0xe4], 0
0051fff2 c785fcfeffff00000000     mov        dword ptr [ebp - 0x104], 0
0051fffc 51                       push       ecx
0051fffd 0f57c0                   xorps      xmm0, xmm0
00520000 f30f110424               movss      dword ptr [esp], xmm0
00520005 51                       push       ecx
00520006 f30f100558105700         movss      xmm0, dword ptr [0x571058]
0052000e f30f110424               movss      dword ptr [esp], xmm0
00520013 51                       push       ecx
00520014 f30f100578565700         movss      xmm0, dword ptr [0x575678]
0052001c f30f110424               movss      dword ptr [esp], xmm0
00520021 8d8d2cffffff             lea        ecx, [ebp - 0xd4]
00520027 e8a42df0ff               call       0x422dd0
0052002c 90                       nop        
0052002d c78524ffffff00000000     mov        dword ptr [ebp - 0xdc], 0
00520037 eb0f                     jmp        0x520048
00520039 8b8d24ffffff             mov        ecx, dword ptr [ebp - 0xdc]
0052003f 83c101                   add        ecx, 1
00520042 898d24ffffff             mov        dword ptr [ebp - 0xdc], ecx
00520048 83bd24ffffff05           cmp        dword ptr [ebp - 0xdc], 5
0052004f 0f8d45040000             jge        0x52049a
00520055 69550804010000           imul       edx, dword ptr [ebp + 8], 0x104
0052005c 6b450c14                 imul       eax, dword ptr [ebp + 0xc], 0x14
00520060 8d8c0220f25a00           lea        ecx, [edx + eax + 0x5af220]
00520067 8b9524ffffff             mov        edx, dword ptr [ebp - 0xdc]
0052006d 8b0491                   mov        eax, dword ptr [ecx + edx*4]
00520070 898528ffffff             mov        dword ptr [ebp - 0xd8], eax
00520076 83bd28ffffff00           cmp        dword ptr [ebp - 0xd8], 0
0052007d 7d05                     jge        0x520084
0052007f e916040000               jmp        0x52049a
00520084 837d0806                 cmp        dword ptr [ebp + 8], 6
00520088 7534                     jne        0x5200be
0052008a 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
00520090 833c8d58f05a0004         cmp        dword ptr [ecx*4 + 0x5af058], 4
00520098 750c                     jne        0x5200a6
0052009a c78508ffffff00000000     mov        dword ptr [ebp - 0xf8], 0
005200a4 eb0a                     jmp        0x5200b0
005200a6 c78508ffffff01000000     mov        dword ptr [ebp - 0xf8], 1
005200b0 8b9508ffffff             mov        edx, dword ptr [ebp - 0xf8]
005200b6 899514ffffff             mov        dword ptr [ebp - 0xec], edx
005200bc eb3b                     jmp        0x5200f9
005200be 8b8528ffffff             mov        eax, dword ptr [ebp - 0xd8]
005200c4 833c8558f05a0005         cmp        dword ptr [eax*4 + 0x5af058], 5
005200cc 7c0c                     jl         0x5200da
005200ce c78504ffffff04000000     mov        dword ptr [ebp - 0xfc], 4
005200d8 eb13                     jmp        0x5200ed
005200da 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
005200e0 8b148d58f05a00           mov        edx, dword ptr [ecx*4 + 0x5af058]
005200e7 899504ffffff             mov        dword ptr [ebp - 0xfc], edx
005200ed 8b8504ffffff             mov        eax, dword ptr [ebp - 0xfc]
005200f3 898514ffffff             mov        dword ptr [ebp - 0xec], eax
005200f9 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
005200ff e8fc85f6ff               call       0x488700
00520104 05080b0000               add        eax, 0xb08
00520109 8985f4feffff             mov        dword ptr [ebp - 0x10c], eax
0052010f 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
00520115 51                       push       ecx
00520116 8b8df4feffff             mov        ecx, dword ptr [ebp - 0x10c]
0052011c e8ef6cf6ff               call       0x486e10
00520121 ba04000000               mov        edx, 4
00520126 6bca00                   imul       ecx, edx, 0
00520129 83bc08c800000000         cmp        dword ptr [eax + ecx + 0xc8], 0
00520131 0f84cf020000             je         0x520406
00520137 8b9514ffffff             mov        edx, dword ptr [ebp - 0xec]
0052013d 8b4510                   mov        eax, dword ptr [ebp + 0x10]
00520140 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
00520146 890c90                   mov        dword ptr [eax + edx*4], ecx
00520149 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052014f e8ac85f6ff               call       0x488700
00520154 05080b0000               add        eax, 0xb08
00520159 8985f0feffff             mov        dword ptr [ebp - 0x110], eax
0052015f 8b9528ffffff             mov        edx, dword ptr [ebp - 0xd8]
00520165 52                       push       edx
00520166 8b8df0feffff             mov        ecx, dword ptr [ebp - 0x110]
0052016c e89f6cf6ff               call       0x486e10
00520171 50                       push       eax
00520172 68c1000000               push       0xc1
00520177 8d8538ffffff             lea        eax, [ebp - 0xc8]
0052017d 50                       push       eax
0052017e e88d840200               call       0x548610
00520183 83c40c                   add        esp, 0xc
00520186 8d8d38ffffff             lea        ecx, [ebp - 0xc8]
0052018c 51                       push       ecx
0052018d e88efc0200               call       0x54fe20
00520192 83c404                   add        esp, 4
00520195 89851cffffff             mov        dword ptr [ebp - 0xe4], eax
0052019b eb0f                     jmp        0x5201ac
0052019d 8b951cffffff             mov        edx, dword ptr [ebp - 0xe4]
005201a3 83c201                   add        edx, 1
005201a6 89951cffffff             mov        dword ptr [ebp - 0xe4], edx
005201ac 83bd1cffffff2a           cmp        dword ptr [ebp - 0xe4], 0x2a
005201b3 7d10                     jge        0x5201c5
005201b5 8b851cffffff             mov        eax, dword ptr [ebp - 0xe4]
005201bb c6840538ffffff20         mov        byte ptr [ebp + eax - 0xc8], 0x20
005201c3 ebd8                     jmp        0x52019d
005201c5 8b8d1cffffff             mov        ecx, dword ptr [ebp - 0xe4]
005201cb 898d00ffffff             mov        dword ptr [ebp - 0x100], ecx
005201d1 81bd00ffffffc1000000     cmp        dword ptr [ebp - 0x100], 0xc1
005201db 7302                     jae        0x5201df
005201dd eb06                     jmp        0x5201e5
005201df e871290200               call       0x542b55
005201e4 90                       nop        
005201e5 8b9500ffffff             mov        edx, dword ptr [ebp - 0x100]
005201eb c6841538ffffff00         mov        byte ptr [ebp + edx - 0xc8], 0
005201f3 8b4514                   mov        eax, dword ptr [ebp + 0x14]
005201f6 3b8524ffffff             cmp        eax, dword ptr [ebp - 0xdc]
005201fc 7513                     jne        0x520211
005201fe 6800ffffff               push       0xffffff00
00520203 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520209 e8c208f5ff               call       0x470ad0
0052020e 90                       nop        
0052020f eb11                     jmp        0x520222
00520211 68808080ff               push       0xff808080
00520216 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052021c e8af08f5ff               call       0x470ad0
00520221 90                       nop        
00520222 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00520228 e823fafeff               call       0x50fc50
0052022d 05080b0000               add        eax, 0xb08
00520232 8985ecfeffff             mov        dword ptr [ebp - 0x114], eax
00520238 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
0052023e 51                       push       ecx
0052023f 8b8decfeffff             mov        ecx, dword ptr [ebp - 0x114]
00520245 e8c66bf6ff               call       0x486e10
0052024a ba04000000               mov        edx, 4
0052024f 6bca00                   imul       ecx, edx, 0
00520252 83bc08c800000000         cmp        dword ptr [eax + ecx + 0xc8], 0
0052025a 7569                     jne        0x5202c5
0052025c 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00520262 e8e9f9feff               call       0x50fc50
00520267 05080b0000               add        eax, 0xb08
0052026c 8985e8feffff             mov        dword ptr [ebp - 0x118], eax
00520272 8b9528ffffff             mov        edx, dword ptr [ebp - 0xd8]
00520278 52                       push       edx
00520279 8b8de8feffff             mov        ecx, dword ptr [ebp - 0x118]
0052027f e88c6bf6ff               call       0x486e10
00520284 b904000000               mov        ecx, 4
00520289 c1e100                   shl        ecx, 0
0052028c 83bc08c800000000         cmp        dword ptr [eax + ecx + 0xc8], 0
00520294 752f                     jne        0x5202c5
00520296 8b5514                   mov        edx, dword ptr [ebp + 0x14]
00520299 3b9524ffffff             cmp        edx, dword ptr [ebp - 0xdc]
0052029f 7513                     jne        0x5202b4
005202a1 68408080ff               push       0xff808040
005202a6 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005202ac e81f08f5ff               call       0x470ad0
005202b1 90                       nop        
005202b2 eb11                     jmp        0x5202c5
005202b4 68404040ff               push       0xff404040
005202b9 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005202bf e80c08f5ff               call       0x470ad0
005202c4 90                       nop        
005202c5 6a04                     push       4
005202c7 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005202cd e8ae5efaff               call       0x4c6180
005202d2 90                       nop        
005202d3 8d8538ffffff             lea        eax, [ebp - 0xc8]
005202d9 50                       push       eax
005202da 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
005202e0 83c101                   add        ecx, 1
005202e3 51                       push       ecx
005202e4 83ec08                   sub        esp, 8
005202e7 8bcc                     mov        ecx, esp
005202e9 6828555700               push       0x575528
005202ee e8dddaeeff               call       0x40ddd0
005202f3 8d952cffffff             lea        edx, [ebp - 0xd4]
005202f9 52                       push       edx
005202fa a198065c00               mov        eax, dword ptr [0x5c0698]
005202ff 50                       push       eax
00520300 e80bbff4ff               call       0x46c210
00520305 83c418                   add        esp, 0x18
00520308 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
0052030b 3b8d24ffffff             cmp        ecx, dword ptr [ebp - 0xdc]
00520311 0f85ed000000             jne        0x520404
00520317 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052031d e82ef9feff               call       0x50fc50
00520322 05080b0000               add        eax, 0xb08
00520327 8985e4feffff             mov        dword ptr [ebp - 0x11c], eax
0052032d 8b9528ffffff             mov        edx, dword ptr [ebp - 0xd8]
00520333 52                       push       edx
00520334 8b8de4feffff             mov        ecx, dword ptr [ebp - 0x11c]
0052033a e8d16af6ff               call       0x486e10
0052033f b904000000               mov        ecx, 4
00520344 6bd100                   imul       edx, ecx, 0
00520347 83bc10c800000000         cmp        dword ptr [eax + edx + 0xc8], 0
0052034f 0f85af000000             jne        0x520404
00520355 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052035b e8f0f8feff               call       0x50fc50
00520360 05080b0000               add        eax, 0xb08
00520365 8985e0feffff             mov        dword ptr [ebp - 0x120], eax
0052036b 8b8528ffffff             mov        eax, dword ptr [ebp - 0xd8]
00520371 50                       push       eax
00520372 8b8de0feffff             mov        ecx, dword ptr [ebp - 0x120]
00520378 e8936af6ff               call       0x486e10
0052037d b904000000               mov        ecx, 4
00520382 c1e100                   shl        ecx, 0
00520385 83bc08c800000000         cmp        dword ptr [eax + ecx + 0xc8], 0
0052038d 7575                     jne        0x520404
0052038f 6aff                     push       -1
00520391 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520397 e83407f5ff               call       0x470ad0
0052039c 6a04                     push       4
0052039e 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005203a4 e8d75dfaff               call       0x4c6180
005203a9 90                       nop        
005203aa 51                       push       ecx
005203ab 0f57c0                   xorps      xmm0, xmm0
005203ae f30f110424               movss      dword ptr [esp], xmm0
005203b3 51                       push       ecx
005203b4 f30f100560105700         movss      xmm0, dword ptr [0x571060]
005203bc f30f110424               movss      dword ptr [esp], xmm0
005203c1 51                       push       ecx
005203c2 f30f10057c565700         movss      xmm0, dword ptr [0x57567c]
005203ca f30f110424               movss      dword ptr [esp], xmm0
005203cf 8d8dd0feffff             lea        ecx, [ebp - 0x130]
005203d5 e8f629f0ff               call       0x422dd0
005203da 8985dcfeffff             mov        dword ptr [ebp - 0x124], eax
005203e0 83ec08                   sub        esp, 8
005203e3 8bcc                     mov        ecx, esp
005203e5 6834555700               push       0x575534
005203ea e8e1d9eeff               call       0x40ddd0
005203ef 8b95dcfeffff             mov        edx, dword ptr [ebp - 0x124]
005203f5 52                       push       edx
005203f6 a198065c00               mov        eax, dword ptr [0x5c0698]
005203fb 50                       push       eax
005203fc e80fbef4ff               call       0x46c210
00520401 83c410                   add        esp, 0x10
00520404 eb5d                     jmp        0x520463
00520406 8b8d14ffffff             mov        ecx, dword ptr [ebp - 0xec]
0052040c 8b5510                   mov        edx, dword ptr [ebp + 0x10]
0052040f c7048affffffff           mov        dword ptr [edx + ecx*4], 0xffffffff
00520416 68404040ff               push       0xff404040
0052041b 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520421 e8aa06f5ff               call       0x470ad0
00520426 6a04                     push       4
00520428 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052042e e84d5dfaff               call       0x4c6180
00520433 90                       nop        
00520434 8b8528ffffff             mov        eax, dword ptr [ebp - 0xd8]
0052043a 83c001                   add        eax, 1
0052043d 50                       push       eax
0052043e 83ec08                   sub        esp, 8
00520441 8bcc                     mov        ecx, esp
00520443 685c555700               push       0x57555c
00520448 e883d9eeff               call       0x40ddd0
0052044d 8d8d2cffffff             lea        ecx, [ebp - 0xd4]
00520453 51                       push       ecx
00520454 8b1598065c00             mov        edx, dword ptr [0x5c0698]
0052045a 52                       push       edx
0052045b e8b0bdf4ff               call       0x46c210
00520460 83c414                   add        esp, 0x14
00520463 f30f108530ffffff         movss      xmm0, dword ptr [ebp - 0xd0]
0052046b f30f58055c565700         addss      xmm0, dword ptr [0x57565c]
00520473 f30f118530ffffff         movss      dword ptr [ebp - 0xd0], xmm0
0052047b 8b85fcfeffff             mov        eax, dword ptr [ebp - 0x104]
00520481 3b8514ffffff             cmp        eax, dword ptr [ebp - 0xec]
00520487 7d0c                     jge        0x520495
00520489 8b8d14ffffff             mov        ecx, dword ptr [ebp - 0xec]
0052048f 898dfcfeffff             mov        dword ptr [ebp - 0x104], ecx
00520495 e99ffbffff               jmp        0x520039
0052049a 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005204a0 e83b63fcff               call       0x4e67e0
005204a5 33c0                     xor        eax, eax
005204a7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005204aa 33cd                     xor        ecx, ebp
005204ac e83d250200               call       0x5429ee
005204b1 8be5                     mov        esp, ebp
005204b3 5d                       pop        ebp
005204b4 c21000                   ret        0x10
005204b7 cc                       int3       
005204b8 cc                       int3       
005204b9 cc                       int3       
005204ba cc                       int3       
005204bb cc                       int3       
005204bc cc                       int3       
005204bd cc                       int3       
005204be cc                       int3       
005204bf cc                       int3       
