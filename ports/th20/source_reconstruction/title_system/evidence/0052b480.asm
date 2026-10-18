0052b480 55                       push       ebp
0052b481 8bec                     mov        ebp, esp
0052b483 81ec94000000             sub        esp, 0x94
0052b489 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052b48e 33c5                     xor        eax, ebp
0052b490 8945fc                   mov        dword ptr [ebp - 4], eax
0052b493 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0052b496 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0052b499 8b4820                   mov        ecx, dword ptr [eax + 0x20]
0052b49c 894dd0                   mov        dword ptr [ebp - 0x30], ecx
0052b49f 837dd001                 cmp        dword ptr [ebp - 0x30], 1
0052b4a3 7405                     je         0x52b4aa
0052b4a5 e91f040000               jmp        0x52b8c9
0052b4aa 51                       push       ecx
0052b4ab 0f57c0                   xorps      xmm0, xmm0
0052b4ae f30f110424               movss      dword ptr [esp], xmm0
0052b4b3 51                       push       ecx
0052b4b4 f30f1005d00a5700         movss      xmm0, dword ptr [0x570ad0]
0052b4bc f30f110424               movss      dword ptr [esp], xmm0
0052b4c1 51                       push       ecx
0052b4c2 f30f100564565700         movss      xmm0, dword ptr [0x575664]
0052b4ca f30f110424               movss      dword ptr [esp], xmm0
0052b4cf 8d4df0                   lea        ecx, [ebp - 0x10]
0052b4d2 e8f978efff               call       0x422dd0
0052b4d7 8d4de4                   lea        ecx, [ebp - 0x1c]
0052b4da e83179efff               call       0x422e10
0052b4df 6a04                     push       4
0052b4e1 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052b4e7 e814d6f5ff               call       0x488b00
0052b4ec 6a01                     push       1
0052b4ee 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052b4f4 e827b5fbff               call       0x4e6a20
0052b4f9 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0052b4fc 8955e4                   mov        dword ptr [ebp - 0x1c], edx
0052b4ff 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0052b502 8945e8                   mov        dword ptr [ebp - 0x18], eax
0052b505 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052b508 894dec                   mov        dword ptr [ebp - 0x14], ecx
0052b50b c745e000000000           mov        dword ptr [ebp - 0x20], 0
0052b512 eb09                     jmp        0x52b51d
0052b514 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052b517 83c201                   add        edx, 1
0052b51a 8955e0                   mov        dword ptr [ebp - 0x20], edx
0052b51d 837de029                 cmp        dword ptr [ebp - 0x20], 0x29
0052b521 0f8dce000000             jge        0x52b5f5
0052b527 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0052b52a 83c024                   add        eax, 0x24
0052b52d 8945cc                   mov        dword ptr [ebp - 0x34], eax
0052b530 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052b533 51                       push       ecx
0052b534 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0052b537 e874fcf3ff               call       0x46b1b0
0052b53c 85c0                     test       eax, eax
0052b53e 7410                     je         0x52b550
0052b540 6aff                     push       -1
0052b542 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052b548 e88355f4ff               call       0x470ad0
0052b54d 90                       nop        
0052b54e eb11                     jmp        0x52b561
0052b550 68707070ff               push       0xff707070
0052b555 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052b55b e87055f4ff               call       0x470ad0
0052b560 90                       nop        
0052b561 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052b564 52                       push       edx
0052b565 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052b56b e800150000               call       0x52ca70
0052b570 85c0                     test       eax, eax
0052b572 741d                     je         0x52b591
0052b574 6a20                     push       0x20
0052b576 6a02                     push       2
0052b578 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0052b57b 83c001                   add        eax, 1
0052b57e 50                       push       eax
0052b57f 8d4df0                   lea        ecx, [ebp - 0x10]
0052b582 51                       push       ecx
0052b583 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052b589 e8b217f4ff               call       0x46cd40
0052b58e 90                       nop        
0052b58f eb1f                     jmp        0x52b5b0
0052b591 83ec08                   sub        esp, 8
0052b594 8bcc                     mov        ecx, esp
0052b596 68ec555700               push       0x5755ec
0052b59b e83028eeff               call       0x40ddd0
0052b5a0 8d55f0                   lea        edx, [ebp - 0x10]
0052b5a3 52                       push       edx
0052b5a4 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052b5aa e8e118f4ff               call       0x46ce90
0052b5af 90                       nop        
0052b5b0 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0052b5b3 99                       cdq        
0052b5b4 b90a000000               mov        ecx, 0xa
0052b5b9 f7f9                     idiv       ecx
0052b5bb 83fa09                   cmp        edx, 9
0052b5be 751e                     jne        0x52b5de
0052b5c0 f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
0052b5c5 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
0052b5ca f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
0052b5cf f30f580554565700         addss      xmm0, dword ptr [0x575654]
0052b5d7 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
0052b5dc eb12                     jmp        0x52b5f0
0052b5de f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
0052b5e3 f30f580558565700         addss      xmm0, dword ptr [0x575658]
0052b5eb f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
0052b5f0 e91fffffff               jmp        0x52b514
0052b5f5 6a00                     push       0
0052b5f7 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052b5fd e8fed4f5ff               call       0x488b00
0052b602 6a00                     push       0
0052b604 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052b60a e811b4fbff               call       0x4e6a20
0052b60f 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0052b612 8b8200590000             mov        eax, dword ptr [edx + 0x5900]
0052b618 50                       push       eax
0052b619 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052b61f e84c140000               call       0x52ca70
0052b624 85c0                     test       eax, eax
0052b626 7409                     je         0x52b631
0052b628 c745d401000000           mov        dword ptr [ebp - 0x2c], 1
0052b62f eb07                     jmp        0x52b638
0052b631 c745d400000000           mov        dword ptr [ebp - 0x2c], 0
0052b638 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0052b63b 894dd8                   mov        dword ptr [ebp - 0x28], ecx
0052b63e 6aff                     push       -1
0052b640 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052b646 e815abf9ff               call       0x4c6160
0052b64b 90                       nop        
0052b64c 837dd800                 cmp        dword ptr [ebp - 0x28], 0
0052b650 7410                     je         0x52b662
0052b652 6aff                     push       -1
0052b654 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052b65a e87154f4ff               call       0x470ad0
0052b65f 90                       nop        
0052b660 eb11                     jmp        0x52b673
0052b662 68808080ff               push       0xff808080
0052b667 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052b66d e85e54f4ff               call       0x470ad0
0052b672 90                       nop        
0052b673 68000000ff               push       0xff000000
0052b678 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052b67e e82d54f4ff               call       0x470ab0
0052b683 6a05                     push       5
0052b685 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052b68b e8f0aaf9ff               call       0x4c6180
0052b690 6a01                     push       1
0052b692 6a00                     push       0
0052b694 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052b69a e881cff8ff               call       0x4b8620
0052b69f 90                       nop        
0052b6a0 837dd800                 cmp        dword ptr [ebp - 0x28], 0
0052b6a4 746c                     je         0x52b712
0052b6a6 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0052b6a9 8b8200590000             mov        eax, dword ptr [edx + 0x5900]
0052b6af 50                       push       eax
0052b6b0 e88b3e0000               call       0x52f540
0052b6b5 83c404                   add        esp, 4
0052b6b8 8945c8                   mov        dword ptr [ebp - 0x38], eax
0052b6bb 51                       push       ecx
0052b6bc 0f57c0                   xorps      xmm0, xmm0
0052b6bf f30f110424               movss      dword ptr [esp], xmm0
0052b6c4 51                       push       ecx
0052b6c5 f30f100588565700         movss      xmm0, dword ptr [0x575688]
0052b6cd f30f110424               movss      dword ptr [esp], xmm0
0052b6d2 51                       push       ecx
0052b6d3 f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
0052b6db f30f110424               movss      dword ptr [esp], xmm0
0052b6e0 8d4d9c                   lea        ecx, [ebp - 0x64]
0052b6e3 e8e876efff               call       0x422dd0
0052b6e8 8945c4                   mov        dword ptr [ebp - 0x3c], eax
0052b6eb 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0052b6ee 51                       push       ecx
0052b6ef 83ec08                   sub        esp, 8
0052b6f2 8bcc                     mov        ecx, esp
0052b6f4 68e4e05600               push       0x56e0e4
0052b6f9 e8d226eeff               call       0x40ddd0
0052b6fe 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
0052b701 52                       push       edx
0052b702 a198065c00               mov        eax, dword ptr [0x5c0698]
0052b707 50                       push       eax
0052b708 e8030bf4ff               call       0x46c210
0052b70d 83c414                   add        esp, 0x14
0052b710 eb52                     jmp        0x52b764
0052b712 51                       push       ecx
0052b713 0f57c0                   xorps      xmm0, xmm0
0052b716 f30f110424               movss      dword ptr [esp], xmm0
0052b71b 51                       push       ecx
0052b71c f30f100588565700         movss      xmm0, dword ptr [0x575688]
0052b724 f30f110424               movss      dword ptr [esp], xmm0
0052b729 51                       push       ecx
0052b72a f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
0052b732 f30f110424               movss      dword ptr [esp], xmm0
0052b737 8d4d90                   lea        ecx, [ebp - 0x70]
0052b73a e89176efff               call       0x422dd0
0052b73f 8945c0                   mov        dword ptr [ebp - 0x40], eax
0052b742 83ec08                   sub        esp, 8
0052b745 8bcc                     mov        ecx, esp
0052b747 68f0555700               push       0x5755f0
0052b74c e87f26eeff               call       0x40ddd0
0052b751 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0052b754 51                       push       ecx
0052b755 8b1598065c00             mov        edx, dword ptr [0x5c0698]
0052b75b 52                       push       edx
0052b75c e8af0af4ff               call       0x46c210
0052b761 83c410                   add        esp, 0x10
0052b764 6a00                     push       0
0052b766 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
0052b769 50                       push       eax
0052b76a 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0052b76d 8b9100590000             mov        edx, dword ptr [ecx + 0x5900]
0052b773 52                       push       edx
0052b774 e8673d0000               call       0x52f4e0
0052b779 83c40c                   add        esp, 0xc
0052b77c 8945bc                   mov        dword ptr [ebp - 0x44], eax
0052b77f 51                       push       ecx
0052b780 0f57c0                   xorps      xmm0, xmm0
0052b783 f30f110424               movss      dword ptr [esp], xmm0
0052b788 51                       push       ecx
0052b789 f30f10058c565700         movss      xmm0, dword ptr [0x57568c]
0052b791 f30f110424               movss      dword ptr [esp], xmm0
0052b796 51                       push       ecx
0052b797 f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
0052b79f f30f110424               movss      dword ptr [esp], xmm0
0052b7a4 8d4d84                   lea        ecx, [ebp - 0x7c]
0052b7a7 e82476efff               call       0x422dd0
0052b7ac 8945b8                   mov        dword ptr [ebp - 0x48], eax
0052b7af 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
0052b7b2 50                       push       eax
0052b7b3 83ec08                   sub        esp, 8
0052b7b6 8bcc                     mov        ecx, esp
0052b7b8 68e4e05600               push       0x56e0e4
0052b7bd e80e26eeff               call       0x40ddd0
0052b7c2 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
0052b7c5 51                       push       ecx
0052b7c6 8b1598065c00             mov        edx, dword ptr [0x5c0698]
0052b7cc 52                       push       edx
0052b7cd e83e0af4ff               call       0x46c210
0052b7d2 83c414                   add        esp, 0x14
0052b7d5 6a01                     push       1
0052b7d7 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
0052b7da 50                       push       eax
0052b7db 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0052b7de 8b9100590000             mov        edx, dword ptr [ecx + 0x5900]
0052b7e4 52                       push       edx
0052b7e5 e8f63c0000               call       0x52f4e0
0052b7ea 83c40c                   add        esp, 0xc
0052b7ed 8945b4                   mov        dword ptr [ebp - 0x4c], eax
0052b7f0 51                       push       ecx
0052b7f1 0f57c0                   xorps      xmm0, xmm0
0052b7f4 f30f110424               movss      dword ptr [esp], xmm0
0052b7f9 51                       push       ecx
0052b7fa f30f100560265700         movss      xmm0, dword ptr [0x572660]
0052b802 f30f110424               movss      dword ptr [esp], xmm0
0052b807 51                       push       ecx
0052b808 f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
0052b810 f30f110424               movss      dword ptr [esp], xmm0
0052b815 8d8d78ffffff             lea        ecx, [ebp - 0x88]
0052b81b e8b075efff               call       0x422dd0
0052b820 8945b0                   mov        dword ptr [ebp - 0x50], eax
0052b823 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
0052b826 50                       push       eax
0052b827 83ec08                   sub        esp, 8
0052b82a 8bcc                     mov        ecx, esp
0052b82c 68e4e05600               push       0x56e0e4
0052b831 e89a25eeff               call       0x40ddd0
0052b836 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0052b839 51                       push       ecx
0052b83a 8b1598065c00             mov        edx, dword ptr [0x5c0698]
0052b840 52                       push       edx
0052b841 e8ca09f4ff               call       0x46c210
0052b846 83c414                   add        esp, 0x14
0052b849 6a02                     push       2
0052b84b 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
0052b84e 50                       push       eax
0052b84f 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0052b852 8b9100590000             mov        edx, dword ptr [ecx + 0x5900]
0052b858 52                       push       edx
0052b859 e8823c0000               call       0x52f4e0
0052b85e 83c40c                   add        esp, 0xc
0052b861 8945ac                   mov        dword ptr [ebp - 0x54], eax
0052b864 51                       push       ecx
0052b865 0f57c0                   xorps      xmm0, xmm0
0052b868 f30f110424               movss      dword ptr [esp], xmm0
0052b86d 51                       push       ecx
0052b86e f30f100594565700         movss      xmm0, dword ptr [0x575694]
0052b876 f30f110424               movss      dword ptr [esp], xmm0
0052b87b 51                       push       ecx
0052b87c f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
0052b884 f30f110424               movss      dword ptr [esp], xmm0
0052b889 8d8d6cffffff             lea        ecx, [ebp - 0x94]
0052b88f e83c75efff               call       0x422dd0
0052b894 8945a8                   mov        dword ptr [ebp - 0x58], eax
0052b897 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
0052b89a 50                       push       eax
0052b89b 83ec08                   sub        esp, 8
0052b89e 8bcc                     mov        ecx, esp
0052b8a0 68e4e05600               push       0x56e0e4
0052b8a5 e82625eeff               call       0x40ddd0
0052b8aa 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0052b8ad 51                       push       ecx
0052b8ae 8b1598065c00             mov        edx, dword ptr [0x5c0698]
0052b8b4 52                       push       edx
0052b8b5 e85609f4ff               call       0x46c210
0052b8ba 83c414                   add        esp, 0x14
0052b8bd 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052b8c3 e818affbff               call       0x4e67e0
0052b8c8 90                       nop        
0052b8c9 b801000000               mov        eax, 1
0052b8ce 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052b8d1 33cd                     xor        ecx, ebp
0052b8d3 e816710100               call       0x5429ee
0052b8d8 8be5                     mov        esp, ebp
0052b8da 5d                       pop        ebp
0052b8db c3                       ret        
0052b8dc cc                       int3       
0052b8dd cc                       int3       
0052b8de cc                       int3       
0052b8df cc                       int3       
