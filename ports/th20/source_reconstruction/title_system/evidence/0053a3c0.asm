0053a3c0 55                       push       ebp
0053a3c1 8bec                     mov        ebp, esp
0053a3c3 83ec34                   sub        esp, 0x34
0053a3c6 56                       push       esi
0053a3c7 894df0                   mov        dword ptr [ebp - 0x10], ecx
0053a3ca c745f800000000           mov        dword ptr [ebp - 8], 0
0053a3d1 c645ff00                 mov        byte ptr [ebp - 1], 0
0053a3d5 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0053a3d8 83780c00                 cmp        dword ptr [eax + 0xc], 0
0053a3dc 7507                     jne        0x53a3e5
0053a3de 33c0                     xor        eax, eax
0053a3e0 e928020000               jmp        0x53a60d
0053a3e5 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0053a3e8 51                       push       ecx
0053a3e9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0053a3ec e85fffffff               call       0x53a350
0053a3f1 8945f4                   mov        dword ptr [ebp - 0xc], eax
0053a3f4 837df400                 cmp        dword ptr [ebp - 0xc], 0
0053a3f8 750a                     jne        0x53a404
0053a3fa e9d6010000               jmp        0x53a5d5
0053a3ff e9d1010000               jmp        0x53a5d5
0053a404 ba10000000               mov        edx, 0x10
0053a409 c1e200                   shl        edx, 0
0053a40c b810000000               mov        eax, 0x10
0053a411 6bc800                   imul       ecx, eax, 0
0053a414 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0053a417 8b75f4                   mov        esi, dword ptr [ebp - 0xc]
0053a41a 8b541004                 mov        edx, dword ptr [eax + edx + 4]
0053a41e 2b540e04                 sub        edx, dword ptr [esi + ecx + 4]
0053a422 8955ec                   mov        dword ptr [ebp - 0x14], edx
0053a425 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0053a428 8b4808                   mov        ecx, dword ptr [eax + 8]
0053a42b 894de8                   mov        dword ptr [ebp - 0x18], ecx
0053a42e 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0053a431 3b55e8                   cmp        edx, dword ptr [ebp - 0x18]
0053a434 7506                     jne        0x53a43c
0053a436 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
0053a43a 7529                     jne        0x53a465
0053a43c 6810695700               push       0x576910
0053a441 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0053a444 50                       push       eax
0053a445 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0053a44b e8c051eeff               call       0x41f610
0053a450 8945f8                   mov        dword ptr [ebp - 8], eax
0053a453 837df800                 cmp        dword ptr [ebp - 8], 0
0053a457 750a                     jne        0x53a463
0053a459 e977010000               jmp        0x53a5d5
0053a45e e972010000               jmp        0x53a5d5
0053a463 eb06                     jmp        0x53a46b
0053a465 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0053a468 894df8                   mov        dword ptr [ebp - 8], ecx
0053a46b 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0053a46e 8b420c                   mov        eax, dword ptr [edx + 0xc]
0053a471 8945e0                   mov        dword ptr [ebp - 0x20], eax
0053a474 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0053a477 8b510c                   mov        edx, dword ptr [ecx + 0xc]
0053a47a 8b02                     mov        eax, dword ptr [edx]
0053a47c 8b4818                   mov        ecx, dword ptr [eax + 0x18]
0053a47f 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0053a482 8b15d4675700             mov        edx, dword ptr [0x5767d4]
0053a488 52                       push       edx
0053a489 b810000000               mov        eax, 0x10
0053a48e 6bc800                   imul       ecx, eax, 0
0053a491 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0053a494 8b440a04                 mov        eax, dword ptr [edx + ecx + 4]
0053a498 50                       push       eax
0053a499 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0053a49c ff55dc                   call       dword ptr [ebp - 0x24]
0053a49f 0fb6c8                   movzx      ecx, al
0053a4a2 85c9                     test       ecx, ecx
0053a4a4 750a                     jne        0x53a4b0
0053a4a6 e92a010000               jmp        0x53a5d5
0053a4ab e925010000               jmp        0x53a5d5
0053a4b0 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0053a4b3 8b420c                   mov        eax, dword ptr [edx + 0xc]
0053a4b6 8945d8                   mov        dword ptr [ebp - 0x28], eax
0053a4b9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0053a4bc 8b510c                   mov        edx, dword ptr [ecx + 0xc]
0053a4bf 8b02                     mov        eax, dword ptr [edx]
0053a4c1 8b4808                   mov        ecx, dword ptr [eax + 8]
0053a4c4 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
0053a4c7 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0053a4ca 52                       push       edx
0053a4cb 8b45f8                   mov        eax, dword ptr [ebp - 8]
0053a4ce 50                       push       eax
0053a4cf 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0053a4d2 ff55d4                   call       dword ptr [ebp - 0x2c]
0053a4d5 85c0                     test       eax, eax
0053a4d7 750a                     jne        0x53a4e3
0053a4d9 e9f7000000               jmp        0x53a5d5
0053a4de e9f2000000               jmp        0x53a5d5
0053a4e3 b910000000               mov        ecx, 0x10
0053a4e8 6bd100                   imul       edx, ecx, 0
0053a4eb 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0053a4ee 8b0c10                   mov        ecx, dword ptr [eax + edx]
0053a4f1 51                       push       ecx
0053a4f2 e829590100               call       0x54fe20
0053a4f7 83c404                   add        esp, 4
0053a4fa 8945d0                   mov        dword ptr [ebp - 0x30], eax
0053a4fd ba10000000               mov        edx, 0x10
0053a502 6bc200                   imul       eax, edx, 0
0053a505 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0053a508 8b1401                   mov        edx, dword ptr [ecx + eax]
0053a50b 8955cc                   mov        dword ptr [ebp - 0x34], edx
0053a50e 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
0053a511 50                       push       eax
0053a512 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0053a515 51                       push       ecx
0053a516 e855bdf1ff               call       0x456270
0053a51b 83c408                   add        esp, 8
0053a51e 0fb6d0                   movzx      edx, al
0053a521 81e207000080             and        edx, 0x80000007
0053a527 7905                     jns        0x53a52e
0053a529 4a                       dec        edx
0053a52a 83caf8                   or         edx, 0xfffffff8
0053a52d 42                       inc        edx
0053a52e 8855ff                   mov        byte ptr [ebp - 1], dl
0053a531 0fb645ff                 movzx      eax, byte ptr [ebp - 1]
0053a535 6bc80c                   imul       ecx, eax, 0xc
0053a538 8b9108e05a00             mov        edx, dword ptr [ecx + 0x5ae008]
0053a53e 52                       push       edx
0053a53f 0fb645ff                 movzx      eax, byte ptr [ebp - 1]
0053a543 6bc80c                   imul       ecx, eax, 0xc
0053a546 8b9104e05a00             mov        edx, dword ptr [ecx + 0x5ae004]
0053a54c 52                       push       edx
0053a54d 0fb645ff                 movzx      eax, byte ptr [ebp - 1]
0053a551 6bc80c                   imul       ecx, eax, 0xc
0053a554 0fb69101e05a00           movzx      edx, byte ptr [ecx + 0x5ae001]
0053a55b 52                       push       edx
0053a55c 0fb645ff                 movzx      eax, byte ptr [ebp - 1]
0053a560 6bc80c                   imul       ecx, eax, 0xc
0053a563 0fb69100e05a00           movzx      edx, byte ptr [ecx + 0x5ae000]
0053a56a 52                       push       edx
0053a56b 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0053a56e 50                       push       eax
0053a56f 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0053a572 51                       push       ecx
0053a573 e8685bedff               call       0x4100e0
0053a578 83c418                   add        esp, 0x18
0053a57b 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0053a57e 3b55e8                   cmp        edx, dword ptr [ebp - 0x18]
0053a581 741d                     je         0x53a5a0
0053a583 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0053a586 50                       push       eax
0053a587 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0053a58a 51                       push       ecx
0053a58b 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0053a58e 52                       push       edx
0053a58f 8b45f8                   mov        eax, dword ptr [ebp - 8]
0053a592 50                       push       eax
0053a593 e858ecffff               call       0x5391f0
0053a598 83c410                   add        esp, 0x10
0053a59b 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0053a59e eb06                     jmp        0x53a5a6
0053a5a0 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0053a5a3 894de4                   mov        dword ptr [ebp - 0x1c], ecx
0053a5a6 8b55f8                   mov        edx, dword ptr [ebp - 8]
0053a5a9 3b550c                   cmp        edx, dword ptr [ebp + 0xc]
0053a5ac 7422                     je         0x53a5d0
0053a5ae 837df800                 cmp        dword ptr [ebp - 8], 0
0053a5b2 741c                     je         0x53a5d0
0053a5b4 837df800                 cmp        dword ptr [ebp - 8], 0
0053a5b8 7416                     je         0x53a5d0
0053a5ba 8b45f8                   mov        eax, dword ptr [ebp - 8]
0053a5bd 50                       push       eax
0053a5be 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0053a5c4 e8a750eeff               call       0x41f670
0053a5c9 c745f800000000           mov        dword ptr [ebp - 8], 0
0053a5d0 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0053a5d3 eb38                     jmp        0x53a60d
0053a5d5 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0053a5d8 8b5108                   mov        edx, dword ptr [ecx + 8]
0053a5db 52                       push       edx
0053a5dc 684c695700               push       0x57694c
0053a5e1 e8ca20edff               call       0x40c6b0
0053a5e6 83c408                   add        esp, 8
0053a5e9 837df800                 cmp        dword ptr [ebp - 8], 0
0053a5ed 741c                     je         0x53a60b
0053a5ef 837df800                 cmp        dword ptr [ebp - 8], 0
0053a5f3 7416                     je         0x53a60b
0053a5f5 8b45f8                   mov        eax, dword ptr [ebp - 8]
0053a5f8 50                       push       eax
0053a5f9 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0053a5ff e86c50eeff               call       0x41f670
0053a604 c745f800000000           mov        dword ptr [ebp - 8], 0
0053a60b 33c0                     xor        eax, eax
0053a60d 5e                       pop        esi
0053a60e 8be5                     mov        esp, ebp
0053a610 5d                       pop        ebp
0053a611 c20800                   ret        8
0053a614 cc                       int3       
0053a615 cc                       int3       
0053a616 cc                       int3       
0053a617 cc                       int3       
0053a618 cc                       int3       
0053a619 cc                       int3       
0053a61a cc                       int3       
0053a61b cc                       int3       
0053a61c cc                       int3       
0053a61d cc                       int3       
0053a61e cc                       int3       
0053a61f cc                       int3       
