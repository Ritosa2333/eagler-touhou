005240d0 55                       push       ebp
005240d1 8bec                     mov        ebp, esp
005240d3 81ec28010000             sub        esp, 0x128
005240d9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
005240de 33c5                     xor        eax, ebp
005240e0 8945fc                   mov        dword ptr [ebp - 4], eax
005240e3 894d98                   mov        dword ptr [ebp - 0x68], ecx
005240e6 8b4598                   mov        eax, dword ptr [ebp - 0x68]
005240e9 8b4820                   mov        ecx, dword ptr [eax + 0x20]
005240ec 894d84                   mov        dword ptr [ebp - 0x7c], ecx
005240ef 837d8402                 cmp        dword ptr [ebp - 0x7c], 2
005240f3 740f                     je         0x524104
005240f5 837d8404                 cmp        dword ptr [ebp - 0x7c], 4
005240f9 0f8478040000             je         0x524577
005240ff e9f60a0000               jmp        0x524bfa
00524104 51                       push       ecx
00524105 0f57c0                   xorps      xmm0, xmm0
00524108 f30f110424               movss      dword ptr [esp], xmm0
0052410d 51                       push       ecx
0052410e f30f100530fa5600         movss      xmm0, dword ptr [0x56fa30]
00524116 f30f110424               movss      dword ptr [esp], xmm0
0052411b 51                       push       ecx
0052411c f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
00524124 f30f110424               movss      dword ptr [esp], xmm0
00524129 8d4d9c                   lea        ecx, [ebp - 0x64]
0052412c e89fecefff               call       0x422dd0
00524131 6a01                     push       1
00524133 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00524139 e8e228fcff               call       0x4e6a20
0052413e 90                       nop        
0052413f 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00524142 81c108010000             add        ecx, 0x108
00524148 e8b381eeff               call       0x40c300
0052414d 6bd019                   imul       edx, eax, 0x19
00524150 895588                   mov        dword ptr [ebp - 0x78], edx
00524153 eb09                     jmp        0x52415e
00524155 8b4588                   mov        eax, dword ptr [ebp - 0x78]
00524158 83c001                   add        eax, 1
0052415b 894588                   mov        dword ptr [ebp - 0x78], eax
0052415e 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00524161 81c108010000             add        ecx, 0x108
00524167 e89481eeff               call       0x40c300
0052416c 6bc819                   imul       ecx, eax, 0x19
0052416f 83c119                   add        ecx, 0x19
00524172 394d88                   cmp        dword ptr [ebp - 0x78], ecx
00524175 0f8ddc030000             jge        0x524557
0052417b 8b5598                   mov        edx, dword ptr [ebp - 0x68]
0052417e 83c224                   add        edx, 0x24
00524181 899514ffffff             mov        dword ptr [ebp - 0xec], edx
00524187 8b4588                   mov        eax, dword ptr [ebp - 0x78]
0052418a 99                       cdq        
0052418b b919000000               mov        ecx, 0x19
00524190 f7f9                     idiv       ecx
00524192 52                       push       edx
00524193 8b8d14ffffff             mov        ecx, dword ptr [ebp - 0xec]
00524199 e81270f4ff               call       0x46b1b0
0052419e 85c0                     test       eax, eax
005241a0 7413                     je         0x5241b5
005241a2 6800ffffff               push       0xffffff00
005241a7 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005241ad e81ec9f4ff               call       0x470ad0
005241b2 90                       nop        
005241b3 eb11                     jmp        0x5241c6
005241b5 68808080ff               push       0xff808080
005241ba 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005241c0 e80bc9f4ff               call       0x470ad0
005241c5 90                       nop        
005241c6 8b5598                   mov        edx, dword ptr [ebp - 0x68]
005241c9 81c240570000             add        edx, 0x5740
005241cf 8995f8feffff             mov        dword ptr [ebp - 0x108], edx
005241d5 8b4588                   mov        eax, dword ptr [ebp - 0x78]
005241d8 50                       push       eax
005241d9 8b8df8feffff             mov        ecx, dword ptr [ebp - 0x108]
005241df e89c03efff               call       0x414580
005241e4 833800                   cmp        dword ptr [eax], 0
005241e7 0f84f4020000             je         0x5244e1
005241ed 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
005241f0 81c140570000             add        ecx, 0x5740
005241f6 898df4feffff             mov        dword ptr [ebp - 0x10c], ecx
005241fc 8b5588                   mov        edx, dword ptr [ebp - 0x78]
005241ff 52                       push       edx
00524200 8b8df4feffff             mov        ecx, dword ptr [ebp - 0x10c]
00524206 e87503efff               call       0x414580
0052420b 8b08                     mov        ecx, dword ptr [eax]
0052420d e8fed5efff               call       0x421810
00524212 894590                   mov        dword ptr [ebp - 0x70], eax
00524215 33c0                     xor        eax, eax
00524217 8945a8                   mov        dword ptr [ebp - 0x58], eax
0052421a 8945ac                   mov        dword ptr [ebp - 0x54], eax
0052421d 8945b0                   mov        dword ptr [ebp - 0x50], eax
00524220 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00524223 8945b8                   mov        dword ptr [ebp - 0x48], eax
00524226 8945bc                   mov        dword ptr [ebp - 0x44], eax
00524229 8945c0                   mov        dword ptr [ebp - 0x40], eax
0052422c 8945c4                   mov        dword ptr [ebp - 0x3c], eax
0052422f 8945c8                   mov        dword ptr [ebp - 0x38], eax
00524232 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00524235 83c110                   add        ecx, 0x10
00524238 51                       push       ecx
00524239 8d55a8                   lea        edx, [ebp - 0x58]
0052423c 52                       push       edx
0052423d e82e80f8ff               call       0x4ac270
00524242 83c408                   add        esp, 8
00524245 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00524248 81c108010000             add        ecx, 0x108
0052424e e8ad80eeff               call       0x40c300
00524253 85c0                     test       eax, eax
00524255 0f8563010000             jne        0x5243be
0052425b 8b4590                   mov        eax, dword ptr [ebp - 0x70]
0052425e 8a480a                   mov        cl, byte ptr [eax + 0xa]
00524261 d0e9                     shr        cl, 1
00524263 80e101                   and        cl, 1
00524266 0fb6d1                   movzx      edx, cl
00524269 85d2                     test       edx, edx
0052426b 0f85b1000000             jne        0x524322
00524271 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00524274 f30f5a80d0000000         cvtss2sd   xmm0, dword ptr [eax + 0xd0]
0052427c 83ec08                   sub        esp, 8
0052427f f20f110424               movsd      qword ptr [esp], xmm0
00524284 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00524287 8b91f4000000             mov        edx, dword ptr [ecx + 0xf4]
0052428d 8b0495d0ff5a00           mov        eax, dword ptr [edx*4 + 0x5affd0]
00524294 50                       push       eax
00524295 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00524298 8b91f0000000             mov        edx, dword ptr [ecx + 0xf0]
0052429e 8b049590ff5a00           mov        eax, dword ptr [edx*4 + 0x5aff90]
005242a5 50                       push       eax
005242a6 b904000000               mov        ecx, 4
005242ab 6bd100                   imul       edx, ecx, 0
005242ae 8b4590                   mov        eax, dword ptr [ebp - 0x70]
005242b1 8b8c10dc000000           mov        ecx, dword ptr [eax + edx + 0xdc]
005242b8 8b148dfcff5a00           mov        edx, dword ptr [ecx*4 + 0x5afffc]
005242bf 52                       push       edx
005242c0 8b4590                   mov        eax, dword ptr [ebp - 0x70]
005242c3 8b88d8000000             mov        ecx, dword ptr [eax + 0xd8]
005242c9 8b148d80ff5a00           mov        edx, dword ptr [ecx*4 + 0x5aff80]
005242d0 52                       push       edx
005242d1 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
005242d4 50                       push       eax
005242d5 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
005242d8 51                       push       ecx
005242d9 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
005242dc 52                       push       edx
005242dd 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
005242e0 83c001                   add        eax, 1
005242e3 50                       push       eax
005242e4 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
005242e7 99                       cdq        
005242e8 b964000000               mov        ecx, 0x64
005242ed f7f9                     idiv       ecx
005242ef 52                       push       edx
005242f0 8b5590                   mov        edx, dword ptr [ebp - 0x70]
005242f3 52                       push       edx
005242f4 8b4588                   mov        eax, dword ptr [ebp - 0x78]
005242f7 83c001                   add        eax, 1
005242fa 50                       push       eax
005242fb b904000000               mov        ecx, 4
00524300 6bd100                   imul       edx, ecx, 0
00524303 8b82740a5b00             mov        eax, dword ptr [edx + 0x5b0a74]
00524309 50                       push       eax
0052430a 8d4d9c                   lea        ecx, [ebp - 0x64]
0052430d 51                       push       ecx
0052430e 8b1598065c00             mov        edx, dword ptr [0x5c0698]
00524314 52                       push       edx
00524315 e87686f4ff               call       0x46c990
0052431a 83c440                   add        esp, 0x40
0052431d e997000000               jmp        0x5243b9
00524322 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00524325 f30f5a80d0000000         cvtss2sd   xmm0, dword ptr [eax + 0xd0]
0052432d 83ec08                   sub        esp, 8
00524330 f20f110424               movsd      qword ptr [esp], xmm0
00524335 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00524338 8b91fc000000             mov        edx, dword ptr [ecx + 0xfc]
0052433e 83c201                   add        edx, 1
00524341 52                       push       edx
00524342 b804000000               mov        eax, 4
00524347 6bc800                   imul       ecx, eax, 0
0052434a 8b5590                   mov        edx, dword ptr [ebp - 0x70]
0052434d 8b840adc000000           mov        eax, dword ptr [edx + ecx + 0xdc]
00524354 8b0c85fcff5a00           mov        ecx, dword ptr [eax*4 + 0x5afffc]
0052435b 51                       push       ecx
0052435c 8b5590                   mov        edx, dword ptr [ebp - 0x70]
0052435f 8b82d8000000             mov        eax, dword ptr [edx + 0xd8]
00524365 8b0c8580ff5a00           mov        ecx, dword ptr [eax*4 + 0x5aff80]
0052436c 51                       push       ecx
0052436d 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
00524370 52                       push       edx
00524371 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00524374 50                       push       eax
00524375 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00524378 51                       push       ecx
00524379 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
0052437c 83c201                   add        edx, 1
0052437f 52                       push       edx
00524380 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
00524383 99                       cdq        
00524384 b964000000               mov        ecx, 0x64
00524389 f7f9                     idiv       ecx
0052438b 52                       push       edx
0052438c 8b5590                   mov        edx, dword ptr [ebp - 0x70]
0052438f 52                       push       edx
00524390 8b4588                   mov        eax, dword ptr [ebp - 0x78]
00524393 83c001                   add        eax, 1
00524396 50                       push       eax
00524397 b904000000               mov        ecx, 4
0052439c c1e102                   shl        ecx, 2
0052439f 8b91740a5b00             mov        edx, dword ptr [ecx + 0x5b0a74]
005243a5 52                       push       edx
005243a6 8d459c                   lea        eax, [ebp - 0x64]
005243a9 50                       push       eax
005243aa 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005243b0 51                       push       ecx
005243b1 e8da85f4ff               call       0x46c990
005243b6 83c43c                   add        esp, 0x3c
005243b9 e921010000               jmp        0x5244df
005243be 8b5590                   mov        edx, dword ptr [ebp - 0x70]
005243c1 f30f5a82d0000000         cvtss2sd   xmm0, dword ptr [edx + 0xd0]
005243c9 f20f1185e0feffff         movsd      qword ptr [ebp - 0x120], xmm0
005243d1 8b4590                   mov        eax, dword ptr [ebp - 0x70]
005243d4 8b88f4000000             mov        ecx, dword ptr [eax + 0xf4]
005243da 8b148dd0ff5a00           mov        edx, dword ptr [ecx*4 + 0x5affd0]
005243e1 899578ffffff             mov        dword ptr [ebp - 0x88], edx
005243e7 8b4590                   mov        eax, dword ptr [ebp - 0x70]
005243ea 8b88f0000000             mov        ecx, dword ptr [eax + 0xf0]
005243f0 8b148d90ff5a00           mov        edx, dword ptr [ecx*4 + 0x5aff90]
005243f7 899574ffffff             mov        dword ptr [ebp - 0x8c], edx
005243fd b804000000               mov        eax, 4
00524402 6bc800                   imul       ecx, eax, 0
00524405 8b5590                   mov        edx, dword ptr [ebp - 0x70]
00524408 8b840adc000000           mov        eax, dword ptr [edx + ecx + 0xdc]
0052440f 8b0c85fcff5a00           mov        ecx, dword ptr [eax*4 + 0x5afffc]
00524416 898d70ffffff             mov        dword ptr [ebp - 0x90], ecx
0052441c 8b5590                   mov        edx, dword ptr [ebp - 0x70]
0052441f 8b82d8000000             mov        eax, dword ptr [edx + 0xd8]
00524425 8b0c8580ff5a00           mov        ecx, dword ptr [eax*4 + 0x5aff80]
0052442c 898d6cffffff             mov        dword ptr [ebp - 0x94], ecx
00524432 8b5598                   mov        edx, dword ptr [ebp - 0x68]
00524435 81c240570000             add        edx, 0x5740
0052443b 8995f0feffff             mov        dword ptr [ebp - 0x110], edx
00524441 8b4588                   mov        eax, dword ptr [ebp - 0x78]
00524444 50                       push       eax
00524445 8b8df0feffff             mov        ecx, dword ptr [ebp - 0x110]
0052444b e83001efff               call       0x414580
00524450 8b08                     mov        ecx, dword ptr [eax]
00524452 e8c95dfeff               call       0x50a220
00524457 898568ffffff             mov        dword ptr [ebp - 0x98], eax
0052445d b904000000               mov        ecx, 4
00524462 d1e1                     shl        ecx, 1
00524464 8b91740a5b00             mov        edx, dword ptr [ecx + 0x5b0a74]
0052446a 899564ffffff             mov        dword ptr [ebp - 0x9c], edx
00524470 83ec08                   sub        esp, 8
00524473 f20f1085e0feffff         movsd      xmm0, qword ptr [ebp - 0x120]
0052447b f20f110424               movsd      qword ptr [esp], xmm0
00524480 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00524486 50                       push       eax
00524487 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
0052448d 51                       push       ecx
0052448e 8b9570ffffff             mov        edx, dword ptr [ebp - 0x90]
00524494 52                       push       edx
00524495 8b856cffffff             mov        eax, dword ptr [ebp - 0x94]
0052449b 50                       push       eax
0052449c 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
0052449f 51                       push       ecx
005244a0 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
005244a3 52                       push       edx
005244a4 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
005244a7 50                       push       eax
005244a8 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
005244ab 83c101                   add        ecx, 1
005244ae 51                       push       ecx
005244af 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
005244b2 99                       cdq        
005244b3 b964000000               mov        ecx, 0x64
005244b8 f7f9                     idiv       ecx
005244ba 52                       push       edx
005244bb 8b5590                   mov        edx, dword ptr [ebp - 0x70]
005244be 52                       push       edx
005244bf 8b8568ffffff             mov        eax, dword ptr [ebp - 0x98]
005244c5 50                       push       eax
005244c6 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
005244cc 51                       push       ecx
005244cd 8d559c                   lea        edx, [ebp - 0x64]
005244d0 52                       push       edx
005244d1 a198065c00               mov        eax, dword ptr [0x5c0698]
005244d6 50                       push       eax
005244d7 e8b484f4ff               call       0x46c990
005244dc 83c440                   add        esp, 0x40
005244df eb5f                     jmp        0x524540
005244e1 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
005244e4 83c101                   add        ecx, 1
005244e7 898d60ffffff             mov        dword ptr [ebp - 0xa0], ecx
005244ed 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
005244f0 81c108010000             add        ecx, 0x108
005244f6 e8057eeeff               call       0x40c300
005244fb 85c0                     test       eax, eax
005244fd 7509                     jne        0x524508
005244ff c7458001000000           mov        dword ptr [ebp - 0x80], 1
00524506 eb07                     jmp        0x52450f
00524508 c7458003000000           mov        dword ptr [ebp - 0x80], 3
0052450f 8b5580                   mov        edx, dword ptr [ebp - 0x80]
00524512 8b0495740a5b00           mov        eax, dword ptr [edx*4 + 0x5b0a74]
00524519 89855cffffff             mov        dword ptr [ebp - 0xa4], eax
0052451f 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
00524525 51                       push       ecx
00524526 8b955cffffff             mov        edx, dword ptr [ebp - 0xa4]
0052452c 52                       push       edx
0052452d 8d459c                   lea        eax, [ebp - 0x64]
00524530 50                       push       eax
00524531 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00524537 51                       push       ecx
00524538 e85384f4ff               call       0x46c990
0052453d 83c410                   add        esp, 0x10
00524540 f30f1045a0               movss      xmm0, dword ptr [ebp - 0x60]
00524545 f30f5805fcf25600         addss      xmm0, dword ptr [0x56f2fc]
0052454d f30f1145a0               movss      dword ptr [ebp - 0x60], xmm0
00524552 e9fefbffff               jmp        0x524155
00524557 6aff                     push       -1
00524559 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052455f e86cc5f4ff               call       0x470ad0
00524564 6a00                     push       0
00524566 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052456c e8af24fcff               call       0x4e6a20
00524571 90                       nop        
00524572 e983060000               jmp        0x524bfa
00524577 51                       push       ecx
00524578 0f57c0                   xorps      xmm0, xmm0
0052457b f30f110424               movss      dword ptr [esp], xmm0
00524580 51                       push       ecx
00524581 f30f100530fa5600         movss      xmm0, dword ptr [0x56fa30]
00524589 f30f110424               movss      dword ptr [esp], xmm0
0052458e 51                       push       ecx
0052458f f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
00524597 f30f110424               movss      dword ptr [esp], xmm0
0052459c 8d4df0                   lea        ecx, [ebp - 0x10]
0052459f e82ce8efff               call       0x422dd0
005245a4 8b5598                   mov        edx, dword ptr [ebp - 0x68]
005245a7 81c240570000             add        edx, 0x5740
005245ad 899558ffffff             mov        dword ptr [ebp - 0xa8], edx
005245b3 8b4598                   mov        eax, dword ptr [ebp - 0x68]
005245b6 8b8838570000             mov        ecx, dword ptr [eax + 0x5738]
005245bc 51                       push       ecx
005245bd 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
005245c3 e8b8ffeeff               call       0x414580
005245c8 8b08                     mov        ecx, dword ptr [eax]
005245ca e841d2efff               call       0x421810
005245cf 894594                   mov        dword ptr [ebp - 0x6c], eax
005245d2 8b5598                   mov        edx, dword ptr [ebp - 0x68]
005245d5 81c254010000             add        edx, 0x154
005245db 899554ffffff             mov        dword ptr [ebp - 0xac], edx
005245e1 6a0a                     push       0xa
005245e3 8b8d54ffffff             mov        ecx, dword ptr [ebp - 0xac]
005245e9 e872efefff               call       0x423560
005245ee 0fb6c0                   movzx      eax, al
005245f1 85c0                     test       eax, eax
005245f3 7465                     je         0x52465a
005245f5 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
005245f8 8b8138570000             mov        eax, dword ptr [ecx + 0x5738]
005245fe 99                       cdq        
005245ff b919000000               mov        ecx, 0x19
00524604 f7f9                     idiv       ecx
00524606 6bd20f                   imul       edx, edx, 0xf
00524609 f30f2ac2                 cvtsi2ss   xmm0, edx
0052460d 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00524610 81c154010000             add        ecx, 0x154
00524616 f30f11854cffffff         movss      dword ptr [ebp - 0xb4], xmm0
0052461e e8adf5efff               call       0x423bd0
00524623 d99d50ffffff             fstp       dword ptr [ebp - 0xb0]
00524629 f30f100534e75600         movss      xmm0, dword ptr [0x56e734]
00524631 f30f5c8550ffffff         subss      xmm0, dword ptr [ebp - 0xb0]
00524639 f30f108d4cffffff         movss      xmm1, dword ptr [ebp - 0xb4]
00524641 f30f59c8                 mulss      xmm1, xmm0
00524645 f30f5e0d34e75600         divss      xmm1, dword ptr [0x56e734]
0052464d f30f580d30fa5600         addss      xmm1, dword ptr [0x56fa30]
00524655 f30f114df4               movss      dword ptr [ebp - 0xc], xmm1
0052465a 6a01                     push       1
0052465c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00524662 e8b923fcff               call       0x4e6a20
00524667 90                       nop        
00524668 33c0                     xor        eax, eax
0052466a 8945cc                   mov        dword ptr [ebp - 0x34], eax
0052466d 8945d0                   mov        dword ptr [ebp - 0x30], eax
00524670 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00524673 8945d8                   mov        dword ptr [ebp - 0x28], eax
00524676 8945dc                   mov        dword ptr [ebp - 0x24], eax
00524679 8945e0                   mov        dword ptr [ebp - 0x20], eax
0052467c 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0052467f 8945e8                   mov        dword ptr [ebp - 0x18], eax
00524682 8945ec                   mov        dword ptr [ebp - 0x14], eax
00524685 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00524688 83c110                   add        ecx, 0x10
0052468b 51                       push       ecx
0052468c 8d55cc                   lea        edx, [ebp - 0x34]
0052468f 52                       push       edx
00524690 e8db7bf8ff               call       0x4ac270
00524695 83c408                   add        esp, 8
00524698 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
0052469b 81c108010000             add        ecx, 0x108
005246a1 e85a7ceeff               call       0x40c300
005246a6 85c0                     test       eax, eax
005246a8 0f856e010000             jne        0x52481c
005246ae 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
005246b1 8a480a                   mov        cl, byte ptr [eax + 0xa]
005246b4 d0e9                     shr        cl, 1
005246b6 80e101                   and        cl, 1
005246b9 0fb6d1                   movzx      edx, cl
005246bc 85d2                     test       edx, edx
005246be 0f85b6000000             jne        0x52477a
005246c4 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
005246c7 f30f5a80d0000000         cvtss2sd   xmm0, dword ptr [eax + 0xd0]
005246cf 83ec08                   sub        esp, 8
005246d2 f20f110424               movsd      qword ptr [esp], xmm0
005246d7 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
005246da 8b91f4000000             mov        edx, dword ptr [ecx + 0xf4]
005246e0 8b0495d0ff5a00           mov        eax, dword ptr [edx*4 + 0x5affd0]
005246e7 50                       push       eax
005246e8 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
005246eb 8b91f0000000             mov        edx, dword ptr [ecx + 0xf0]
005246f1 8b049590ff5a00           mov        eax, dword ptr [edx*4 + 0x5aff90]
005246f8 50                       push       eax
005246f9 b904000000               mov        ecx, 4
005246fe 6bd100                   imul       edx, ecx, 0
00524701 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
00524704 8b8c10dc000000           mov        ecx, dword ptr [eax + edx + 0xdc]
0052470b 8b148dfcff5a00           mov        edx, dword ptr [ecx*4 + 0x5afffc]
00524712 52                       push       edx
00524713 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
00524716 8b88d8000000             mov        ecx, dword ptr [eax + 0xd8]
0052471c 8b148d80ff5a00           mov        edx, dword ptr [ecx*4 + 0x5aff80]
00524723 52                       push       edx
00524724 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00524727 50                       push       eax
00524728 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0052472b 51                       push       ecx
0052472c 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
0052472f 52                       push       edx
00524730 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
00524733 83c001                   add        eax, 1
00524736 50                       push       eax
00524737 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0052473a 99                       cdq        
0052473b b964000000               mov        ecx, 0x64
00524740 f7f9                     idiv       ecx
00524742 52                       push       edx
00524743 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
00524746 52                       push       edx
00524747 8b4598                   mov        eax, dword ptr [ebp - 0x68]
0052474a 8b8838570000             mov        ecx, dword ptr [eax + 0x5738]
00524750 83c101                   add        ecx, 1
00524753 51                       push       ecx
00524754 ba04000000               mov        edx, 4
00524759 6bc200                   imul       eax, edx, 0
0052475c 8b88740a5b00             mov        ecx, dword ptr [eax + 0x5b0a74]
00524762 51                       push       ecx
00524763 8d55f0                   lea        edx, [ebp - 0x10]
00524766 52                       push       edx
00524767 a198065c00               mov        eax, dword ptr [0x5c0698]
0052476c 50                       push       eax
0052476d e81e82f4ff               call       0x46c990
00524772 83c440                   add        esp, 0x40
00524775 e99d000000               jmp        0x524817
0052477a 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
0052477d f30f5a81d0000000         cvtss2sd   xmm0, dword ptr [ecx + 0xd0]
00524785 83ec08                   sub        esp, 8
00524788 f20f110424               movsd      qword ptr [esp], xmm0
0052478d 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
00524790 8b82fc000000             mov        eax, dword ptr [edx + 0xfc]
00524796 83c001                   add        eax, 1
00524799 50                       push       eax
0052479a b904000000               mov        ecx, 4
0052479f 6bd100                   imul       edx, ecx, 0
005247a2 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
005247a5 8b8c10dc000000           mov        ecx, dword ptr [eax + edx + 0xdc]
005247ac 8b148dfcff5a00           mov        edx, dword ptr [ecx*4 + 0x5afffc]
005247b3 52                       push       edx
005247b4 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
005247b7 8b88d8000000             mov        ecx, dword ptr [eax + 0xd8]
005247bd 8b148d80ff5a00           mov        edx, dword ptr [ecx*4 + 0x5aff80]
005247c4 52                       push       edx
005247c5 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
005247c8 50                       push       eax
005247c9 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
005247cc 51                       push       ecx
005247cd 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
005247d0 52                       push       edx
005247d1 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
005247d4 83c001                   add        eax, 1
005247d7 50                       push       eax
005247d8 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
005247db 99                       cdq        
005247dc b964000000               mov        ecx, 0x64
005247e1 f7f9                     idiv       ecx
005247e3 52                       push       edx
005247e4 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
005247e7 52                       push       edx
005247e8 8b4598                   mov        eax, dword ptr [ebp - 0x68]
005247eb 8b8838570000             mov        ecx, dword ptr [eax + 0x5738]
005247f1 83c101                   add        ecx, 1
005247f4 51                       push       ecx
005247f5 ba04000000               mov        edx, 4
005247fa c1e202                   shl        edx, 2
005247fd 8b82740a5b00             mov        eax, dword ptr [edx + 0x5b0a74]
00524803 50                       push       eax
00524804 8d4df0                   lea        ecx, [ebp - 0x10]
00524807 51                       push       ecx
00524808 8b1598065c00             mov        edx, dword ptr [0x5c0698]
0052480e 52                       push       edx
0052480f e87c81f4ff               call       0x46c990
00524814 83c43c                   add        esp, 0x3c
00524817 e926010000               jmp        0x524942
0052481c 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
0052481f f30f5a80d0000000         cvtss2sd   xmm0, dword ptr [eax + 0xd0]
00524827 f20f1185d8feffff         movsd      qword ptr [ebp - 0x128], xmm0
0052482f 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00524832 8b91f4000000             mov        edx, dword ptr [ecx + 0xf4]
00524838 8b0495d0ff5a00           mov        eax, dword ptr [edx*4 + 0x5affd0]
0052483f 898544ffffff             mov        dword ptr [ebp - 0xbc], eax
00524845 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00524848 8b91f0000000             mov        edx, dword ptr [ecx + 0xf0]
0052484e 8b049590ff5a00           mov        eax, dword ptr [edx*4 + 0x5aff90]
00524855 898540ffffff             mov        dword ptr [ebp - 0xc0], eax
0052485b b904000000               mov        ecx, 4
00524860 6bd100                   imul       edx, ecx, 0
00524863 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
00524866 8b8c10dc000000           mov        ecx, dword ptr [eax + edx + 0xdc]
0052486d 8b148dfcff5a00           mov        edx, dword ptr [ecx*4 + 0x5afffc]
00524874 89953cffffff             mov        dword ptr [ebp - 0xc4], edx
0052487a 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
0052487d 8b88d8000000             mov        ecx, dword ptr [eax + 0xd8]
00524883 8b148d80ff5a00           mov        edx, dword ptr [ecx*4 + 0x5aff80]
0052488a 899538ffffff             mov        dword ptr [ebp - 0xc8], edx
00524890 8b4598                   mov        eax, dword ptr [ebp - 0x68]
00524893 0540570000               add        eax, 0x5740
00524898 898548ffffff             mov        dword ptr [ebp - 0xb8], eax
0052489e 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
005248a1 8b9138570000             mov        edx, dword ptr [ecx + 0x5738]
005248a7 52                       push       edx
005248a8 8b8d48ffffff             mov        ecx, dword ptr [ebp - 0xb8]
005248ae e8cdfceeff               call       0x414580
005248b3 8b08                     mov        ecx, dword ptr [eax]
005248b5 e86659feff               call       0x50a220
005248ba 898534ffffff             mov        dword ptr [ebp - 0xcc], eax
005248c0 b804000000               mov        eax, 4
005248c5 d1e0                     shl        eax, 1
005248c7 8b88740a5b00             mov        ecx, dword ptr [eax + 0x5b0a74]
005248cd 898d30ffffff             mov        dword ptr [ebp - 0xd0], ecx
005248d3 83ec08                   sub        esp, 8
005248d6 f20f1085d8feffff         movsd      xmm0, qword ptr [ebp - 0x128]
005248de f20f110424               movsd      qword ptr [esp], xmm0
005248e3 8b9544ffffff             mov        edx, dword ptr [ebp - 0xbc]
005248e9 52                       push       edx
005248ea 8b8540ffffff             mov        eax, dword ptr [ebp - 0xc0]
005248f0 50                       push       eax
005248f1 8b8d3cffffff             mov        ecx, dword ptr [ebp - 0xc4]
005248f7 51                       push       ecx
005248f8 8b9538ffffff             mov        edx, dword ptr [ebp - 0xc8]
005248fe 52                       push       edx
005248ff 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00524902 50                       push       eax
00524903 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00524906 51                       push       ecx
00524907 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
0052490a 52                       push       edx
0052490b 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0052490e 83c001                   add        eax, 1
00524911 50                       push       eax
00524912 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00524915 99                       cdq        
00524916 b964000000               mov        ecx, 0x64
0052491b f7f9                     idiv       ecx
0052491d 52                       push       edx
0052491e 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
00524921 52                       push       edx
00524922 8b8534ffffff             mov        eax, dword ptr [ebp - 0xcc]
00524928 50                       push       eax
00524929 8b8d30ffffff             mov        ecx, dword ptr [ebp - 0xd0]
0052492f 51                       push       ecx
00524930 8d55f0                   lea        edx, [ebp - 0x10]
00524933 52                       push       edx
00524934 a198065c00               mov        eax, dword ptr [0x5c0698]
00524939 50                       push       eax
0052493a e85180f4ff               call       0x46c990
0052493f 83c440                   add        esp, 0x40
00524942 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00524945 81c154010000             add        ecx, 0x154
0052494b 898d2cffffff             mov        dword ptr [ebp - 0xd4], ecx
00524951 6a0a                     push       0xa
00524953 8b8d2cffffff             mov        ecx, dword ptr [ebp - 0xd4]
00524959 e862ecefff               call       0x4235c0
0052495e 0fb6d0                   movzx      edx, al
00524961 85d2                     test       edx, edx
00524963 0f8476020000             je         0x524bdf
00524969 f30f100578565700         movss      xmm0, dword ptr [0x575678]
00524971 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00524976 f30f100598cd5600         movss      xmm0, dword ptr [0x56cd98]
0052497e f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00524983 c7458c01000000           mov        dword ptr [ebp - 0x74], 1
0052498a eb09                     jmp        0x524995
0052498c 8b458c                   mov        eax, dword ptr [ebp - 0x74]
0052498f 83c001                   add        eax, 1
00524992 89458c                   mov        dword ptr [ebp - 0x74], eax
00524995 837d8c08                 cmp        dword ptr [ebp - 0x74], 8
00524999 0f8d40020000             jge        0x524bdf
0052499f 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
005249a2 83c124                   add        ecx, 0x24
005249a5 898d28ffffff             mov        dword ptr [ebp - 0xd8], ecx
005249ab 8b558c                   mov        edx, dword ptr [ebp - 0x74]
005249ae 83ea01                   sub        edx, 1
005249b1 52                       push       edx
005249b2 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
005249b8 e8f367f4ff               call       0x46b1b0
005249bd 85c0                     test       eax, eax
005249bf 7413                     je         0x5249d4
005249c1 6800ffffff               push       0xffffff00
005249c6 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005249cc e8ffc0f4ff               call       0x470ad0
005249d1 90                       nop        
005249d2 eb11                     jmp        0x5249e5
005249d4 68808080ff               push       0xff808080
005249d9 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005249df e8ecc0f4ff               call       0x470ad0
005249e4 90                       nop        
005249e5 8b4598                   mov        eax, dword ptr [ebp - 0x68]
005249e8 0540570000               add        eax, 0x5740
005249ed 898524ffffff             mov        dword ptr [ebp - 0xdc], eax
005249f3 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
005249f6 8b9138570000             mov        edx, dword ptr [ecx + 0x5738]
005249fc 52                       push       edx
005249fd 8b8d24ffffff             mov        ecx, dword ptr [ebp - 0xdc]
00524a03 e878fbeeff               call       0x414580
00524a08 8b00                     mov        eax, dword ptr [eax]
00524a0a 898520ffffff             mov        dword ptr [ebp - 0xe0], eax
00524a10 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00524a13 51                       push       ecx
00524a14 8b8d20ffffff             mov        ecx, dword ptr [ebp - 0xe0]
00524a1a e8513df6ff               call       0x488770
00524a1f 85c0                     test       eax, eax
00524a21 7528                     jne        0x524a4b
00524a23 8b558c                   mov        edx, dword ptr [ebp - 0x74]
00524a26 8b0495a8ff5a00           mov        eax, dword ptr [edx*4 + 0x5affa8]
00524a2d 50                       push       eax
00524a2e 687c525700               push       0x57527c
00524a33 8d4df0                   lea        ecx, [ebp - 0x10]
00524a36 51                       push       ecx
00524a37 8b1598065c00             mov        edx, dword ptr [0x5c0698]
00524a3d 52                       push       edx
00524a3e e84d7ff4ff               call       0x46c990
00524a43 83c410                   add        esp, 0x10
00524a46 e97d010000               jmp        0x524bc8
00524a4b 837d8c07                 cmp        dword ptr [ebp - 0x74], 7
00524a4f 0f8dfd000000             jge        0x524b52
00524a55 8b4598                   mov        eax, dword ptr [ebp - 0x68]
00524a58 0540570000               add        eax, 0x5740
00524a5d 89851cffffff             mov        dword ptr [ebp - 0xe4], eax
00524a63 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00524a66 8b9138570000             mov        edx, dword ptr [ecx + 0x5738]
00524a6c 52                       push       edx
00524a6d 8b8d1cffffff             mov        ecx, dword ptr [ebp - 0xe4]
00524a73 e808fbeeff               call       0x414580
00524a78 8b00                     mov        eax, dword ptr [eax]
00524a7a 898518ffffff             mov        dword ptr [ebp - 0xe8], eax
00524a80 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00524a83 83c101                   add        ecx, 1
00524a86 51                       push       ecx
00524a87 8b8d18ffffff             mov        ecx, dword ptr [ebp - 0xe8]
00524a8d e8de3cf6ff               call       0x488770
00524a92 85c0                     test       eax, eax
00524a94 0f84b8000000             je         0x524b52
00524a9a 8b5598                   mov        edx, dword ptr [ebp - 0x68]
00524a9d 81c240570000             add        edx, 0x5740
00524aa3 8995fcfeffff             mov        dword ptr [ebp - 0x104], edx
00524aa9 8b4598                   mov        eax, dword ptr [ebp - 0x68]
00524aac 8b8838570000             mov        ecx, dword ptr [eax + 0x5738]
00524ab2 51                       push       ecx
00524ab3 8b8dfcfeffff             mov        ecx, dword ptr [ebp - 0x104]
00524ab9 e8c2faeeff               call       0x414580
00524abe 8b10                     mov        edx, dword ptr [eax]
00524ac0 899510ffffff             mov        dword ptr [ebp - 0xf0], edx
00524ac6 8b458c                   mov        eax, dword ptr [ebp - 0x74]
00524ac9 83c001                   add        eax, 1
00524acc 50                       push       eax
00524acd 8b8d10ffffff             mov        ecx, dword ptr [ebp - 0xf0]
00524ad3 e8983cf6ff               call       0x488770
00524ad8 89857cffffff             mov        dword ptr [ebp - 0x84], eax
00524ade 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00524ae4 83c170                   add        ecx, 0x70
00524ae7 e8d434f9ff               call       0x4b7fc0
00524aec 89850cffffff             mov        dword ptr [ebp - 0xf4], eax
00524af2 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00524af8 83c170                   add        ecx, 0x70
00524afb e800790000               call       0x52c400
00524b00 8985e8feffff             mov        dword ptr [ebp - 0x118], eax
00524b06 8995ecfeffff             mov        dword ptr [ebp - 0x114], edx
00524b0c 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00524b0f 8b148da8ff5a00           mov        edx, dword ptr [ecx*4 + 0x5affa8]
00524b16 899508ffffff             mov        dword ptr [ebp - 0xf8], edx
00524b1c 8b850cffffff             mov        eax, dword ptr [ebp - 0xf4]
00524b22 50                       push       eax
00524b23 8b8decfeffff             mov        ecx, dword ptr [ebp - 0x114]
00524b29 51                       push       ecx
00524b2a 8b95e8feffff             mov        edx, dword ptr [ebp - 0x118]
00524b30 52                       push       edx
00524b31 8b8508ffffff             mov        eax, dword ptr [ebp - 0xf8]
00524b37 50                       push       eax
00524b38 688c525700               push       0x57528c
00524b3d 8d4df0                   lea        ecx, [ebp - 0x10]
00524b40 51                       push       ecx
00524b41 8b1598065c00             mov        edx, dword ptr [0x5c0698]
00524b47 52                       push       edx
00524b48 e8437ef4ff               call       0x46c990
00524b4d 83c41c                   add        esp, 0x1c
00524b50 eb76                     jmp        0x524bc8
00524b52 8b4598                   mov        eax, dword ptr [ebp - 0x68]
00524b55 0540570000               add        eax, 0x5740
00524b5a 898504ffffff             mov        dword ptr [ebp - 0xfc], eax
00524b60 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00524b63 8b9138570000             mov        edx, dword ptr [ecx + 0x5738]
00524b69 52                       push       edx
00524b6a 8b8d04ffffff             mov        ecx, dword ptr [ebp - 0xfc]
00524b70 e80bfaeeff               call       0x414580
00524b75 8b00                     mov        eax, dword ptr [eax]
00524b77 898500ffffff             mov        dword ptr [ebp - 0x100], eax
00524b7d 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00524b80 51                       push       ecx
00524b81 8b8d00ffffff             mov        ecx, dword ptr [ebp - 0x100]
00524b87 e8e43bf6ff               call       0x488770
00524b8c 85c0                     test       eax, eax
00524b8e 7438                     je         0x524bc8
00524b90 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
00524b93 8b82f8000000             mov        eax, dword ptr [edx + 0xf8]
00524b99 50                       push       eax
00524b9a 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00524b9d 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
00524ba0 52                       push       edx
00524ba1 8b4118                   mov        eax, dword ptr [ecx + 0x18]
00524ba4 50                       push       eax
00524ba5 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00524ba8 8b148da8ff5a00           mov        edx, dword ptr [ecx*4 + 0x5affa8]
00524baf 52                       push       edx
00524bb0 688c525700               push       0x57528c
00524bb5 8d45f0                   lea        eax, [ebp - 0x10]
00524bb8 50                       push       eax
00524bb9 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00524bbf 51                       push       ecx
00524bc0 e8cb7df4ff               call       0x46c990
00524bc5 83c41c                   add        esp, 0x1c
00524bc8 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00524bcd f30f580544265700         addss      xmm0, dword ptr [0x572644]
00524bd5 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00524bda e9adfdffff               jmp        0x52498c
00524bdf 6aff                     push       -1
00524be1 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00524be7 e8e4bef4ff               call       0x470ad0
00524bec 6a00                     push       0
00524bee 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00524bf4 e8271efcff               call       0x4e6a20
00524bf9 90                       nop        
00524bfa b801000000               mov        eax, 1
00524bff 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524c02 33cd                     xor        ecx, ebp
00524c04 e8e5dd0100               call       0x5429ee
00524c09 8be5                     mov        esp, ebp
00524c0b 5d                       pop        ebp
00524c0c c3                       ret        
00524c0d cc                       int3       
00524c0e cc                       int3       
00524c0f cc                       int3       
