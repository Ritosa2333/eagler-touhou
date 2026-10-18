0044e350 55                       push       ebp
0044e351 8bec                     mov        ebp, esp
0044e353 81ece0000000             sub        esp, 0xe0
0044e359 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0044e35e 33c5                     xor        eax, ebp
0044e360 8945fc                   mov        dword ptr [ebp - 4], eax
0044e363 898d70ffffff             mov        dword ptr [ebp - 0x90], ecx
0044e369 c78540ffffff00000000     mov        dword ptr [ebp - 0xc0], 0
0044e373 837d1800                 cmp        dword ptr [ebp + 0x18], 0
0044e377 7521                     jne        0x44e39a
0044e379 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044e37c 83c104                   add        ecx, 4
0044e37f e86cd7ffff               call       0x44baf0
0044e384 50                       push       eax
0044e385 68fce45600               push       0x56e4fc
0044e38a e821e3fbff               call       0x40c6b0
0044e38f 83c408                   add        esp, 8
0044e392 83c8ff                   or         eax, 0xffffffff
0044e395 e942050000               jmp        0x44e8dc
0044e39a 8b4518                   mov        eax, dword ptr [ebp + 0x18]
0044e39d 894580                   mov        dword ptr [ebp - 0x80], eax
0044e3a0 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
0044e3a3 833908                   cmp        dword ptr [ecx], 8
0044e3a6 7426                     je         0x44e3ce
0044e3a8 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044e3ab 83c104                   add        ecx, 4
0044e3ae e83dd7ffff               call       0x44baf0
0044e3b3 50                       push       eax
0044e3b4 683ce55600               push       0x56e53c
0044e3b9 6878065c00               push       0x5c0678
0044e3be e86d5e0000               call       0x454230
0044e3c3 83c40c                   add        esp, 0xc
0044e3c6 83c8ff                   or         eax, 0xffffffff
0044e3c9 e90e050000               jmp        0x44e8dc
0044e3ce 6b550c18                 imul       edx, dword ptr [ebp + 0xc], 0x18
0044e3d2 8b4508                   mov        eax, dword ptr [ebp + 8]
0044e3d5 8b4858                   mov        ecx, dword ptr [eax + 0x58]
0044e3d8 8b4580                   mov        eax, dword ptr [ebp - 0x80]
0044e3db 89441110                 mov        dword ptr [ecx + edx + 0x10], eax
0044e3df 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
0044e3e2 0fb65120                 movzx      edx, byte ptr [ecx + 0x20]
0044e3e6 85d2                     test       edx, edx
0044e3e8 0f8570010000             jne        0x44e55e
0044e3ee 8b4580                   mov        eax, dword ptr [ebp - 0x80]
0044e3f1 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
0044e3f4 034810                   add        ecx, dword ptr [eax + 0x10]
0044e3f7 898d6cffffff             mov        dword ptr [ebp - 0x94], ecx
0044e3fd ba01000000               mov        edx, 1
0044e402 6bc200                   imul       eax, edx, 0
0044e405 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
0044e40b 0fbe1401                 movsx      edx, byte ptr [ecx + eax]
0044e40f 83fa40                   cmp        edx, 0x40
0044e412 0f85b0000000             jne        0x44e4c8
0044e418 b801000000               mov        eax, 1
0044e41d c1e000                   shl        eax, 0
0044e420 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
0044e426 0fbe1401                 movsx      edx, byte ptr [ecx + eax]
0044e42a 83fa52                   cmp        edx, 0x52
0044e42d 7542                     jne        0x44e471
0044e42f 8b4580                   mov        eax, dword ptr [ebp - 0x80]
0044e432 668b0df8875b00           mov        cx, word ptr [0x5b87f8]
0044e439 6689480a                 mov        word ptr [eax + 0xa], cx
0044e43d 8b5580                   mov        edx, dword ptr [ebp - 0x80]
0044e440 66a1fc875b00             mov        ax, word ptr [0x5b87fc]
0044e446 6689420c                 mov        word ptr [edx + 0xc], ax
0044e44a 8b0dfc875b00             mov        ecx, dword ptr [0x5b87fc]
0044e450 51                       push       ecx
0044e451 8b15f8875b00             mov        edx, dword ptr [0x5b87f8]
0044e457 52                       push       edx
0044e458 6b450c18                 imul       eax, dword ptr [ebp + 0xc], 0x18
0044e45c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044e45f 034158                   add        eax, dword ptr [ecx + 0x58]
0044e462 50                       push       eax
0044e463 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
0044e469 e8e2dcffff               call       0x44c150
0044e46e 90                       nop        
0044e46f eb52                     jmp        0x44e4c3
0044e471 8b5580                   mov        edx, dword ptr [ebp - 0x80]
0044e474 0fb7420e                 movzx      eax, word ptr [edx + 0xe]
0044e478 50                       push       eax
0044e479 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
0044e47c 0fb7510c                 movzx      edx, word ptr [ecx + 0xc]
0044e480 52                       push       edx
0044e481 8b4580                   mov        eax, dword ptr [ebp - 0x80]
0044e484 0fb7480a                 movzx      ecx, word ptr [eax + 0xa]
0044e488 51                       push       ecx
0044e489 6b550c18                 imul       edx, dword ptr [ebp + 0xc], 0x18
0044e48d 8b4508                   mov        eax, dword ptr [ebp + 8]
0044e490 035058                   add        edx, dword ptr [eax + 0x58]
0044e493 52                       push       edx
0044e494 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
0044e49a e811dcffff               call       0x44c0b0
0044e49f 89854cffffff             mov        dword ptr [ebp - 0xb4], eax
0044e4a5 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044e4a8 8b5164                   mov        edx, dword ptr [ecx + 0x64]
0044e4ab 899550ffffff             mov        dword ptr [ebp - 0xb0], edx
0044e4b1 8b8550ffffff             mov        eax, dword ptr [ebp - 0xb0]
0044e4b7 03854cffffff             add        eax, dword ptr [ebp - 0xb4]
0044e4bd 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044e4c0 894164                   mov        dword ptr [ecx + 0x64], eax
0044e4c3 e994000000               jmp        0x44e55c
0044e4c8 8b5580                   mov        edx, dword ptr [ebp - 0x80]
0044e4cb 0fb7422a                 movzx      eax, word ptr [edx + 0x2a]
0044e4cf 50                       push       eax
0044e4d0 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
0044e4d3 0fb75128                 movzx      edx, word ptr [ecx + 0x28]
0044e4d7 52                       push       edx
0044e4d8 8b4580                   mov        eax, dword ptr [ebp - 0x80]
0044e4db 0fbf4816                 movsx      ecx, word ptr [eax + 0x16]
0044e4df 51                       push       ecx
0044e4e0 8b5580                   mov        edx, dword ptr [ebp - 0x80]
0044e4e3 0fbf4214                 movsx      eax, word ptr [edx + 0x14]
0044e4e7 50                       push       eax
0044e4e8 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
0044e4eb 0fb7510c                 movzx      edx, word ptr [ecx + 0xc]
0044e4ef 52                       push       edx
0044e4f0 8b4580                   mov        eax, dword ptr [ebp - 0x80]
0044e4f3 0fb7480a                 movzx      ecx, word ptr [eax + 0xa]
0044e4f7 51                       push       ecx
0044e4f8 6a00                     push       0
0044e4fa 8b5580                   mov        edx, dword ptr [ebp - 0x80]
0044e4fd 0fb7420e                 movzx      eax, word ptr [edx + 0xe]
0044e501 50                       push       eax
0044e502 6b4d0c18                 imul       ecx, dword ptr [ebp + 0xc], 0x18
0044e506 8b5508                   mov        edx, dword ptr [ebp + 8]
0044e509 034a58                   add        ecx, dword ptr [edx + 0x58]
0044e50c 51                       push       ecx
0044e50d 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
0044e513 e8d8030000               call       0x44e8f0
0044e518 898568ffffff             mov        dword ptr [ebp - 0x98], eax
0044e51e 83bd68ffffff00           cmp        dword ptr [ebp - 0x98], 0
0044e525 7d23                     jge        0x44e54a
0044e527 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044e52a 83c104                   add        ecx, 4
0044e52d e8bed5ffff               call       0x44baf0
0044e532 50                       push       eax
0044e533 6860e55600               push       0x56e560
0044e538 e873e1fbff               call       0x40c6b0
0044e53d 83c408                   add        esp, 8
0044e540 83c8ff                   or         eax, 0xffffffff
0044e543 e994030000               jmp        0x44e8dc
0044e548 eb12                     jmp        0x44e55c
0044e54a 8b4508                   mov        eax, dword ptr [ebp + 8]
0044e54d 8b4864                   mov        ecx, dword ptr [eax + 0x64]
0044e550 038d68ffffff             add        ecx, dword ptr [ebp - 0x98]
0044e556 8b5508                   mov        edx, dword ptr [ebp + 8]
0044e559 894a64                   mov        dword ptr [edx + 0x64], ecx
0044e55c eb7c                     jmp        0x44e5da
0044e55e 8b4580                   mov        eax, dword ptr [ebp - 0x80]
0044e561 0fb7480c                 movzx      ecx, word ptr [eax + 0xc]
0044e565 51                       push       ecx
0044e566 8b5580                   mov        edx, dword ptr [ebp - 0x80]
0044e569 0fb7420a                 movzx      eax, word ptr [edx + 0xa]
0044e56d 50                       push       eax
0044e56e 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
0044e571 0fb7510e                 movzx      edx, word ptr [ecx + 0xe]
0044e575 52                       push       edx
0044e576 8b4580                   mov        eax, dword ptr [ebp - 0x80]
0044e579 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
0044e57c 03481c                   add        ecx, dword ptr [eax + 0x1c]
0044e57f 51                       push       ecx
0044e580 6b550c18                 imul       edx, dword ptr [ebp + 0xc], 0x18
0044e584 8b4508                   mov        eax, dword ptr [ebp + 8]
0044e587 035058                   add        edx, dword ptr [eax + 0x58]
0044e58a 52                       push       edx
0044e58b 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
0044e591 e83af4ffff               call       0x44d9d0
0044e596 898564ffffff             mov        dword ptr [ebp - 0x9c], eax
0044e59c 83bd64ffffff00           cmp        dword ptr [ebp - 0x9c], 0
0044e5a3 7d23                     jge        0x44e5c8
0044e5a5 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044e5a8 83c104                   add        ecx, 4
0044e5ab e840d5ffff               call       0x44baf0
0044e5b0 50                       push       eax
0044e5b1 6860e55600               push       0x56e560
0044e5b6 e8f5e0fbff               call       0x40c6b0
0044e5bb 83c408                   add        esp, 8
0044e5be 83c8ff                   or         eax, 0xffffffff
0044e5c1 e916030000               jmp        0x44e8dc
0044e5c6 eb12                     jmp        0x44e5da
0044e5c8 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044e5cb 8b5164                   mov        edx, dword ptr [ecx + 0x64]
0044e5ce 039564ffffff             add        edx, dword ptr [ebp - 0x9c]
0044e5d4 8b4508                   mov        eax, dword ptr [ebp + 8]
0044e5d7 895064                   mov        dword ptr [eax + 0x64], edx
0044e5da 6b4d0c18                 imul       ecx, dword ptr [ebp + 0xc], 0x18
0044e5de 8b5508                   mov        edx, dword ptr [ebp + 8]
0044e5e1 8b4258                   mov        eax, dword ptr [edx + 0x58]
0044e5e4 8b0c08                   mov        ecx, dword ptr [eax + ecx]
0044e5e7 898d48ffffff             mov        dword ptr [ebp - 0xb8], ecx
0044e5ed 6b550c18                 imul       edx, dword ptr [ebp + 0xc], 0x18
0044e5f1 8b4508                   mov        eax, dword ptr [ebp + 8]
0044e5f4 8b4858                   mov        ecx, dword ptr [eax + 0x58]
0044e5f7 8b1411                   mov        edx, dword ptr [ecx + edx]
0044e5fa 8b02                     mov        eax, dword ptr [edx]
0044e5fc 8b4844                   mov        ecx, dword ptr [eax + 0x44]
0044e5ff 898d44ffffff             mov        dword ptr [ebp - 0xbc], ecx
0044e605 8d5584                   lea        edx, [ebp - 0x7c]
0044e608 52                       push       edx
0044e609 6a00                     push       0
0044e60b 8b8548ffffff             mov        eax, dword ptr [ebp - 0xb8]
0044e611 50                       push       eax
0044e612 ff9544ffffff             call       dword ptr [ebp - 0xbc]
0044e618 90                       nop        
0044e619 8d4da4                   lea        ecx, [ebp - 0x5c]
0044e61c e8cfa2ffff               call       0x4488f0
0044e621 b940000000               mov        ecx, 0x40
0044e626 c1e100                   shl        ecx, 0
0044e629 034d80                   add        ecx, dword ptr [ebp - 0x80]
0044e62c 898d74ffffff             mov        dword ptr [ebp - 0x8c], ecx
0044e632 c78578ffffff00000000     mov        dword ptr [ebp - 0x88], 0
0044e63c eb1e                     jmp        0x44e65c
0044e63e 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
0044e644 83c201                   add        edx, 1
0044e647 899578ffffff             mov        dword ptr [ebp - 0x88], edx
0044e64d 8b8574ffffff             mov        eax, dword ptr [ebp - 0x8c]
0044e653 83c004                   add        eax, 4
0044e656 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
0044e65c 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
0044e65f 0fb75104                 movzx      edx, word ptr [ecx + 4]
0044e663 399578ffffff             cmp        dword ptr [ebp - 0x88], edx
0044e669 0f8d04020000             jge        0x44e873
0044e66f 8b8574ffffff             mov        eax, dword ptr [ebp - 0x8c]
0044e675 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
0044e678 0308                     add        ecx, dword ptr [eax]
0044e67a 898d7cffffff             mov        dword ptr [ebp - 0x84], ecx
0044e680 8b5508                   mov        edx, dword ptr [ebp + 8]
0044e683 8b02                     mov        eax, dword ptr [edx]
0044e685 8945a4                   mov        dword ptr [ebp - 0x5c], eax
0044e688 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0044e68b 51                       push       ecx
0044e68c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044e68f e86ce9ffff               call       0x44d000
0044e694 8945ac                   mov        dword ptr [ebp - 0x54], eax
0044e697 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0044e69a 8955a8                   mov        dword ptr [ebp - 0x58], edx
0044e69d 8b459c                   mov        eax, dword ptr [ebp - 0x64]
0044e6a0 898560ffffff             mov        dword ptr [ebp - 0xa0], eax
0044e6a6 f20f2a8560ffffff         cvtsi2sd   xmm0, dword ptr [ebp - 0xa0]
0044e6ae 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
0044e6b4 c1e91f                   shr        ecx, 0x1f
0044e6b7 f20f5804cdd0cd5600       addsd      xmm0, qword ptr [ecx*8 + 0x56cdd0]
0044e6c0 f20f118538ffffff         movsd      qword ptr [ebp - 0xc8], xmm0
0044e6c8 f20f5a8538ffffff         cvtsd2ss   xmm0, qword ptr [ebp - 0xc8]
0044e6d0 8b5580                   mov        edx, dword ptr [ebp - 0x80]
0044e6d3 0fb7420a                 movzx      eax, word ptr [edx + 0xa]
0044e6d7 f30f2ac8                 cvtsi2ss   xmm1, eax
0044e6db f30f5ec1                 divss      xmm0, xmm1
0044e6df f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
0044e6e4 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
0044e6e7 898d5cffffff             mov        dword ptr [ebp - 0xa4], ecx
0044e6ed f20f2a855cffffff         cvtsi2sd   xmm0, dword ptr [ebp - 0xa4]
0044e6f5 8b955cffffff             mov        edx, dword ptr [ebp - 0xa4]
0044e6fb c1ea1f                   shr        edx, 0x1f
0044e6fe f20f5804d5d0cd5600       addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
0044e707 f20f118530ffffff         movsd      qword ptr [ebp - 0xd0], xmm0
0044e70f f20f5a8530ffffff         cvtsd2ss   xmm0, qword ptr [ebp - 0xd0]
0044e717 8b4580                   mov        eax, dword ptr [ebp - 0x80]
0044e71a 0fb7480c                 movzx      ecx, word ptr [eax + 0xc]
0044e71e f30f2ac9                 cvtsi2ss   xmm1, ecx
0044e722 f30f5ec1                 divss      xmm0, xmm1
0044e726 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
0044e72b 8b957cffffff             mov        edx, dword ptr [ebp - 0x84]
0044e731 f30f104204               movss      xmm0, dword ptr [edx + 4]
0044e736 f30f5945f4               mulss      xmm0, dword ptr [ebp - 0xc]
0044e73b f30f1145b0               movss      dword ptr [ebp - 0x50], xmm0
0044e740 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
0044e746 f30f104008               movss      xmm0, dword ptr [eax + 8]
0044e74b f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
0044e750 f30f1145b4               movss      dword ptr [ebp - 0x4c], xmm0
0044e755 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
0044e75b 8b957cffffff             mov        edx, dword ptr [ebp - 0x84]
0044e761 f30f104104               movss      xmm0, dword ptr [ecx + 4]
0044e766 f30f58420c               addss      xmm0, dword ptr [edx + 0xc]
0044e76b f30f5945f4               mulss      xmm0, dword ptr [ebp - 0xc]
0044e770 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
0044e775 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
0044e77b 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
0044e781 f30f104008               movss      xmm0, dword ptr [eax + 8]
0044e786 f30f584110               addss      xmm0, dword ptr [ecx + 0x10]
0044e78b f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
0044e790 f30f1145bc               movss      dword ptr [ebp - 0x44], xmm0
0044e795 8b559c                   mov        edx, dword ptr [ebp - 0x64]
0044e798 899558ffffff             mov        dword ptr [ebp - 0xa8], edx
0044e79e f20f2a8558ffffff         cvtsi2sd   xmm0, dword ptr [ebp - 0xa8]
0044e7a6 8b8558ffffff             mov        eax, dword ptr [ebp - 0xa8]
0044e7ac c1e81f                   shr        eax, 0x1f
0044e7af f20f5804c5d0cd5600       addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
0044e7b8 f20f118528ffffff         movsd      qword ptr [ebp - 0xd8], xmm0
0044e7c0 f20f5a8528ffffff         cvtsd2ss   xmm0, qword ptr [ebp - 0xd8]
0044e7c8 f30f1145c4               movss      dword ptr [ebp - 0x3c], xmm0
0044e7cd 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
0044e7d0 898d54ffffff             mov        dword ptr [ebp - 0xac], ecx
0044e7d6 f20f2a8554ffffff         cvtsi2sd   xmm0, dword ptr [ebp - 0xac]
0044e7de 8b9554ffffff             mov        edx, dword ptr [ebp - 0xac]
0044e7e4 c1ea1f                   shr        edx, 0x1f
0044e7e7 f20f5804d5d0cd5600       addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
0044e7f0 f20f118520ffffff         movsd      qword ptr [ebp - 0xe0], xmm0
0044e7f8 f20f5a8520ffffff         cvtsd2ss   xmm0, qword ptr [ebp - 0xe0]
0044e800 f30f1145c0               movss      dword ptr [ebp - 0x40], xmm0
0044e805 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
0044e80b f30f104014               movss      xmm0, dword ptr [eax + 0x14]
0044e810 f30f1145c8               movss      dword ptr [ebp - 0x38], xmm0
0044e815 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
0044e81b f30f104118               movss      xmm0, dword ptr [ecx + 0x18]
0044e820 f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
0044e825 8b957cffffff             mov        edx, dword ptr [ebp - 0x84]
0044e82b f30f10421c               movss      xmm0, dword ptr [edx + 0x1c]
0044e830 f30f1145d0               movss      dword ptr [ebp - 0x30], xmm0
0044e835 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
0044e83b f30f104020               movss      xmm0, dword ptr [eax + 0x20]
0044e840 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
0044e845 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
0044e84b f30f104124               movss      xmm0, dword ptr [ecx + 0x24]
0044e850 f30f1145d8               movss      dword ptr [ebp - 0x28], xmm0
0044e855 8d55a4                   lea        edx, [ebp - 0x5c]
0044e858 52                       push       edx
0044e859 8b4510                   mov        eax, dword ptr [ebp + 0x10]
0044e85c 50                       push       eax
0044e85d 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044e860 e88b1e0000               call       0x4506f0
0044e865 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
0044e868 83c101                   add        ecx, 1
0044e86b 894d10                   mov        dword ptr [ebp + 0x10], ecx
0044e86e e9cbfdffff               jmp        0x44e63e
0044e873 c78578ffffff00000000     mov        dword ptr [ebp - 0x88], 0
0044e87d eb1e                     jmp        0x44e89d
0044e87f 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
0044e885 83c201                   add        edx, 1
0044e888 899578ffffff             mov        dword ptr [ebp - 0x88], edx
0044e88e 8b8574ffffff             mov        eax, dword ptr [ebp - 0x8c]
0044e894 83c008                   add        eax, 8
0044e897 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
0044e89d 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
0044e8a0 0fb75106                 movzx      edx, word ptr [ecx + 6]
0044e8a4 399578ffffff             cmp        dword ptr [ebp - 0x88], edx
0044e8aa 7d2b                     jge        0x44e8d7
0044e8ac b804000000               mov        eax, 4
0044e8b1 c1e000                   shl        eax, 0
0044e8b4 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
0044e8ba 8b5580                   mov        edx, dword ptr [ebp - 0x80]
0044e8bd 031401                   add        edx, dword ptr [ecx + eax]
0044e8c0 8b4508                   mov        eax, dword ptr [ebp + 8]
0044e8c3 8b4854                   mov        ecx, dword ptr [eax + 0x54]
0044e8c6 8b4514                   mov        eax, dword ptr [ebp + 0x14]
0044e8c9 891481                   mov        dword ptr [ecx + eax*4], edx
0044e8cc 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
0044e8cf 83c101                   add        ecx, 1
0044e8d2 894d14                   mov        dword ptr [ebp + 0x14], ecx
0044e8d5 eba8                     jmp        0x44e87f
0044e8d7 b801000000               mov        eax, 1
0044e8dc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044e8df 33cd                     xor        ecx, ebp
0044e8e1 e808410f00               call       0x5429ee
0044e8e6 8be5                     mov        esp, ebp
0044e8e8 5d                       pop        ebp
0044e8e9 c21400                   ret        0x14
0044e8ec cc                       int3       
0044e8ed cc                       int3       
0044e8ee cc                       int3       
0044e8ef cc                       int3       
