005189c0 55                       push       ebp
005189c1 8bec                     mov        ebp, esp
005189c3 81ec04010000             sub        esp, 0x104
005189c9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
005189ce 33c5                     xor        eax, ebp
005189d0 8945fc                   mov        dword ptr [ebp - 4], eax
005189d3 56                       push       esi
005189d4 57                       push       edi
005189d5 894db0                   mov        dword ptr [ebp - 0x50], ecx
005189d8 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
005189db 8b88f4100200             mov        ecx, dword ptr [eax + 0x210f4]
005189e1 898d7cffffff             mov        dword ptr [ebp - 0x84], ecx
005189e7 83bd7cffffff03           cmp        dword ptr [ebp - 0x84], 3
005189ee 7412                     je         0x518a02
005189f0 83bd7cffffff04           cmp        dword ptr [ebp - 0x84], 4
005189f7 0f848e090000             je         0x51938b
005189fd e9210e0000               jmp        0x519823
00518a02 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00518a05 83c128                   add        ecx, 0x28
00518a08 e8f338efff               call       0x40c300
00518a0d 85c0                     test       eax, eax
00518a0f 0f85e6010000             jne        0x518bfb
00518a15 6a01                     push       1
00518a17 6a02                     push       2
00518a19 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518a1f e8fcfbf9ff               call       0x4b8620
00518a24 6a0c                     push       0xc
00518a26 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518a2c e84fd7faff               call       0x4c6180
00518a31 51                       push       ecx
00518a32 f30f10054ce05600         movss      xmm0, dword ptr [0x56e04c]
00518a3a f30f110424               movss      dword ptr [esp], xmm0
00518a3f 51                       push       ecx
00518a40 f30f1005a04f5700         movss      xmm0, dword ptr [0x574fa0]
00518a48 f30f110424               movss      dword ptr [esp], xmm0
00518a4d 8d4dcc                   lea        ecx, [ebp - 0x34]
00518a50 e8fbd0ffff               call       0x515b50
00518a55 51                       push       ecx
00518a56 f30f1005e43f5700         movss      xmm0, dword ptr [0x573fe4]
00518a5e f30f110424               movss      dword ptr [esp], xmm0
00518a63 51                       push       ecx
00518a64 f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
00518a6c f30f110424               movss      dword ptr [esp], xmm0
00518a71 8d4dd8                   lea        ecx, [ebp - 0x28]
00518a74 e8d7d0ffff               call       0x515b50
00518a79 51                       push       ecx
00518a7a f30f1005e43f5700         movss      xmm0, dword ptr [0x573fe4]
00518a82 f30f110424               movss      dword ptr [esp], xmm0
00518a87 51                       push       ecx
00518a88 f30f1005f40a5700         movss      xmm0, dword ptr [0x570af4]
00518a90 f30f110424               movss      dword ptr [esp], xmm0
00518a95 8d4de4                   lea        ecx, [ebp - 0x1c]
00518a98 e8b3d0ffff               call       0x515b50
00518a9d 51                       push       ecx
00518a9e f30f100560265700         movss      xmm0, dword ptr [0x572660]
00518aa6 f30f110424               movss      dword ptr [esp], xmm0
00518aab 51                       push       ecx
00518aac f30f10051c065700         movss      xmm0, dword ptr [0x57061c]
00518ab4 f30f110424               movss      dword ptr [esp], xmm0
00518ab9 8d4df0                   lea        ecx, [ebp - 0x10]
00518abc e88fd0ffff               call       0x515b50
00518ac1 90                       nop        
00518ac2 c745a400000000           mov        dword ptr [ebp - 0x5c], 0
00518ac9 eb09                     jmp        0x518ad4
00518acb 8b55a4                   mov        edx, dword ptr [ebp - 0x5c]
00518ace 83c201                   add        edx, 1
00518ad1 8955a4                   mov        dword ptr [ebp - 0x5c], edx
00518ad4 837da404                 cmp        dword ptr [ebp - 0x5c], 4
00518ad8 0f8d0c010000             jge        0x518bea
00518ade 6a00                     push       0
00518ae0 e89bb5f4ff               call       0x464080
00518ae5 83c404                   add        esp, 4
00518ae8 8bc8                     mov        ecx, eax
00518aea e8419cefff               call       0x412730
00518aef 89852cffffff             mov        dword ptr [ebp - 0xd4], eax
00518af5 8b852cffffff             mov        eax, dword ptr [ebp - 0xd4]
00518afb 50                       push       eax
00518afc 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00518aff 51                       push       ecx
00518b00 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00518b06 e8352f0000               call       0x51ba40
00518b0b 8b1485ec4e5700           mov        edx, dword ptr [eax*4 + 0x574eec]
00518b12 899504ffffff             mov        dword ptr [ebp - 0xfc], edx
00518b18 8b8504ffffff             mov        eax, dword ptr [ebp - 0xfc]
00518b1e 50                       push       eax
00518b1f 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518b25 e836d6faff               call       0x4c6160
00518b2a 68000000ff               push       0xff000000
00518b2f 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518b35 e8767ff5ff               call       0x470ab0
00518b3a 6aff                     push       -1
00518b3c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518b42 e8897ff5ff               call       0x470ad0
00518b47 6a00                     push       0
00518b49 e832b5f4ff               call       0x464080
00518b4e 83c404                   add        esp, 4
00518b51 8bc8                     mov        ecx, eax
00518b53 e8d89befff               call       0x412730
00518b58 898500ffffff             mov        dword ptr [ebp - 0x100], eax
00518b5e 8b8d00ffffff             mov        ecx, dword ptr [ebp - 0x100]
00518b64 51                       push       ecx
00518b65 8b55a4                   mov        edx, dword ptr [ebp - 0x5c]
00518b68 52                       push       edx
00518b69 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00518b6f e8cc2e0000               call       0x51ba40
00518b74 8bf0                     mov        esi, eax
00518b76 6a00                     push       0
00518b78 e803b5f4ff               call       0x464080
00518b7d 83c404                   add        esp, 4
00518b80 8bc8                     mov        ecx, eax
00518b82 e8a99befff               call       0x412730
00518b87 6bc009                   imul       eax, eax, 9
00518b8a 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00518b8d 8d1481                   lea        edx, [ecx + eax*4]
00518b90 8d04b2                   lea        eax, [edx + esi*4]
00518b93 898570ffffff             mov        dword ptr [ebp - 0x90], eax
00518b99 837da403                 cmp        dword ptr [ebp - 0x5c], 3
00518b9d 7510                     jne        0x518baf
00518b9f 6a01                     push       1
00518ba1 6a01                     push       1
00518ba3 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518ba9 e872faf9ff               call       0x4b8620
00518bae 90                       nop        
00518baf 83ec08                   sub        esp, 8
00518bb2 8bcc                     mov        ecx, esp
00518bb4 8b9570ffffff             mov        edx, dword ptr [ebp - 0x90]
00518bba c1e208                   shl        edx, 8
00518bbd 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00518bc0 8d9410f4000000           lea        edx, [eax + edx + 0xf4]
00518bc7 52                       push       edx
00518bc8 e80352efff               call       0x40ddd0
00518bcd 6b45a40c                 imul       eax, dword ptr [ebp - 0x5c], 0xc
00518bd1 8d4c05cc                 lea        ecx, [ebp + eax - 0x34]
00518bd5 51                       push       ecx
00518bd6 8b1598065c00             mov        edx, dword ptr [0x5c0698]
00518bdc 52                       push       edx
00518bdd e82e36f5ff               call       0x46c210
00518be2 83c410                   add        esp, 0x10
00518be5 e9e1feffff               jmp        0x518acb
00518bea 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518bf0 e89b140000               call       0x51a090
00518bf5 90                       nop        
00518bf6 e98b070000               jmp        0x519386
00518bfb 6a01                     push       1
00518bfd 6a00                     push       0
00518bff 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518c05 e816faf9ff               call       0x4b8620
00518c0a 6a00                     push       0
00518c0c e86fb4f4ff               call       0x464080
00518c11 83c404                   add        esp, 4
00518c14 8bc8                     mov        ecx, eax
00518c16 e8159befff               call       0x412730
00518c1b 89856cffffff             mov        dword ptr [ebp - 0x94], eax
00518c21 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00518c24 83c128                   add        ecx, 0x28
00518c27 e8d436efff               call       0x40c300
00518c2c 83e801                   sub        eax, 1
00518c2f 898568ffffff             mov        dword ptr [ebp - 0x98], eax
00518c35 8b856cffffff             mov        eax, dword ptr [ebp - 0x94]
00518c3b 50                       push       eax
00518c3c 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
00518c42 51                       push       ecx
00518c43 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00518c49 e8f22d0000               call       0x51ba40
00518c4e 8bf0                     mov        esi, eax
00518c50 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00518c53 83c128                   add        ecx, 0x28
00518c56 e8a536efff               call       0x40c300
00518c5b 8bf8                     mov        edi, eax
00518c5d 6a00                     push       0
00518c5f e81cb4f4ff               call       0x464080
00518c64 83c404                   add        esp, 4
00518c67 8bc8                     mov        ecx, eax
00518c69 e8c29aefff               call       0x412730
00518c6e 6bd009                   imul       edx, eax, 9
00518c71 8d0497                   lea        eax, [edi + edx*4]
00518c74 8d4cb0ff                 lea        ecx, [eax + esi*4 - 1]
00518c78 894dac                   mov        dword ptr [ebp - 0x54], ecx
00518c7b b968a55b00               mov        ecx, 0x5ba568
00518c80 e81bb4f4ff               call       0x4640a0
00518c85 83f804                   cmp        eax, 4
00518c88 756e                     jne        0x518cf8
00518c8a 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00518c8d 83c128                   add        ecx, 0x28
00518c90 e86b36efff               call       0x40c300
00518c95 83f801                   cmp        eax, 1
00518c98 755e                     jne        0x518cf8
00518c9a 6a00                     push       0
00518c9c e8dfb3f4ff               call       0x464080
00518ca1 83c404                   add        esp, 4
00518ca4 8bc8                     mov        ecx, eax
00518ca6 e8859aefff               call       0x412730
00518cab 898564ffffff             mov        dword ptr [ebp - 0x9c], eax
00518cb1 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00518cb4 83c128                   add        ecx, 0x28
00518cb7 e84436efff               call       0x40c300
00518cbc 83e801                   sub        eax, 1
00518cbf 898560ffffff             mov        dword ptr [ebp - 0xa0], eax
00518cc5 8b9564ffffff             mov        edx, dword ptr [ebp - 0x9c]
00518ccb 52                       push       edx
00518ccc 8b8560ffffff             mov        eax, dword ptr [ebp - 0xa0]
00518cd2 50                       push       eax
00518cd3 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00518cd9 e8622d0000               call       0x51ba40
00518cde 8bf0                     mov        esi, eax
00518ce0 6a00                     push       0
00518ce2 e899b3f4ff               call       0x464080
00518ce7 83c404                   add        esp, 4
00518cea 8bc8                     mov        ecx, eax
00518cec e83f9aefff               call       0x412730
00518cf1 8d4cc648                 lea        ecx, [esi + eax*8 + 0x48]
00518cf5 894dac                   mov        dword ptr [ebp - 0x54], ecx
00518cf8 6a10                     push       0x10
00518cfa 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518d00 e87bd4faff               call       0x4c6180
00518d05 6aff                     push       -1
00518d07 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518d0d e8be7df5ff               call       0x470ad0
00518d12 68000000ff               push       0xff000000
00518d17 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518d1d e83ed4faff               call       0x4c6160
00518d22 6a00                     push       0
00518d24 e857b3f4ff               call       0x464080
00518d29 83c404                   add        esp, 4
00518d2c 8bc8                     mov        ecx, eax
00518d2e e8fd99efff               call       0x412730
00518d33 89855cffffff             mov        dword ptr [ebp - 0xa4], eax
00518d39 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00518d3c 83c128                   add        ecx, 0x28
00518d3f e8bc35efff               call       0x40c300
00518d44 83e801                   sub        eax, 1
00518d47 898558ffffff             mov        dword ptr [ebp - 0xa8], eax
00518d4d 8b955cffffff             mov        edx, dword ptr [ebp - 0xa4]
00518d53 52                       push       edx
00518d54 8b8558ffffff             mov        eax, dword ptr [ebp - 0xa8]
00518d5a 50                       push       eax
00518d5b 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00518d61 e8da2c0000               call       0x51ba40
00518d66 8b0c85ec4e5700           mov        ecx, dword ptr [eax*4 + 0x574eec]
00518d6d 898d54ffffff             mov        dword ptr [ebp - 0xac], ecx
00518d73 8b9554ffffff             mov        edx, dword ptr [ebp - 0xac]
00518d79 52                       push       edx
00518d7a 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518d80 e82b7df5ff               call       0x470ab0
00518d85 8d4db4                   lea        ecx, [ebp - 0x4c]
00518d88 e883a0f0ff               call       0x422e10
00518d8d 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00518d90 05d0000000               add        eax, 0xd0
00518d95 898550ffffff             mov        dword ptr [ebp - 0xb0], eax
00518d9b 8d4db4                   lea        ecx, [ebp - 0x4c]
00518d9e 51                       push       ecx
00518d9f 8b8d50ffffff             mov        ecx, dword ptr [ebp - 0xb0]
00518da5 e8c63ef3ff               call       0x44cc70
00518daa b958675b00               mov        ecx, 0x5b6758
00518daf e8dcf4f1ff               call       0x438290
00518db4 d99d4cffffff             fstp       dword ptr [ebp - 0xb4]
00518dba f30f10854cffffff         movss      xmm0, dword ptr [ebp - 0xb4]
00518dc2 f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
00518dca 51                       push       ecx
00518dcb f30f110424               movss      dword ptr [esp], xmm0
00518dd0 8d4db4                   lea        ecx, [ebp - 0x4c]
00518dd3 e86874faff               call       0x4c0240
00518dd8 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
00518ddb 81c2d4000000             add        edx, 0xd4
00518de1 899548ffffff             mov        dword ptr [ebp - 0xb8], edx
00518de7 8d45b4                   lea        eax, [ebp - 0x4c]
00518dea 50                       push       eax
00518deb 8b8d48ffffff             mov        ecx, dword ptr [ebp - 0xb8]
00518df1 e8eab7f4ff               call       0x4645e0
00518df6 51                       push       ecx
00518df7 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
00518dff f30f110424               movss      dword ptr [esp], xmm0
00518e04 8d4db4                   lea        ecx, [ebp - 0x4c]
00518e07 e83474faff               call       0x4c0240
00518e0c c7459400000000           mov        dword ptr [ebp - 0x6c], 0
00518e13 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00518e16 83c128                   add        ecx, 0x28
00518e19 898d44ffffff             mov        dword ptr [ebp - 0xbc], ecx
00518e1f 6a01                     push       1
00518e21 8b8d44ffffff             mov        ecx, dword ptr [ebp - 0xbc]
00518e27 e88423f5ff               call       0x46b1b0
00518e2c 85c0                     test       eax, eax
00518e2e 746c                     je         0x518e9c
00518e30 c7458400000000           mov        dword ptr [ebp - 0x7c], 0
00518e37 c7459000000000           mov        dword ptr [ebp - 0x70], 0
00518e3e eb09                     jmp        0x518e49
00518e40 8b5590                   mov        edx, dword ptr [ebp - 0x70]
00518e43 83c201                   add        edx, 1
00518e46 895590                   mov        dword ptr [ebp - 0x70], edx
00518e49 837d9005                 cmp        dword ptr [ebp - 0x70], 5
00518e4d 7d34                     jge        0x518e83
00518e4f 6945ac00050000           imul       eax, dword ptr [ebp - 0x54], 0x500
00518e56 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00518e59 8d9401f4580000           lea        edx, [ecx + eax + 0x58f4]
00518e60 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00518e63 c1e008                   shl        eax, 8
00518e66 03d0                     add        edx, eax
00518e68 b901000000               mov        ecx, 1
00518e6d 6bc100                   imul       eax, ecx, 0
00518e70 0fbe0c02                 movsx      ecx, byte ptr [edx + eax]
00518e74 85c9                     test       ecx, ecx
00518e76 7409                     je         0x518e81
00518e78 8b5584                   mov        edx, dword ptr [ebp - 0x7c]
00518e7b 83c201                   add        edx, 1
00518e7e 895584                   mov        dword ptr [ebp - 0x7c], edx
00518e81 ebbd                     jmp        0x518e40
00518e83 b805000000               mov        eax, 5
00518e88 2b4584                   sub        eax, dword ptr [ebp - 0x7c]
00518e8b 6bc814                   imul       ecx, eax, 0x14
00518e8e f30f2ac1                 cvtsi2ss   xmm0, ecx
00518e92 f30f5845b8               addss      xmm0, dword ptr [ebp - 0x48]
00518e97 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
00518e9c 83ec08                   sub        esp, 8
00518e9f 8bcc                     mov        ecx, esp
00518ea1 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
00518ea4 c1e208                   shl        edx, 8
00518ea7 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00518eaa 8d9410f4000000           lea        edx, [eax + edx + 0xf4]
00518eb1 52                       push       edx
00518eb2 e8194fefff               call       0x40ddd0
00518eb7 8d45b4                   lea        eax, [ebp - 0x4c]
00518eba 50                       push       eax
00518ebb 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518ec1 51                       push       ecx
00518ec2 e84933f5ff               call       0x46c210
00518ec7 83c410                   add        esp, 0x10
00518eca 6a03                     push       3
00518ecc 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518ed2 e8a9d2faff               call       0x4c6180
00518ed7 6aff                     push       -1
00518ed9 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518edf e87cd2faff               call       0x4c6160
00518ee4 68000000ff               push       0xff000000
00518ee9 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00518eef e8bc7bf5ff               call       0x470ab0
00518ef4 f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
00518ef9 f30f580560055700         addss      xmm0, dword ptr [0x570560]
00518f01 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
00518f06 6a00                     push       0
00518f08 e873b1f4ff               call       0x464080
00518f0d 83c404                   add        esp, 4
00518f10 8bc8                     mov        ecx, eax
00518f12 e81998efff               call       0x412730
00518f17 898540ffffff             mov        dword ptr [ebp - 0xc0], eax
00518f1d 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00518f20 83c128                   add        ecx, 0x28
00518f23 e8d833efff               call       0x40c300
00518f28 83e801                   sub        eax, 1
00518f2b 89853cffffff             mov        dword ptr [ebp - 0xc4], eax
00518f31 8b9540ffffff             mov        edx, dword ptr [ebp - 0xc0]
00518f37 52                       push       edx
00518f38 8b853cffffff             mov        eax, dword ptr [ebp - 0xc4]
00518f3e 50                       push       eax
00518f3f 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00518f45 e8b6b1f4ff               call       0x464100
00518f4a 83f808                   cmp        eax, 8
00518f4d 0f84c3000000             je         0x519016
00518f53 c745a000000000           mov        dword ptr [ebp - 0x60], 0
00518f5a eb09                     jmp        0x518f65
00518f5c 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00518f5f 83c101                   add        ecx, 1
00518f62 894da0                   mov        dword ptr [ebp - 0x60], ecx
00518f65 837da005                 cmp        dword ptr [ebp - 0x60], 5
00518f69 0f8da2000000             jge        0x519011
00518f6f 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
00518f72 83c218                   add        edx, 0x18
00518f75 899538ffffff             mov        dword ptr [ebp - 0xc8], edx
00518f7b 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00518f7e 50                       push       eax
00518f7f 8b8d38ffffff             mov        ecx, dword ptr [ebp - 0xc8]
00518f85 e8e680f4ff               call       0x461070
00518f8a 0fb6c8                   movzx      ecx, al
00518f8d 85c9                     test       ecx, ecx
00518f8f 747b                     je         0x51900c
00518f91 6955ac00050000           imul       edx, dword ptr [ebp - 0x54], 0x500
00518f98 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00518f9b 8d8c10f4580000           lea        ecx, [eax + edx + 0x58f4]
00518fa2 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00518fa5 c1e208                   shl        edx, 8
00518fa8 03ca                     add        ecx, edx
00518faa b801000000               mov        eax, 1
00518faf 6bd000                   imul       edx, eax, 0
00518fb2 0fbe0411                 movsx      eax, byte ptr [ecx + edx]
00518fb6 85c0                     test       eax, eax
00518fb8 7440                     je         0x518ffa
00518fba 83ec08                   sub        esp, 8
00518fbd 8bcc                     mov        ecx, esp
00518fbf 6955ac00050000           imul       edx, dword ptr [ebp - 0x54], 0x500
00518fc6 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00518fc9 8d9410f4580000           lea        edx, [eax + edx + 0x58f4]
00518fd0 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00518fd3 c1e008                   shl        eax, 8
00518fd6 03d0                     add        edx, eax
00518fd8 52                       push       edx
00518fd9 e8f24defff               call       0x40ddd0
00518fde 8d4db4                   lea        ecx, [ebp - 0x4c]
00518fe1 51                       push       ecx
00518fe2 8b1598065c00             mov        edx, dword ptr [0x5c0698]
00518fe8 52                       push       edx
00518fe9 e82232f5ff               call       0x46c210
00518fee 83c410                   add        esp, 0x10
00518ff1 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
00518ff4 83c001                   add        eax, 1
00518ff7 894594                   mov        dword ptr [ebp - 0x6c], eax
00518ffa f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
00518fff f30f58058cec5600         addss      xmm0, dword ptr [0x56ec8c]
00519007 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
0051900c e94bffffff               jmp        0x518f5c
00519011 e9e4020000               jmp        0x5192fa
00519016 6a00                     push       0
00519018 e863b0f4ff               call       0x464080
0051901d 83c404                   add        esp, 4
00519020 8bc8                     mov        ecx, eax
00519022 e80997efff               call       0x412730
00519027 898534ffffff             mov        dword ptr [ebp - 0xcc], eax
0051902d 8b8d34ffffff             mov        ecx, dword ptr [ebp - 0xcc]
00519033 51                       push       ecx
00519034 6a00                     push       0
00519036 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0051903c e8ff290000               call       0x51ba40
00519041 8bf0                     mov        esi, eax
00519043 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00519046 83c128                   add        ecx, 0x28
00519049 e8b232efff               call       0x40c300
0051904e 8bf8                     mov        edi, eax
00519050 6a00                     push       0
00519052 e829b0f4ff               call       0x464080
00519057 83c404                   add        esp, 4
0051905a 8bc8                     mov        ecx, eax
0051905c e8cf96efff               call       0x412730
00519061 6bd009                   imul       edx, eax, 9
00519064 8d0497                   lea        eax, [edi + edx*4]
00519067 8d4cb0ff                 lea        ecx, [eax + esi*4 - 1]
0051906b 894d8c                   mov        dword ptr [ebp - 0x74], ecx
0051906e 69558c00050000           imul       edx, dword ptr [ebp - 0x74], 0x500
00519075 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00519078 8d8c10f4580000           lea        ecx, [eax + edx + 0x58f4]
0051907f ba00010000               mov        edx, 0x100
00519084 6bc200                   imul       eax, edx, 0
00519087 03c8                     add        ecx, eax
00519089 ba01000000               mov        edx, 1
0051908e 6bc200                   imul       eax, edx, 0
00519091 0fbe0c01                 movsx      ecx, byte ptr [ecx + eax]
00519095 85c9                     test       ecx, ecx
00519097 7439                     je         0x5190d2
00519099 83ec08                   sub        esp, 8
0051909c 8bcc                     mov        ecx, esp
0051909e 69558c00050000           imul       edx, dword ptr [ebp - 0x74], 0x500
005190a5 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
005190a8 8d9410f4580000           lea        edx, [eax + edx + 0x58f4]
005190af b800010000               mov        eax, 0x100
005190b4 6bc000                   imul       eax, eax, 0
005190b7 03d0                     add        edx, eax
005190b9 52                       push       edx
005190ba e8114defff               call       0x40ddd0
005190bf 8d4db4                   lea        ecx, [ebp - 0x4c]
005190c2 51                       push       ecx
005190c3 8b1598065c00             mov        edx, dword ptr [0x5c0698]
005190c9 52                       push       edx
005190ca e84131f5ff               call       0x46c210
005190cf 83c410                   add        esp, 0x10
005190d2 f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
005190d7 f30f58058cec5600         addss      xmm0, dword ptr [0x56ec8c]
005190df f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
005190e4 69458c00050000           imul       eax, dword ptr [ebp - 0x74], 0x500
005190eb 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
005190ee 8d9401f4580000           lea        edx, [ecx + eax + 0x58f4]
005190f5 b800010000               mov        eax, 0x100
005190fa d1e0                     shl        eax, 1
005190fc 03d0                     add        edx, eax
005190fe b901000000               mov        ecx, 1
00519103 6bc100                   imul       eax, ecx, 0
00519106 0fbe0c02                 movsx      ecx, byte ptr [edx + eax]
0051910a 85c9                     test       ecx, ecx
0051910c 7438                     je         0x519146
0051910e 83ec08                   sub        esp, 8
00519111 8bcc                     mov        ecx, esp
00519113 69558c00050000           imul       edx, dword ptr [ebp - 0x74], 0x500
0051911a 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
0051911d 8d9410f4580000           lea        edx, [eax + edx + 0x58f4]
00519124 b800010000               mov        eax, 0x100
00519129 d1e0                     shl        eax, 1
0051912b 03d0                     add        edx, eax
0051912d 52                       push       edx
0051912e e89d4cefff               call       0x40ddd0
00519133 8d4db4                   lea        ecx, [ebp - 0x4c]
00519136 51                       push       ecx
00519137 8b1598065c00             mov        edx, dword ptr [0x5c0698]
0051913d 52                       push       edx
0051913e e8cd30f5ff               call       0x46c210
00519143 83c410                   add        esp, 0x10
00519146 f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
0051914b f30f580560055700         addss      xmm0, dword ptr [0x570560]
00519153 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
00519158 68c0c0c0ff               push       0xffc0c0c0
0051915d 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00519163 e8f8cffaff               call       0x4c6160
00519168 90                       nop        
00519169 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0051916c 83c128                   add        ecx, 0x28
0051916f e88c31efff               call       0x40c300
00519174 83f804                   cmp        eax, 4
00519177 0f84c1000000             je         0x51923e
0051917d c7459c00000000           mov        dword ptr [ebp - 0x64], 0
00519184 eb09                     jmp        0x51918f
00519186 8b459c                   mov        eax, dword ptr [ebp - 0x64]
00519189 83c001                   add        eax, 1
0051918c 89459c                   mov        dword ptr [ebp - 0x64], eax
0051918f 837d9c01                 cmp        dword ptr [ebp - 0x64], 1
00519193 0f8d99000000             jge        0x519232
00519199 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0051919c 83c118                   add        ecx, 0x18
0051919f 898d30ffffff             mov        dword ptr [ebp - 0xd0], ecx
005191a5 8b559c                   mov        edx, dword ptr [ebp - 0x64]
005191a8 52                       push       edx
005191a9 8b8d30ffffff             mov        ecx, dword ptr [ebp - 0xd0]
005191af e8bc7ef4ff               call       0x461070
005191b4 0fb6c0                   movzx      eax, al
005191b7 85c0                     test       eax, eax
005191b9 7472                     je         0x51922d
005191bb 694dac00050000           imul       ecx, dword ptr [ebp - 0x54], 0x500
005191c2 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
005191c5 8d840af4580000           lea        eax, [edx + ecx + 0x58f4]
005191cc 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
005191cf c1e108                   shl        ecx, 8
005191d2 03c1                     add        eax, ecx
005191d4 ba01000000               mov        edx, 1
005191d9 6bca00                   imul       ecx, edx, 0
005191dc 0fbe1408                 movsx      edx, byte ptr [eax + ecx]
005191e0 85d2                     test       edx, edx
005191e2 7437                     je         0x51921b
005191e4 83ec08                   sub        esp, 8
005191e7 8bcc                     mov        ecx, esp
005191e9 6945ac00050000           imul       eax, dword ptr [ebp - 0x54], 0x500
005191f0 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
005191f3 8d8402f4580000           lea        eax, [edx + eax + 0x58f4]
005191fa 8b559c                   mov        edx, dword ptr [ebp - 0x64]
005191fd c1e208                   shl        edx, 8
00519200 03c2                     add        eax, edx
00519202 50                       push       eax
00519203 e8c84befff               call       0x40ddd0
00519208 8d45b4                   lea        eax, [ebp - 0x4c]
0051920b 50                       push       eax
0051920c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00519212 51                       push       ecx
00519213 e8f82ff5ff               call       0x46c210
00519218 83c410                   add        esp, 0x10
0051921b f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
00519220 f30f58058cec5600         addss      xmm0, dword ptr [0x56ec8c]
00519228 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
0051922d e954ffffff               jmp        0x519186
00519232 c7459404000000           mov        dword ptr [ebp - 0x6c], 4
00519239 e9bc000000               jmp        0x5192fa
0051923e c7459800000000           mov        dword ptr [ebp - 0x68], 0
00519245 eb09                     jmp        0x519250
00519247 8b5598                   mov        edx, dword ptr [ebp - 0x68]
0051924a 83c201                   add        edx, 1
0051924d 895598                   mov        dword ptr [ebp - 0x68], edx
00519250 837d9802                 cmp        dword ptr [ebp - 0x68], 2
00519254 0f8d99000000             jge        0x5192f3
0051925a 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
0051925d 83c018                   add        eax, 0x18
00519260 898514ffffff             mov        dword ptr [ebp - 0xec], eax
00519266 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00519269 51                       push       ecx
0051926a 8b8d14ffffff             mov        ecx, dword ptr [ebp - 0xec]
00519270 e8fb7df4ff               call       0x461070
00519275 0fb6d0                   movzx      edx, al
00519278 85d2                     test       edx, edx
0051927a 7472                     je         0x5192ee
0051927c 6945ac00050000           imul       eax, dword ptr [ebp - 0x54], 0x500
00519283 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00519286 8d9401f4580000           lea        edx, [ecx + eax + 0x58f4]
0051928d 8b4598                   mov        eax, dword ptr [ebp - 0x68]
00519290 c1e008                   shl        eax, 8
00519293 03d0                     add        edx, eax
00519295 b901000000               mov        ecx, 1
0051929a 6bc100                   imul       eax, ecx, 0
0051929d 0fbe0c02                 movsx      ecx, byte ptr [edx + eax]
005192a1 85c9                     test       ecx, ecx
005192a3 7437                     je         0x5192dc
005192a5 83ec08                   sub        esp, 8
005192a8 8bcc                     mov        ecx, esp
005192aa 6955ac00050000           imul       edx, dword ptr [ebp - 0x54], 0x500
005192b1 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
005192b4 8d9410f4580000           lea        edx, [eax + edx + 0x58f4]
005192bb 8b4598                   mov        eax, dword ptr [ebp - 0x68]
005192be c1e008                   shl        eax, 8
005192c1 03d0                     add        edx, eax
005192c3 52                       push       edx
005192c4 e8074befff               call       0x40ddd0
005192c9 8d4db4                   lea        ecx, [ebp - 0x4c]
005192cc 51                       push       ecx
005192cd 8b1598065c00             mov        edx, dword ptr [0x5c0698]
005192d3 52                       push       edx
005192d4 e8372ff5ff               call       0x46c210
005192d9 83c410                   add        esp, 0x10
005192dc f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
005192e1 f30f58058cec5600         addss      xmm0, dword ptr [0x56ec8c]
005192e9 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
005192ee e954ffffff               jmp        0x519247
005192f3 c7459405000000           mov        dword ptr [ebp - 0x6c], 5
005192fa 6a02                     push       2
005192fc 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00519302 e879cefaff               call       0x4c6180
00519307 6a01                     push       1
00519309 6a01                     push       1
0051930b 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00519311 e80af3f9ff               call       0x4b8620
00519316 6aff                     push       -1
00519318 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0051931e e8ad77f5ff               call       0x470ad0
00519323 68000000ff               push       0xff000000
00519328 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0051932e e87d77f5ff               call       0x470ab0
00519333 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00519336 05d4000000               add        eax, 0xd4
0051933b 898528ffffff             mov        dword ptr [ebp - 0xd8], eax
00519341 f30f2a4594               cvtsi2ss   xmm0, dword ptr [ebp - 0x6c]
00519346 f30f590560055700         mulss      xmm0, dword ptr [0x570560]
0051934e f30f580530fa5600         addss      xmm0, dword ptr [0x56fa30]
00519356 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0051935e 51                       push       ecx
0051935f f30f110424               movss      dword ptr [esp], xmm0
00519364 f30f100550e05600         movss      xmm0, dword ptr [0x56e050]
0051936c f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00519374 51                       push       ecx
00519375 f30f110424               movss      dword ptr [esp], xmm0
0051937a 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
00519380 e82b73f3ff               call       0x4506b0
00519385 90                       nop        
00519386 e998040000               jmp        0x519823
0051938b 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0051938e 83c118                   add        ecx, 0x18
00519391 898d24ffffff             mov        dword ptr [ebp - 0xdc], ecx
00519397 6a14                     push       0x14
00519399 8b8d24ffffff             mov        ecx, dword ptr [ebp - 0xdc]
0051939f e8cc7cf4ff               call       0x461070
005193a4 0fb6d0                   movzx      edx, al
005193a7 85d2                     test       edx, edx
005193a9 0f8474040000             je         0x519823
005193af 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
005193b2 83c128                   add        ecx, 0x28
005193b5 e8462fefff               call       0x40c300
005193ba 85c0                     test       eax, eax
005193bc 0f8461040000             je         0x519823
005193c2 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
005193c5 05e8000000               add        eax, 0xe8
005193ca 8b08                     mov        ecx, dword ptr [eax]
005193cc 894dc0                   mov        dword ptr [ebp - 0x40], ecx
005193cf 8b5004                   mov        edx, dword ptr [eax + 4]
005193d2 8955c4                   mov        dword ptr [ebp - 0x3c], edx
005193d5 8b4008                   mov        eax, dword ptr [eax + 8]
005193d8 8945c8                   mov        dword ptr [ebp - 0x38], eax
005193db 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
005193de 83c128                   add        ecx, 0x28
005193e1 898d20ffffff             mov        dword ptr [ebp - 0xe0], ecx
005193e7 6a01                     push       1
005193e9 8b8d20ffffff             mov        ecx, dword ptr [ebp - 0xe0]
005193ef e8cccfffff               call       0x5163c0
005193f4 85c0                     test       eax, eax
005193f6 0f84bf000000             je         0x5194bb
005193fc 6a01                     push       1
005193fe 6a02                     push       2
00519400 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00519406 e815f2f9ff               call       0x4b8620
0051940b 6a00                     push       0
0051940d 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00519413 e868cdfaff               call       0x4c6180
00519418 68a0a0a0ff               push       0xffa0a0a0
0051941d 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00519423 e8a876f5ff               call       0x470ad0
00519428 6aff                     push       -1
0051942a 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00519430 e82bcdfaff               call       0x4c6160
00519435 f30f1045c0               movss      xmm0, dword ptr [ebp - 0x40]
0051943a f30f580508f15600         addss      xmm0, dword ptr [0x56f108]
00519442 f30f1145c0               movss      dword ptr [ebp - 0x40], xmm0
00519447 f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
0051944c f30f5c05984f5700         subss      xmm0, dword ptr [0x574f98]
00519454 f30f1145c4               movss      dword ptr [ebp - 0x3c], xmm0
00519459 51                       push       ecx
0051945a f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
00519462 f30f110424               movss      dword ptr [esp], xmm0
00519467 8d4dc0                   lea        ecx, [ebp - 0x40]
0051946a e8d16dfaff               call       0x4c0240
0051946f 68000000c0               push       0xc0000000
00519474 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0051947a e83176f5ff               call       0x470ab0
0051947f 90                       nop        
00519480 83ec08                   sub        esp, 8
00519483 8bcc                     mov        ecx, esp
00519485 68884f5700               push       0x574f88
0051948a e84149efff               call       0x40ddd0
0051948f 8d55c0                   lea        edx, [ebp - 0x40]
00519492 52                       push       edx
00519493 a198065c00               mov        eax, dword ptr [0x5c0698]
00519498 50                       push       eax
00519499 e8722df5ff               call       0x46c210
0051949e 83c410                   add        esp, 0x10
005194a1 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
005194a4 81c1e8000000             add        ecx, 0xe8
005194aa 8b11                     mov        edx, dword ptr [ecx]
005194ac 8955c0                   mov        dword ptr [ebp - 0x40], edx
005194af 8b4104                   mov        eax, dword ptr [ecx + 4]
005194b2 8945c4                   mov        dword ptr [ebp - 0x3c], eax
005194b5 8b4908                   mov        ecx, dword ptr [ecx + 8]
005194b8 894dc8                   mov        dword ptr [ebp - 0x38], ecx
005194bb f30f1045c0               movss      xmm0, dword ptr [ebp - 0x40]
005194c0 f30f5c0508f15600         subss      xmm0, dword ptr [0x56f108]
005194c8 f30f1145c0               movss      dword ptr [ebp - 0x40], xmm0
005194cd 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
005194d0 83c228                   add        edx, 0x28
005194d3 89951cffffff             mov        dword ptr [ebp - 0xe4], edx
005194d9 6a01                     push       1
005194db 8b8d1cffffff             mov        ecx, dword ptr [ebp - 0xe4]
005194e1 e8ca1cf5ff               call       0x46b1b0
005194e6 85c0                     test       eax, eax
005194e8 740c                     je         0x5194f6
005194ea c78578ffffff14000000     mov        dword ptr [ebp - 0x88], 0x14
005194f4 eb0a                     jmp        0x519500
005194f6 c78578ffffff00000000     mov        dword ptr [ebp - 0x88], 0
00519500 b8d2000000               mov        eax, 0xd2
00519505 2b8578ffffff             sub        eax, dword ptr [ebp - 0x88]
0051950b f30f2ac0                 cvtsi2ss   xmm0, eax
0051950f f30f104dc4               movss      xmm1, dword ptr [ebp - 0x3c]
00519514 f30f5cc8                 subss      xmm1, xmm0
00519518 f30f114dc4               movss      dword ptr [ebp - 0x3c], xmm1
0051951d 51                       push       ecx
0051951e f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
00519526 f30f110424               movss      dword ptr [esp], xmm0
0051952b 8d4dc0                   lea        ecx, [ebp - 0x40]
0051952e e80d6dfaff               call       0x4c0240
00519533 6a00                     push       0
00519535 e846abf4ff               call       0x464080
0051953a 83c404                   add        esp, 4
0051953d 8bc8                     mov        ecx, eax
0051953f e8ec91efff               call       0x412730
00519544 6bc809                   imul       ecx, eax, 9
00519547 c1e102                   shl        ecx, 2
0051954a 894d80                   mov        dword ptr [ebp - 0x80], ecx
0051954d c745a800000000           mov        dword ptr [ebp - 0x58], 0
00519554 eb09                     jmp        0x51955f
00519556 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
00519559 83c201                   add        edx, 1
0051955c 8955a8                   mov        dword ptr [ebp - 0x58], edx
0051955f 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00519562 83c028                   add        eax, 0x28
00519565 898518ffffff             mov        dword ptr [ebp - 0xe8], eax
0051956b 6a01                     push       1
0051956d 8b8d18ffffff             mov        ecx, dword ptr [ebp - 0xe8]
00519573 e8381cf5ff               call       0x46b1b0
00519578 85c0                     test       eax, eax
0051957a 740c                     je         0x519588
0051957c c78574ffffff08000000     mov        dword ptr [ebp - 0x8c], 8
00519586 eb0a                     jmp        0x519592
00519588 c78574ffffff09000000     mov        dword ptr [ebp - 0x8c], 9
00519592 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00519595 3b8d74ffffff             cmp        ecx, dword ptr [ebp - 0x8c]
0051959b 0f8d48020000             jge        0x5197e9
005195a1 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
005195a4 52                       push       edx
005195a5 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
005195ab e86040faff               call       0x4bd610
005195b0 8bf0                     mov        esi, eax
005195b2 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
005195b5 50                       push       eax
005195b6 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
005195bc e8af230000               call       0x51b970
005195c1 2bf0                     sub        esi, eax
005195c3 897588                   mov        dword ptr [ebp - 0x78], esi
005195c6 6a01                     push       1
005195c8 6a01                     push       1
005195ca 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005195d0 e84bf0f9ff               call       0x4b8620
005195d5 6a05                     push       5
005195d7 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005195dd e89ecbfaff               call       0x4c6180
005195e2 90                       nop        
005195e3 837d8809                 cmp        dword ptr [ebp - 0x78], 9
005195e7 7e07                     jle        0x5195f0
005195e9 c7458809000000           mov        dword ptr [ebp - 0x78], 9
005195f0 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
005195f3 8b91c0000000             mov        edx, dword ptr [ecx + 0xc0]
005195f9 3b55a8                   cmp        edx, dword ptr [ebp - 0x58]
005195fc 7510                     jne        0x51960e
005195fe 6aa0                     push       -0x60
00519600 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00519606 e8c574f5ff               call       0x470ad0
0051960b 90                       nop        
0051960c eb4a                     jmp        0x519658
0051960e 837d8800                 cmp        dword ptr [ebp - 0x78], 0
00519612 7523                     jne        0x519637
00519614 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00519617 83c028                   add        eax, 0x28
0051961a 8985fcfeffff             mov        dword ptr [ebp - 0x104], eax
00519620 6a01                     push       1
00519622 8b8dfcfeffff             mov        ecx, dword ptr [ebp - 0x104]
00519628 e8831bf5ff               call       0x46b1b0
0051962d 85c0                     test       eax, eax
0051962f 7506                     jne        0x519637
00519631 837da808                 cmp        dword ptr [ebp - 0x58], 8
00519635 7510                     jne        0x519647
00519637 6aff                     push       -1
00519639 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0051963f e88c74f5ff               call       0x470ad0
00519644 90                       nop        
00519645 eb11                     jmp        0x519658
00519647 68606060ff               push       0xff606060
0051964c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00519652 e87974f5ff               call       0x470ad0
00519657 90                       nop        
00519658 b968a55b00               mov        ecx, 0x5ba568
0051965d e83eaaf4ff               call       0x4640a0
00519662 83f804                   cmp        eax, 4
00519665 7562                     jne        0x5196c9
00519667 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0051966a 83c128                   add        ecx, 0x28
0051966d 898d10ffffff             mov        dword ptr [ebp - 0xf0], ecx
00519673 6a01                     push       1
00519675 8b8d10ffffff             mov        ecx, dword ptr [ebp - 0xf0]
0051967b e8301bf5ff               call       0x46b1b0
00519680 85c0                     test       eax, eax
00519682 7445                     je         0x5196c9
00519684 6a00                     push       0
00519686 e8f5a9f4ff               call       0x464080
0051968b 83c404                   add        esp, 4
0051968e 8bc8                     mov        ecx, eax
00519690 e89b90efff               call       0x412730
00519695 89850cffffff             mov        dword ptr [ebp - 0xf4], eax
0051969b 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0051969e 52                       push       edx
0051969f 8b850cffffff             mov        eax, dword ptr [ebp - 0xf4]
005196a5 50                       push       eax
005196a6 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
005196ac e85f250000               call       0x51bc10
005196b1 0fb6c8                   movzx      ecx, al
005196b4 85c9                     test       ecx, ecx
005196b6 7511                     jne        0x5196c9
005196b8 68606060ff               push       0xff606060
005196bd 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005196c3 e80874f5ff               call       0x470ad0
005196c8 90                       nop        
005196c9 6aff                     push       -1
005196cb 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005196d1 e88acafaff               call       0x4c6160
005196d6 68000000c0               push       0xc0000000
005196db 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005196e1 e8ca73f5ff               call       0x470ab0
005196e6 90                       nop        
005196e7 83ec08                   sub        esp, 8
005196ea 8bcc                     mov        ecx, esp
005196ec 8b5580                   mov        edx, dword ptr [ebp - 0x80]
005196ef c1e208                   shl        edx, 8
005196f2 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
005196f5 8d9410f4000000           lea        edx, [eax + edx + 0xf4]
005196fc 52                       push       edx
005196fd e8ce46efff               call       0x40ddd0
00519702 8d45c0                   lea        eax, [ebp - 0x40]
00519705 50                       push       eax
00519706 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0051970c 51                       push       ecx
0051970d e8fe2af5ff               call       0x46c210
00519712 83c410                   add        esp, 0x10
00519715 f30f1045c0               movss      xmm0, dword ptr [ebp - 0x40]
0051971a f30f580514ed5600         addss      xmm0, dword ptr [0x56ed14]
00519722 f30f1145c0               movss      dword ptr [ebp - 0x40], xmm0
00519727 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
0051972a 83c228                   add        edx, 0x28
0051972d 899508ffffff             mov        dword ptr [ebp - 0xf8], edx
00519733 6a01                     push       1
00519735 8b8d08ffffff             mov        ecx, dword ptr [ebp - 0xf8]
0051973b e8701af5ff               call       0x46b1b0
00519740 85c0                     test       eax, eax
00519742 7424                     je         0x519768
00519744 83ec08                   sub        esp, 8
00519747 8bcc                     mov        ecx, esp
00519749 68904f5700               push       0x574f90
0051974e e87d46efff               call       0x40ddd0
00519753 8d45c0                   lea        eax, [ebp - 0x40]
00519756 50                       push       eax
00519757 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0051975d 51                       push       ecx
0051975e e8ad2af5ff               call       0x46c210
00519763 83c410                   add        esp, 0x10
00519766 eb4f                     jmp        0x5197b7
00519768 837da808                 cmp        dword ptr [ebp - 0x58], 8
0051976c 7523                     jne        0x519791
0051976e 83ec08                   sub        esp, 8
00519771 8bcc                     mov        ecx, esp
00519773 68944f5700               push       0x574f94
00519778 e85346efff               call       0x40ddd0
0051977d 8d55c0                   lea        edx, [ebp - 0x40]
00519780 52                       push       edx
00519781 a198065c00               mov        eax, dword ptr [0x5c0698]
00519786 50                       push       eax
00519787 e8842af5ff               call       0x46c210
0051978c 83c410                   add        esp, 0x10
0051978f eb26                     jmp        0x5197b7
00519791 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00519794 8b14cd144f5700           mov        edx, dword ptr [ecx*8 + 0x574f14]
0051979b 52                       push       edx
0051979c 8b04cd104f5700           mov        eax, dword ptr [ecx*8 + 0x574f10]
005197a3 50                       push       eax
005197a4 8d4dc0                   lea        ecx, [ebp - 0x40]
005197a7 51                       push       ecx
005197a8 8b1598065c00             mov        edx, dword ptr [0x5c0698]
005197ae 52                       push       edx
005197af e85c2af5ff               call       0x46c210
005197b4 83c410                   add        esp, 0x10
005197b7 f30f1045c0               movss      xmm0, dword ptr [ebp - 0x40]
005197bc f30f5c0514ed5600         subss      xmm0, dword ptr [0x56ed14]
005197c4 f30f1145c0               movss      dword ptr [ebp - 0x40], xmm0
005197c9 f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
005197ce f30f5805b4f75600         addss      xmm0, dword ptr [0x56f7b4]
005197d6 f30f1145c4               movss      dword ptr [ebp - 0x3c], xmm0
005197db 8b4580                   mov        eax, dword ptr [ebp - 0x80]
005197de 83c004                   add        eax, 4
005197e1 894580                   mov        dword ptr [ebp - 0x80], eax
005197e4 e96dfdffff               jmp        0x519556
005197e9 6a02                     push       2
005197eb 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005197f1 e88ac9faff               call       0x4c6180
005197f6 6a01                     push       1
005197f8 6a01                     push       1
005197fa 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00519800 e81beef9ff               call       0x4b8620
00519805 6aff                     push       -1
00519807 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0051980d e8be72f5ff               call       0x470ad0
00519812 68000000ff               push       0xff000000
00519817 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0051981d e88e72f5ff               call       0x470ab0
00519822 90                       nop        
00519823 b801000000               mov        eax, 1
00519828 5f                       pop        edi
00519829 5e                       pop        esi
0051982a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051982d 33cd                     xor        ecx, ebp
0051982f e8ba910200               call       0x5429ee
00519834 8be5                     mov        esp, ebp
00519836 5d                       pop        ebp
00519837 c3                       ret        
00519838 cc                       int3       
00519839 cc                       int3       
0051983a cc                       int3       
0051983b cc                       int3       
0051983c cc                       int3       
0051983d cc                       int3       
0051983e cc                       int3       
0051983f cc                       int3       
