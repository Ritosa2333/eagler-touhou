00484a20 55                       push       ebp
00484a21 8bec                     mov        ebp, esp
00484a23 81ec18010000             sub        esp, 0x118
00484a29 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00484a2e 33c5                     xor        eax, ebp
00484a30 8945fc                   mov        dword ptr [ebp - 4], eax
00484a33 894da0                   mov        dword ptr [ebp - 0x60], ecx
00484a36 c7459c00000000           mov        dword ptr [ebp - 0x64], 0
00484a3d c7857cffffff00000000     mov        dword ptr [ebp - 0x84], 0
00484a47 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484a4a 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
00484a4d 898d20ffffff             mov        dword ptr [ebp - 0xe0], ecx
00484a53 6a00                     push       0
00484a55 8b8d20ffffff             mov        ecx, dword ptr [ebp - 0xe0]
00484a5b e860190000               call       0x4863c0
00484a60 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00484a63 8b421c                   mov        eax, dword ptr [edx + 0x1c]
00484a66 898504ffffff             mov        dword ptr [ebp - 0xfc], eax
00484a6c 6808fc5600               push       0x56fc08
00484a71 8b8d04ffffff             mov        ecx, dword ptr [ebp - 0xfc]
00484a77 e874c0feff               call       0x470af0
00484a7c 90                       nop        
00484a7d 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484a80 8b5114                   mov        edx, dword ptr [ecx + 0x14]
00484a83 d1ea                     shr        edx, 1
00484a85 83e201                   and        edx, 1
00484a88 0f8461070000             je         0x4851ef
00484a8e 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484a91 f30f10407c               movss      xmm0, dword ptr [eax + 0x7c]
00484a96 0f2f05e8e05600           comiss     xmm0, dword ptr [0x56e0e8]
00484a9d 0f864c070000             jbe        0x4851ef
00484aa3 51                       push       ecx
00484aa4 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00484aac f30f110424               movss      dword ptr [esp], xmm0
00484ab1 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484ab4 81c1d8040000             add        ecx, 0x4d8
00484aba e8317bffff               call       0x47c5f0
00484abf 90                       nop        
00484ac0 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484ac3 8b5114                   mov        edx, dword ptr [ecx + 0x14]
00484ac6 c1ea06                   shr        edx, 6
00484ac9 83e201                   and        edx, 1
00484acc 0f858e000000             jne        0x484b60
00484ad2 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484ad5 8b4814                   mov        ecx, dword ptr [eax + 0x14]
00484ad8 c1e904                   shr        ecx, 4
00484adb 83e101                   and        ecx, 1
00484ade 7536                     jne        0x484b16
00484ae0 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00484ae3 8b8a20050000             mov        ecx, dword ptr [edx + 0x520]
00484ae9 e8a2b4f8ff               call       0x40ff90
00484aee 898500ffffff             mov        dword ptr [ebp - 0x100], eax
00484af4 8b4508                   mov        eax, dword ptr [ebp + 8]
00484af7 50                       push       eax
00484af8 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484afb 83c17c                   add        ecx, 0x7c
00484afe 51                       push       ecx
00484aff 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00484b02 83c264                   add        edx, 0x64
00484b05 52                       push       edx
00484b06 8b8d00ffffff             mov        ecx, dword ptr [ebp - 0x100]
00484b0c e8cf410700               call       0x4f8ce0
00484b11 89459c                   mov        dword ptr [ebp - 0x64], eax
00484b14 eb45                     jmp        0x484b5b
00484b16 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484b19 8b8820050000             mov        ecx, dword ptr [eax + 0x520]
00484b1f e86cb4f8ff               call       0x40ff90
00484b24 894590                   mov        dword ptr [ebp - 0x70], eax
00484b27 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484b2a f30f10417c               movss      xmm0, dword ptr [ecx + 0x7c]
00484b2f f30f1185fcfeffff         movss      dword ptr [ebp - 0x104], xmm0
00484b37 8b5508                   mov        edx, dword ptr [ebp + 8]
00484b3a 52                       push       edx
00484b3b 51                       push       ecx
00484b3c f30f1085fcfeffff         movss      xmm0, dword ptr [ebp - 0x104]
00484b44 f30f110424               movss      dword ptr [esp], xmm0
00484b49 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484b4c 83c064                   add        eax, 0x64
00484b4f 50                       push       eax
00484b50 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00484b53 e898440700               call       0x4f8ff0
00484b58 89459c                   mov        dword ptr [ebp - 0x64], eax
00484b5b e9a5000000               jmp        0x484c05
00484b60 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484b63 8b5114                   mov        edx, dword ptr [ecx + 0x14]
00484b66 c1ea04                   shr        edx, 4
00484b69 83e201                   and        edx, 1
00484b6c 7550                     jne        0x484bbe
00484b6e 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484b71 83c07c                   add        eax, 0x7c
00484b74 89458c                   mov        dword ptr [ebp - 0x74], eax
00484b77 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484b7a 51                       push       ecx
00484b7b f30f104148               movss      xmm0, dword ptr [ecx + 0x48]
00484b80 f30f110424               movss      dword ptr [esp], xmm0
00484b85 8d55f4                   lea        edx, [ebp - 0xc]
00484b88 52                       push       edx
00484b89 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00484b8c e82f48faff               call       0x4293c0
00484b91 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484b94 8b8820050000             mov        ecx, dword ptr [eax + 0x520]
00484b9a e8f1b3f8ff               call       0x40ff90
00484b9f 894588                   mov        dword ptr [ebp - 0x78], eax
00484ba2 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00484ba5 51                       push       ecx
00484ba6 8d55f4                   lea        edx, [ebp - 0xc]
00484ba9 52                       push       edx
00484baa 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484bad 83c064                   add        eax, 0x64
00484bb0 50                       push       eax
00484bb1 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00484bb4 e827410700               call       0x4f8ce0
00484bb9 89459c                   mov        dword ptr [ebp - 0x64], eax
00484bbc eb47                     jmp        0x484c05
00484bbe 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484bc1 8b8920050000             mov        ecx, dword ptr [ecx + 0x520]
00484bc7 e8c4b3f8ff               call       0x40ff90
00484bcc 894580                   mov        dword ptr [ebp - 0x80], eax
00484bcf 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00484bd2 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484bd5 f30f10427c               movss      xmm0, dword ptr [edx + 0x7c]
00484bda f30f594048               mulss      xmm0, dword ptr [eax + 0x48]
00484bdf f30f114584               movss      dword ptr [ebp - 0x7c], xmm0
00484be4 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00484be7 51                       push       ecx
00484be8 51                       push       ecx
00484be9 f30f104584               movss      xmm0, dword ptr [ebp - 0x7c]
00484bee f30f110424               movss      dword ptr [esp], xmm0
00484bf3 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00484bf6 83c264                   add        edx, 0x64
00484bf9 52                       push       edx
00484bfa 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00484bfd e8ee430700               call       0x4f8ff0
00484c02 89459c                   mov        dword ptr [ebp - 0x64], eax
00484c05 83bd7cffffff00           cmp        dword ptr [ebp - 0x84], 0
00484c0c 740d                     je         0x484c1b
00484c0e 837d9c00                 cmp        dword ptr [ebp - 0x64], 0
00484c12 7507                     jne        0x484c1b
00484c14 c7459c02000000           mov        dword ptr [ebp - 0x64], 2
00484c1b 837d9c00                 cmp        dword ptr [ebp - 0x64], 0
00484c1f 7569                     jne        0x484c8a
00484c21 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484c24 05c0040000               add        eax, 0x4c0
00484c29 898578ffffff             mov        dword ptr [ebp - 0x88], eax
00484c2f 6a00                     push       0
00484c31 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00484c37 e804e9f9ff               call       0x423540
00484c3c 90                       nop        
00484c3d 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484c40 81c1c0040000             add        ecx, 0x4c0
00484c46 898d74ffffff             mov        dword ptr [ebp - 0x8c], ecx
00484c4c 6a3c                     push       0x3c
00484c4e 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
00484c54 e867e9f9ff               call       0x4235c0
00484c59 0fb6d0                   movzx      edx, al
00484c5c 85d2                     test       edx, edx
00484c5e 742a                     je         0x484c8a
00484c60 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484c63 83b8d404000000           cmp        dword ptr [eax + 0x4d4], 0
00484c6a 751e                     jne        0x484c8a
00484c6c 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484c6f c781d404000003000000     mov        dword ptr [ecx + 0x4d4], 3
00484c79 6a00                     push       0
00484c7b 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484c7e 81c1a0040000             add        ecx, 0x4a0
00484c84 e897e8f9ff               call       0x423520
00484c89 90                       nop        
00484c8a 837d9c01                 cmp        dword ptr [ebp - 0x64], 1
00484c8e 0f85b7010000             jne        0x484e4b
00484c94 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00484c97 837a1800                 cmp        dword ptr [edx + 0x18], 0
00484c9b 0f85aa010000             jne        0x484e4b
00484ca1 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484ca4 c7405003000000           mov        dword ptr [eax + 0x50], 3
00484cab 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484cae 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
00484cb1 899570ffffff             mov        dword ptr [ebp - 0x90], edx
00484cb7 6a01                     push       1
00484cb9 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
00484cbf e82c3cfbff               call       0x4388f0
00484cc4 90                       nop        
00484cc5 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484cc8 83c15c                   add        ecx, 0x5c
00484ccb e8400d0000               call       0x485a10
00484cd0 85c0                     test       eax, eax
00484cd2 741a                     je         0x484cee
00484cd4 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484cd7 83c05c                   add        eax, 0x5c
00484cda 89856cffffff             mov        dword ptr [ebp - 0x94], eax
00484ce0 6a01                     push       1
00484ce2 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00484ce8 e8a3a2fcff               call       0x44ef90
00484ced 90                       nop        
00484cee 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484cf1 83793400                 cmp        dword ptr [ecx + 0x34], 0
00484cf5 0f8c4b010000             jl         0x484e46
00484cfb 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00484cfe 8b8a20050000             mov        ecx, dword ptr [edx + 0x520]
00484d04 e8f775f8ff               call       0x40c300
00484d09 8bc8                     mov        ecx, eax
00484d0b e8a00c0000               call       0x4859b0
00484d10 898564ffffff             mov        dword ptr [ebp - 0x9c], eax
00484d16 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484d19 8b4834                   mov        ecx, dword ptr [eax + 0x34]
00484d1c 898d68ffffff             mov        dword ptr [ebp - 0x98], ecx
00484d22 6a00                     push       0
00484d24 6aff                     push       -1
00484d26 51                       push       ecx
00484d27 0f57c0                   xorps      xmm0, xmm0
00484d2a f30f110424               movss      dword ptr [esp], xmm0
00484d2f 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00484d32 83c264                   add        edx, 0x64
00484d35 52                       push       edx
00484d36 8b8568ffffff             mov        eax, dword ptr [ebp - 0x98]
00484d3c 50                       push       eax
00484d3d 6a00                     push       0
00484d3f 8d8df4feffff             lea        ecx, [ebp - 0x10c]
00484d45 51                       push       ecx
00484d46 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
00484d4c e88f43ffff               call       0x4790e0
00484d51 8bc8                     mov        ecx, eax
00484d53 e87881fcff               call       0x44ced0
00484d58 894594                   mov        dword ptr [ebp - 0x6c], eax
00484d5b 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00484d5e 8b4214                   mov        eax, dword ptr [edx + 0x14]
00484d61 c1e80b                   shr        eax, 0xb
00484d64 83e003                   and        eax, 3
00484d67 83f801                   cmp        eax, 1
00484d6a 7509                     jne        0x484d75
00484d6c 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00484d6f e8fc150000               call       0x486370
00484d74 90                       nop        
00484d75 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484d78 83c170                   add        ecx, 0x70
00484d7b 898d60ffffff             mov        dword ptr [ebp - 0xa0], ecx
00484d81 b9e4ef5a00               mov        ecx, 0x5aefe4
00484d86 e85545faff               call       0x4292e0
00484d8b 51                       push       ecx
00484d8c d91c24                   fstp       dword ptr [esp]
00484d8f 8d95e8feffff             lea        edx, [ebp - 0x118]
00484d95 52                       push       edx
00484d96 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
00484d9c e84f45faff               call       0x4292f0
00484da1 89855cffffff             mov        dword ptr [ebp - 0xa4], eax
00484da7 51                       push       ecx
00484da8 f30f100534e75600         movss      xmm0, dword ptr [0x56e734]
00484db0 f30f110424               movss      dword ptr [esp], xmm0
00484db5 8d45e8                   lea        eax, [ebp - 0x18]
00484db8 50                       push       eax
00484db9 8b8d5cffffff             mov        ecx, dword ptr [ebp - 0xa4]
00484dbf e82c45faff               call       0x4292f0
00484dc4 90                       nop        
00484dc5 8d4de8                   lea        ecx, [ebp - 0x18]
00484dc8 51                       push       ecx
00484dc9 6808fc5600               push       0x56fc08
00484dce 6a06                     push       6
00484dd0 6a1e                     push       0x1e
00484dd2 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00484dd5 e8c6c6fdff               call       0x4614a0
00484dda 90                       nop        
00484ddb 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00484dde 8b4214                   mov        eax, dword ptr [edx + 0x14]
00484de1 c1e80b                   shr        eax, 0xb
00484de4 83e003                   and        eax, 3
00484de7 83f802                   cmp        eax, 2
00484dea 755a                     jne        0x484e46
00484dec 8d4da4                   lea        ecx, [ebp - 0x5c]
00484def e83c6cffff               call       0x47ba30
00484df4 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484df7 83c164                   add        ecx, 0x64
00484dfa 8b11                     mov        edx, dword ptr [ecx]
00484dfc 8955a4                   mov        dword ptr [ebp - 0x5c], edx
00484dff 8b4104                   mov        eax, dword ptr [ecx + 4]
00484e02 8945a8                   mov        dword ptr [ebp - 0x58], eax
00484e05 8b4908                   mov        ecx, dword ptr [ecx + 8]
00484e08 894dac                   mov        dword ptr [ebp - 0x54], ecx
00484e0b 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00484e0e 8b8288000000             mov        eax, dword ptr [edx + 0x88]
00484e14 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00484e17 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484e1a 8b8920050000             mov        ecx, dword ptr [ecx + 0x520]
00484e20 e8ab7cf9ff               call       0x41cad0
00484e25 898558ffffff             mov        dword ptr [ebp - 0xa8], eax
00484e2b 6a00                     push       0
00484e2d 8d55a4                   lea        edx, [ebp - 0x5c]
00484e30 52                       push       edx
00484e31 6a04                     push       4
00484e33 8d85f8feffff             lea        eax, [ebp - 0x108]
00484e39 50                       push       eax
00484e3a 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
00484e40 e82b8d0100               call       0x49db70
00484e45 90                       nop        
00484e46 e9a4030000               jmp        0x4851ef
00484e4b 837d9c02                 cmp        dword ptr [ebp - 0x64], 2
00484e4f 0f855a030000             jne        0x4851af
00484e55 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484e58 83b9d404000000           cmp        dword ptr [ecx + 0x4d4], 0
00484e5f 0f84b2010000             je         0x485017
00484e65 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00484e68 81c2a0040000             add        edx, 0x4a0
00484e6e 899554ffffff             mov        dword ptr [ebp - 0xac], edx
00484e74 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484e77 8b88d0040000             mov        ecx, dword ptr [eax + 0x4d0]
00484e7d 51                       push       ecx
00484e7e 8b8d54ffffff             mov        ecx, dword ptr [ebp - 0xac]
00484e84 e8773fffff               call       0x478e00
00484e89 85c0                     test       eax, eax
00484e8b 0f8486010000             je         0x485017
00484e91 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00484e94 8b8a20050000             mov        ecx, dword ptr [edx + 0x520]
00484e9a e8f1b0f8ff               call       0x40ff90
00484e9f 89854cffffff             mov        dword ptr [ebp - 0xb4], eax
00484ea5 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484ea8 81c188000000             add        ecx, 0x88
00484eae e84d74f8ff               call       0x40c300
00484eb3 898550ffffff             mov        dword ptr [ebp - 0xb0], eax
00484eb9 8b8550ffffff             mov        eax, dword ptr [ebp - 0xb0]
00484ebf 50                       push       eax
00484ec0 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484ec3 83c164                   add        ecx, 0x64
00484ec6 51                       push       ecx
00484ec7 8b8d4cffffff             mov        ecx, dword ptr [ebp - 0xb4]
00484ecd e8be3c0700               call       0x4f8b90
00484ed2 6a00                     push       0
00484ed4 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484ed7 81c1a0040000             add        ecx, 0x4a0
00484edd e83ee6f9ff               call       0x423520
00484ee2 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00484ee5 8b82d4040000             mov        eax, dword ptr [edx + 0x4d4]
00484eeb 83e801                   sub        eax, 1
00484eee 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00484ef1 8981d4040000             mov        dword ptr [ecx + 0x4d4], eax
00484ef7 6a00                     push       0
00484ef9 e882f1fdff               call       0x464080
00484efe 83c404                   add        esp, 4
00484f01 8bc8                     mov        ecx, eax
00484f03 e8380b0000               call       0x485a40
00484f08 0fb6d0                   movzx      edx, al
00484f0b 85d2                     test       edx, edx
00484f0d 0f8404010000             je         0x485017
00484f13 6a00                     push       0
00484f15 e866f1fdff               call       0x464080
00484f1a 83c404                   add        esp, 4
00484f1d 898548ffffff             mov        dword ptr [ebp - 0xb8], eax
00484f23 6a01                     push       1
00484f25 8b8d48ffffff             mov        ecx, dword ptr [ebp - 0xb8]
00484f2b e820f9ffff               call       0x484850
00484f30 90                       nop        
00484f31 6a00                     push       0
00484f33 e848f1fdff               call       0x464080
00484f38 83c404                   add        esp, 4
00484f3b 8bc8                     mov        ecx, eax
00484f3d e8de090000               call       0x485920
00484f42 83f816                   cmp        eax, 0x16
00484f45 0f8ecc000000             jle        0x485017
00484f4b 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00484f4e 83781800                 cmp        dword ptr [eax + 0x18], 0
00484f52 0f85bf000000             jne        0x485017
00484f58 6a00                     push       0
00484f5a e8d1f2fdff               call       0x464230
00484f5f 83c404                   add        esp, 4
00484f62 8bc8                     mov        ecx, eax
00484f64 e8670a0000               call       0x4859d0
00484f69 898544ffffff             mov        dword ptr [ebp - 0xbc], eax
00484f6f 6a01                     push       1
00484f71 8b8d44ffffff             mov        ecx, dword ptr [ebp - 0xbc]
00484f77 e834160000               call       0x4865b0
00484f7c 90                       nop        
00484f7d 6a00                     push       0
00484f7f e8fcf0fdff               call       0x464080
00484f84 83c404                   add        esp, 4
00484f87 8bc8                     mov        ecx, eax
00484f89 e872f4fdff               call       0x464400
00484f8e 0fb6c8                   movzx      ecx, al
00484f91 85c9                     test       ecx, ecx
00484f93 7441                     je         0x484fd6
00484f95 6a00                     push       0
00484f97 e8e4f0fdff               call       0x464080
00484f9c 83c404                   add        esp, 4
00484f9f 89853cffffff             mov        dword ptr [ebp - 0xc4], eax
00484fa5 b9a8a45b00               mov        ecx, 0x5ba4a8
00484faa e831eff9ff               call       0x423ee0
00484faf 33d2                     xor        edx, edx
00484fb1 b90a000000               mov        ecx, 0xa
00484fb6 f7f1                     div        ecx
00484fb8 83c207                   add        edx, 7
00484fbb 899540ffffff             mov        dword ptr [ebp - 0xc0], edx
00484fc1 8b9540ffffff             mov        edx, dword ptr [ebp - 0xc0]
00484fc7 52                       push       edx
00484fc8 8b8d3cffffff             mov        ecx, dword ptr [ebp - 0xc4]
00484fce e85d140000               call       0x486430
00484fd3 90                       nop        
00484fd4 eb3c                     jmp        0x485012
00484fd6 6a00                     push       0
00484fd8 e8a3f0fdff               call       0x464080
00484fdd 83c404                   add        esp, 4
00484fe0 898534ffffff             mov        dword ptr [ebp - 0xcc], eax
00484fe6 b9a8a45b00               mov        ecx, 0x5ba4a8
00484feb e8f0eef9ff               call       0x423ee0
00484ff0 33d2                     xor        edx, edx
00484ff2 b90a000000               mov        ecx, 0xa
00484ff7 f7f1                     div        ecx
00484ff9 899538ffffff             mov        dword ptr [ebp - 0xc8], edx
00484fff 8b9538ffffff             mov        edx, dword ptr [ebp - 0xc8]
00485005 52                       push       edx
00485006 8b8d34ffffff             mov        ecx, dword ptr [ebp - 0xcc]
0048500c e81f140000               call       0x486430
00485011 90                       nop        
00485012 e98afcffff               jmp        0x484ca1
00485017 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
0048501a 05a0040000               add        eax, 0x4a0
0048501f 898530ffffff             mov        dword ptr [ebp - 0xd0], eax
00485025 6a00                     push       0
00485027 8b8d30ffffff             mov        ecx, dword ptr [ebp - 0xd0]
0048502d e80ee5f9ff               call       0x423540
00485032 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00485035 81c1b0040000             add        ecx, 0x4b0
0048503b 898d2cffffff             mov        dword ptr [ebp - 0xd4], ecx
00485041 6a00                     push       0
00485043 8b8d2cffffff             mov        ecx, dword ptr [ebp - 0xd4]
00485049 e8f2e4f9ff               call       0x423540
0048504e 6a00                     push       0
00485050 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00485053 81c1c0040000             add        ecx, 0x4c0
00485059 e8c2e4f9ff               call       0x423520
0048505e 90                       nop        
0048505f 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
00485062 8b8294000000             mov        eax, dword ptr [edx + 0x94]
00485068 d1e8                     shr        eax, 1
0048506a 83e001                   and        eax, 1
0048506d 0f853a010000             jne        0x4851ad
00485073 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00485076 81c1b0040000             add        ecx, 0x4b0
0048507c 898d28ffffff             mov        dword ptr [ebp - 0xd8], ecx
00485082 6a01                     push       1
00485084 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
0048508a e801e5f9ff               call       0x423590
0048508f 0fb6d0                   movzx      edx, al
00485092 85d2                     test       edx, edx
00485094 740c                     je         0x4850a2
00485096 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00485099 83c15c                   add        ecx, 0x5c
0048509c e86f120000               call       0x486310
004850a1 90                       nop        
004850a2 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
004850a5 05b0040000               add        eax, 0x4b0
004850aa 898524ffffff             mov        dword ptr [ebp - 0xdc], eax
004850b0 6a02                     push       2
004850b2 8b8d24ffffff             mov        ecx, dword ptr [ebp - 0xdc]
004850b8 e82372fdff               call       0x45c2e0
004850bd b9d0000000               mov        ecx, 0xd0
004850c2 2bc8                     sub        ecx, eax
004850c4 894d98                   mov        dword ptr [ebp - 0x68], ecx
004850c7 817d98ff000000           cmp        dword ptr [ebp - 0x68], 0xff
004850ce 7e07                     jle        0x4850d7
004850d0 c74598ff000000           mov        dword ptr [ebp - 0x68], 0xff
004850d7 837d9860                 cmp        dword ptr [ebp - 0x68], 0x60
004850db 7d07                     jge        0x4850e4
004850dd c7459860000000           mov        dword ptr [ebp - 0x68], 0x60
004850e4 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
004850e7 8b421c                   mov        eax, dword ptr [edx + 0x1c]
004850ea 898508ffffff             mov        dword ptr [ebp - 0xf8], eax
004850f0 6a01                     push       1
004850f2 8b8d08ffffff             mov        ecx, dword ptr [ebp - 0xf8]
004850f8 e8c3120000               call       0x4863c0
004850fd 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00485100 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
00485103 89951cffffff             mov        dword ptr [ebp - 0xe4], edx
00485109 68ff000000               push       0xff
0048510e 6880000000               push       0x80
00485113 0fb64598                 movzx      eax, byte ptr [ebp - 0x68]
00485117 50                       push       eax
00485118 68ff000000               push       0xff
0048511d 8b8d1cffffff             mov        ecx, dword ptr [ebp - 0xe4]
00485123 e888130000               call       0x4864b0
00485128 51                       push       ecx
00485129 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00485131 f30f110424               movss      dword ptr [esp], xmm0
00485136 b9c4a45b00               mov        ecx, 0x5ba4c4
0048513b e87047faff               call       0x4298b0
00485140 d99d18ffffff             fstp       dword ptr [ebp - 0xe8]
00485146 51                       push       ecx
00485147 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
0048514f f30f110424               movss      dword ptr [esp], xmm0
00485154 b9c4a45b00               mov        ecx, 0x5ba4c4
00485159 e85247faff               call       0x4298b0
0048515e d99d14ffffff             fstp       dword ptr [ebp - 0xec]
00485164 51                       push       ecx
00485165 0f57c0                   xorps      xmm0, xmm0
00485168 f30f110424               movss      dword ptr [esp], xmm0
0048516d 51                       push       ecx
0048516e f30f108518ffffff         movss      xmm0, dword ptr [ebp - 0xe8]
00485176 f30f110424               movss      dword ptr [esp], xmm0
0048517b 51                       push       ecx
0048517c f30f108514ffffff         movss      xmm0, dword ptr [ebp - 0xec]
00485184 f30f110424               movss      dword ptr [esp], xmm0
00485189 8d4ddc                   lea        ecx, [ebp - 0x24]
0048518c e83fdcf9ff               call       0x422dd0
00485191 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00485194 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
00485197 899510ffffff             mov        dword ptr [ebp - 0xf0], edx
0048519d 8d45dc                   lea        eax, [ebp - 0x24]
004851a0 50                       push       eax
004851a1 8b8d10ffffff             mov        ecx, dword ptr [ebp - 0xf0]
004851a7 e844b9feff               call       0x470af0
004851ac 90                       nop        
004851ad eb40                     jmp        0x4851ef
004851af 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
004851b2 81c1b0040000             add        ecx, 0x4b0
004851b8 898d0cffffff             mov        dword ptr [ebp - 0xf4], ecx
004851be 6a00                     push       0
004851c0 8b8d0cffffff             mov        ecx, dword ptr [ebp - 0xf4]
004851c6 e85574ffff               call       0x47c620
004851cb 0fb6d0                   movzx      edx, al
004851ce 85d2                     test       edx, edx
004851d0 740c                     je         0x4851de
004851d2 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
004851d5 83c15c                   add        ecx, 0x5c
004851d8 e873110000               call       0x486350
004851dd 90                       nop        
004851de 6a00                     push       0
004851e0 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
004851e3 81c1b0040000             add        ecx, 0x4b0
004851e9 e832e3f9ff               call       0x423520
004851ee 90                       nop        
004851ef 8b459c                   mov        eax, dword ptr [ebp - 0x64]
004851f2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004851f5 33cd                     xor        ecx, ebp
004851f7 e8f2d70b00               call       0x5429ee
004851fc 8be5                     mov        esp, ebp
004851fe 5d                       pop        ebp
004851ff c20400                   ret        4
00485202 cc                       int3       
00485203 cc                       int3       
00485204 cc                       int3       
00485205 cc                       int3       
00485206 cc                       int3       
00485207 cc                       int3       
00485208 cc                       int3       
00485209 cc                       int3       
0048520a cc                       int3       
0048520b cc                       int3       
0048520c cc                       int3       
0048520d cc                       int3       
0048520e cc                       int3       
0048520f cc                       int3       
