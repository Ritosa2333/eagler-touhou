004244c0 55                       push       ebp
004244c1 8bec                     mov        ebp, esp
004244c3 83ec40                   sub        esp, 0x40
004244c6 56                       push       esi
004244c7 833d18a55b0000           cmp        dword ptr [0x5ba518], 0
004244ce 740a                     je         0x4244da
004244d0 b807000000               mov        eax, 7
004244d5 e907050000               jmp        0x4249e1
004244da 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
004244e1 7465                     je         0x424548
004244e3 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004244e9 e842fbffff               call       0x424030
004244ee 85c0                     test       eax, eax
004244f0 754a                     jne        0x42453c
004244f2 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004244f8 e863fbffff               call       0x424060
004244fd 85c0                     test       eax, eax
004244ff 753b                     jne        0x42453c
00424501 a128a85b00               mov        eax, dword ptr [0x5ba828]
00424506 8b88e8000000             mov        ecx, dword ptr [eax + 0xe8]
0042450c c1e904                   shr        ecx, 4
0042450f 83e101                   and        ecx, 1
00424512 7528                     jne        0x42453c
00424514 8b1528a85b00             mov        edx, dword ptr [0x5ba828]
0042451a 8b82e8000000             mov        eax, dword ptr [edx + 0xe8]
00424520 c1e805                   shr        eax, 5
00424523 83e001                   and        eax, 1
00424526 7514                     jne        0x42453c
00424528 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
0042452e 8b91e8000000             mov        edx, dword ptr [ecx + 0xe8]
00424534 c1ea06                   shr        edx, 6
00424537 83e201                   and        edx, 1
0042453a 740a                     je         0x424546
0042453c b801000000               mov        eax, 1
00424541 e99b040000               jmp        0x4249e1
00424546 eb0a                     jmp        0x424552
00424548 b801000000               mov        eax, 1
0042454d e98f040000               jmp        0x4249e1
00424552 8b4508                   mov        eax, dword ptr [ebp + 8]
00424555 83c030                   add        eax, 0x30
00424558 8945d8                   mov        dword ptr [ebp - 0x28], eax
0042455b 6a00                     push       0
0042455d 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00424560 e8dbefffff               call       0x423540
00424565 90                       nop        
00424566 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424569 83c130                   add        ecx, 0x30
0042456c 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
0042456f 8b5508                   mov        edx, dword ptr [ebp + 8]
00424572 8b4220                   mov        eax, dword ptr [edx + 0x20]
00424575 50                       push       eax
00424576 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00424579 e8e2efffff               call       0x423560
0042457e 0fb6c8                   movzx      ecx, al
00424581 85c9                     test       ecx, ecx
00424583 7429                     je         0x4245ae
00424585 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424588 83c130                   add        ecx, 0x30
0042458b e840f6ffff               call       0x423bd0
00424590 d95dd0                   fstp       dword ptr [ebp - 0x30]
00424593 f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
00424598 8b5508                   mov        edx, dword ptr [ebp + 8]
0042459b f30f2a4a20               cvtsi2ss   xmm1, dword ptr [edx + 0x20]
004245a0 f30f5ec1                 divss      xmm0, xmm1
004245a4 f30f1145fc               movss      dword ptr [ebp - 4], xmm0
004245a9 e9ca000000               jmp        0x424678
004245ae 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004245b1 83c130                   add        ecx, 0x30
004245b4 e8d7b9feff               call       0x40ff90
004245b9 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004245bc 8b5120                   mov        edx, dword ptr [ecx + 0x20]
004245bf 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004245c2 035124                   add        edx, dword ptr [ecx + 0x24]
004245c5 3bc2                     cmp        eax, edx
004245c7 7d12                     jge        0x4245db
004245c9 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004245d1 f30f1145fc               movss      dword ptr [ebp - 4], xmm0
004245d6 e99d000000               jmp        0x424678
004245db 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004245de 83c130                   add        ecx, 0x30
004245e1 e8aab9feff               call       0x40ff90
004245e6 8b5508                   mov        edx, dword ptr [ebp + 8]
004245e9 8b4a20                   mov        ecx, dword ptr [edx + 0x20]
004245ec 8b5508                   mov        edx, dword ptr [ebp + 8]
004245ef 034a24                   add        ecx, dword ptr [edx + 0x24]
004245f2 8b5508                   mov        edx, dword ptr [ebp + 8]
004245f5 034a28                   add        ecx, dword ptr [edx + 0x28]
004245f8 3bc1                     cmp        eax, ecx
004245fa 7d72                     jge        0x42466e
004245fc 8b4508                   mov        eax, dword ptr [ebp + 8]
004245ff 8b7020                   mov        esi, dword ptr [eax + 0x20]
00424602 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424605 037124                   add        esi, dword ptr [ecx + 0x24]
00424608 8b5508                   mov        edx, dword ptr [ebp + 8]
0042460b 037228                   add        esi, dword ptr [edx + 0x28]
0042460e 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424611 83c130                   add        ecx, 0x30
00424614 e877b9feff               call       0x40ff90
00424619 2bf0                     sub        esi, eax
0042461b 8975e0                   mov        dword ptr [ebp - 0x20], esi
0042461e f20f2a45e0               cvtsi2sd   xmm0, dword ptr [ebp - 0x20]
00424623 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00424626 c1e81f                   shr        eax, 0x1f
00424629 f20f5804c5d0cd5600       addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
00424632 f20f1145c8               movsd      qword ptr [ebp - 0x38], xmm0
00424637 f20f5a45c8               cvtsd2ss   xmm0, qword ptr [ebp - 0x38]
0042463c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0042463f 8b5128                   mov        edx, dword ptr [ecx + 0x28]
00424642 8955dc                   mov        dword ptr [ebp - 0x24], edx
00424645 f20f2a4ddc               cvtsi2sd   xmm1, dword ptr [ebp - 0x24]
0042464a 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0042464d c1e81f                   shr        eax, 0x1f
00424650 f20f580cc5d0cd5600       addsd      xmm1, qword ptr [eax*8 + 0x56cdd0]
00424659 f20f114dc0               movsd      qword ptr [ebp - 0x40], xmm1
0042465e f20f5a4dc0               cvtsd2ss   xmm1, qword ptr [ebp - 0x40]
00424663 f30f5ec1                 divss      xmm0, xmm1
00424667 f30f1145fc               movss      dword ptr [ebp - 4], xmm0
0042466c eb0a                     jmp        0x424678
0042466e b807000000               mov        eax, 7
00424673 e969030000               jmp        0x4249e1
00424678 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0042467b f30f2a411c               cvtsi2ss   xmm0, dword ptr [ecx + 0x1c]
00424680 f30f5945fc               mulss      xmm0, dword ptr [ebp - 4]
00424685 f30f1145fc               movss      dword ptr [ebp - 4], xmm0
0042468a 6a03                     push       3
0042468c b9c4a45b00               mov        ecx, 0x5ba4c4
00424691 e80af8ffff               call       0x423ea0
00424696 8945f8                   mov        dword ptr [ebp - 8], eax
00424699 837df800                 cmp        dword ptr [ebp - 8], 0
0042469d 7410                     je         0x4246af
0042469f 837df801                 cmp        dword ptr [ebp - 8], 1
004246a3 7477                     je         0x42471c
004246a5 837df802                 cmp        dword ptr [ebp - 8], 2
004246a9 0f84f4000000             je         0x4247a3
004246af ba6c010000               mov        edx, 0x16c
004246b4 6bc203                   imul       eax, edx, 3
004246b7 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004246ba 8b5140                   mov        edx, dword ptr [ecx + 0x40]
004246bd 0f57c0                   xorps      xmm0, xmm0
004246c0 f30f1184d0e4505c00       movss      dword ptr [eax + edx*8 + 0x5c50e4], xmm0
004246c9 b86c010000               mov        eax, 0x16c
004246ce c1e000                   shl        eax, 0
004246d1 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004246d4 8b5140                   mov        edx, dword ptr [ecx + 0x40]
004246d7 0f57c0                   xorps      xmm0, xmm0
004246da f30f1184d0e4505c00       movss      dword ptr [eax + edx*8 + 0x5c50e4], xmm0
004246e3 b86c010000               mov        eax, 0x16c
004246e8 6bc800                   imul       ecx, eax, 0
004246eb 8b5508                   mov        edx, dword ptr [ebp + 8]
004246ee 8b4240                   mov        eax, dword ptr [edx + 0x40]
004246f1 0f57c0                   xorps      xmm0, xmm0
004246f4 f30f1184c1e4505c00       movss      dword ptr [ecx + eax*8 + 0x5c50e4], xmm0
004246fd b96c010000               mov        ecx, 0x16c
00424702 6bd105                   imul       edx, ecx, 5
00424705 8b4508                   mov        eax, dword ptr [ebp + 8]
00424708 8b4840                   mov        ecx, dword ptr [eax + 0x40]
0042470b 0f57c0                   xorps      xmm0, xmm0
0042470e f30f1184cae4505c00       movss      dword ptr [edx + ecx*8 + 0x5c50e4], xmm0
00424717 e917010000               jmp        0x424833
0042471c ba6c010000               mov        edx, 0x16c
00424721 6bc203                   imul       eax, edx, 3
00424724 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424727 8b5140                   mov        edx, dword ptr [ecx + 0x40]
0042472a f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
0042472f f30f1184d0e4505c00       movss      dword ptr [eax + edx*8 + 0x5c50e4], xmm0
00424738 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
0042473d f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
00424745 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
0042474a b86c010000               mov        eax, 0x16c
0042474f c1e000                   shl        eax, 0
00424752 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424755 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00424758 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
0042475d f30f1184d0e4505c00       movss      dword ptr [eax + edx*8 + 0x5c50e4], xmm0
00424766 b86c010000               mov        eax, 0x16c
0042476b 6bc800                   imul       ecx, eax, 0
0042476e 8b5508                   mov        edx, dword ptr [ebp + 8]
00424771 8b4240                   mov        eax, dword ptr [edx + 0x40]
00424774 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00424779 f30f1184c1e4505c00       movss      dword ptr [ecx + eax*8 + 0x5c50e4], xmm0
00424782 b96c010000               mov        ecx, 0x16c
00424787 6bd105                   imul       edx, ecx, 5
0042478a 8b4508                   mov        eax, dword ptr [ebp + 8]
0042478d 8b4840                   mov        ecx, dword ptr [eax + 0x40]
00424790 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00424795 f30f1184cae4505c00       movss      dword ptr [edx + ecx*8 + 0x5c50e4], xmm0
0042479e e990000000               jmp        0x424833
004247a3 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
004247a8 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
004247af ba6c010000               mov        edx, 0x16c
004247b4 6bc203                   imul       eax, edx, 3
004247b7 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004247ba 8b5140                   mov        edx, dword ptr [ecx + 0x40]
004247bd f30f1184d0e4505c00       movss      dword ptr [eax + edx*8 + 0x5c50e4], xmm0
004247c6 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
004247cb 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
004247d2 f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
004247da f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004247df b86c010000               mov        eax, 0x16c
004247e4 c1e000                   shl        eax, 0
004247e7 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004247ea 8b5140                   mov        edx, dword ptr [ecx + 0x40]
004247ed f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004247f2 f30f1184d0e4505c00       movss      dword ptr [eax + edx*8 + 0x5c50e4], xmm0
004247fb b86c010000               mov        eax, 0x16c
00424800 6bc800                   imul       ecx, eax, 0
00424803 8b5508                   mov        edx, dword ptr [ebp + 8]
00424806 8b4240                   mov        eax, dword ptr [edx + 0x40]
00424809 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
0042480e f30f1184c1e4505c00       movss      dword ptr [ecx + eax*8 + 0x5c50e4], xmm0
00424817 b96c010000               mov        ecx, 0x16c
0042481c 6bd105                   imul       edx, ecx, 5
0042481f 8b4508                   mov        eax, dword ptr [ebp + 8]
00424822 8b4840                   mov        ecx, dword ptr [eax + 0x40]
00424825 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
0042482a f30f1184cae4505c00       movss      dword ptr [edx + ecx*8 + 0x5c50e4], xmm0
00424833 6a03                     push       3
00424835 b9c4a45b00               mov        ecx, 0x5ba4c4
0042483a e861f6ffff               call       0x423ea0
0042483f 8945ec                   mov        dword ptr [ebp - 0x14], eax
00424842 837dec00                 cmp        dword ptr [ebp - 0x14], 0
00424846 7410                     je         0x424858
00424848 837dec01                 cmp        dword ptr [ebp - 0x14], 1
0042484c 7477                     je         0x4248c5
0042484e 837dec02                 cmp        dword ptr [ebp - 0x14], 2
00424852 0f84f4000000             je         0x42494c
00424858 ba6c010000               mov        edx, 0x16c
0042485d 6bc203                   imul       eax, edx, 3
00424860 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424863 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00424866 0f57c0                   xorps      xmm0, xmm0
00424869 f30f1184d0e8505c00       movss      dword ptr [eax + edx*8 + 0x5c50e8], xmm0
00424872 b86c010000               mov        eax, 0x16c
00424877 c1e000                   shl        eax, 0
0042487a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0042487d 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00424880 0f57c0                   xorps      xmm0, xmm0
00424883 f30f1184d0e8505c00       movss      dword ptr [eax + edx*8 + 0x5c50e8], xmm0
0042488c b86c010000               mov        eax, 0x16c
00424891 6bc800                   imul       ecx, eax, 0
00424894 8b5508                   mov        edx, dword ptr [ebp + 8]
00424897 8b4240                   mov        eax, dword ptr [edx + 0x40]
0042489a 0f57c0                   xorps      xmm0, xmm0
0042489d f30f1184c1e8505c00       movss      dword ptr [ecx + eax*8 + 0x5c50e8], xmm0
004248a6 b96c010000               mov        ecx, 0x16c
004248ab 6bd105                   imul       edx, ecx, 5
004248ae 8b4508                   mov        eax, dword ptr [ebp + 8]
004248b1 8b4840                   mov        ecx, dword ptr [eax + 0x40]
004248b4 0f57c0                   xorps      xmm0, xmm0
004248b7 f30f1184cae8505c00       movss      dword ptr [edx + ecx*8 + 0x5c50e8], xmm0
004248c0 e917010000               jmp        0x4249dc
004248c5 ba6c010000               mov        edx, 0x16c
004248ca 6bc203                   imul       eax, edx, 3
004248cd 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004248d0 8b5140                   mov        edx, dword ptr [ecx + 0x40]
004248d3 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
004248d8 f30f1184d0e8505c00       movss      dword ptr [eax + edx*8 + 0x5c50e8], xmm0
004248e1 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
004248e6 f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
004248ee f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
004248f3 b86c010000               mov        eax, 0x16c
004248f8 c1e000                   shl        eax, 0
004248fb 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004248fe 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00424901 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00424906 f30f1184d0e8505c00       movss      dword ptr [eax + edx*8 + 0x5c50e8], xmm0
0042490f b86c010000               mov        eax, 0x16c
00424914 6bc800                   imul       ecx, eax, 0
00424917 8b5508                   mov        edx, dword ptr [ebp + 8]
0042491a 8b4240                   mov        eax, dword ptr [edx + 0x40]
0042491d f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00424922 f30f1184c1e8505c00       movss      dword ptr [ecx + eax*8 + 0x5c50e8], xmm0
0042492b b96c010000               mov        ecx, 0x16c
00424930 6bd105                   imul       edx, ecx, 5
00424933 8b4508                   mov        eax, dword ptr [ebp + 8]
00424936 8b4840                   mov        ecx, dword ptr [eax + 0x40]
00424939 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
0042493e f30f1184cae8505c00       movss      dword ptr [edx + ecx*8 + 0x5c50e8], xmm0
00424947 e990000000               jmp        0x4249dc
0042494c f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00424951 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00424958 ba6c010000               mov        edx, 0x16c
0042495d 6bc203                   imul       eax, edx, 3
00424960 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424963 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00424966 f30f1184d0e8505c00       movss      dword ptr [eax + edx*8 + 0x5c50e8], xmm0
0042496f f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00424974 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
0042497b f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
00424983 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00424988 b86c010000               mov        eax, 0x16c
0042498d c1e000                   shl        eax, 0
00424990 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424993 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00424996 f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
0042499b f30f1184d0e8505c00       movss      dword ptr [eax + edx*8 + 0x5c50e8], xmm0
004249a4 b86c010000               mov        eax, 0x16c
004249a9 6bc800                   imul       ecx, eax, 0
004249ac 8b5508                   mov        edx, dword ptr [ebp + 8]
004249af 8b4240                   mov        eax, dword ptr [edx + 0x40]
004249b2 f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
004249b7 f30f1184c1e8505c00       movss      dword ptr [ecx + eax*8 + 0x5c50e8], xmm0
004249c0 b96c010000               mov        ecx, 0x16c
004249c5 6bd105                   imul       edx, ecx, 5
004249c8 8b4508                   mov        eax, dword ptr [ebp + 8]
004249cb 8b4840                   mov        ecx, dword ptr [eax + 0x40]
004249ce f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
004249d3 f30f1184cae8505c00       movss      dword ptr [edx + ecx*8 + 0x5c50e8], xmm0
004249dc b801000000               mov        eax, 1
004249e1 5e                       pop        esi
004249e2 8be5                     mov        esp, ebp
004249e4 5d                       pop        ebp
004249e5 c3                       ret        
004249e6 cc                       int3       
004249e7 cc                       int3       
004249e8 cc                       int3       
004249e9 cc                       int3       
004249ea cc                       int3       
004249eb cc                       int3       
004249ec cc                       int3       
004249ed cc                       int3       
004249ee cc                       int3       
004249ef cc                       int3       
