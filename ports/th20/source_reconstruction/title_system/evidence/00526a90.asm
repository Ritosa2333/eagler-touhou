00526a90 55                       push       ebp
00526a91 8bec                     mov        ebp, esp
00526a93 81ec5c010000             sub        esp, 0x15c
00526a99 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00526a9e 33c5                     xor        eax, ebp
00526aa0 8945fc                   mov        dword ptr [ebp - 4], eax
00526aa3 898d78ffffff             mov        dword ptr [ebp - 0x88], ecx
00526aa9 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00526aaf 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00526ab2 898d60ffffff             mov        dword ptr [ebp - 0xa0], ecx
00526ab8 83bd60ffffff04           cmp        dword ptr [ebp - 0xa0], 4
00526abf 0f87fa0c0000             ja         0x5277bf
00526ac5 8b9560ffffff             mov        edx, dword ptr [ebp - 0xa0]
00526acb ff2495d4775200           jmp        dword ptr [edx*4 + 0x5277d4]
00526ad2 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00526ad8 83c024                   add        eax, 0x24
00526adb 8985dcfeffff             mov        dword ptr [ebp - 0x124], eax
00526ae1 6a19                     push       0x19
00526ae3 8b8ddcfeffff             mov        ecx, dword ptr [ebp - 0x124]
00526ae9 e8f259f7ff               call       0x49c4e0
00526aee 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526af4 83c124                   add        ecx, 0x24
00526af7 898da8feffff             mov        dword ptr [ebp - 0x158], ecx
00526afd 6a01                     push       1
00526aff 8b8da8feffff             mov        ecx, dword ptr [ebp - 0x158]
00526b05 e8d690f9ff               call       0x4bfbe0
00526b0a 6a00                     push       0
00526b0c 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526b12 83c124                   add        ecx, 0x24
00526b15 e83682f9ff               call       0x4bed50
00526b1a 6a08                     push       8
00526b1c b968a55b00               mov        ecx, 0x5ba568
00526b21 e83a78f9ff               call       0x4be360
00526b26 90                       nop        
00526b27 c7856cffffff01000000     mov        dword ptr [ebp - 0x94], 1
00526b31 eb0f                     jmp        0x526b42
00526b33 8b956cffffff             mov        edx, dword ptr [ebp - 0x94]
00526b39 83c201                   add        edx, 1
00526b3c 89956cffffff             mov        dword ptr [ebp - 0x94], edx
00526b42 83bd6cffffff19           cmp        dword ptr [ebp - 0x94], 0x19
00526b49 7f62                     jg         0x526bad
00526b4b 8b856cffffff             mov        eax, dword ptr [ebp - 0x94]
00526b51 50                       push       eax
00526b52 6834265700               push       0x572634
00526b57 6a40                     push       0x40
00526b59 8d8d7cffffff             lea        ecx, [ebp - 0x84]
00526b5f 51                       push       ecx
00526b60 e8bb86efff               call       0x41f220
00526b65 83c410                   add        esp, 0x10
00526b68 8d957cffffff             lea        edx, [ebp - 0x84]
00526b6e 52                       push       edx
00526b6f e86c3dfeff               call       0x50a8e0
00526b74 83c404                   add        esp, 4
00526b77 898530ffffff             mov        dword ptr [ebp - 0xd0], eax
00526b7d 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00526b83 0540570000               add        eax, 0x5740
00526b88 898534ffffff             mov        dword ptr [ebp - 0xcc], eax
00526b8e 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00526b94 83e901                   sub        ecx, 1
00526b97 51                       push       ecx
00526b98 8b8d34ffffff             mov        ecx, dword ptr [ebp - 0xcc]
00526b9e e8ddd9eeff               call       0x414580
00526ba3 8b9530ffffff             mov        edx, dword ptr [ebp - 0xd0]
00526ba9 8910                     mov        dword ptr [eax], edx
00526bab eb86                     jmp        0x526b33
00526bad 6a00                     push       0
00526baf 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526bb5 e8965b0000               call       0x52c750
00526bba 85c0                     test       eax, eax
00526bbc 751b                     jne        0x526bd9
00526bbe 6a00                     push       0
00526bc0 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526bc6 e8b5630000               call       0x52cf80
00526bcb 6a00                     push       0
00526bcd 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526bd3 e8b8600000               call       0x52cc90
00526bd8 90                       nop        
00526bd9 6a2a                     push       0x2a
00526bdb 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526be1 e89a630000               call       0x52cf80
00526be6 6a01                     push       1
00526be8 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526bee e85d620000               call       0x52ce50
00526bf3 90                       nop        
00526bf4 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00526bfa 0554010000               add        eax, 0x154
00526bff 89852cffffff             mov        dword ptr [ebp - 0xd4], eax
00526c05 6a06                     push       6
00526c07 8b8d2cffffff             mov        ecx, dword ptr [ebp - 0xd4]
00526c0d e85ea4f3ff               call       0x461070
00526c12 0fb6c8                   movzx      ecx, al
00526c15 85c9                     test       ecx, ecx
00526c17 740e                     je         0x526c27
00526c19 6a02                     push       2
00526c1b 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526c21 e82a620000               call       0x52ce50
00526c26 90                       nop        
00526c27 e9930b0000               jmp        0x5277bf
00526c2c 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526c32 83c124                   add        ecx, 0x24
00526c35 e8c68ff9ff               call       0x4bfc00
00526c3a 90                       nop        
00526c3b 33d2                     xor        edx, edx
00526c3d 753a                     jne        0x526c79
00526c3f b804000000               mov        eax, 4
00526c44 6bc800                   imul       ecx, eax, 0
00526c47 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
00526c4e 7429                     je         0x526c79
00526c50 ba04000000               mov        edx, 4
00526c55 6bc200                   imul       eax, edx, 0
00526c58 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
00526c5e 898d28ffffff             mov        dword ptr [ebp - 0xd8], ecx
00526c64 6a10                     push       0x10
00526c66 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
00526c6c e80f36efff               call       0x41a280
00526c71 89855cffffff             mov        dword ptr [ebp - 0xa4], eax
00526c77 eb0a                     jmp        0x526c83
00526c79 c7855cffffff00000000     mov        dword ptr [ebp - 0xa4], 0
00526c83 83bd5cffffff00           cmp        dword ptr [ebp - 0xa4], 0
00526c8a 741d                     je         0x526ca9
00526c8c 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
00526c92 83c224                   add        edx, 0x24
00526c95 899524ffffff             mov        dword ptr [ebp - 0xdc], edx
00526c9b 6a00                     push       0
00526c9d 8b8d24ffffff             mov        ecx, dword ptr [ebp - 0xdc]
00526ca3 e89883f9ff               call       0x4bf040
00526ca8 90                       nop        
00526ca9 33c0                     xor        eax, eax
00526cab 753a                     jne        0x526ce7
00526cad b904000000               mov        ecx, 4
00526cb2 6bd100                   imul       edx, ecx, 0
00526cb5 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00526cbc 7429                     je         0x526ce7
00526cbe b804000000               mov        eax, 4
00526cc3 6bc800                   imul       ecx, eax, 0
00526cc6 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00526ccc 899520ffffff             mov        dword ptr [ebp - 0xe0], edx
00526cd2 6a20                     push       0x20
00526cd4 8b8d20ffffff             mov        ecx, dword ptr [ebp - 0xe0]
00526cda e8a135efff               call       0x41a280
00526cdf 898558ffffff             mov        dword ptr [ebp - 0xa8], eax
00526ce5 eb0a                     jmp        0x526cf1
00526ce7 c78558ffffff00000000     mov        dword ptr [ebp - 0xa8], 0
00526cf1 83bd58ffffff00           cmp        dword ptr [ebp - 0xa8], 0
00526cf8 741d                     je         0x526d17
00526cfa 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00526d00 83c024                   add        eax, 0x24
00526d03 89851cffffff             mov        dword ptr [ebp - 0xe4], eax
00526d09 6a00                     push       0
00526d0b 8b8d1cffffff             mov        ecx, dword ptr [ebp - 0xe4]
00526d11 e80a83f9ff               call       0x4bf020
00526d16 90                       nop        
00526d17 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526d1d 83c124                   add        ecx, 0x24
00526d20 e80b63f3ff               call       0x45d030
00526d25 85c0                     test       eax, eax
00526d27 740f                     je         0x526d38
00526d29 6a00                     push       0
00526d2b 6a0a                     push       0xa
00526d2d b930a85b00               mov        ecx, 0x5ba830
00526d32 e83900f0ff               call       0x426d70
00526d37 90                       nop        
00526d38 33c9                     xor        ecx, ecx
00526d3a 753d                     jne        0x526d79
00526d3c ba04000000               mov        edx, 4
00526d41 6bc200                   imul       eax, edx, 0
00526d44 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
00526d4b 742c                     je         0x526d79
00526d4d b904000000               mov        ecx, 4
00526d52 6bd100                   imul       edx, ecx, 0
00526d55 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
00526d5b 898518ffffff             mov        dword ptr [ebp - 0xe8], eax
00526d61 6806010000               push       0x106
00526d66 8b8d18ffffff             mov        ecx, dword ptr [ebp - 0xe8]
00526d6c e88f2eefff               call       0x419c00
00526d71 898554ffffff             mov        dword ptr [ebp - 0xac], eax
00526d77 eb0a                     jmp        0x526d83
00526d79 c78554ffffff00000000     mov        dword ptr [ebp - 0xac], 0
00526d83 83bd54ffffff00           cmp        dword ptr [ebp - 0xac], 0
00526d8a 7421                     je         0x526dad
00526d8c 6a04                     push       4
00526d8e 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526d94 e8b7600000               call       0x52ce50
00526d99 6a00                     push       0
00526d9b 6a09                     push       9
00526d9d b930a85b00               mov        ecx, 0x5ba830
00526da2 e8c9ffefff               call       0x426d70
00526da7 90                       nop        
00526da8 e9120a0000               jmp        0x5277bf
00526dad 33c9                     xor        ecx, ecx
00526daf 753d                     jne        0x526dee
00526db1 ba04000000               mov        edx, 4
00526db6 6bc200                   imul       eax, edx, 0
00526db9 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
00526dc0 742c                     je         0x526dee
00526dc2 b904000000               mov        ecx, 4
00526dc7 6bd100                   imul       edx, ecx, 0
00526dca 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
00526dd0 898514ffffff             mov        dword ptr [ebp - 0xec], eax
00526dd6 6801000800               push       0x80001
00526ddb 8b8d14ffffff             mov        ecx, dword ptr [ebp - 0xec]
00526de1 e81a2eefff               call       0x419c00
00526de6 898550ffffff             mov        dword ptr [ebp - 0xb0], eax
00526dec eb0a                     jmp        0x526df8
00526dee c78550ffffff00000000     mov        dword ptr [ebp - 0xb0], 0
00526df8 83bd50ffffff00           cmp        dword ptr [ebp - 0xb0], 0
00526dff 0f8467010000             je         0x526f6c
00526e05 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526e0b 83c124                   add        ecx, 0x24
00526e0e e8ed54eeff               call       0x40c300
00526e13 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526e19 898138570000             mov        dword ptr [ecx + 0x5738], eax
00526e1f 6a00                     push       0
00526e21 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526e27 81c1e8560000             add        ecx, 0x56e8
00526e2d e81e7ff9ff               call       0x4bed50
00526e32 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
00526e38 81c2e8560000             add        edx, 0x56e8
00526e3e 899510ffffff             mov        dword ptr [ebp - 0xf0], edx
00526e44 a17cff5a00               mov        eax, dword ptr [0x5aff7c]
00526e49 50                       push       eax
00526e4a e8d18f0200               call       0x54fe20
00526e4f 83c404                   add        esp, 4
00526e52 50                       push       eax
00526e53 8b8d10ffffff             mov        ecx, dword ptr [ebp - 0xf0]
00526e59 e88256f7ff               call       0x49c4e0
00526e5e 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526e64 81c1e8560000             add        ecx, 0x56e8
00526e6a 898d0cffffff             mov        dword ptr [ebp - 0xf4], ecx
00526e70 6a01                     push       1
00526e72 8b8d0cffffff             mov        ecx, dword ptr [ebp - 0xf4]
00526e78 e8638df9ff               call       0x4bfbe0
00526e7d 6a01                     push       1
00526e7f 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
00526e85 e88614feff               call       0x508310
00526e8a 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00526e90 e84bd2f3ff               call       0x4640e0
00526e95 83c00c                   add        eax, 0xc
00526e98 50                       push       eax
00526e99 6a0a                     push       0xa
00526e9b 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
00526ea1 81c2d4560000             add        edx, 0x56d4
00526ea7 52                       push       edx
00526ea8 e863170200               call       0x548610
00526ead 83c40c                   add        esp, 0xc
00526eb0 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00526eb6 c780e056000000000000     mov        dword ptr [eax + 0x56e0], 0
00526ec0 68ac245700               push       0x5724ac
00526ec5 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526ecb 81c1d4560000             add        ecx, 0x56d4
00526ed1 51                       push       ecx
00526ed2 e8398e0200               call       0x54fd10
00526ed7 83c408                   add        esp, 8
00526eda 85c0                     test       eax, eax
00526edc 7420                     je         0x526efe
00526ede 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
00526ee4 81c2e8560000             add        edx, 0x56e8
00526eea 899508ffffff             mov        dword ptr [ebp - 0xf8], edx
00526ef0 6a00                     push       0
00526ef2 8b8d08ffffff             mov        ecx, dword ptr [ebp - 0xf8]
00526ef8 e84381f9ff               call       0x4bf040
00526efd 90                       nop        
00526efe c78568ffffff08000000     mov        dword ptr [ebp - 0x98], 8
00526f08 eb0f                     jmp        0x526f19
00526f0a 8b8568ffffff             mov        eax, dword ptr [ebp - 0x98]
00526f10 83e801                   sub        eax, 1
00526f13 898568ffffff             mov        dword ptr [ebp - 0x98], eax
00526f19 83bd68ffffff00           cmp        dword ptr [ebp - 0x98], 0
00526f20 7e1c                     jle        0x526f3e
00526f22 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526f28 038d68ffffff             add        ecx, dword ptr [ebp - 0x98]
00526f2e 0fbe91d3560000           movsx      edx, byte ptr [ecx + 0x56d3]
00526f35 83fa20                   cmp        edx, 0x20
00526f38 7402                     je         0x526f3c
00526f3a eb02                     jmp        0x526f3e
00526f3c ebcc                     jmp        0x526f0a
00526f3e 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00526f44 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
00526f4a 8988e0560000             mov        dword ptr [eax + 0x56e0], ecx
00526f50 6a00                     push       0
00526f52 6a07                     push       7
00526f54 b930a85b00               mov        ecx, 0x5ba830
00526f59 e812feefff               call       0x426d70
00526f5e 6a03                     push       3
00526f60 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526f66 e8e55e0000               call       0x52ce50
00526f6b 90                       nop        
00526f6c e94e080000               jmp        0x5277bf
00526f71 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526f77 81c1e8560000             add        ecx, 0x56e8
00526f7d e87e8cf9ff               call       0x4bfc00
00526f82 90                       nop        
00526f83 33d2                     xor        edx, edx
00526f85 753a                     jne        0x526fc1
00526f87 b804000000               mov        eax, 4
00526f8c 6bc800                   imul       ecx, eax, 0
00526f8f 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
00526f96 7429                     je         0x526fc1
00526f98 ba04000000               mov        edx, 4
00526f9d 6bc200                   imul       eax, edx, 0
00526fa0 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
00526fa6 898d04ffffff             mov        dword ptr [ebp - 0xfc], ecx
00526fac 6a10                     push       0x10
00526fae 8b8d04ffffff             mov        ecx, dword ptr [ebp - 0xfc]
00526fb4 e8c732efff               call       0x41a280
00526fb9 89854cffffff             mov        dword ptr [ebp - 0xb4], eax
00526fbf eb0a                     jmp        0x526fcb
00526fc1 c7854cffffff00000000     mov        dword ptr [ebp - 0xb4], 0
00526fcb 83bd4cffffff00           cmp        dword ptr [ebp - 0xb4], 0
00526fd2 7414                     je         0x526fe8
00526fd4 6a0d                     push       0xd
00526fd6 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00526fdc 81c1e8560000             add        ecx, 0x56e8
00526fe2 e839b0fbff               call       0x4e2020
00526fe7 90                       nop        
00526fe8 33d2                     xor        edx, edx
00526fea 753a                     jne        0x527026
00526fec b804000000               mov        eax, 4
00526ff1 6bc800                   imul       ecx, eax, 0
00526ff4 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
00526ffb 7429                     je         0x527026
00526ffd ba04000000               mov        edx, 4
00527002 6bc200                   imul       eax, edx, 0
00527005 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
0052700b 898d00ffffff             mov        dword ptr [ebp - 0x100], ecx
00527011 6a20                     push       0x20
00527013 8b8d00ffffff             mov        ecx, dword ptr [ebp - 0x100]
00527019 e86232efff               call       0x41a280
0052701e 898548ffffff             mov        dword ptr [ebp - 0xb8], eax
00527024 eb0a                     jmp        0x527030
00527026 c78548ffffff00000000     mov        dword ptr [ebp - 0xb8], 0
00527030 83bd48ffffff00           cmp        dword ptr [ebp - 0xb8], 0
00527037 7414                     je         0x52704d
00527039 6a0d                     push       0xd
0052703b 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00527041 81c1e8560000             add        ecx, 0x56e8
00527047 e8b4affbff               call       0x4e2000
0052704c 90                       nop        
0052704d 33d2                     xor        edx, edx
0052704f 753a                     jne        0x52708b
00527051 b804000000               mov        eax, 4
00527056 6bc800                   imul       ecx, eax, 0
00527059 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
00527060 7429                     je         0x52708b
00527062 ba04000000               mov        edx, 4
00527067 6bc200                   imul       eax, edx, 0
0052706a 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
00527070 898dfcfeffff             mov        dword ptr [ebp - 0x104], ecx
00527076 6a40                     push       0x40
00527078 8b8dfcfeffff             mov        ecx, dword ptr [ebp - 0x104]
0052707e e8fd31efff               call       0x41a280
00527083 898544ffffff             mov        dword ptr [ebp - 0xbc], eax
00527089 eb0a                     jmp        0x527095
0052708b c78544ffffff00000000     mov        dword ptr [ebp - 0xbc], 0
00527095 83bd44ffffff00           cmp        dword ptr [ebp - 0xbc], 0
0052709c 7453                     je         0x5270f1
0052709e 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005270a4 81c1e8560000             add        ecx, 0x56e8
005270aa e85152eeff               call       0x40c300
005270af 99                       cdq        
005270b0 b90d000000               mov        ecx, 0xd
005270b5 f7f9                     idiv       ecx
005270b7 85d2                     test       edx, edx
005270b9 7422                     je         0x5270dd
005270bb 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
005270c1 81c2e8560000             add        edx, 0x56e8
005270c7 8995f8feffff             mov        dword ptr [ebp - 0x108], edx
005270cd 6a00                     push       0
005270cf 8b8df8feffff             mov        ecx, dword ptr [ebp - 0x108]
005270d5 e8667ff9ff               call       0x4bf040
005270da 90                       nop        
005270db eb14                     jmp        0x5270f1
005270dd 6a0c                     push       0xc
005270df 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005270e5 81c1e8560000             add        ecx, 0x56e8
005270eb e810affbff               call       0x4e2000
005270f0 90                       nop        
005270f1 33c0                     xor        eax, eax
005270f3 753d                     jne        0x527132
005270f5 b904000000               mov        ecx, 4
005270fa 6bd100                   imul       edx, ecx, 0
005270fd 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00527104 742c                     je         0x527132
00527106 b804000000               mov        eax, 4
0052710b 6bc800                   imul       ecx, eax, 0
0052710e 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00527114 8995f4feffff             mov        dword ptr [ebp - 0x10c], edx
0052711a 6880000000               push       0x80
0052711f 8b8df4feffff             mov        ecx, dword ptr [ebp - 0x10c]
00527125 e85631efff               call       0x41a280
0052712a 898540ffffff             mov        dword ptr [ebp - 0xc0], eax
00527130 eb0a                     jmp        0x52713c
00527132 c78540ffffff00000000     mov        dword ptr [ebp - 0xc0], 0
0052713c 83bd40ffffff00           cmp        dword ptr [ebp - 0xc0], 0
00527143 7454                     je         0x527199
00527145 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0052714b 81c1e8560000             add        ecx, 0x56e8
00527151 e8aa51eeff               call       0x40c300
00527156 99                       cdq        
00527157 b90d000000               mov        ecx, 0xd
0052715c f7f9                     idiv       ecx
0052715e 83fa0c                   cmp        edx, 0xc
00527161 7422                     je         0x527185
00527163 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
00527169 81c2e8560000             add        edx, 0x56e8
0052716f 8995f0feffff             mov        dword ptr [ebp - 0x110], edx
00527175 6a00                     push       0
00527177 8b8df0feffff             mov        ecx, dword ptr [ebp - 0x110]
0052717d e89e7ef9ff               call       0x4bf020
00527182 90                       nop        
00527183 eb14                     jmp        0x527199
00527185 6a0c                     push       0xc
00527187 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0052718d 81c1e8560000             add        ecx, 0x56e8
00527193 e888aefbff               call       0x4e2020
00527198 90                       nop        
00527199 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0052719f 81c1e8560000             add        ecx, 0x56e8
005271a5 e8865ef3ff               call       0x45d030
005271aa 85c0                     test       eax, eax
005271ac 740f                     je         0x5271bd
005271ae 6a00                     push       0
005271b0 6a0a                     push       0xa
005271b2 b930a85b00               mov        ecx, 0x5ba830
005271b7 e8b4fbefff               call       0x426d70
005271bc 90                       nop        
005271bd 33c0                     xor        eax, eax
005271bf 753d                     jne        0x5271fe
005271c1 b904000000               mov        ecx, 4
005271c6 6bd100                   imul       edx, ecx, 0
005271c9 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
005271d0 742c                     je         0x5271fe
005271d2 b804000000               mov        eax, 4
005271d7 6bc800                   imul       ecx, eax, 0
005271da 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
005271e0 8995ecfeffff             mov        dword ptr [ebp - 0x114], edx
005271e6 6801000800               push       0x80001
005271eb 8b8decfeffff             mov        ecx, dword ptr [ebp - 0x114]
005271f1 e80a2aefff               call       0x419c00
005271f6 89853cffffff             mov        dword ptr [ebp - 0xc4], eax
005271fc eb0a                     jmp        0x527208
005271fe c7853cffffff00000000     mov        dword ptr [ebp - 0xc4], 0
00527208 83bd3cffffff00           cmp        dword ptr [ebp - 0xc4], 0
0052720f 0f84c7030000             je         0x5275dc
00527215 a17cff5a00               mov        eax, dword ptr [0x5aff7c]
0052721a 50                       push       eax
0052721b e8008c0200               call       0x54fe20
00527220 83c404                   add        esp, 4
00527223 898570ffffff             mov        dword ptr [ebp - 0x90], eax
00527229 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0052722f 81c1e8560000             add        ecx, 0x56e8
00527235 898de8feffff             mov        dword ptr [ebp - 0x118], ecx
0052723b 8b9570ffffff             mov        edx, dword ptr [ebp - 0x90]
00527241 83ea03                   sub        edx, 3
00527244 52                       push       edx
00527245 8b8de8feffff             mov        ecx, dword ptr [ebp - 0x118]
0052724b e880adfbff               call       0x4e1fd0
00527250 85c0                     test       eax, eax
00527252 0f840c010000             je         0x527364
00527258 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
0052725e 83b8e056000008           cmp        dword ptr [eax + 0x56e0], 8
00527265 0f8d9e000000             jge        0x527309
0052726b 8b0d7cff5a00             mov        ecx, dword ptr [0x5aff7c]
00527271 898de4feffff             mov        dword ptr [ebp - 0x11c], ecx
00527277 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0052727d 81c1e8560000             add        ecx, 0x56e8
00527283 e87850eeff               call       0x40c300
00527288 8985e0feffff             mov        dword ptr [ebp - 0x120], eax
0052728e 8b95e4feffff             mov        edx, dword ptr [ebp - 0x11c]
00527294 0395e0feffff             add        edx, dword ptr [ebp - 0x120]
0052729a 8a02                     mov        al, byte ptr [edx]
0052729c 888577ffffff             mov        byte ptr [ebp - 0x89], al
005272a2 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005272a8 8b91e0560000             mov        edx, dword ptr [ecx + 0x56e0]
005272ae 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
005272b4 8a8d77ffffff             mov        cl, byte ptr [ebp - 0x89]
005272ba 888c10d4560000           mov        byte ptr [eax + edx + 0x56d4], cl
005272c1 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
005272c7 8b82e0560000             mov        eax, dword ptr [edx + 0x56e0]
005272cd 83c001                   add        eax, 1
005272d0 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005272d6 8981e0560000             mov        dword ptr [ecx + 0x56e0], eax
005272dc 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
005272e2 83bae056000008           cmp        dword ptr [edx + 0x56e0], 8
005272e9 7c1c                     jl         0x527307
005272eb 8b8570ffffff             mov        eax, dword ptr [ebp - 0x90]
005272f1 83e801                   sub        eax, 1
005272f4 50                       push       eax
005272f5 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005272fb 81c1e8560000             add        ecx, 0x56e8
00527301 e84a7af9ff               call       0x4bed50
00527306 90                       nop        
00527307 eb56                     jmp        0x52735f
00527309 8b0d7cff5a00             mov        ecx, dword ptr [0x5aff7c]
0052730f 898dc0feffff             mov        dword ptr [ebp - 0x140], ecx
00527315 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0052731b 81c1e8560000             add        ecx, 0x56e8
00527321 e8da4feeff               call       0x40c300
00527326 8985d8feffff             mov        dword ptr [ebp - 0x128], eax
0052732c 8b95c0feffff             mov        edx, dword ptr [ebp - 0x140]
00527332 0395d8feffff             add        edx, dword ptr [ebp - 0x128]
00527338 8a02                     mov        al, byte ptr [edx]
0052733a 888576ffffff             mov        byte ptr [ebp - 0x8a], al
00527340 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00527346 8b91e0560000             mov        edx, dword ptr [ecx + 0x56e0]
0052734c 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00527352 8a8d76ffffff             mov        cl, byte ptr [ebp - 0x8a]
00527358 888c10d3560000           mov        byte ptr [eax + edx + 0x56d3], cl
0052735f e969020000               jmp        0x5275cd
00527364 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
0052736a 81c2e8560000             add        edx, 0x56e8
00527370 8995d4feffff             mov        dword ptr [ebp - 0x12c], edx
00527376 8b8570ffffff             mov        eax, dword ptr [ebp - 0x90]
0052737c 83e803                   sub        eax, 3
0052737f 50                       push       eax
00527380 8b8dd4feffff             mov        ecx, dword ptr [ebp - 0x12c]
00527386 e8253ef4ff               call       0x46b1b0
0052738b 85c0                     test       eax, eax
0052738d 0f8490000000             je         0x527423
00527393 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00527399 83b9e056000008           cmp        dword ptr [ecx + 0x56e0], 8
005273a0 7d62                     jge        0x527404
005273a2 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
005273a8 8b82e0560000             mov        eax, dword ptr [edx + 0x56e0]
005273ae 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005273b4 c68401d456000020         mov        byte ptr [ecx + eax + 0x56d4], 0x20
005273bc 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
005273c2 8b82e0560000             mov        eax, dword ptr [edx + 0x56e0]
005273c8 83c001                   add        eax, 1
005273cb 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005273d1 8981e0560000             mov        dword ptr [ecx + 0x56e0], eax
005273d7 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
005273dd 83bae056000008           cmp        dword ptr [edx + 0x56e0], 8
005273e4 7c1c                     jl         0x527402
005273e6 8b8570ffffff             mov        eax, dword ptr [ebp - 0x90]
005273ec 83e801                   sub        eax, 1
005273ef 50                       push       eax
005273f0 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005273f6 81c1e8560000             add        ecx, 0x56e8
005273fc e84f79f9ff               call       0x4bed50
00527401 90                       nop        
00527402 eb1a                     jmp        0x52741e
00527404 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0052740a 8b91e0560000             mov        edx, dword ptr [ecx + 0x56e0]
00527410 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00527416 c68410d356000020         mov        byte ptr [eax + edx + 0x56d3], 0x20
0052741e e9aa010000               jmp        0x5275cd
00527423 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00527429 81c1e8560000             add        ecx, 0x56e8
0052742f 898dd0feffff             mov        dword ptr [ebp - 0x130], ecx
00527435 8b9570ffffff             mov        edx, dword ptr [ebp - 0x90]
0052743b 83ea02                   sub        edx, 2
0052743e 52                       push       edx
0052743f 8b8dd0feffff             mov        ecx, dword ptr [ebp - 0x130]
00527445 e8663df4ff               call       0x46b1b0
0052744a 85c0                     test       eax, eax
0052744c 744e                     je         0x52749c
0052744e 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00527454 83b8e056000000           cmp        dword ptr [eax + 0x56e0], 0
0052745b 7505                     jne        0x527462
0052745d e95d030000               jmp        0x5277bf
00527462 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00527468 8b91e0560000             mov        edx, dword ptr [ecx + 0x56e0]
0052746e 83ea01                   sub        edx, 1
00527471 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00527477 8990e0560000             mov        dword ptr [eax + 0x56e0], edx
0052747d 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00527483 8b91e0560000             mov        edx, dword ptr [ecx + 0x56e0]
00527489 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
0052748f c68410d456000020         mov        byte ptr [eax + edx + 0x56d4], 0x20
00527497 e931010000               jmp        0x5275cd
0052749c 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005274a2 81c1e8560000             add        ecx, 0x56e8
005274a8 898dccfeffff             mov        dword ptr [ebp - 0x134], ecx
005274ae 8b9570ffffff             mov        edx, dword ptr [ebp - 0x90]
005274b4 83ea01                   sub        edx, 1
005274b7 52                       push       edx
005274b8 8b8dccfeffff             mov        ecx, dword ptr [ebp - 0x134]
005274be e8ed3cf4ff               call       0x46b1b0
005274c3 85c0                     test       eax, eax
005274c5 0f8402010000             je         0x5275cd
005274cb 6a00                     push       0
005274cd 6a11                     push       0x11
005274cf b930a85b00               mov        ecx, 0x5ba830
005274d4 e897f8efff               call       0x426d70
005274d9 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005274df 83c124                   add        ecx, 0x24
005274e2 e8194eeeff               call       0x40c300
005274e7 83c001                   add        eax, 1
005274ea 50                       push       eax
005274eb 6834265700               push       0x572634
005274f0 6a40                     push       0x40
005274f2 8d45bc                   lea        eax, [ebp - 0x44]
005274f5 50                       push       eax
005274f6 e8257defff               call       0x41f220
005274fb 83c410                   add        esp, 0x10
005274fe 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00527504 81c140570000             add        ecx, 0x5740
0052750a 898dc8feffff             mov        dword ptr [ebp - 0x138], ecx
00527510 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00527516 83c124                   add        ecx, 0x24
00527519 e8e24deeff               call       0x40c300
0052751e 50                       push       eax
0052751f 8b8dc8feffff             mov        ecx, dword ptr [ebp - 0x138]
00527525 e856d0eeff               call       0x414580
0052752a 8b10                     mov        edx, dword ptr [eax]
0052752c 52                       push       edx
0052752d e88ea2efff               call       0x4217c0
00527532 83c404                   add        esp, 4
00527535 6a00                     push       0
00527537 6a01                     push       1
00527539 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
0052753f 05d4560000               add        eax, 0x56d4
00527544 50                       push       eax
00527545 8d4dbc                   lea        ecx, [ebp - 0x44]
00527548 51                       push       ecx
00527549 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
0052754f e82c1dfeff               call       0x509280
00527554 8d55bc                   lea        edx, [ebp - 0x44]
00527557 52                       push       edx
00527558 e88333feff               call       0x50a8e0
0052755d 83c404                   add        esp, 4
00527560 8985a4feffff             mov        dword ptr [ebp - 0x15c], eax
00527566 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
0052756c 0540570000               add        eax, 0x5740
00527571 8985c4feffff             mov        dword ptr [ebp - 0x13c], eax
00527577 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0052757d 83c124                   add        ecx, 0x24
00527580 e87b4deeff               call       0x40c300
00527585 50                       push       eax
00527586 8b8dc4feffff             mov        ecx, dword ptr [ebp - 0x13c]
0052758c e8efcfeeff               call       0x414580
00527591 8b8da4feffff             mov        ecx, dword ptr [ebp - 0x15c]
00527597 8908                     mov        dword ptr [eax], ecx
00527599 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
0052759f 81c2d4560000             add        edx, 0x56d4
005275a5 52                       push       edx
005275a6 6a0a                     push       0xa
005275a8 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
005275ae e82dcbf3ff               call       0x4640e0
005275b3 83c00c                   add        eax, 0xc
005275b6 50                       push       eax
005275b7 e854100200               call       0x548610
005275bc 83c40c                   add        esp, 0xc
005275bf 6a02                     push       2
005275c1 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005275c7 e884580000               call       0x52ce50
005275cc 90                       nop        
005275cd 6a00                     push       0
005275cf 6a07                     push       7
005275d1 b930a85b00               mov        ecx, 0x5ba830
005275d6 e895f7efff               call       0x426d70
005275db 90                       nop        
005275dc 33c0                     xor        eax, eax
005275de 753d                     jne        0x52761d
005275e0 b904000000               mov        ecx, 4
005275e5 6bd100                   imul       edx, ecx, 0
005275e8 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
005275ef 742c                     je         0x52761d
005275f1 b804000000               mov        eax, 4
005275f6 6bc800                   imul       ecx, eax, 0
005275f9 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
005275ff 8995bcfeffff             mov        dword ptr [ebp - 0x144], edx
00527605 6806010000               push       0x106
0052760a 8b8dbcfeffff             mov        ecx, dword ptr [ebp - 0x144]
00527610 e8eb25efff               call       0x419c00
00527615 898538ffffff             mov        dword ptr [ebp - 0xc8], eax
0052761b eb0a                     jmp        0x527627
0052761d c78538ffffff00000000     mov        dword ptr [ebp - 0xc8], 0
00527627 83bd38ffffff00           cmp        dword ptr [ebp - 0xc8], 0
0052762e 7462                     je         0x527692
00527630 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00527636 83b8e056000000           cmp        dword ptr [eax + 0x56e0], 0
0052763d 7445                     je         0x527684
0052763f 6a00                     push       0
00527641 6a09                     push       9
00527643 b930a85b00               mov        ecx, 0x5ba830
00527648 e823f7efff               call       0x426d70
0052764d 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00527653 8b91e0560000             mov        edx, dword ptr [ecx + 0x56e0]
00527659 83ea01                   sub        edx, 1
0052765c 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00527662 8990e0560000             mov        dword ptr [eax + 0x56e0], edx
00527668 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0052766e 8b91e0560000             mov        edx, dword ptr [ecx + 0x56e0]
00527674 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
0052767a c68410d456000020         mov        byte ptr [eax + edx + 0x56d4], 0x20
00527682 eb0e                     jmp        0x527692
00527684 6a02                     push       2
00527686 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0052768c e8bf570000               call       0x52ce50
00527691 90                       nop        
00527692 e928010000               jmp        0x5277bf
00527697 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0052769d 81c154010000             add        ecx, 0x154
005276a3 898db8feffff             mov        dword ptr [ebp - 0x148], ecx
005276a9 6a06                     push       6
005276ab 8b8db8feffff             mov        ecx, dword ptr [ebp - 0x148]
005276b1 e80abfefff               call       0x4235c0
005276b6 0fb6d0                   movzx      edx, al
005276b9 85d2                     test       edx, edx
005276bb 0f84fe000000             je         0x5277bf
005276c1 6a2a                     push       0x2a
005276c3 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005276c9 e862550000               call       0x52cc30
005276ce 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005276d4 81c190030000             add        ecx, 0x390
005276da e86119f5ff               call       0x479040
005276df 6a00                     push       0
005276e1 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005276e7 81c190030000             add        ecx, 0x390
005276ed e8aea0eeff               call       0x4117a0
005276f2 6a01                     push       1
005276f4 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005276fa e8d1570000               call       0x52ced0
005276ff 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00527705 83c124                   add        ecx, 0x24
00527708 e803e9f9ff               call       0x4c6010
0052770d a1fc605c00               mov        eax, dword ptr [0x5c60fc]
00527712 50                       push       eax
00527713 e8a8a0efff               call       0x4217c0
00527718 83c404                   add        esp, 4
0052771b 6898505700               push       0x575098
00527720 6a00                     push       0
00527722 b9404d5c00               mov        ecx, 0x5c4d40
00527727 e84423fbff               call       0x4d9a70
0052772c 6a00                     push       0
0052772e 6a00                     push       0
00527730 b9404d5c00               mov        ecx, 0x5c4d40
00527735 e81624fbff               call       0x4d9b50
0052773a 90                       nop        
0052773b c78564ffffff00000000     mov        dword ptr [ebp - 0x9c], 0
00527745 eb0f                     jmp        0x527756
00527747 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
0052774d 83c101                   add        ecx, 1
00527750 898d64ffffff             mov        dword ptr [ebp - 0x9c], ecx
00527756 83bd64ffffff19           cmp        dword ptr [ebp - 0x9c], 0x19
0052775d 7d31                     jge        0x527790
0052775f 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
00527765 81c240570000             add        edx, 0x5740
0052776b 8995b4feffff             mov        dword ptr [ebp - 0x14c], edx
00527771 8b8564ffffff             mov        eax, dword ptr [ebp - 0x9c]
00527777 50                       push       eax
00527778 8b8db4feffff             mov        ecx, dword ptr [ebp - 0x14c]
0052777e e8fdcdeeff               call       0x414580
00527783 8b08                     mov        ecx, dword ptr [eax]
00527785 51                       push       ecx
00527786 e835a0efff               call       0x4217c0
0052778b 83c404                   add        esp, 4
0052778e ebb7                     jmp        0x527747
00527790 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
00527796 81c240570000             add        edx, 0x5740
0052779c 8995acfeffff             mov        dword ptr [ebp - 0x154], edx
005277a2 c785b0feffff00000000     mov        dword ptr [ebp - 0x150], 0
005277ac 8d85b0feffff             lea        eax, [ebp - 0x150]
005277b2 50                       push       eax
005277b3 8b8dacfeffff             mov        ecx, dword ptr [ebp - 0x154]
005277b9 e8624a0000               call       0x52c220
005277be 90                       nop        
005277bf b801000000               mov        eax, 1
005277c4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005277c7 33cd                     xor        ecx, ebp
005277c9 e820b20100               call       0x5429ee
005277ce 8be5                     mov        esp, ebp
005277d0 5d                       pop        ebp
005277d1 c3                       ret        
005277d2 6690                     nop        
005277d4 d26a52                   shr        byte ptr [edx + 0x52], cl
005277d7 00f4                     add        ah, dh
005277d9 6b52002c                 imul       edx, dword ptr [edx], 0x2c
005277dd 6c                       insb       byte ptr es:[edi], dx
005277de 52                       push       edx
005277df 00716f                   add        byte ptr [ecx + 0x6f], dh
005277e2 52                       push       edx
005277e3 0097765200cc             add        byte ptr [edi - 0x33ffad8a], dl
005277e9 cc                       int3       
005277ea cc                       int3       
005277eb cc                       int3       
005277ec cc                       int3       
005277ed cc                       int3       
005277ee cc                       int3       
005277ef cc                       int3       
