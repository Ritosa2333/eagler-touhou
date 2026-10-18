00528fc0 55                       push       ebp
00528fc1 8bec                     mov        ebp, esp
00528fc3 83ec58                   sub        esp, 0x58
00528fc6 894dfc                   mov        dword ptr [ebp - 4], ecx
00528fc9 8b45fc                   mov        eax, dword ptr [ebp - 4]
00528fcc 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00528fcf 894df8                   mov        dword ptr [ebp - 8], ecx
00528fd2 837df804                 cmp        dword ptr [ebp - 8], 4
00528fd6 0f872b040000             ja         0x529407
00528fdc 8b55f8                   mov        edx, dword ptr [ebp - 8]
00528fdf ff249510945200           jmp        dword ptr [edx*4 + 0x529410]
00528fe6 8b45fc                   mov        eax, dword ptr [ebp - 4]
00528fe9 0590030000               add        eax, 0x390
00528fee 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00528ff1 6a00                     push       0
00528ff3 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00528ff6 e83504f2ff               call       0x449430
00528ffb 0fb6c8                   movzx      ecx, al
00528ffe 85c9                     test       ecx, ecx
00529000 742d                     je         0x52902f
00529002 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00529008 e843e9f0ff               call       0x437950
0052900d 8945e0                   mov        dword ptr [ebp - 0x20], eax
00529010 6a00                     push       0
00529012 6aff                     push       -1
00529014 6a13                     push       0x13
00529016 6a00                     push       0
00529018 8d55a8                   lea        edx, [ebp - 0x58]
0052901b 52                       push       edx
0052901c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052901f e84c7cf2ff               call       0x450c70
00529024 8b00                     mov        eax, dword ptr [eax]
00529026 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529029 898190030000             mov        dword ptr [ecx + 0x390], eax
0052902f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00529032 83c224                   add        edx, 0x24
00529035 8955dc                   mov        dword ptr [ebp - 0x24], edx
00529038 6a07                     push       7
0052903a 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0052903d e89e34f7ff               call       0x49c4e0
00529042 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529045 83c124                   add        ecx, 0x24
00529048 e853d8fbff               call       0x4e68a0
0052904d 6a2f                     push       0x2f
0052904f 6a0c                     push       0xc
00529051 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529054 e8b73c0000               call       0x52cd10
00529059 90                       nop        
0052905a 6a55                     push       0x55
0052905c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052905f e8ec360000               call       0x52c750
00529064 85c0                     test       eax, eax
00529066 750b                     jne        0x529073
00529068 6a55                     push       0x55
0052906a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052906d e80e3f0000               call       0x52cf80
00529072 90                       nop        
00529073 6a01                     push       1
00529075 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529078 e8d33d0000               call       0x52ce50
0052907d 90                       nop        
0052907e 833d28615c0005           cmp        dword ptr [0x5c6128], 5
00529085 756a                     jne        0x5290f1
00529087 a1540a5b00               mov        eax, dword ptr [0x5b0a54]
0052908c 50                       push       eax
0052908d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529090 83c124                   add        ecx, 0x24
00529093 e8b85cf9ff               call       0x4bed50
00529098 c705540a5b00ffffffff     mov        dword ptr [0x5b0a54], 0xffffffff
005290a2 6a55                     push       0x55
005290a4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005290a7 e8e43b0000               call       0x52cc90
005290ac 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005290af 83c124                   add        ecx, 0x24
005290b2 894dd8                   mov        dword ptr [ebp - 0x28], ecx
005290b5 6a07                     push       7
005290b7 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
005290ba e8c14fffff               call       0x51e080
005290bf 0fbfd0                   movsx      edx, ax
005290c2 52                       push       edx
005290c3 6a55                     push       0x55
005290c5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005290c8 e8433c0000               call       0x52cd10
005290cd 6a06                     push       6
005290cf 6a55                     push       0x55
005290d1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005290d4 e8373c0000               call       0x52cd10
005290d9 6888000000               push       0x88
005290de 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005290e1 e8aa3b0000               call       0x52cc90
005290e6 90                       nop        
005290e7 e985020000               jmp        0x529371
005290ec e980020000               jmp        0x529371
005290f1 6a2c                     push       0x2c
005290f3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005290f6 e8853e0000               call       0x52cf80
005290fb 90                       nop        
005290fc 8b45fc                   mov        eax, dword ptr [ebp - 4]
005290ff 0554010000               add        eax, 0x154
00529104 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00529107 6a0a                     push       0xa
00529109 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0052910c e85f7ff3ff               call       0x461070
00529111 0fb6c8                   movzx      ecx, al
00529114 85c9                     test       ecx, ecx
00529116 7441                     je         0x529159
00529118 6a02                     push       2
0052911a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052911d e82e3d0000               call       0x52ce50
00529122 6a55                     push       0x55
00529124 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529127 e8643b0000               call       0x52cc90
0052912c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052912f 83c224                   add        edx, 0x24
00529132 8955d0                   mov        dword ptr [ebp - 0x30], edx
00529135 6a07                     push       7
00529137 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0052913a e8414fffff               call       0x51e080
0052913f 98                       cwde       
00529140 50                       push       eax
00529141 6a55                     push       0x55
00529143 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529146 e8853b0000               call       0x52ccd0
0052914b 6888000000               push       0x88
00529150 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529153 e8383b0000               call       0x52cc90
00529158 90                       nop        
00529159 e9a9020000               jmp        0x529407
0052915e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529161 83c124                   add        ecx, 0x24
00529164 e8976af9ff               call       0x4bfc00
00529169 90                       nop        
0052916a 33c9                     xor        ecx, ecx
0052916c 7531                     jne        0x52919f
0052916e ba04000000               mov        edx, 4
00529173 6bc200                   imul       eax, edx, 0
00529176 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
0052917d 7420                     je         0x52919f
0052917f b904000000               mov        ecx, 4
00529184 6bd100                   imul       edx, ecx, 0
00529187 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
0052918d 8945cc                   mov        dword ptr [ebp - 0x34], eax
00529190 6a10                     push       0x10
00529192 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00529195 e8e610efff               call       0x41a280
0052919a 8945f4                   mov        dword ptr [ebp - 0xc], eax
0052919d eb07                     jmp        0x5291a6
0052919f c745f400000000           mov        dword ptr [ebp - 0xc], 0
005291a6 837df400                 cmp        dword ptr [ebp - 0xc], 0
005291aa 7414                     je         0x5291c0
005291ac 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005291af 83c124                   add        ecx, 0x24
005291b2 894dc8                   mov        dword ptr [ebp - 0x38], ecx
005291b5 6a00                     push       0
005291b7 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
005291ba e8815ef9ff               call       0x4bf040
005291bf 90                       nop        
005291c0 33d2                     xor        edx, edx
005291c2 7531                     jne        0x5291f5
005291c4 b804000000               mov        eax, 4
005291c9 6bc800                   imul       ecx, eax, 0
005291cc 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
005291d3 7420                     je         0x5291f5
005291d5 ba04000000               mov        edx, 4
005291da 6bc200                   imul       eax, edx, 0
005291dd 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
005291e3 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
005291e6 6a20                     push       0x20
005291e8 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
005291eb e89010efff               call       0x41a280
005291f0 8945f0                   mov        dword ptr [ebp - 0x10], eax
005291f3 eb07                     jmp        0x5291fc
005291f5 c745f000000000           mov        dword ptr [ebp - 0x10], 0
005291fc 837df000                 cmp        dword ptr [ebp - 0x10], 0
00529200 7414                     je         0x529216
00529202 8b55fc                   mov        edx, dword ptr [ebp - 4]
00529205 83c224                   add        edx, 0x24
00529208 8955c0                   mov        dword ptr [ebp - 0x40], edx
0052920b 6a00                     push       0
0052920d 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00529210 e80b5ef9ff               call       0x4bf020
00529215 90                       nop        
00529216 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529219 83c124                   add        ecx, 0x24
0052921c e80f3ef3ff               call       0x45d030
00529221 85c0                     test       eax, eax
00529223 743a                     je         0x52925f
00529225 6a00                     push       0
00529227 6a0a                     push       0xa
00529229 b930a85b00               mov        ecx, 0x5ba830
0052922e e83ddbefff               call       0x426d70
00529233 6a55                     push       0x55
00529235 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529238 e8533a0000               call       0x52cc90
0052923d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00529240 83c024                   add        eax, 0x24
00529243 8945bc                   mov        dword ptr [ebp - 0x44], eax
00529246 6a07                     push       7
00529248 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
0052924b e8304effff               call       0x51e080
00529250 0fbfc8                   movsx      ecx, ax
00529253 51                       push       ecx
00529254 6a55                     push       0x55
00529256 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529259 e8723a0000               call       0x52ccd0
0052925e 90                       nop        
0052925f 33d2                     xor        edx, edx
00529261 7534                     jne        0x529297
00529263 b804000000               mov        eax, 4
00529268 6bc800                   imul       ecx, eax, 0
0052926b 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
00529272 7423                     je         0x529297
00529274 ba04000000               mov        edx, 4
00529279 6bc200                   imul       eax, edx, 0
0052927c 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
00529282 894db8                   mov        dword ptr [ebp - 0x48], ecx
00529285 6806010000               push       0x106
0052928a 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
0052928d e86e09efff               call       0x419c00
00529292 8945ec                   mov        dword ptr [ebp - 0x14], eax
00529295 eb07                     jmp        0x52929e
00529297 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
0052929e 837dec00                 cmp        dword ptr [ebp - 0x14], 0
005292a2 742d                     je         0x5292d1
005292a4 6a04                     push       4
005292a6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005292a9 e8a23b0000               call       0x52ce50
005292ae 6a00                     push       0
005292b0 6a09                     push       9
005292b2 b930a85b00               mov        ecx, 0x5ba830
005292b7 e8b4daefff               call       0x426d70
005292bc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005292bf 83c124                   add        ecx, 0x24
005292c2 e83930eeff               call       0x40c300
005292c7 a3500a5b00               mov        dword ptr [0x5b0a50], eax
005292cc e936010000               jmp        0x529407
005292d1 33d2                     xor        edx, edx
005292d3 7534                     jne        0x529309
005292d5 b804000000               mov        eax, 4
005292da 6bc800                   imul       ecx, eax, 0
005292dd 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
005292e4 7423                     je         0x529309
005292e6 ba04000000               mov        edx, 4
005292eb 6bc200                   imul       eax, edx, 0
005292ee 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
005292f4 894db4                   mov        dword ptr [ebp - 0x4c], ecx
005292f7 6801000800               push       0x80001
005292fc 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
005292ff e8fc08efff               call       0x419c00
00529304 8945e8                   mov        dword ptr [ebp - 0x18], eax
00529307 eb07                     jmp        0x529310
00529309 c745e800000000           mov        dword ptr [ebp - 0x18], 0
00529310 837de800                 cmp        dword ptr [ebp - 0x18], 0
00529314 7439                     je         0x52934f
00529316 6a06                     push       6
00529318 6a55                     push       0x55
0052931a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052931d e8ae390000               call       0x52ccd0
00529322 6a03                     push       3
00529324 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529327 e8243b0000               call       0x52ce50
0052932c 6a00                     push       0
0052932e 6a07                     push       7
00529330 b930a85b00               mov        ecx, 0x5ba830
00529335 e836daefff               call       0x426d70
0052933a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052933d 83c124                   add        ecx, 0x24
00529340 e8bb2feeff               call       0x40c300
00529345 a3500a5b00               mov        dword ptr [0x5b0a50], eax
0052934a e9b8000000               jmp        0x529407
0052934f e9b3000000               jmp        0x529407
00529354 8b55fc                   mov        edx, dword ptr [ebp - 4]
00529357 81c254010000             add        edx, 0x154
0052935d 8955b0                   mov        dword ptr [ebp - 0x50], edx
00529360 6a14                     push       0x14
00529362 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00529365 e856a2efff               call       0x4235c0
0052936a 0fb6c0                   movzx      eax, al
0052936d 85c0                     test       eax, eax
0052936f 7441                     je         0x5293b2
00529371 6a2c                     push       0x2c
00529373 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529376 e8b5380000               call       0x52cc30
0052937b 6a13                     push       0x13
0052937d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529380 e84b3b0000               call       0x52ced0
00529385 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529388 83c124                   add        ecx, 0x24
0052938b e8702feeff               call       0x40c300
00529390 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529393 8981d8580000             mov        dword ptr [ecx + 0x58d8], eax
00529399 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052939c 83c124                   add        ecx, 0x24
0052939f e80ccdf9ff               call       0x4c60b0
005293a4 6a00                     push       0
005293a6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005293a9 83c124                   add        ecx, 0x24
005293ac e89f59f9ff               call       0x4bed50
005293b1 90                       nop        
005293b2 eb53                     jmp        0x529407
005293b4 8b55fc                   mov        edx, dword ptr [ebp - 4]
005293b7 81c254010000             add        edx, 0x154
005293bd 8955ac                   mov        dword ptr [ebp - 0x54], edx
005293c0 6a06                     push       6
005293c2 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
005293c5 e8f6a1efff               call       0x4235c0
005293ca 0fb6c0                   movzx      eax, al
005293cd 85c0                     test       eax, eax
005293cf 7436                     je         0x529407
005293d1 6a2c                     push       0x2c
005293d3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005293d6 e855380000               call       0x52cc30
005293db 6a55                     push       0x55
005293dd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005293e0 e84b380000               call       0x52cc30
005293e5 6a30                     push       0x30
005293e7 6a0c                     push       0xc
005293e9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005293ec e81f390000               call       0x52cd10
005293f1 6a07                     push       7
005293f3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005293f6 e8d53a0000               call       0x52ced0
005293fb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005293fe 83c124                   add        ecx, 0x24
00529401 e80accf9ff               call       0x4c6010
00529406 90                       nop        
00529407 b801000000               mov        eax, 1
0052940c 8be5                     mov        esp, ebp
0052940e 5d                       pop        ebp
0052940f c3                       ret        
00529410 e68f                     out        0x8f, al
00529412 52                       push       edx
00529413 00fc                     add        ah, bh
00529415 90                       nop        
00529416 52                       push       edx
00529417 005e91                   add        byte ptr [esi - 0x6f], bl
0052941a 52                       push       edx
0052941b 00549352                 add        byte ptr [ebx + edx*4 + 0x52], dl
0052941f 00b4935200cccc           add        byte ptr [ebx + edx*4 - 0x3333ffae], dh
00529426 cc                       int3       
00529427 cc                       int3       
00529428 cc                       int3       
00529429 cc                       int3       
0052942a cc                       int3       
0052942b cc                       int3       
0052942c cc                       int3       
0052942d cc                       int3       
0052942e cc                       int3       
0052942f cc                       int3       
