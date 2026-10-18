00512fb0 55                       push       ebp
00512fb1 8bec                     mov        ebp, esp
00512fb3 81ec68010000             sub        esp, 0x168
00512fb9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00512fbe 33c5                     xor        eax, ebp
00512fc0 8945fc                   mov        dword ptr [ebp - 4], eax
00512fc3 56                       push       esi
00512fc4 57                       push       edi
00512fc5 898d68ffffff             mov        dword ptr [ebp - 0x98], ecx
00512fcb 8b8568ffffff             mov        eax, dword ptr [ebp - 0x98]
00512fd1 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00512fd4 894814                   mov        dword ptr [eax + 0x14], ecx
00512fd7 6a00                     push       0
00512fd9 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
00512fdf 83c11c                   add        ecx, 0x1c
00512fe2 e83905f1ff               call       0x423520
00512fe7 8b9568ffffff             mov        edx, dword ptr [ebp - 0x98]
00512fed 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00512ff0 89422c                   mov        dword ptr [edx + 0x2c], eax
00512ff3 6a00                     push       0
00512ff5 e86650f6ff               call       0x478060
00512ffa 83c404                   add        esp, 4
00512ffd 8985e8feffff             mov        dword ptr [ebp - 0x118], eax
00513003 6a02                     push       2
00513005 8b8de8feffff             mov        ecx, dword ptr [ebp - 0x118]
0051300b e8007ef9ff               call       0x4aae10
00513010 898558ffffff             mov        dword ptr [ebp - 0xa8], eax
00513016 51                       push       ecx
00513017 0f57c0                   xorps      xmm0, xmm0
0051301a f30f110424               movss      dword ptr [esp], xmm0
0051301f 51                       push       ecx
00513020 0f57c0                   xorps      xmm0, xmm0
00513023 f30f110424               movss      dword ptr [esp], xmm0
00513028 51                       push       ecx
00513029 0f57c0                   xorps      xmm0, xmm0
0051302c f30f110424               movss      dword ptr [esp], xmm0
00513031 8d8d98feffff             lea        ecx, [ebp - 0x168]
00513037 e894fdf0ff               call       0x422dd0
0051303c 8985e0feffff             mov        dword ptr [ebp - 0x120], eax
00513042 6a00                     push       0
00513044 6aff                     push       -1
00513046 51                       push       ecx
00513047 0f57c0                   xorps      xmm0, xmm0
0051304a f30f110424               movss      dword ptr [esp], xmm0
0051304f 8b8de0feffff             mov        ecx, dword ptr [ebp - 0x120]
00513055 51                       push       ecx
00513056 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00513059 81c23f010000             add        edx, 0x13f
0051305f 52                       push       edx
00513060 68a8425700               push       0x5742a8
00513065 8d85b4feffff             lea        eax, [ebp - 0x14c]
0051306b 50                       push       eax
0051306c 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
00513072 e86960f6ff               call       0x4790e0
00513077 8b08                     mov        ecx, dword ptr [eax]
00513079 8b9568ffffff             mov        edx, dword ptr [ebp - 0x98]
0051307f 894a18                   mov        dword ptr [edx + 0x18], ecx
00513082 8d4dc4                   lea        ecx, [ebp - 0x3c]
00513085 e8a689f6ff               call       0x47ba30
0051308a 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
00513090 83c114                   add        ecx, 0x14
00513093 e8187df9ff               call       0x4aadb0
00513098 8b08                     mov        ecx, dword ptr [eax]
0051309a 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
0051309d 8b5004                   mov        edx, dword ptr [eax + 4]
005130a0 8955c8                   mov        dword ptr [ebp - 0x38], edx
005130a3 8b4008                   mov        eax, dword ptr [eax + 8]
005130a6 8945cc                   mov        dword ptr [ebp - 0x34], eax
005130a9 0f57c0                   xorps      xmm0, xmm0
005130ac f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
005130b1 f30f1005cc425700         movss      xmm0, dword ptr [0x5742cc]
005130b9 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
005130be f30f100598cd5600         movss      xmm0, dword ptr [0x56cd98]
005130c6 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
005130cb c785d0feffff0000ffff     mov        dword ptr [ebp - 0x130], 0xffff0000
005130d5 c785d4feffffff0000ff     mov        dword ptr [ebp - 0x12c], 0xff0000ff
005130df c785d8feffff00ffffff     mov        dword ptr [ebp - 0x128], 0xffffff00
005130e9 c785dcfeffff00ff00ff     mov        dword ptr [ebp - 0x124], 0xff00ff00
005130f3 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
005130f6 8b948dd0feffff           mov        edx, dword ptr [ebp + ecx*4 - 0x130]
005130fd 52                       push       edx
005130fe 8d4de4                   lea        ecx, [ebp - 0x1c]
00513101 e84aa5f4ff               call       0x45d650
00513106 90                       nop        
00513107 6a00                     push       0
00513109 e81244f2ff               call       0x437520
0051310e 83c404                   add        esp, 4
00513111 898554ffffff             mov        dword ptr [ebp - 0xac], eax
00513117 6a00                     push       0
00513119 8d45c4                   lea        eax, [ebp - 0x3c]
0051311c 50                       push       eax
0051311d 6a0b                     push       0xb
0051311f 8d8db0feffff             lea        ecx, [ebp - 0x150]
00513125 51                       push       ecx
00513126 8b8d54ffffff             mov        ecx, dword ptr [ebp - 0xac]
0051312c e83faaf8ff               call       0x49db70
00513131 90                       nop        
00513132 8b9568ffffff             mov        edx, dword ptr [ebp - 0x98]
00513138 83c218                   add        edx, 0x18
0051313b 899550ffffff             mov        dword ptr [ebp - 0xb0], edx
00513141 8b8568ffffff             mov        eax, dword ptr [ebp - 0x98]
00513147 50                       push       eax
00513148 8b8d50ffffff             mov        ecx, dword ptr [ebp - 0xb0]
0051314e e8fdd6f3ff               call       0x450850
00513153 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
00513159 83c118                   add        ecx, 0x18
0051315c 898d4cffffff             mov        dword ptr [ebp - 0xb4], ecx
00513162 6840165100               push       0x511640
00513167 8b8d4cffffff             mov        ecx, dword ptr [ebp - 0xb4]
0051316d e88ebff3ff               call       0x44f100
00513172 8d4d08                   lea        ecx, [ebp + 8]
00513175 e84679f9ff               call       0x4aaac0
0051317a 89856cffffff             mov        dword ptr [ebp - 0x94], eax
00513180 8d8d70ffffff             lea        ecx, [ebp - 0x90]
00513186 e8a589f6ff               call       0x47bb30
0051318b 90                       nop        
0051318c 51                       push       ecx
0051318d 0f57c0                   xorps      xmm0, xmm0
00513190 f30f110424               movss      dword ptr [esp], xmm0
00513195 51                       push       ecx
00513196 0f57c0                   xorps      xmm0, xmm0
00513199 f30f110424               movss      dword ptr [esp], xmm0
0051319e 51                       push       ecx
0051319f 0f57c0                   xorps      xmm0, xmm0
005131a2 f30f110424               movss      dword ptr [esp], xmm0
005131a7 8d8da4feffff             lea        ecx, [ebp - 0x15c]
005131ad e81efcf0ff               call       0x422dd0
005131b2 8b10                     mov        edx, dword ptr [eax]
005131b4 899570ffffff             mov        dword ptr [ebp - 0x90], edx
005131ba 8b4804                   mov        ecx, dword ptr [eax + 4]
005131bd 898d74ffffff             mov        dword ptr [ebp - 0x8c], ecx
005131c3 8b5008                   mov        edx, dword ptr [eax + 8]
005131c6 899578ffffff             mov        dword ptr [ebp - 0x88], edx
005131cc 83bd6cffffff00           cmp        dword ptr [ebp - 0x94], 0
005131d3 7417                     je         0x5131ec
005131d5 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
005131db e8800b0000               call       0x513d60
005131e0 b90c000000               mov        ecx, 0xc
005131e5 8bf0                     mov        esi, eax
005131e7 8d7d90                   lea        edi, [ebp - 0x70]
005131ea f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
005131ec 8b4508                   mov        eax, dword ptr [ebp + 8]
005131ef 8945c0                   mov        dword ptr [ebp - 0x40], eax
005131f2 6a00                     push       0
005131f4 e8870ef5ff               call       0x464080
005131f9 83c404                   add        esp, 4
005131fc 8bc8                     mov        ecx, eax
005131fe e8ede2ffff               call       0x5114f0
00513203 8985c0feffff             mov        dword ptr [ebp - 0x140], eax
00513209 6a00                     push       0
0051320b e8700ef5ff               call       0x464080
00513210 83c404                   add        esp, 4
00513213 8bc8                     mov        ecx, eax
00513215 e846e2ffff               call       0x511460
0051321a 8985c4feffff             mov        dword ptr [ebp - 0x13c], eax
00513220 6a00                     push       0
00513222 e8590ef5ff               call       0x464080
00513227 83c404                   add        esp, 4
0051322a 8bc8                     mov        ecx, eax
0051322c e8ffe2ffff               call       0x511530
00513231 8985c8feffff             mov        dword ptr [ebp - 0x138], eax
00513237 6a00                     push       0
00513239 e8420ef5ff               call       0x464080
0051323e 83c404                   add        esp, 4
00513241 8bc8                     mov        ecx, eax
00513243 e858e2ffff               call       0x5114a0
00513248 8985ccfeffff             mov        dword ptr [ebp - 0x134], eax
0051324e 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
00513254 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00513257 8b8495c0feffff           mov        eax, dword ptr [ebp + edx*4 - 0x140]
0051325e 894130                   mov        dword ptr [ecx + 0x30], eax
00513261 6a00                     push       0
00513263 e8f84df6ff               call       0x478060
00513268 83c404                   add        esp, 4
0051326b 898544ffffff             mov        dword ptr [ebp - 0xbc], eax
00513271 6b4d0c14                 imul       ecx, dword ptr [ebp + 0xc], 0x14
00513275 8b9568ffffff             mov        edx, dword ptr [ebp - 0x98]
0051327b 8b4230                   mov        eax, dword ptr [edx + 0x30]
0051327e 8b8c81e8095b00           mov        ecx, dword ptr [ecx + eax*4 + 0x5b09e8]
00513285 898d48ffffff             mov        dword ptr [ebp - 0xb8], ecx
0051328b 8b956cffffff             mov        edx, dword ptr [ebp - 0x94]
00513291 52                       push       edx
00513292 8d8570ffffff             lea        eax, [ebp - 0x90]
00513298 50                       push       eax
00513299 8b8d48ffffff             mov        ecx, dword ptr [ebp - 0xb8]
0051329f 51                       push       ecx
005132a0 8b8d44ffffff             mov        ecx, dword ptr [ebp - 0xbc]
005132a6 e87556f9ff               call       0x4a8920
005132ab 89856cffffff             mov        dword ptr [ebp - 0x94], eax
005132b1 6a00                     push       0
005132b3 e8c80df5ff               call       0x464080
005132b8 83c404                   add        esp, 4
005132bb 8bc8                     mov        ecx, eax
005132bd e8be0a0000               call       0x513d80
005132c2 99                       cdq        
005132c3 b903000000               mov        ecx, 3
005132c8 f7f9                     idiv       ecx
005132ca 899564ffffff             mov        dword ptr [ebp - 0x9c], edx
005132d0 83bd64ffffff00           cmp        dword ptr [ebp - 0x9c], 0
005132d7 7414                     je         0x5132ed
005132d9 83bd64ffffff01           cmp        dword ptr [ebp - 0x9c], 1
005132e0 742c                     je         0x51330e
005132e2 83bd64ffffff02           cmp        dword ptr [ebp - 0x9c], 2
005132e9 7444                     je         0x51332f
005132eb eb61                     jmp        0x51334e
005132ed 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
005132f3 e8480a0000               call       0x513d40
005132f8 898540ffffff             mov        dword ptr [ebp - 0xc0], eax
005132fe 6a06                     push       6
00513300 8b8d40ffffff             mov        ecx, dword ptr [ebp - 0xc0]
00513306 e865e2efff               call       0x411570
0051330b 90                       nop        
0051330c eb40                     jmp        0x51334e
0051330e 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00513314 e8270a0000               call       0x513d40
00513319 89853cffffff             mov        dword ptr [ebp - 0xc4], eax
0051331f 6a06                     push       6
00513321 8b8d3cffffff             mov        ecx, dword ptr [ebp - 0xc4]
00513327 e844e2efff               call       0x411570
0051332c 90                       nop        
0051332d eb1f                     jmp        0x51334e
0051332f 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00513335 e8060a0000               call       0x513d40
0051333a 898538ffffff             mov        dword ptr [ebp - 0xc8], eax
00513340 6a04                     push       4
00513342 8b8d38ffffff             mov        ecx, dword ptr [ebp - 0xc8]
00513348 e823e2efff               call       0x411570
0051334d 90                       nop        
0051334e 8b9568ffffff             mov        edx, dword ptr [ebp - 0x98]
00513354 8b4230                   mov        eax, dword ptr [edx + 0x30]
00513357 898560ffffff             mov        dword ptr [ebp - 0xa0], eax
0051335d 83bd60ffffff04           cmp        dword ptr [ebp - 0xa0], 4
00513364 0f8702020000             ja         0x51356c
0051336a 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
00513370 ff248de8365100           jmp        dword ptr [ecx*4 + 0x5136e8]
00513377 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
0051337d e8be090000               call       0x513d40
00513382 898534ffffff             mov        dword ptr [ebp - 0xcc], eax
00513388 6a1e                     push       0x1e
0051338a 6a01                     push       1
0051338c 8b8d34ffffff             mov        ecx, dword ptr [ebp - 0xcc]
00513392 e8990d0000               call       0x514130
00513397 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
0051339d e89e090000               call       0x513d40
005133a2 898530ffffff             mov        dword ptr [ebp - 0xd0], eax
005133a8 6a1e                     push       0x1e
005133aa 6a02                     push       2
005133ac 8b8d30ffffff             mov        ecx, dword ptr [ebp - 0xd0]
005133b2 e8790d0000               call       0x514130
005133b7 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
005133bd e87e090000               call       0x513d40
005133c2 89852cffffff             mov        dword ptr [ebp - 0xd4], eax
005133c8 51                       push       ecx
005133c9 f30f100528fa5600         movss      xmm0, dword ptr [0x56fa28]
005133d1 f30f110424               movss      dword ptr [esp], xmm0
005133d6 51                       push       ecx
005133d7 f30f100528fa5600         movss      xmm0, dword ptr [0x56fa28]
005133df f30f110424               movss      dword ptr [esp], xmm0
005133e4 8b8d2cffffff             mov        ecx, dword ptr [ebp - 0xd4]
005133ea e84194f8ff               call       0x49c830
005133ef 90                       nop        
005133f0 e9f0010000               jmp        0x5135e5
005133f5 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
005133fb e840090000               call       0x513d40
00513400 898528ffffff             mov        dword ptr [ebp - 0xd8], eax
00513406 6a21                     push       0x21
00513408 6a01                     push       1
0051340a 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
00513410 e81b0d0000               call       0x514130
00513415 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
0051341b e820090000               call       0x513d40
00513420 898524ffffff             mov        dword ptr [ebp - 0xdc], eax
00513426 6a21                     push       0x21
00513428 6a02                     push       2
0051342a 8b8d24ffffff             mov        ecx, dword ptr [ebp - 0xdc]
00513430 e8fb0c0000               call       0x514130
00513435 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
0051343b e800090000               call       0x513d40
00513440 898520ffffff             mov        dword ptr [ebp - 0xe0], eax
00513446 51                       push       ecx
00513447 f30f100530fa5600         movss      xmm0, dword ptr [0x56fa30]
0051344f f30f110424               movss      dword ptr [esp], xmm0
00513454 51                       push       ecx
00513455 f30f100530fa5600         movss      xmm0, dword ptr [0x56fa30]
0051345d f30f110424               movss      dword ptr [esp], xmm0
00513462 8b8d20ffffff             mov        ecx, dword ptr [ebp - 0xe0]
00513468 e8c393f8ff               call       0x49c830
0051346d 90                       nop        
0051346e e972010000               jmp        0x5135e5
00513473 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00513479 e8c2080000               call       0x513d40
0051347e 89851cffffff             mov        dword ptr [ebp - 0xe4], eax
00513484 6a24                     push       0x24
00513486 6a01                     push       1
00513488 8b8d1cffffff             mov        ecx, dword ptr [ebp - 0xe4]
0051348e e89d0c0000               call       0x514130
00513493 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00513499 e8a2080000               call       0x513d40
0051349e 898518ffffff             mov        dword ptr [ebp - 0xe8], eax
005134a4 6a24                     push       0x24
005134a6 6a02                     push       2
005134a8 8b8d18ffffff             mov        ecx, dword ptr [ebp - 0xe8]
005134ae e87d0c0000               call       0x514130
005134b3 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
005134b9 e882080000               call       0x513d40
005134be 898514ffffff             mov        dword ptr [ebp - 0xec], eax
005134c4 51                       push       ecx
005134c5 f30f1005d4425700         movss      xmm0, dword ptr [0x5742d4]
005134cd f30f110424               movss      dword ptr [esp], xmm0
005134d2 51                       push       ecx
005134d3 f30f1005d4425700         movss      xmm0, dword ptr [0x5742d4]
005134db f30f110424               movss      dword ptr [esp], xmm0
005134e0 8b8d14ffffff             mov        ecx, dword ptr [ebp - 0xec]
005134e6 e84593f8ff               call       0x49c830
005134eb 90                       nop        
005134ec e9f4000000               jmp        0x5135e5
005134f1 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
005134f7 e844080000               call       0x513d40
005134fc 898510ffffff             mov        dword ptr [ebp - 0xf0], eax
00513502 6a27                     push       0x27
00513504 6a01                     push       1
00513506 8b8d10ffffff             mov        ecx, dword ptr [ebp - 0xf0]
0051350c e81f0c0000               call       0x514130
00513511 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00513517 e824080000               call       0x513d40
0051351c 89850cffffff             mov        dword ptr [ebp - 0xf4], eax
00513522 6a27                     push       0x27
00513524 6a02                     push       2
00513526 8b8d0cffffff             mov        ecx, dword ptr [ebp - 0xf4]
0051352c e8ff0b0000               call       0x514130
00513531 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00513537 e804080000               call       0x513d40
0051353c 898508ffffff             mov        dword ptr [ebp - 0xf8], eax
00513542 51                       push       ecx
00513543 f30f10057cfe5600         movss      xmm0, dword ptr [0x56fe7c]
0051354b f30f110424               movss      dword ptr [esp], xmm0
00513550 51                       push       ecx
00513551 f30f10057cfe5600         movss      xmm0, dword ptr [0x56fe7c]
00513559 f30f110424               movss      dword ptr [esp], xmm0
0051355e 8b8d08ffffff             mov        ecx, dword ptr [ebp - 0xf8]
00513564 e8c792f8ff               call       0x49c830
00513569 90                       nop        
0051356a eb79                     jmp        0x5135e5
0051356c 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00513572 e8c9070000               call       0x513d40
00513577 898504ffffff             mov        dword ptr [ebp - 0xfc], eax
0051357d 6a2a                     push       0x2a
0051357f 6a01                     push       1
00513581 8b8d04ffffff             mov        ecx, dword ptr [ebp - 0xfc]
00513587 e8a40b0000               call       0x514130
0051358c 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00513592 e8a9070000               call       0x513d40
00513597 898500ffffff             mov        dword ptr [ebp - 0x100], eax
0051359d 6a2a                     push       0x2a
0051359f 6a02                     push       2
005135a1 8b8d00ffffff             mov        ecx, dword ptr [ebp - 0x100]
005135a7 e8840b0000               call       0x514130
005135ac 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
005135b2 e889070000               call       0x513d40
005135b7 8985fcfeffff             mov        dword ptr [ebp - 0x104], eax
005135bd 51                       push       ecx
005135be f30f100590f05600         movss      xmm0, dword ptr [0x56f090]
005135c6 f30f110424               movss      dword ptr [esp], xmm0
005135cb 51                       push       ecx
005135cc f30f100590f05600         movss      xmm0, dword ptr [0x56f090]
005135d4 f30f110424               movss      dword ptr [esp], xmm0
005135d9 8b8dfcfeffff             mov        ecx, dword ptr [ebp - 0x104]
005135df e84c92f8ff               call       0x49c830
005135e4 90                       nop        
005135e5 8b550c                   mov        edx, dword ptr [ebp + 0xc]
005135e8 89955cffffff             mov        dword ptr [ebp - 0xa4], edx
005135ee 83bd5cffffff03           cmp        dword ptr [ebp - 0xa4], 3
005135f5 0f878b000000             ja         0x513686
005135fb 8b855cffffff             mov        eax, dword ptr [ebp - 0xa4]
00513601 ff2485fc365100           jmp        dword ptr [eax*4 + 0x5136fc]
00513608 6a00                     push       0
0051360a e8710af5ff               call       0x464080
0051360f 83c404                   add        esp, 4
00513612 8985f8feffff             mov        dword ptr [ebp - 0x108], eax
00513618 6a01                     push       1
0051361a 8b8df8feffff             mov        ecx, dword ptr [ebp - 0x108]
00513620 e82b050000               call       0x513b50
00513625 90                       nop        
00513626 eb5e                     jmp        0x513686
00513628 6a00                     push       0
0051362a e8510af5ff               call       0x464080
0051362f 83c404                   add        esp, 4
00513632 8985f4feffff             mov        dword ptr [ebp - 0x10c], eax
00513638 6a01                     push       1
0051363a 8b8df4feffff             mov        ecx, dword ptr [ebp - 0x10c]
00513640 e84b040000               call       0x513a90
00513645 90                       nop        
00513646 eb3e                     jmp        0x513686
00513648 6a00                     push       0
0051364a e8310af5ff               call       0x464080
0051364f 83c404                   add        esp, 4
00513652 8985f0feffff             mov        dword ptr [ebp - 0x110], eax
00513658 6a01                     push       1
0051365a 8b8df0feffff             mov        ecx, dword ptr [ebp - 0x110]
00513660 e8ab050000               call       0x513c10
00513665 90                       nop        
00513666 eb1e                     jmp        0x513686
00513668 6a00                     push       0
0051366a e8110af5ff               call       0x464080
0051366f 83c404                   add        esp, 4
00513672 8985ecfeffff             mov        dword ptr [ebp - 0x114], eax
00513678 6a01                     push       1
0051367a 8b8decfeffff             mov        ecx, dword ptr [ebp - 0x114]
00513680 e86b040000               call       0x513af0
00513685 90                       nop        
00513686 6a00                     push       0
00513688 e8f309f5ff               call       0x464080
0051368d 83c404                   add        esp, 4
00513690 8985e4feffff             mov        dword ptr [ebp - 0x11c], eax
00513696 6a01                     push       1
00513698 8b8de4feffff             mov        ecx, dword ptr [ebp - 0x11c]
0051369e e80d050000               call       0x513bb0
005136a3 90                       nop        
005136a4 83ec28                   sub        esp, 0x28
005136a7 8bf4                     mov        esi, esp
005136a9 89a5bcfeffff             mov        dword ptr [ebp - 0x144], esp
005136af 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
005136b5 51                       push       ecx
005136b6 8d8db8feffff             lea        ecx, [ebp - 0x148]
005136bc e8df0bf0ff               call       0x4142a0
005136c1 50                       push       eax
005136c2 8bce                     mov        ecx, esi
005136c4 e8d7dfffff               call       0x5116a0
005136c9 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
005136cf e8dc090000               call       0x5140b0
005136d4 90                       nop        
005136d5 5f                       pop        edi
005136d6 5e                       pop        esi
005136d7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005136da 33cd                     xor        ecx, ebp
005136dc e80df30200               call       0x5429ee
005136e1 8be5                     mov        esp, ebp
005136e3 5d                       pop        ebp
005136e4 c20800                   ret        8
005136e7 90                       nop        
005136e8 7733                     ja         0x51371d
005136ea 51                       push       ecx
005136eb 00f5                     add        ch, dh
005136ed 335100                   xor        edx, dword ptr [ecx]
005136f0 7334                     jae        0x513726
005136f2 51                       push       ecx
005136f3 00f1                     add        cl, dh
005136f5 3451                     xor        al, 0x51
005136f7 006c3551                 add        byte ptr [ebp + esi + 0x51], ch
005136fb 0008                     add        byte ptr [eax], cl
005136fd 3651                     push       ecx
005136ff 0028                     add        byte ptr [eax], ch
00513701 3651                     push       ecx
00513703 004836                   add        byte ptr [eax + 0x36], cl
00513706 51                       push       ecx
00513707 006836                   add        byte ptr [eax + 0x36], ch
0051370a 51                       push       ecx
0051370b 00cc                     add        ah, cl
0051370d cc                       int3       
0051370e cc                       int3       
0051370f cc                       int3       
