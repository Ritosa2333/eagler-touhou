004af310 55                       push       ebp
004af311 8bec                     mov        ebp, esp
004af313 6aff                     push       -1
004af315 68d0985600               push       0x5698d0
004af31a 64a100000000             mov        eax, dword ptr fs:[0]
004af320 50                       push       eax
004af321 81ec88000000             sub        esp, 0x88
004af327 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004af32c 33c5                     xor        eax, ebp
004af32e 50                       push       eax
004af32f 8d45f4                   lea        eax, [ebp - 0xc]
004af332 64a300000000             mov        dword ptr fs:[0], eax
004af338 894df0                   mov        dword ptr [ebp - 0x10], ecx
004af33b 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004af33e c70000000000             mov        dword ptr [eax], 0
004af344 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af347 83c104                   add        ecx, 4
004af34a e8413af7ff               call       0x422d90
004af34f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af352 83c114                   add        ecx, 0x14
004af355 e8363af7ff               call       0x422d90
004af35a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af35d 83c124                   add        ecx, 0x24
004af360 e82b3af7ff               call       0x422d90
004af365 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af368 83c134                   add        ecx, 0x34
004af36b e87023fbff               call       0x4616e0
004af370 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af373 83c144                   add        ecx, 0x44
004af376 e86523fbff               call       0x4616e0
004af37b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af37e 83c154                   add        ecx, 0x54
004af381 e83a69f7ff               call       0x425cc0
004af386 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af389 83c158                   add        ecx, 0x58
004af38c e82f69f7ff               call       0x425cc0
004af391 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af394 83c15c                   add        ecx, 0x5c
004af397 e82469f7ff               call       0x425cc0
004af39c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af39f 83c160                   add        ecx, 0x60
004af3a2 e81969f7ff               call       0x425cc0
004af3a7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af3aa 83c164                   add        ecx, 0x64
004af3ad e80e69f7ff               call       0x425cc0
004af3b2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af3b5 83c168                   add        ecx, 0x68
004af3b8 e80369f7ff               call       0x425cc0
004af3bd 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af3c0 83c16c                   add        ecx, 0x6c
004af3c3 e8f868f7ff               call       0x425cc0
004af3c8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af3cb 83c170                   add        ecx, 0x70
004af3ce e8ed68f7ff               call       0x425cc0
004af3d3 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af3d6 c7417400000000           mov        dword ptr [ecx + 0x74], 0
004af3dd 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004af3e0 c7427800000000           mov        dword ptr [edx + 0x78], 0
004af3e7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af3ea 83c17c                   add        ecx, 0x7c
004af3ed e80efbffff               call       0x4aef00
004af3f2 c745fc00000000           mov        dword ptr [ebp - 4], 0
004af3f9 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004af3fc c780c800000000000000     mov        dword ptr [eax + 0xc8], 0
004af406 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af409 c781cc00000000000000     mov        dword ptr [ecx + 0xcc], 0
004af413 68102e4200               push       0x422e10
004af418 6a04                     push       4
004af41a 6a0c                     push       0xc
004af41c 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004af41f 81c2d0000000             add        edx, 0xd0
004af425 52                       push       edx
004af426 e8f5c7f5ff               call       0x40bc20
004af42b 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004af42e c7800001000000000000     mov        dword ptr [eax + 0x100], 0
004af438 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af43b 81c104010000             add        ecx, 0x104
004af441 e84af8ffff               call       0x4aec90
004af446 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af449 c7810801000000000000     mov        dword ptr [ecx + 0x108], 0
004af453 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004af456 c7820c01000000000000     mov        dword ptr [edx + 0x10c], 0
004af460 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004af463 c7801001000000000000     mov        dword ptr [eax + 0x110], 0
004af46d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af470 c7811401000000000000     mov        dword ptr [ecx + 0x114], 0
004af47a 33d2                     xor        edx, edx
004af47c 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004af47f 0518010000               add        eax, 0x118
004af484 8910                     mov        dword ptr [eax], edx
004af486 895004                   mov        dword ptr [eax + 4], edx
004af489 895008                   mov        dword ptr [eax + 8], edx
004af48c 89500c                   mov        dword ptr [eax + 0xc], edx
004af48f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af492 81c128010000             add        ecx, 0x128
004af498 e87339f7ff               call       0x422e10
004af49d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af4a0 0f57c0                   xorps      xmm0, xmm0
004af4a3 f30f118134010000         movss      dword ptr [ecx + 0x134], xmm0
004af4ab 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004af4ae c7823801000000000000     mov        dword ptr [edx + 0x138], 0
004af4b8 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004af4bb c7803c01000000000000     mov        dword ptr [eax + 0x13c], 0
004af4c5 6a00                     push       0
004af4c7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af4ca 83c104                   add        ecx, 4
004af4cd e84e40f7ff               call       0x423520
004af4d2 6a00                     push       0
004af4d4 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af4d7 83c114                   add        ecx, 0x14
004af4da e84140f7ff               call       0x423520
004af4df 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af4e2 c781c800000000000000     mov        dword ptr [ecx + 0xc8], 0
004af4ec 6a00                     push       0
004af4ee 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af4f1 83c124                   add        ecx, 0x24
004af4f4 e82740f7ff               call       0x423520
004af4f9 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004af4fc 8b4508                   mov        eax, dword ptr [ebp + 8]
004af4ff 8982cc000000             mov        dword ptr [edx + 0xcc], eax
004af505 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004af50b e86011fcff               call       0x470670
004af510 8945d8                   mov        dword ptr [ebp - 0x28], eax
004af513 6a00                     push       0
004af515 6aff                     push       -1
004af517 6a14                     push       0x14
004af519 681cfe5600               push       0x56fe1c
004af51e 8d4da8                   lea        ecx, [ebp - 0x58]
004af521 51                       push       ecx
004af522 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004af525 e84617faff               call       0x450c70
004af52a 8b10                     mov        edx, dword ptr [eax]
004af52c 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004af52f 895058                   mov        dword ptr [eax + 0x58], edx
004af532 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004af538 e83311fcff               call       0x470670
004af53d 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004af540 6a00                     push       0
004af542 6aff                     push       -1
004af544 6a14                     push       0x14
004af546 681cfe5600               push       0x56fe1c
004af54b 8d4da4                   lea        ecx, [ebp - 0x5c]
004af54e 51                       push       ecx
004af54f 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004af552 e81917faff               call       0x450c70
004af557 8b10                     mov        edx, dword ptr [eax]
004af559 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004af55c 89505c                   mov        dword ptr [eax + 0x5c], edx
004af55f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af562 83c15c                   add        ecx, 0x5c
004af565 894dd0                   mov        dword ptr [ebp - 0x30], ecx
004af568 6a00                     push       0
004af56a 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004af56d e89e4ffbff               call       0x464510
004af572 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af575 83c158                   add        ecx, 0x58
004af578 e853d9f9ff               call       0x44ced0
004af57d c6807805000015           mov        byte ptr [eax + 0x578], 0x15
004af584 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af587 83c158                   add        ecx, 0x58
004af58a e841d9f9ff               call       0x44ced0
004af58f c6807905000015           mov        byte ptr [eax + 0x579], 0x15
004af596 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af599 83c15c                   add        ecx, 0x5c
004af59c e82fd9f9ff               call       0x44ced0
004af5a1 c6807805000015           mov        byte ptr [eax + 0x578], 0x15
004af5a8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af5ab 83c15c                   add        ecx, 0x5c
004af5ae e81dd9f9ff               call       0x44ced0
004af5b3 c6807905000015           mov        byte ptr [eax + 0x579], 0x15
004af5ba 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004af5c0 e8ab10fcff               call       0x470670
004af5c5 8945cc                   mov        dword ptr [ebp - 0x34], eax
004af5c8 6a00                     push       0
004af5ca 6aff                     push       -1
004af5cc 6a15                     push       0x15
004af5ce 681cfe5600               push       0x56fe1c
004af5d3 8d55a0                   lea        edx, [ebp - 0x60]
004af5d6 52                       push       edx
004af5d7 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004af5da e89116faff               call       0x450c70
004af5df 8b00                     mov        eax, dword ptr [eax]
004af5e1 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af5e4 894160                   mov        dword ptr [ecx + 0x60], eax
004af5e7 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004af5ed e87e10fcff               call       0x470670
004af5f2 8945c8                   mov        dword ptr [ebp - 0x38], eax
004af5f5 6a00                     push       0
004af5f7 6aff                     push       -1
004af5f9 6a15                     push       0x15
004af5fb 681cfe5600               push       0x56fe1c
004af600 8d559c                   lea        edx, [ebp - 0x64]
004af603 52                       push       edx
004af604 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004af607 e86416faff               call       0x450c70
004af60c 8b00                     mov        eax, dword ptr [eax]
004af60e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af611 894164                   mov        dword ptr [ecx + 0x64], eax
004af614 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004af617 83c264                   add        edx, 0x64
004af61a 8955c4                   mov        dword ptr [ebp - 0x3c], edx
004af61d 6a00                     push       0
004af61f 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004af622 e8e94efbff               call       0x464510
004af627 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af62a 83c160                   add        ecx, 0x60
004af62d e89ed8f9ff               call       0x44ced0
004af632 c6807805000015           mov        byte ptr [eax + 0x578], 0x15
004af639 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af63c 83c160                   add        ecx, 0x60
004af63f e88cd8f9ff               call       0x44ced0
004af644 c6807905000015           mov        byte ptr [eax + 0x579], 0x15
004af64b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af64e 83c164                   add        ecx, 0x64
004af651 e87ad8f9ff               call       0x44ced0
004af656 c6807805000015           mov        byte ptr [eax + 0x578], 0x15
004af65d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af660 83c164                   add        ecx, 0x64
004af663 e868d8f9ff               call       0x44ced0
004af668 c6807905000015           mov        byte ptr [eax + 0x579], 0x15
004af66f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af672 83c158                   add        ecx, 0x58
004af675 e856d8f9ff               call       0x44ced0
004af67a 059c040000               add        eax, 0x49c
004af67f 8945e8                   mov        dword ptr [ebp - 0x18], eax
004af682 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004af685 8b08                     mov        ecx, dword ptr [eax]
004af687 81e1fffbffff             and        ecx, 0xfffffbff
004af68d 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004af690 890a                     mov        dword ptr [edx], ecx
004af692 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af695 83c15c                   add        ecx, 0x5c
004af698 e833d8f9ff               call       0x44ced0
004af69d 059c040000               add        eax, 0x49c
004af6a2 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004af6a5 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004af6a8 8b08                     mov        ecx, dword ptr [eax]
004af6aa 81e1fffbffff             and        ecx, 0xfffffbff
004af6b0 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004af6b3 890a                     mov        dword ptr [edx], ecx
004af6b5 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af6b8 83c160                   add        ecx, 0x60
004af6bb e810d8f9ff               call       0x44ced0
004af6c0 059c040000               add        eax, 0x49c
004af6c5 8945e0                   mov        dword ptr [ebp - 0x20], eax
004af6c8 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004af6cb 8b08                     mov        ecx, dword ptr [eax]
004af6cd 81e1fffbffff             and        ecx, 0xfffffbff
004af6d3 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004af6d6 890a                     mov        dword ptr [edx], ecx
004af6d8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af6db 83c164                   add        ecx, 0x64
004af6de e8edd7f9ff               call       0x44ced0
004af6e3 059c040000               add        eax, 0x49c
004af6e8 8945dc                   mov        dword ptr [ebp - 0x24], eax
004af6eb 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004af6ee 8b08                     mov        ecx, dword ptr [eax]
004af6f0 81e1fffbffff             and        ecx, 0xfffffbff
004af6f6 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004af6f9 890a                     mov        dword ptr [edx], ecx
004af6fb c745ec90c24a00           mov        dword ptr [ebp - 0x14], 0x4ac290
004af702 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af705 83c158                   add        ecx, 0x58
004af708 e8c3d7f9ff               call       0x44ced0
004af70d 8945c0                   mov        dword ptr [ebp - 0x40], eax
004af710 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004af713 50                       push       eax
004af714 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004af717 e83493f8ff               call       0x438a50
004af71c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af71f 83c15c                   add        ecx, 0x5c
004af722 e8a9d7f9ff               call       0x44ced0
004af727 8945bc                   mov        dword ptr [ebp - 0x44], eax
004af72a 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004af72d 51                       push       ecx
004af72e 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004af731 e81a93f8ff               call       0x438a50
004af736 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af739 83c160                   add        ecx, 0x60
004af73c e88fd7f9ff               call       0x44ced0
004af741 8945b8                   mov        dword ptr [ebp - 0x48], eax
004af744 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004af747 52                       push       edx
004af748 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004af74b e80093f8ff               call       0x438a50
004af750 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af753 83c164                   add        ecx, 0x64
004af756 e875d7f9ff               call       0x44ced0
004af75b 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004af75e 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004af761 50                       push       eax
004af762 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004af765 e8e692f8ff               call       0x438a50
004af76a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af76d c7810801000000000000     mov        dword ptr [ecx + 0x108], 0
004af777 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004af77a c7820c01000000000000     mov        dword ptr [edx + 0x10c], 0
004af784 b804000000               mov        eax, 4
004af789 6bc800                   imul       ecx, eax, 0
004af78c 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004af78f c7840a1801000000000000   mov        dword ptr [edx + ecx + 0x118], 0
004af79a b804000000               mov        eax, 4
004af79f c1e000                   shl        eax, 0
004af7a2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af7a5 c784011801000000000000   mov        dword ptr [ecx + eax + 0x118], 0
004af7b0 ba04000000               mov        edx, 4
004af7b5 d1e2                     shl        edx, 1
004af7b7 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004af7ba c784101801000000000000   mov        dword ptr [eax + edx + 0x118], 0
004af7c5 b904000000               mov        ecx, 4
004af7ca 6bd103                   imul       edx, ecx, 3
004af7cd 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004af7d0 c784101801000000000000   mov        dword ptr [eax + edx + 0x118], 0
004af7db 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af7de c7811401000000000000     mov        dword ptr [ecx + 0x114], 0
004af7e8 51                       push       ecx
004af7e9 0f57c0                   xorps      xmm0, xmm0
004af7ec f30f110424               movss      dword ptr [esp], xmm0
004af7f1 51                       push       ecx
004af7f2 0f57c0                   xorps      xmm0, xmm0
004af7f5 f30f110424               movss      dword ptr [esp], xmm0
004af7fa 51                       push       ecx
004af7fb f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004af803 f30f110424               movss      dword ptr [esp], xmm0
004af808 8d4d90                   lea        ecx, [ebp - 0x70]
004af80b e8c035f7ff               call       0x422dd0
004af810 ba0c000000               mov        edx, 0xc
004af815 6bca00                   imul       ecx, edx, 0
004af818 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004af81b 8d8c0ad0000000           lea        ecx, [edx + ecx + 0xd0]
004af822 8b10                     mov        edx, dword ptr [eax]
004af824 8911                     mov        dword ptr [ecx], edx
004af826 8b5004                   mov        edx, dword ptr [eax + 4]
004af829 895104                   mov        dword ptr [ecx + 4], edx
004af82c 8b4008                   mov        eax, dword ptr [eax + 8]
004af82f 894108                   mov        dword ptr [ecx + 8], eax
004af832 51                       push       ecx
004af833 0f57c0                   xorps      xmm0, xmm0
004af836 f30f110424               movss      dword ptr [esp], xmm0
004af83b 51                       push       ecx
004af83c 0f57c0                   xorps      xmm0, xmm0
004af83f f30f110424               movss      dword ptr [esp], xmm0
004af844 51                       push       ecx
004af845 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004af84d f30f110424               movss      dword ptr [esp], xmm0
004af852 8d4d84                   lea        ecx, [ebp - 0x7c]
004af855 e87635f7ff               call       0x422dd0
004af85a b90c000000               mov        ecx, 0xc
004af85f c1e100                   shl        ecx, 0
004af862 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004af865 8d8c0ad0000000           lea        ecx, [edx + ecx + 0xd0]
004af86c 8b10                     mov        edx, dword ptr [eax]
004af86e 8911                     mov        dword ptr [ecx], edx
004af870 8b5004                   mov        edx, dword ptr [eax + 4]
004af873 895104                   mov        dword ptr [ecx + 4], edx
004af876 8b4008                   mov        eax, dword ptr [eax + 8]
004af879 894108                   mov        dword ptr [ecx + 8], eax
004af87c 51                       push       ecx
004af87d 0f57c0                   xorps      xmm0, xmm0
004af880 f30f110424               movss      dword ptr [esp], xmm0
004af885 51                       push       ecx
004af886 0f57c0                   xorps      xmm0, xmm0
004af889 f30f110424               movss      dword ptr [esp], xmm0
004af88e 51                       push       ecx
004af88f f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004af897 f30f110424               movss      dword ptr [esp], xmm0
004af89c 8d8d78ffffff             lea        ecx, [ebp - 0x88]
004af8a2 e82935f7ff               call       0x422dd0
004af8a7 b90c000000               mov        ecx, 0xc
004af8ac d1e1                     shl        ecx, 1
004af8ae 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004af8b1 8d8c0ad0000000           lea        ecx, [edx + ecx + 0xd0]
004af8b8 8b10                     mov        edx, dword ptr [eax]
004af8ba 8911                     mov        dword ptr [ecx], edx
004af8bc 8b5004                   mov        edx, dword ptr [eax + 4]
004af8bf 895104                   mov        dword ptr [ecx + 4], edx
004af8c2 8b4008                   mov        eax, dword ptr [eax + 8]
004af8c5 894108                   mov        dword ptr [ecx + 8], eax
004af8c8 51                       push       ecx
004af8c9 0f57c0                   xorps      xmm0, xmm0
004af8cc f30f110424               movss      dword ptr [esp], xmm0
004af8d1 51                       push       ecx
004af8d2 0f57c0                   xorps      xmm0, xmm0
004af8d5 f30f110424               movss      dword ptr [esp], xmm0
004af8da 51                       push       ecx
004af8db f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004af8e3 f30f110424               movss      dword ptr [esp], xmm0
004af8e8 8d8d6cffffff             lea        ecx, [ebp - 0x94]
004af8ee e8dd34f7ff               call       0x422dd0
004af8f3 b90c000000               mov        ecx, 0xc
004af8f8 6bd103                   imul       edx, ecx, 3
004af8fb 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af8fe 8d9411d0000000           lea        edx, [ecx + edx + 0xd0]
004af905 8b08                     mov        ecx, dword ptr [eax]
004af907 890a                     mov        dword ptr [edx], ecx
004af909 8b4804                   mov        ecx, dword ptr [eax + 4]
004af90c 894a04                   mov        dword ptr [edx + 4], ecx
004af90f 8b4008                   mov        eax, dword ptr [eax + 8]
004af912 894208                   mov        dword ptr [edx + 8], eax
004af915 6a00                     push       0
004af917 e86495fcff               call       0x478e80
004af91c 83c404                   add        esp, 4
004af91f 8945b0                   mov        dword ptr [ebp - 0x50], eax
004af922 6a00                     push       0
004af924 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004af927 e864d2fcff               call       0x47cb90
004af92c 6a00                     push       0
004af92e e8ad95fcff               call       0x478ee0
004af933 83c404                   add        esp, 4
004af936 8945ac                   mov        dword ptr [ebp - 0x54], eax
004af939 6a00                     push       0
004af93b 6a00                     push       0
004af93d 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
004af940 e84b9b0100               call       0x4c9490
004af945 6a00                     push       0
004af947 e81487fcff               call       0x478060
004af94c 83c404                   add        esp, 4
004af94f 8bc8                     mov        ecx, eax
004af951 e8ca50ffff               call       0x4a4a20
004af956 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af959 f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
004af961 f30f118128010000         movss      dword ptr [ecx + 0x128], xmm0
004af969 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004af96c f30f1005accd5600         movss      xmm0, dword ptr [0x56cdac]
004af974 f30f11822c010000         movss      dword ptr [edx + 0x12c], xmm0
004af97c 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004af97f 0f57c0                   xorps      xmm0, xmm0
004af982 f30f118030010000         movss      dword ptr [eax + 0x130], xmm0
004af98a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af98d f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
004af995 f30f118134010000         movss      dword ptr [ecx + 0x134], xmm0
004af99d 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004af9a0 8b8204010000             mov        eax, dword ptr [edx + 0x104]
004af9a6 83e0bf                   and        eax, 0xffffffbf
004af9a9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004af9ac 898104010000             mov        dword ptr [ecx + 0x104], eax
004af9b2 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004af9b5 c7821001000000000000     mov        dword ptr [edx + 0x110], 0
004af9bf c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004af9c6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004af9c9 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004af9cc 64890d00000000           mov        dword ptr fs:[0], ecx
004af9d3 59                       pop        ecx
004af9d4 8be5                     mov        esp, ebp
004af9d6 5d                       pop        ebp
004af9d7 c20400                   ret        4
004af9da cc                       int3       
004af9db cc                       int3       
004af9dc cc                       int3       
004af9dd cc                       int3       
004af9de cc                       int3       
004af9df cc                       int3       
