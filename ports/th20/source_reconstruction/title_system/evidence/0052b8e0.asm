0052b8e0 55                       push       ebp
0052b8e1 8bec                     mov        ebp, esp
0052b8e3 81eca0000000             sub        esp, 0xa0
0052b8e9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052b8ee 33c5                     xor        eax, ebp
0052b8f0 8945fc                   mov        dword ptr [ebp - 4], eax
0052b8f3 894de0                   mov        dword ptr [ebp - 0x20], ecx
0052b8f6 b968a55b00               mov        ecx, 0x5ba568
0052b8fb e8a087f3ff               call       0x4640a0
0052b900 83f804                   cmp        eax, 4
0052b903 7509                     jne        0x52b90e
0052b905 c745d00e000000           mov        dword ptr [ebp - 0x30], 0xe
0052b90c eb07                     jmp        0x52b915
0052b90e c745d00c000000           mov        dword ptr [ebp - 0x30], 0xc
0052b915 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
0052b918 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0052b91b c745dc0d000000           mov        dword ptr [ebp - 0x24], 0xd
0052b922 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052b925 8b5120                   mov        edx, dword ptr [ecx + 0x20]
0052b928 8955cc                   mov        dword ptr [ebp - 0x34], edx
0052b92b 837dcc04                 cmp        dword ptr [ebp - 0x34], 4
0052b92f 0f87ca060000             ja         0x52bfff
0052b935 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
0052b938 ff248514c05200           jmp        dword ptr [eax*4 + 0x52c014]
0052b93f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052b942 83c124                   add        ecx, 0x24
0052b945 894d88                   mov        dword ptr [ebp - 0x78], ecx
0052b948 6a03                     push       3
0052b94a 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
0052b94d e88e0bf7ff               call       0x49c4e0
0052b952 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0052b955 52                       push       edx
0052b956 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052b959 e822160000               call       0x52cf80
0052b95e 90                       nop        
0052b95f c745ec00000000           mov        dword ptr [ebp - 0x14], 0
0052b966 8d45ec                   lea        eax, [ebp - 0x14]
0052b969 8945c8                   mov        dword ptr [ebp - 0x38], eax
0052b96c 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0052b96f 8b11                     mov        edx, dword ptr [ecx]
0052b971 52                       push       edx
0052b972 8d45f8                   lea        eax, [ebp - 8]
0052b975 50                       push       eax
0052b976 e8955bf3ff               call       0x461510
0052b97b 83c408                   add        esp, 8
0052b97e 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0052b981 8b11                     mov        edx, dword ptr [ecx]
0052b983 52                       push       edx
0052b984 8d45f0                   lea        eax, [ebp - 0x10]
0052b987 50                       push       eax
0052b988 e8f3f4fdff               call       0x50ae80
0052b98d 83c408                   add        esp, 8
0052b990 eb09                     jmp        0x52b99b
0052b992 8d4df8                   lea        ecx, [ebp - 8]
0052b995 e8c65ef3ff               call       0x461860
0052b99a 90                       nop        
0052b99b 8d4df0                   lea        ecx, [ebp - 0x10]
0052b99e 51                       push       ecx
0052b99f 8d4df8                   lea        ecx, [ebp - 8]
0052b9a2 e8b9daf1ff               call       0x449460
0052b9a7 0fb6d0                   movzx      edx, al
0052b9aa 85d2                     test       edx, edx
0052b9ac 7464                     je         0x52ba12
0052b9ae 8d4df8                   lea        ecx, [ebp - 8]
0052b9b1 e84a09eeff               call       0x40c300
0052b9b6 8945c4                   mov        dword ptr [ebp - 0x3c], eax
0052b9b9 b968a55b00               mov        ecx, 0x5ba568
0052b9be e8adc5f8ff               call       0x4b7f70
0052b9c3 894584                   mov        dword ptr [ebp - 0x7c], eax
0052b9c6 b968a55b00               mov        ecx, 0x5ba568
0052b9cb e8d086f3ff               call       0x4640a0
0052b9d0 8945b8                   mov        dword ptr [ebp - 0x48], eax
0052b9d3 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
0052b9d6 50                       push       eax
0052b9d7 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
0052b9da 51                       push       ecx
0052b9db 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
0052b9de 52                       push       edx
0052b9df 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052b9e5 e8560a0000               call       0x52c440
0052b9ea 85c0                     test       eax, eax
0052b9ec 7522                     jne        0x52ba10
0052b9ee 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
0052b9f1 83c016                   add        eax, 0x16
0052b9f4 50                       push       eax
0052b9f5 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0052b9f8 51                       push       ecx
0052b9f9 8d9574ffffff             lea        edx, [ebp - 0x8c]
0052b9ff 52                       push       edx
0052ba00 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052ba03 e838080000               call       0x52c240
0052ba08 8bc8                     mov        ecx, eax
0052ba0a e81147f2ff               call       0x450120
0052ba0f 90                       nop        
0052ba10 eb80                     jmp        0x52b992
0052ba12 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0052ba15 c78060590000ffffffff     mov        dword ptr [eax + 0x5960], 0xffffffff
0052ba1f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052ba22 c78164590000ffffffff     mov        dword ptr [ecx + 0x5964], 0xffffffff
0052ba2c 833d28615c0004           cmp        dword ptr [0x5c6128], 4
0052ba33 7409                     je         0x52ba3e
0052ba35 833d28615c0005           cmp        dword ptr [0x5c6128], 5
0052ba3c 750a                     jne        0x52ba48
0052ba3e e942030000               jmp        0x52bd85
0052ba43 e93d030000               jmp        0x52bd85
0052ba48 e813fffeff               call       0x51b960
0052ba4d 8945b4                   mov        dword ptr [ebp - 0x4c], eax
0052ba50 6a00                     push       0
0052ba52 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
0052ba55 e856e6feff               call       0x51a0b0
0052ba5a 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052ba5d c7422001000000           mov        dword ptr [edx + 0x20], 1
0052ba64 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0052ba67 0554010000               add        eax, 0x154
0052ba6c 8945b0                   mov        dword ptr [ebp - 0x50], eax
0052ba6f 6a1e                     push       0x1e
0052ba71 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0052ba74 e8f755f3ff               call       0x461070
0052ba79 0fb6c8                   movzx      ecx, al
0052ba7c 85c9                     test       ecx, ecx
0052ba7e 7415                     je         0x52ba95
0052ba80 6a24                     push       0x24
0052ba82 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052ba85 e8f6140000               call       0x52cf80
0052ba8a 6a02                     push       2
0052ba8c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052ba8f e8bc130000               call       0x52ce50
0052ba94 90                       nop        
0052ba95 e965050000               jmp        0x52bfff
0052ba9a e8c1fefeff               call       0x51b960
0052ba9f 8bc8                     mov        ecx, eax
0052baa1 e8ea00ffff               call       0x51bb90
0052baa6 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052baa9 3b8264590000             cmp        eax, dword ptr [edx + 0x5964]
0052baaf 0f8483000000             je         0x52bb38
0052bab5 e8a6fefeff               call       0x51b960
0052baba 8bc8                     mov        ecx, eax
0052babc e8cf00ffff               call       0x51bb90
0052bac1 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bac4 898164590000             mov        dword ptr [ecx + 0x5964], eax
0052baca e891fefeff               call       0x51b960
0052bacf 8bc8                     mov        ecx, eax
0052bad1 e8ba00ffff               call       0x51bb90
0052bad6 8945d8                   mov        dword ptr [ebp - 0x28], eax
0052bad9 837dd800                 cmp        dword ptr [ebp - 0x28], 0
0052badd 744c                     je         0x52bb2b
0052badf 837dd801                 cmp        dword ptr [ebp - 0x28], 1
0052bae3 7427                     je         0x52bb0c
0052bae5 837dd802                 cmp        dword ptr [ebp - 0x28], 2
0052bae9 7402                     je         0x52baed
0052baeb eb4b                     jmp        0x52bb38
0052baed 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052baf0 8b8260590000             mov        eax, dword ptr [edx + 0x5960]
0052baf6 83c025                   add        eax, 0x25
0052baf9 0fbfc8                   movsx      ecx, ax
0052bafc 51                       push       ecx
0052bafd 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0052bb00 52                       push       edx
0052bb01 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bb04 e807120000               call       0x52cd10
0052bb09 90                       nop        
0052bb0a eb2c                     jmp        0x52bb38
0052bb0c 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0052bb0f 8b8860590000             mov        ecx, dword ptr [eax + 0x5960]
0052bb15 83c107                   add        ecx, 7
0052bb18 0fbfd1                   movsx      edx, cx
0052bb1b 52                       push       edx
0052bb1c 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0052bb1f 50                       push       eax
0052bb20 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bb23 e8e8110000               call       0x52cd10
0052bb28 90                       nop        
0052bb29 eb0d                     jmp        0x52bb38
0052bb2b 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0052bb2e 51                       push       ecx
0052bb2f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bb32 e859110000               call       0x52cc90
0052bb37 90                       nop        
0052bb38 6a00                     push       0
0052bb3a e84185f3ff               call       0x464080
0052bb3f 83c404                   add        esp, 4
0052bb42 8bc8                     mov        ecx, eax
0052bb44 e8670fefff               call       0x41cab0
0052bb49 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052bb4c 3b8260590000             cmp        eax, dword ptr [edx + 0x5960]
0052bb52 0f8405010000             je         0x52bc5d
0052bb58 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0052bb5b 50                       push       eax
0052bb5c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bb5f e82c110000               call       0x52cc90
0052bb64 90                       nop        
0052bb65 e8f6fdfeff               call       0x51b960
0052bb6a 8bc8                     mov        ecx, eax
0052bb6c e81f00ffff               call       0x51bb90
0052bb71 85c0                     test       eax, eax
0052bb73 7429                     je         0x52bb9e
0052bb75 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bb78 83b96059000000           cmp        dword ptr [ecx + 0x5960], 0
0052bb7f 7e1d                     jle        0x52bb9e
0052bb81 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052bb84 8b8260590000             mov        eax, dword ptr [edx + 0x5960]
0052bb8a 83c011                   add        eax, 0x11
0052bb8d 0fbfc8                   movsx      ecx, ax
0052bb90 51                       push       ecx
0052bb91 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0052bb94 52                       push       edx
0052bb95 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bb98 e873110000               call       0x52cd10
0052bb9d 90                       nop        
0052bb9e 6a00                     push       0
0052bba0 e8db84f3ff               call       0x464080
0052bba5 83c404                   add        esp, 4
0052bba8 8bc8                     mov        ecx, eax
0052bbaa e8010fefff               call       0x41cab0
0052bbaf 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bbb2 898160590000             mov        dword ptr [ecx + 0x5960], eax
0052bbb8 e8a3fdfeff               call       0x51b960
0052bbbd 8bc8                     mov        ecx, eax
0052bbbf e8ccfffeff               call       0x51bb90
0052bbc4 8945c0                   mov        dword ptr [ebp - 0x40], eax
0052bbc7 837dc001                 cmp        dword ptr [ebp - 0x40], 1
0052bbcb 7427                     je         0x52bbf4
0052bbcd 837dc002                 cmp        dword ptr [ebp - 0x40], 2
0052bbd1 7402                     je         0x52bbd5
0052bbd3 eb3c                     jmp        0x52bc11
0052bbd5 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052bbd8 8b8260590000             mov        eax, dword ptr [edx + 0x5960]
0052bbde 83c025                   add        eax, 0x25
0052bbe1 0fbfc8                   movsx      ecx, ax
0052bbe4 51                       push       ecx
0052bbe5 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0052bbe8 52                       push       edx
0052bbe9 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bbec e81f110000               call       0x52cd10
0052bbf1 90                       nop        
0052bbf2 eb1d                     jmp        0x52bc11
0052bbf4 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0052bbf7 8b8860590000             mov        ecx, dword ptr [eax + 0x5960]
0052bbfd 83c107                   add        ecx, 7
0052bc00 0fbfd1                   movsx      edx, cx
0052bc03 52                       push       edx
0052bc04 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0052bc07 50                       push       eax
0052bc08 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bc0b e800110000               call       0x52cd10
0052bc10 90                       nop        
0052bc11 6a00                     push       0
0052bc13 e86884f3ff               call       0x464080
0052bc18 83c404                   add        esp, 4
0052bc1b 8bc8                     mov        ecx, eax
0052bc1d e80e6beeff               call       0x412730
0052bc22 83c006                   add        eax, 6
0052bc25 50                       push       eax
0052bc26 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0052bc29 51                       push       ecx
0052bc2a 8d9570ffffff             lea        edx, [ebp - 0x90]
0052bc30 52                       push       edx
0052bc31 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bc34 e807060000               call       0x52c240
0052bc39 8945a8                   mov        dword ptr [ebp - 0x58], eax
0052bc3c 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0052bc3f 8b8060590000             mov        eax, dword ptr [eax + 0x5960]
0052bc45 99                       cdq        
0052bc46 2bc2                     sub        eax, edx
0052bc48 d1f8                     sar        eax, 1
0052bc4a 83c01e                   add        eax, 0x1e
0052bc4d 8945ac                   mov        dword ptr [ebp - 0x54], eax
0052bc50 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
0052bc53 51                       push       ecx
0052bc54 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0052bc57 e8540affff               call       0x51c6b0
0052bc5c 90                       nop        
0052bc5d e8fefcfeff               call       0x51b960
0052bc62 8bc8                     mov        ecx, eax
0052bc64 e8570d0000               call       0x52c9c0
0052bc69 0fb6d0                   movzx      edx, al
0052bc6c 85d2                     test       edx, edx
0052bc6e 7410                     je         0x52bc80
0052bc70 6a03                     push       3
0052bc72 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bc75 e8d6110000               call       0x52ce50
0052bc7a 90                       nop        
0052bc7b e97f030000               jmp        0x52bfff
0052bc80 e8dbfcfeff               call       0x51b960
0052bc85 8bc8                     mov        ecx, eax
0052bc87 e8740a0000               call       0x52c700
0052bc8c 0fb6c0                   movzx      eax, al
0052bc8f 85c0                     test       eax, eax
0052bc91 750b                     jne        0x52bc9e
0052bc93 6a04                     push       4
0052bc95 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bc98 e8b3110000               call       0x52ce50
0052bc9d 90                       nop        
0052bc9e e95c030000               jmp        0x52bfff
0052bca3 6a00                     push       0
0052bca5 e876b8f0ff               call       0x437520
0052bcaa 83c404                   add        esp, 4
0052bcad 8bc8                     mov        ecx, eax
0052bcaf e86cc8f0ff               call       0x438520
0052bcb4 85c0                     test       eax, eax
0052bcb6 7520                     jne        0x52bcd8
0052bcb8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bcbb 81c154010000             add        ecx, 0x154
0052bcc1 894da4                   mov        dword ptr [ebp - 0x5c], ecx
0052bcc4 6a00                     push       0
0052bcc6 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
0052bcc9 e852d7efff               call       0x429420
0052bcce b801000000               mov        eax, 1
0052bcd3 e92c030000               jmp        0x52c004
0052bcd8 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052bcdb 81c254010000             add        edx, 0x154
0052bce1 8955a0                   mov        dword ptr [ebp - 0x60], edx
0052bce4 6a0a                     push       0xa
0052bce6 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
0052bce9 e8f2c8f4ff               call       0x4785e0
0052bcee 0fb6c0                   movzx      eax, al
0052bcf1 85c0                     test       eax, eax
0052bcf3 0f8418010000             je         0x52be11
0052bcf9 b968a55b00               mov        ecx, 0x5ba568
0052bcfe e81d1bf9ff               call       0x4bd820
0052bd03 0fb6c8                   movzx      ecx, al
0052bd06 85c9                     test       ecx, ecx
0052bd08 757b                     jne        0x52bd85
0052bd0a 51                       push       ecx
0052bd0b f30f100588035700         movss      xmm0, dword ptr [0x570388]
0052bd13 f30f110424               movss      dword ptr [esp], xmm0
0052bd18 51                       push       ecx
0052bd19 f30f1005a8cd5600         movss      xmm0, dword ptr [0x56cda8]
0052bd21 f30f110424               movss      dword ptr [esp], xmm0
0052bd26 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052bd2c e86f4df7ff               call       0x4a0aa0
0052bd31 90                       nop        
0052bd32 6a00                     push       0
0052bd34 e8e7b7f0ff               call       0x437520
0052bd39 83c404                   add        esp, 4
0052bd3c 89459c                   mov        dword ptr [ebp - 0x64], eax
0052bd3f 6a00                     push       0
0052bd41 6a00                     push       0
0052bd43 6a00                     push       0
0052bd45 8d956cffffff             lea        edx, [ebp - 0x94]
0052bd4b 52                       push       edx
0052bd4c 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
0052bd4f e89c1ff7ff               call       0x49dcf0
0052bd54 8b00                     mov        eax, dword ptr [eax]
0052bd56 a3044f5c00               mov        dword ptr [0x5c4f04], eax
0052bd5b 6a00                     push       0
0052bd5d b9044f5c00               mov        ecx, 0x5c4f04
0052bd62 e84909ffff               call       0x51c6b0
0052bd67 51                       push       ecx
0052bd68 f30f10055cfe5600         movss      xmm0, dword ptr [0x56fe5c]
0052bd70 f30f110424               movss      dword ptr [esp], xmm0
0052bd75 b9404d5c00               mov        ecx, 0x5c4d40
0052bd7a e851dcfaff               call       0x4d99d0
0052bd7f 90                       nop        
0052bd80 e98c000000               jmp        0x52be11
0052bd85 6a00                     push       0
0052bd87 e8f482f3ff               call       0x464080
0052bd8c 83c404                   add        esp, 4
0052bd8f 8bc8                     mov        ecx, eax
0052bd91 e89a69eeff               call       0x412730
0052bd96 83c006                   add        eax, 6
0052bd99 50                       push       eax
0052bd9a 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0052bd9d 51                       push       ecx
0052bd9e 8d9568ffffff             lea        edx, [ebp - 0x98]
0052bda4 52                       push       edx
0052bda5 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bda8 e893040000               call       0x52c240
0052bdad 894594                   mov        dword ptr [ebp - 0x6c], eax
0052bdb0 b968a55b00               mov        ecx, 0x5ba568
0052bdb5 e886c5f8ff               call       0x4b8340
0052bdba 99                       cdq        
0052bdbb 2bc2                     sub        eax, edx
0052bdbd d1f8                     sar        eax, 1
0052bdbf 83c01e                   add        eax, 0x1e
0052bdc2 894598                   mov        dword ptr [ebp - 0x68], eax
0052bdc5 8b4598                   mov        eax, dword ptr [ebp - 0x68]
0052bdc8 50                       push       eax
0052bdc9 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
0052bdcc e8df08ffff               call       0x51c6b0
0052bdd1 90                       nop        
0052bdd2 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bdd5 83c124                   add        ecx, 0x24
0052bdd8 e8d3a2f9ff               call       0x4c60b0
0052bddd 6a24                     push       0x24
0052bddf 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bde2 e8490e0000               call       0x52cc30
0052bde7 90                       nop        
0052bde8 b968a55b00               mov        ecx, 0x5ba568
0052bded e85ecaf5ff               call       0x488850
0052bdf2 0fb6c8                   movzx      ecx, al
0052bdf5 85c9                     test       ecx, ecx
0052bdf7 750d                     jne        0x52be06
0052bdf9 6a08                     push       8
0052bdfb 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bdfe e8cd100000               call       0x52ced0
0052be03 90                       nop        
0052be04 eb0b                     jmp        0x52be11
0052be06 6a12                     push       0x12
0052be08 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052be0b e8c0100000               call       0x52ced0
0052be10 90                       nop        
0052be11 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052be14 81c254010000             add        edx, 0x154
0052be1a 895590                   mov        dword ptr [ebp - 0x70], edx
0052be1d 6a28                     push       0x28
0052be1f 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
0052be22 e89977efff               call       0x4235c0
0052be27 0fb6c0                   movzx      eax, al
0052be2a 85c0                     test       eax, eax
0052be2c 0f8489000000             je         0x52bebb
0052be32 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052be35 83c124                   add        ecx, 0x24
0052be38 e8c304eeff               call       0x40c300
0052be3d 50                       push       eax
0052be3e b968a55b00               mov        ecx, 0x5ba568
0052be43 e8f8100000               call       0x52cf40
0052be48 b968a55b00               mov        ecx, 0x5ba568
0052be4d e8eec4f8ff               call       0x4b8340
0052be52 a344615c00               mov        dword ptr [0x5c6144], eax
0052be57 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052be5a 83c124                   add        ecx, 0x24
0052be5d e84ea2f9ff               call       0x4c60b0
0052be62 6aff                     push       -1
0052be64 b968a55b00               mov        ecx, 0x5ba568
0052be69 e882e9fdff               call       0x50a7f0
0052be6e 6a02                     push       2
0052be70 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052be73 e858100000               call       0x52ced0
0052be78 90                       nop        
0052be79 b968a55b00               mov        ecx, 0x5ba568
0052be7e e81d82f3ff               call       0x4640a0
0052be83 83f804                   cmp        eax, 4
0052be86 7d0f                     jge        0x52be97
0052be88 6a01                     push       1
0052be8a b968a55b00               mov        ecx, 0x5ba568
0052be8f e8cc24f9ff               call       0x4be360
0052be94 90                       nop        
0052be95 eb0d                     jmp        0x52bea4
0052be97 6a07                     push       7
0052be99 b968a55b00               mov        ecx, 0x5ba568
0052be9e e8bd24f9ff               call       0x4be360
0052bea3 90                       nop        
0052bea4 c705fcfc5a00ffffffff     mov        dword ptr [0x5afcfc], 0xffffffff
0052beae 6a07                     push       7
0052beb0 b9404d5c00               mov        ecx, 0x5c4d40
0052beb5 e89623f9ff               call       0x4be250
0052beba 90                       nop        
0052bebb e93f010000               jmp        0x52bfff
0052bec0 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bec3 81c154010000             add        ecx, 0x154
0052bec9 894d8c                   mov        dword ptr [ebp - 0x74], ecx
0052becc 6a06                     push       6
0052bece 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
0052bed1 e8ea76efff               call       0x4235c0
0052bed6 0fb6d0                   movzx      edx, al
0052bed9 85d2                     test       edx, edx
0052bedb 0f841e010000             je         0x52bfff
0052bee1 c745e800000000           mov        dword ptr [ebp - 0x18], 0
0052bee8 8d45e8                   lea        eax, [ebp - 0x18]
0052beeb 8945bc                   mov        dword ptr [ebp - 0x44], eax
0052beee 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
0052bef1 8b11                     mov        edx, dword ptr [ecx]
0052bef3 52                       push       edx
0052bef4 8d45f4                   lea        eax, [ebp - 0xc]
0052bef7 50                       push       eax
0052bef8 e81356f3ff               call       0x461510
0052befd 83c408                   add        esp, 8
0052bf00 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
0052bf03 8b11                     mov        edx, dword ptr [ecx]
0052bf05 52                       push       edx
0052bf06 8d45e4                   lea        eax, [ebp - 0x1c]
0052bf09 50                       push       eax
0052bf0a e871effdff               call       0x50ae80
0052bf0f 83c408                   add        esp, 8
0052bf12 eb09                     jmp        0x52bf1d
0052bf14 8d4df4                   lea        ecx, [ebp - 0xc]
0052bf17 e84459f3ff               call       0x461860
0052bf1c 90                       nop        
0052bf1d 8d4de4                   lea        ecx, [ebp - 0x1c]
0052bf20 51                       push       ecx
0052bf21 8d4df4                   lea        ecx, [ebp - 0xc]
0052bf24 e837d5f1ff               call       0x449460
0052bf29 0fb6d0                   movzx      edx, al
0052bf2c 85d2                     test       edx, edx
0052bf2e 742f                     je         0x52bf5f
0052bf30 8d4df4                   lea        ecx, [ebp - 0xc]
0052bf33 e8c803eeff               call       0x40c300
0052bf38 894580                   mov        dword ptr [ebp - 0x80], eax
0052bf3b 8b4580                   mov        eax, dword ptr [ebp - 0x80]
0052bf3e 83c016                   add        eax, 0x16
0052bf41 50                       push       eax
0052bf42 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0052bf45 51                       push       ecx
0052bf46 8d9564ffffff             lea        edx, [ebp - 0x9c]
0052bf4c 52                       push       edx
0052bf4d 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bf50 e8eb020000               call       0x52c240
0052bf55 8bc8                     mov        ecx, eax
0052bf57 e8c441f2ff               call       0x450120
0052bf5c 90                       nop        
0052bf5d ebb5                     jmp        0x52bf14
0052bf5f b968a55b00               mov        ecx, 0x5ba568
0052bf64 e8d7c3f8ff               call       0x4b8340
0052bf69 a344615c00               mov        dword ptr [0x5c6144], eax
0052bf6e 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0052bf71 50                       push       eax
0052bf72 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bf75 e8b60c0000               call       0x52cc30
0052bf7a 90                       nop        
0052bf7b 6a00                     push       0
0052bf7d e8fe80f3ff               call       0x464080
0052bf82 83c404                   add        esp, 4
0052bf85 8bc8                     mov        ecx, eax
0052bf87 e8a467eeff               call       0x412730
0052bf8c 83c006                   add        eax, 6
0052bf8f 50                       push       eax
0052bf90 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0052bf93 51                       push       ecx
0052bf94 8d9560ffffff             lea        edx, [ebp - 0xa0]
0052bf9a 52                       push       edx
0052bf9b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bf9e e89d020000               call       0x52c240
0052bfa3 898578ffffff             mov        dword ptr [ebp - 0x88], eax
0052bfa9 b968a55b00               mov        ecx, 0x5ba568
0052bfae e88dc3f8ff               call       0x4b8340
0052bfb3 99                       cdq        
0052bfb4 2bc2                     sub        eax, edx
0052bfb6 d1f8                     sar        eax, 1
0052bfb8 83c01e                   add        eax, 0x1e
0052bfbb 89857cffffff             mov        dword ptr [ebp - 0x84], eax
0052bfc1 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
0052bfc7 50                       push       eax
0052bfc8 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0052bfce e8dd06ffff               call       0x51c6b0
0052bfd3 90                       nop        
0052bfd4 6a24                     push       0x24
0052bfd6 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bfd9 e8520c0000               call       0x52cc30
0052bfde 6a06                     push       6
0052bfe0 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bfe3 e8e80e0000               call       0x52ced0
0052bfe8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052bfeb 83c124                   add        ecx, 0x24
0052bfee e81da0f9ff               call       0x4c6010
0052bff3 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052bff9 e86236feff               call       0x50f660
0052bffe 90                       nop        
0052bfff b801000000               mov        eax, 1
0052c004 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052c007 33cd                     xor        ecx, ebp
0052c009 e8e0690100               call       0x5429ee
0052c00e 8be5                     mov        esp, ebp
0052c010 5d                       pop        ebp
0052c011 c3                       ret        
0052c012 6690                     nop        
0052c014 3f                       aas        
0052c015 b9520064ba               mov        ecx, 0xba640052
0052c01a 52                       push       edx
0052c01b 009aba5200a3             add        byte ptr [edx - 0x5cffad46], bl
0052c021 bc5200c0be               mov        esp, 0xbec00052
0052c026 52                       push       edx
0052c027 00cc                     add        ah, cl
0052c029 cc                       int3       
0052c02a cc                       int3       
0052c02b cc                       int3       
0052c02c cc                       int3       
0052c02d cc                       int3       
0052c02e cc                       int3       
0052c02f cc                       int3       
