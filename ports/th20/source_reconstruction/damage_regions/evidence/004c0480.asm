004c0480 55                       push       ebp
004c0481 8bec                     mov        ebp, esp
004c0483 6aff                     push       -1
004c0485 680d9a5600               push       0x569a0d
004c048a 64a100000000             mov        eax, dword ptr fs:[0]
004c0490 50                       push       eax
004c0491 81ecc0000000             sub        esp, 0xc0
004c0497 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c049c 33c5                     xor        eax, ebp
004c049e 8945f0                   mov        dword ptr [ebp - 0x10], eax
004c04a1 56                       push       esi
004c04a2 50                       push       eax
004c04a3 8d45f4                   lea        eax, [ebp - 0xc]
004c04a6 64a300000000             mov        dword ptr fs:[0], eax
004c04ac 894dc8                   mov        dword ptr [ebp - 0x38], ecx
004c04af 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004c04b2 8b885cc40000             mov        ecx, dword ptr [eax + 0xc45c]
004c04b8 e8d3faf4ff               call       0x40ff90
004c04bd 8bc8                     mov        ecx, eax
004c04bf e81c080000               call       0x4c0ce0
004c04c4 85c0                     test       eax, eax
004c04c6 7507                     jne        0x4c04cf
004c04c8 33c0                     xor        eax, eax
004c04ca e940060000               jmp        0x4c0b0f
004c04cf c745d400000000           mov        dword ptr [ebp - 0x2c], 0
004c04d6 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004c04d9 8b895cc40000             mov        ecx, dword ptr [ecx + 0xc45c]
004c04df e82c3bf6ff               call       0x424010
004c04e4 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
004c04ea 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004c04ed 52                       push       edx
004c04ee 8b4508                   mov        eax, dword ptr [ebp + 8]
004c04f1 50                       push       eax
004c04f2 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
004c04f8 e8f377fbff               call       0x477cf0
004c04fd 8945cc                   mov        dword ptr [ebp - 0x34], eax
004c0500 837d1800                 cmp        dword ptr [ebp + 0x18], 0
004c0504 7418                     je         0x4c051e
004c0506 8b4d18                   mov        ecx, dword ptr [ebp + 0x18]
004c0509 c70100000000             mov        dword ptr [ecx], 0
004c050f 837dcc00                 cmp        dword ptr [ebp - 0x34], 0
004c0513 7e09                     jle        0x4c051e
004c0515 8b5518                   mov        edx, dword ptr [ebp + 0x18]
004c0518 c70201000000             mov        dword ptr [edx], 1
004c051e 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
004c0521 0345cc                   add        eax, dword ptr [ebp - 0x34]
004c0524 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004c0527 c78560ffffff00000000     mov        dword ptr [ebp - 0xa0], 0
004c0531 c78564ffffff00000000     mov        dword ptr [ebp - 0x9c], 0
004c053b c78568ffffff00000000     mov        dword ptr [ebp - 0x98], 0
004c0545 c7856cffffff00000000     mov        dword ptr [ebp - 0x94], 0
004c054f c78550ffffff00000000     mov        dword ptr [ebp - 0xb0], 0
004c0559 c78554ffffff00000000     mov        dword ptr [ebp - 0xac], 0
004c0563 c78558ffffff00000000     mov        dword ptr [ebp - 0xa8], 0
004c056d c7855cffffff00000000     mov        dword ptr [ebp - 0xa4], 0
004c0577 c78540ffffff00000000     mov        dword ptr [ebp - 0xc0], 0
004c0581 c78544ffffff00000000     mov        dword ptr [ebp - 0xbc], 0
004c058b c78548ffffff00000000     mov        dword ptr [ebp - 0xb8], 0
004c0595 c7854cffffff00000000     mov        dword ptr [ebp - 0xb4], 0
004c059f c745bc00000000           mov        dword ptr [ebp - 0x44], 0
004c05a6 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004c05a9 81c114c40000             add        ecx, 0xc414
004c05af 894dc0                   mov        dword ptr [ebp - 0x40], ecx
004c05b2 6a08                     push       8
004c05b4 8d4de8                   lea        ecx, [ebp - 0x18]
004c05b7 e8c4baf4ff               call       0x40c080
004c05bc 8d55e8                   lea        edx, [ebp - 0x18]
004c05bf 52                       push       edx
004c05c0 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004c05c3 e8b81cf5ff               call       0x412280
004c05c8 c745fc00000000           mov        dword ptr [ebp - 4], 0
004c05cf 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004c05d2 e8691ff5ff               call       0x412540
004c05d7 898570ffffff             mov        dword ptr [ebp - 0x90], eax
004c05dd eb09                     jmp        0x4c05e8
004c05df 8d4de8                   lea        ecx, [ebp - 0x18]
004c05e2 e84916f5ff               call       0x411c30
004c05e7 90                       nop        
004c05e8 8b8570ffffff             mov        eax, dword ptr [ebp - 0x90]
004c05ee 50                       push       eax
004c05ef 8d4de8                   lea        ecx, [ebp - 0x18]
004c05f2 e8b915f5ff               call       0x411bb0
004c05f7 0fb6c8                   movzx      ecx, al
004c05fa 85c9                     test       ecx, ecx
004c05fc 0f84a2040000             je         0x4c0aa4
004c0602 8d4de8                   lea        ecx, [ebp - 0x18]
004c0605 e8f6bcf4ff               call       0x40c300
004c060a 8945d8                   mov        dword ptr [ebp - 0x28], eax
004c060d 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004c0610 83c201                   add        edx, 1
004c0613 8955bc                   mov        dword ptr [ebp - 0x44], edx
004c0616 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c0619 e842ebf4ff               call       0x40f160
004c061e 8b00                     mov        eax, dword ptr [eax]
004c0620 83c07c                   add        eax, 0x7c
004c0623 8945a8                   mov        dword ptr [ebp - 0x58], eax
004c0626 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c0629 e832ebf4ff               call       0x40f160
004c062e 8b08                     mov        ecx, dword ptr [eax]
004c0630 8b91a0000000             mov        edx, dword ptr [ecx + 0xa0]
004c0636 8955ac                   mov        dword ptr [ebp - 0x54], edx
004c0639 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
004c063c 50                       push       eax
004c063d 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004c0640 e8bb87fbff               call       0x478e00
004c0645 85c0                     test       eax, eax
004c0647 7502                     jne        0x4c064b
004c0649 eb94                     jmp        0x4c05df
004c064b 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c064e e80debf4ff               call       0x40f160
004c0653 8b08                     mov        ecx, dword ptr [eax]
004c0655 83b9a800000000           cmp        dword ptr [ecx + 0xa8], 0
004c065c 7e05                     jle        0x4c0663
004c065e e97cffffff               jmp        0x4c05df
004c0663 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c0666 e8f5eaf4ff               call       0x40f160
004c066b 8b10                     mov        edx, dword ptr [eax]
004c066d 83ba9400000000           cmp        dword ptr [edx + 0x94], 0
004c0674 7505                     jne        0x4c067b
004c0676 e964ffffff               jmp        0x4c05df
004c067b 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c067e e8ddeaf4ff               call       0x40f160
004c0683 8b00                     mov        eax, dword ptr [eax]
004c0685 8945a4                   mov        dword ptr [ebp - 0x5c], eax
004c0688 51                       push       ecx
004c0689 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004c068e f30f110424               movss      dword ptr [esp], xmm0
004c0693 51                       push       ecx
004c0694 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004c0699 f30f110424               movss      dword ptr [esp], xmm0
004c069e 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004c06a1 51                       push       ecx
004c06a2 8b5508                   mov        edx, dword ptr [ebp + 8]
004c06a5 52                       push       edx
004c06a6 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
004c06a9 e882090000               call       0x4c1030
004c06ae 0fb6c0                   movzx      eax, al
004c06b1 85c0                     test       eax, eax
004c06b3 7505                     jne        0x4c06ba
004c06b5 e925ffffff               jmp        0x4c05df
004c06ba 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c06bd e89eeaf4ff               call       0x40f160
004c06c2 8b08                     mov        ecx, dword ptr [eax]
004c06c4 83b9ac00000000           cmp        dword ptr [ecx + 0xac], 0
004c06cb 0f8ec4000000             jle        0x4c0795
004c06d1 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c06d4 e887eaf4ff               call       0x40f160
004c06d9 8b10                     mov        edx, dword ptr [eax]
004c06db 83baac00000004           cmp        dword ptr [edx + 0xac], 4
004c06e2 0f8fad000000             jg         0x4c0795
004c06e8 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c06eb e870eaf4ff               call       0x40f160
004c06f0 8b00                     mov        eax, dword ptr [eax]
004c06f2 8b88ac000000             mov        ecx, dword ptr [eax + 0xac]
004c06f8 83bc8d5cffffff00         cmp        dword ptr [ebp + ecx*4 - 0xa4], 0
004c0700 742e                     je         0x4c0730
004c0702 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c0705 e856eaf4ff               call       0x40f160
004c070a 8b10                     mov        edx, dword ptr [eax]
004c070c 8bb2ac000000             mov        esi, dword ptr [edx + 0xac]
004c0712 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c0715 e846eaf4ff               call       0x40f160
004c071a 8b00                     mov        eax, dword ptr [eax]
004c071c 8b8cb54cffffff           mov        ecx, dword ptr [ebp + esi*4 - 0xb4]
004c0723 3b8894000000             cmp        ecx, dword ptr [eax + 0x94]
004c0729 7e05                     jle        0x4c0730
004c072b e9affeffff               jmp        0x4c05df
004c0730 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c0733 e828eaf4ff               call       0x40f160
004c0738 8b10                     mov        edx, dword ptr [eax]
004c073a 8b82ac000000             mov        eax, dword ptr [edx + 0xac]
004c0740 c784855cffffff01000000   mov        dword ptr [ebp + eax*4 - 0xa4], 1
004c074b 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c074e e80deaf4ff               call       0x40f160
004c0753 8b08                     mov        ecx, dword ptr [eax]
004c0755 8b91ac000000             mov        edx, dword ptr [ecx + 0xac]
004c075b 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
004c075e 2b84953cffffff           sub        eax, dword ptr [ebp + edx*4 - 0xc4]
004c0765 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004c0768 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c076b e8f0e9f4ff               call       0x40f160
004c0770 8b08                     mov        ecx, dword ptr [eax]
004c0772 8b9194000000             mov        edx, dword ptr [ecx + 0x94]
004c0778 8955a0                   mov        dword ptr [ebp - 0x60], edx
004c077b 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c077e e8dde9f4ff               call       0x40f160
004c0783 8b00                     mov        eax, dword ptr [eax]
004c0785 8b88ac000000             mov        ecx, dword ptr [eax + 0xac]
004c078b 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
004c078e 89948d4cffffff           mov        dword ptr [ebp + ecx*4 - 0xb4], edx
004c0795 8d4d24                   lea        ecx, [ebp + 0x24]
004c0798 e863bbf4ff               call       0x40c300
004c079d 85c0                     test       eax, eax
004c079f 0f8487000000             je         0x4c082c
004c07a5 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c07a8 e8b3e9f4ff               call       0x40f160
004c07ad 8b08                     mov        ecx, dword ptr [eax]
004c07af 81c1a4000000             add        ecx, 0xa4
004c07b5 e846bbf4ff               call       0x40c300
004c07ba 8bf0                     mov        esi, eax
004c07bc 8d4d24                   lea        ecx, [ebp + 0x24]
004c07bf e83cbbf4ff               call       0x40c300
004c07c4 3bf0                     cmp        esi, eax
004c07c6 7505                     jne        0x4c07cd
004c07c8 e912feffff               jmp        0x4c05df
004c07cd 8d4d24                   lea        ecx, [ebp + 0x24]
004c07d0 e82bbbf4ff               call       0x40c300
004c07d5 89459c                   mov        dword ptr [ebp - 0x64], eax
004c07d8 8b459c                   mov        eax, dword ptr [ebp - 0x64]
004c07db 50                       push       eax
004c07dc 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c07df e87ce9f4ff               call       0x40f160
004c07e4 8b08                     mov        ecx, dword ptr [eax]
004c07e6 81c1a4000000             add        ecx, 0xa4
004c07ec e8af0ff5ff               call       0x4117a0
004c07f1 8d4d24                   lea        ecx, [ebp + 0x24]
004c07f4 e8c7a2feff               call       0x4aaac0
004c07f9 8945b8                   mov        dword ptr [ebp - 0x48], eax
004c07fc 837db800                 cmp        dword ptr [ebp - 0x48], 0
004c0800 742a                     je         0x4c082c
004c0802 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c0805 e856e9f4ff               call       0x40f160
004c080a 8b08                     mov        ecx, dword ptr [eax]
004c080c 8b5114                   mov        edx, dword ptr [ecx + 0x14]
004c080f c1ea05                   shr        edx, 5
004c0812 83e201                   and        edx, 1
004c0815 7415                     je         0x4c082c
004c0817 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004c081a e8b1a3feff               call       0x4aabd0
004c081f 8bc8                     mov        ecx, eax
004c0821 e86a160000               call       0x4c1e90
004c0826 90                       nop        
004c0827 e9b3fdffff               jmp        0x4c05df
004c082c 837d1800                 cmp        dword ptr [ebp + 0x18], 0
004c0830 741e                     je         0x4c0850
004c0832 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c0835 e826e9f4ff               call       0x40f160
004c083a 8b00                     mov        eax, dword ptr [eax]
004c083c 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004c083f c1e904                   shr        ecx, 4
004c0842 83e101                   and        ecx, 1
004c0845 7409                     je         0x4c0850
004c0847 8b5518                   mov        edx, dword ptr [ebp + 0x18]
004c084a c70201000000             mov        dword ptr [edx], 1
004c0850 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c0853 e808e9f4ff               call       0x40f160
004c0858 8b00                     mov        eax, dword ptr [eax]
004c085a 8945d0                   mov        dword ptr [ebp - 0x30], eax
004c085d 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c0860 e8fbe8f4ff               call       0x40f160
004c0865 8b08                     mov        ecx, dword ptr [eax]
004c0867 81b99c0000007f969800     cmp        dword ptr [ecx + 0x9c], 0x98967f
004c0871 7d32                     jge        0x4c08a5
004c0873 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c0876 e8e5e8f4ff               call       0x40f160
004c087b 8b30                     mov        esi, dword ptr [eax]
004c087d 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c0880 e8dbe8f4ff               call       0x40f160
004c0885 8b10                     mov        edx, dword ptr [eax]
004c0887 8b869c000000             mov        eax, dword ptr [esi + 0x9c]
004c088d 3b8298000000             cmp        eax, dword ptr [edx + 0x98]
004c0893 7f10                     jg         0x4c08a5
004c0895 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c0898 e8c3e8f4ff               call       0x40f160
004c089d 8b08                     mov        ecx, dword ptr [eax]
004c089f e88c160000               call       0x4c1f30
004c08a4 90                       nop        
004c08a5 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004c08a8 8b9194000000             mov        edx, dword ptr [ecx + 0x94]
004c08ae 8955cc                   mov        dword ptr [ebp - 0x34], edx
004c08b1 837d2000                 cmp        dword ptr [ebp + 0x20], 0
004c08b5 0f859d000000             jne        0x4c0958
004c08bb 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c08be e89de8f4ff               call       0x40f160
004c08c3 8b00                     mov        eax, dword ptr [eax]
004c08c5 8b8894000000             mov        ecx, dword ptr [eax + 0x94]
004c08cb 894d94                   mov        dword ptr [ebp - 0x6c], ecx
004c08ce 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004c08d1 8b8298000000             mov        eax, dword ptr [edx + 0x98]
004c08d7 894598                   mov        dword ptr [ebp - 0x68], eax
004c08da 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004c08dd 034d94                   add        ecx, dword ptr [ebp - 0x6c]
004c08e0 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004c08e3 898a98000000             mov        dword ptr [edx + 0x98], ecx
004c08e9 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004c08ec 83b8b400000000           cmp        dword ptr [eax + 0xb4], 0
004c08f3 7463                     je         0x4c0958
004c08f5 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004c08f8 8b5524                   mov        edx, dword ptr [ebp + 0x24]
004c08fb 8991b0000000             mov        dword ptr [ecx + 0xb0], edx
004c0901 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004c0904 8b88b4000000             mov        ecx, dword ptr [eax + 0xb4]
004c090a 8b148d780c5700           mov        edx, dword ptr [ecx*4 + 0x570c78]
004c0911 89558c                   mov        dword ptr [ebp - 0x74], edx
004c0914 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c0917 e844e8f4ff               call       0x40f160
004c091c 8b00                     mov        eax, dword ptr [eax]
004c091e 894590                   mov        dword ptr [ebp - 0x70], eax
004c0921 51                       push       ecx
004c0922 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004c0927 f30f110424               movss      dword ptr [esp], xmm0
004c092c 51                       push       ecx
004c092d f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004c0932 f30f110424               movss      dword ptr [esp], xmm0
004c0937 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004c093a 51                       push       ecx
004c093b 8b5508                   mov        edx, dword ptr [ebp + 8]
004c093e 52                       push       edx
004c093f 8b4590                   mov        eax, dword ptr [ebp - 0x70]
004c0942 50                       push       eax
004c0943 ff558c                   call       dword ptr [ebp - 0x74]
004c0946 83c414                   add        esp, 0x14
004c0949 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004c094c 837db400                 cmp        dword ptr [ebp - 0x4c], 0
004c0950 7c06                     jl         0x4c0958
004c0952 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004c0955 894dcc                   mov        dword ptr [ebp - 0x34], ecx
004c0958 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004c095b 83baac00000000           cmp        dword ptr [edx + 0xac], 0
004c0962 7e1f                     jle        0x4c0983
004c0964 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004c0967 83b8ac00000004           cmp        dword ptr [eax + 0xac], 4
004c096e 7f13                     jg         0x4c0983
004c0970 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004c0973 8b91ac000000             mov        edx, dword ptr [ecx + 0xac]
004c0979 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
004c097c 8984953cffffff           mov        dword ptr [ebp + edx*4 - 0xc4], eax
004c0983 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004c0986 034dcc                   add        ecx, dword ptr [ebp - 0x34]
004c0989 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
004c098c 837d1c00                 cmp        dword ptr [ebp + 0x1c], 0
004c0990 742b                     je         0x4c09bd
004c0992 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004c0995 83c234                   add        edx, 0x34
004c0998 895588                   mov        dword ptr [ebp - 0x78], edx
004c099b 8d8534ffffff             lea        eax, [ebp - 0xcc]
004c09a1 50                       push       eax
004c09a2 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004c09a5 e8c6b3fcff               call       0x48bd70
004c09aa 8b4d1c                   mov        ecx, dword ptr [ebp + 0x1c]
004c09ad 8b10                     mov        edx, dword ptr [eax]
004c09af 8911                     mov        dword ptr [ecx], edx
004c09b1 8b5004                   mov        edx, dword ptr [eax + 4]
004c09b4 895104                   mov        dword ptr [ecx + 4], edx
004c09b7 8b4008                   mov        eax, dword ptr [eax + 8]
004c09ba 894108                   mov        dword ptr [ecx + 8], eax
004c09bd 837dd400                 cmp        dword ptr [ebp - 0x2c], 0
004c09c1 0f8ed8000000             jle        0x4c0a9f
004c09c7 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004c09ca 8b895cc40000             mov        ecx, dword ptr [ecx + 0xc45c]
004c09d0 e85b1df5ff               call       0x412730
004c09d5 894580                   mov        dword ptr [ebp - 0x80], eax
004c09d8 8d4d24                   lea        ecx, [ebp + 0x24]
004c09db e820b9f4ff               call       0x40c300
004c09e0 894584                   mov        dword ptr [ebp - 0x7c], eax
004c09e3 8b5584                   mov        edx, dword ptr [ebp - 0x7c]
004c09e6 52                       push       edx
004c09e7 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004c09ea e89180fdff               call       0x498a80
004c09ef 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004c09f2 837dc400                 cmp        dword ptr [ebp - 0x3c], 0
004c09f6 0f84a3000000             je         0x4c0a9f
004c09fc 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004c09ff 83c134                   add        ecx, 0x34
004c0a02 e899b3f4ff               call       0x40bda0
004c0a07 89857cffffff             mov        dword ptr [ebp - 0x84], eax
004c0a0d 8b4508                   mov        eax, dword ptr [ebp + 8]
004c0a10 50                       push       eax
004c0a11 8d4ddc                   lea        ecx, [ebp - 0x24]
004c0a14 51                       push       ecx
004c0a15 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004c0a1b e8208af6ff               call       0x429440
004c0a20 51                       push       ecx
004c0a21 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
004c0a29 f30f110424               movss      dword ptr [esp], xmm0
004c0a2e 8d4ddc                   lea        ecx, [ebp - 0x24]
004c0a31 e80af8ffff               call       0x4c0240
004c0a36 8b5508                   mov        edx, dword ptr [ebp + 8]
004c0a39 52                       push       edx
004c0a3a 8d4ddc                   lea        ecx, [ebp - 0x24]
004c0a3d e89e8cf6ff               call       0x4296e0
004c0a42 6a00                     push       0
004c0a44 e8e737faff               call       0x464230
004c0a49 83c404                   add        esp, 4
004c0a4c 898578ffffff             mov        dword ptr [ebp - 0x88], eax
004c0a52 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004c0a55 e866100000               call       0x4c1ac0
004c0a5a 0fb6c0                   movzx      eax, al
004c0a5d 85c0                     test       eax, eax
004c0a5f 7508                     jne        0x4c0a69
004c0a61 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004c0a64 894db0                   mov        dword ptr [ebp - 0x50], ecx
004c0a67 eb07                     jmp        0x4c0a70
004c0a69 6b55d40a                 imul       edx, dword ptr [ebp - 0x2c], 0xa
004c0a6d 8955b0                   mov        dword ptr [ebp - 0x50], edx
004c0a70 6a0d                     push       0xd
004c0a72 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
004c0a75 50                       push       eax
004c0a76 8d4ddc                   lea        ecx, [ebp - 0x24]
004c0a79 51                       push       ecx
004c0a7a 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
004c0a80 e8fb94feff               call       0x4a9f80
004c0a85 90                       nop        
004c0a86 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004c0a89 8b4214                   mov        eax, dword ptr [edx + 0x14]
004c0a8c c1e806                   shr        eax, 6
004c0a8f 83e001                   and        eax, 1
004c0a92 740b                     je         0x4c0a9f
004c0a94 6a01                     push       1
004c0a96 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004c0a99 e822150000               call       0x4c1fc0
004c0a9e 90                       nop        
004c0a9f e93bfbffff               jmp        0x4c05df
004c0aa4 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004c0aab 8d4de8                   lea        ecx, [ebp - 0x18]
004c0aae e84d10f5ff               call       0x411b00
004c0ab3 90                       nop        
004c0ab4 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004c0ab7 8b895cc40000             mov        ecx, dword ptr [ecx + 0xc45c]
004c0abd e8cef4f4ff               call       0x40ff90
004c0ac2 8bc8                     mov        ecx, eax
004c0ac4 e817ea0300               call       0x4ff4e0
004c0ac9 3945d4                   cmp        dword ptr [ebp - 0x2c], eax
004c0acc 7e18                     jle        0x4c0ae6
004c0ace 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004c0ad1 8b8a5cc40000             mov        ecx, dword ptr [edx + 0xc45c]
004c0ad7 e8b4f4f4ff               call       0x40ff90
004c0adc 8bc8                     mov        ecx, eax
004c0ade e8fde90300               call       0x4ff4e0
004c0ae3 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004c0ae6 837d2000                 cmp        dword ptr [ebp + 0x20], 0
004c0aea 7520                     jne        0x4c0b0c
004c0aec 837dd400                 cmp        dword ptr [ebp - 0x2c], 0
004c0af0 741a                     je         0x4c0b0c
004c0af2 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
004c0af5 99                       cdq        
004c0af6 b90a000000               mov        ecx, 0xa
004c0afb f7f9                     idiv       ecx
004c0afd 83c00a                   add        eax, 0xa
004c0b00 50                       push       eax
004c0b01 b968a55b00               mov        ecx, 0x5ba568
004c0b06 e8457afcff               call       0x488550
004c0b0b 90                       nop        
004c0b0c 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
004c0b0f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c0b12 64890d00000000           mov        dword ptr fs:[0], ecx
004c0b19 59                       pop        ecx
004c0b1a 5e                       pop        esi
004c0b1b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c0b1e 33cd                     xor        ecx, ebp
004c0b20 e8c91e0800               call       0x5429ee
004c0b25 8be5                     mov        esp, ebp
004c0b27 5d                       pop        ebp
004c0b28 c22000                   ret        0x20
004c0b2b cc                       int3       
004c0b2c cc                       int3       
004c0b2d cc                       int3       
004c0b2e cc                       int3       
004c0b2f cc                       int3       
