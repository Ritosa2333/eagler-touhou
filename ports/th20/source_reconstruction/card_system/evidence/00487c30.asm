00487c30 55                       push       ebp
00487c31 8bec                     mov        ebp, esp
00487c33 6aff                     push       -1
00487c35 6848925600               push       0x569248
00487c3a 64a100000000             mov        eax, dword ptr fs:[0]
00487c40 50                       push       eax
00487c41 81ec14010000             sub        esp, 0x114
00487c47 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00487c4c 33c5                     xor        eax, ebp
00487c4e 8945f0                   mov        dword ptr [ebp - 0x10], eax
00487c51 56                       push       esi
00487c52 50                       push       eax
00487c53 8d45f4                   lea        eax, [ebp - 0xc]
00487c56 64a300000000             mov        dword ptr fs:[0], eax
00487c5c 894dd0                   mov        dword ptr [ebp - 0x30], ecx
00487c5f 6a00                     push       0
00487c61 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00487c64 83c124                   add        ecx, 0x24
00487c67 e8b4b8f9ff               call       0x423520
00487c6c 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00487c6f 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00487c72 894874                   mov        dword ptr [eax + 0x74], ecx
00487c75 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00487c78 52                       push       edx
00487c79 6a40                     push       0x40
00487c7b 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00487c7e 83c034                   add        eax, 0x34
00487c81 50                       push       eax
00487c82 e889090c00               call       0x548610
00487c87 83c40c                   add        esp, 0xc
00487c8a 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00487c8d 8b5178                   mov        edx, dword ptr [ecx + 0x78]
00487c90 83ca01                   or         edx, 1
00487c93 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00487c96 895078                   mov        dword ptr [eax + 0x78], edx
00487c99 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00487c9c 8b5178                   mov        edx, dword ptr [ecx + 0x78]
00487c9f 83ca02                   or         edx, 2
00487ca2 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00487ca5 895078                   mov        dword ptr [eax + 0x78], edx
00487ca8 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00487cab 8b5178                   mov        edx, dword ptr [ecx + 0x78]
00487cae 83e2f7                   and        edx, 0xfffffff7
00487cb1 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00487cb4 895078                   mov        dword ptr [eax + 0x78], edx
00487cb7 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00487cba 8b5178                   mov        edx, dword ptr [ecx + 0x78]
00487cbd 83e2ef                   and        edx, 0xffffffef
00487cc0 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00487cc3 895078                   mov        dword ptr [eax + 0x78], edx
00487cc6 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00487cc9 8b5178                   mov        edx, dword ptr [ecx + 0x78]
00487ccc 81ca00020000             or         edx, 0x200
00487cd2 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00487cd5 895078                   mov        dword ptr [eax + 0x78], edx
00487cd8 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00487cdb e8e00b0000               call       0x4888c0
00487ce0 90                       nop        
00487ce1 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
00487ce7 e8140b0000               call       0x488800
00487cec 85c0                     test       eax, eax
00487cee 0f85ec000000             jne        0x487de0
00487cf4 b968a55b00               mov        ecx, 0x5ba568
00487cf9 e8520b0000               call       0x488850
00487cfe 0fb6c8                   movzx      ecx, al
00487d01 85c9                     test       ecx, ecx
00487d03 7409                     je         0x487d0e
00487d05 c745c401000000           mov        dword ptr [ebp - 0x3c], 1
00487d0c eb07                     jmp        0x487d15
00487d0e c745c400000000           mov        dword ptr [ebp - 0x3c], 0
00487d15 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
00487d18 8955c0                   mov        dword ptr [ebp - 0x40], edx
00487d1b 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00487d21 e82a7f0800               call       0x50fc50
00487d26 05080b0000               add        eax, 0xb08
00487d2b 898544ffffff             mov        dword ptr [ebp - 0xbc], eax
00487d31 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00487d34 50                       push       eax
00487d35 68c0000000               push       0xc0
00487d3a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00487d3d 51                       push       ecx
00487d3e 8b8d44ffffff             mov        ecx, dword ptr [ebp - 0xbc]
00487d44 e8c7f0ffff               call       0x486e10
00487d49 50                       push       eax
00487d4a e8c1080c00               call       0x548610
00487d4f 83c40c                   add        esp, 0xc
00487d52 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00487d58 e8f37e0800               call       0x50fc50
00487d5d 05080b0000               add        eax, 0xb08
00487d62 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00487d65 8b5508                   mov        edx, dword ptr [ebp + 8]
00487d68 52                       push       edx
00487d69 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00487d6c e89ff0ffff               call       0x486e10
00487d71 8945b0                   mov        dword ptr [ebp - 0x50], eax
00487d74 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
00487d77 50                       push       eax
00487d78 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00487d7b e810080000               call       0x488590
00487d80 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00487d86 e875090000               call       0x488700
00487d8b 05080b0000               add        eax, 0xb08
00487d90 8945ac                   mov        dword ptr [ebp - 0x54], eax
00487d93 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00487d96 51                       push       ecx
00487d97 68c0000000               push       0xc0
00487d9c 8b5508                   mov        edx, dword ptr [ebp + 8]
00487d9f 52                       push       edx
00487da0 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00487da3 e868f0ffff               call       0x486e10
00487da8 50                       push       eax
00487da9 e862080c00               call       0x548610
00487dae 83c40c                   add        esp, 0xc
00487db1 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00487db7 e844090000               call       0x488700
00487dbc 05080b0000               add        eax, 0xb08
00487dc1 8945a8                   mov        dword ptr [ebp - 0x58], eax
00487dc4 8b4508                   mov        eax, dword ptr [ebp + 8]
00487dc7 50                       push       eax
00487dc8 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00487dcb e840f0ffff               call       0x486e10
00487dd0 8945a4                   mov        dword ptr [ebp - 0x5c], eax
00487dd3 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00487dd6 51                       push       ecx
00487dd7 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00487dda e8b1070000               call       0x488590
00487ddf 90                       nop        
00487de0 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
00487de6 e8750c0000               call       0x488a60
00487deb 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00487dee 8b4278                   mov        eax, dword ptr [edx + 0x78]
00487df1 83e0df                   and        eax, 0xffffffdf
00487df4 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00487df7 894178                   mov        dword ptr [ecx + 0x78], eax
00487dfa 6a00                     push       0
00487dfc e83f02ffff               call       0x478040
00487e01 83c404                   add        esp, 4
00487e04 8bc8                     mov        ecx, eax
00487e06 e82503ffff               call       0x478130
00487e0b 0fb6d0                   movzx      edx, al
00487e0e 85d2                     test       edx, edx
00487e10 740f                     je         0x487e21
00487e12 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00487e15 8b4878                   mov        ecx, dword ptr [eax + 0x78]
00487e18 83c920                   or         ecx, 0x20
00487e1b 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00487e1e 894a78                   mov        dword ptr [edx + 0x78], ecx
00487e21 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00487e24 c7808c00000001000000     mov        dword ptr [eax + 0x8c], 1
00487e2e 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00487e31 8b5178                   mov        edx, dword ptr [ecx + 0x78]
00487e34 83e2bf                   and        edx, 0xffffffbf
00487e37 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00487e3a 895078                   mov        dword ptr [eax + 0x78], edx
00487e3d 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00487e43 e808fbfaff               call       0x437950
00487e48 8945a0                   mov        dword ptr [ebp - 0x60], eax
00487e4b 6a00                     push       0
00487e4d 6aff                     push       -1
00487e4f 6a00                     push       0
00487e51 6a00                     push       0
00487e53 8d8d04ffffff             lea        ecx, [ebp - 0xfc]
00487e59 51                       push       ecx
00487e5a 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00487e5d e80e8efcff               call       0x450c70
00487e62 8b10                     mov        edx, dword ptr [eax]
00487e64 b804000000               mov        eax, 4
00487e69 6bc800                   imul       ecx, eax, 0
00487e6c 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00487e6f 89540814                 mov        dword ptr [eax + ecx + 0x14], edx
00487e73 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00487e79 e8f287feff               call       0x470670
00487e7e 894598                   mov        dword ptr [ebp - 0x68], eax
00487e81 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00487e84 8b91bc000000             mov        edx, dword ptr [ecx + 0xbc]
00487e8a 83c216                   add        edx, 0x16
00487e8d 89559c                   mov        dword ptr [ebp - 0x64], edx
00487e90 6a00                     push       0
00487e92 6aff                     push       -1
00487e94 8b459c                   mov        eax, dword ptr [ebp - 0x64]
00487e97 50                       push       eax
00487e98 681cfe5600               push       0x56fe1c
00487e9d 8d8d20ffffff             lea        ecx, [ebp - 0xe0]
00487ea3 51                       push       ecx
00487ea4 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00487ea7 e8c48dfcff               call       0x450c70
00487eac 8b10                     mov        edx, dword ptr [eax]
00487eae b804000000               mov        eax, 4
00487eb3 c1e000                   shl        eax, 0
00487eb6 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00487eb9 89540114                 mov        dword ptr [ecx + eax + 0x14], edx
00487ebd 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00487ec3 e888fafaff               call       0x437950
00487ec8 894594                   mov        dword ptr [ebp - 0x6c], eax
00487ecb 6a00                     push       0
00487ecd 6aff                     push       -1
00487ecf 6a01                     push       1
00487ed1 6a00                     push       0
00487ed3 8d951cffffff             lea        edx, [ebp - 0xe4]
00487ed9 52                       push       edx
00487eda 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00487edd e88e8dfcff               call       0x450c70
00487ee2 8b00                     mov        eax, dword ptr [eax]
00487ee4 b904000000               mov        ecx, 4
00487ee9 d1e1                     shl        ecx, 1
00487eeb 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00487eee 89440a14                 mov        dword ptr [edx + ecx + 0x14], eax
00487ef2 6a1c                     push       0x1c
00487ef4 8d4dd4                   lea        ecx, [ebp - 0x2c]
00487ef7 e88441f8ff               call       0x40c080
00487efc 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00487eff 50                       push       eax
00487f00 8d4dd4                   lea        ecx, [ebp - 0x2c]
00487f03 e82829feff               call       0x46a830
00487f08 c745fc00000000           mov        dword ptr [ebp - 4], 0
00487f0f 83ec28                   sub        esp, 0x28
00487f12 8bf4                     mov        esi, esp
00487f14 89a518ffffff             mov        dword ptr [ebp - 0xe8], esp
00487f1a b904000000               mov        ecx, 4
00487f1f c1e100                   shl        ecx, 0
00487f22 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00487f25 8d4c0a14                 lea        ecx, [edx + ecx + 0x14]
00487f29 e8a24ffcff               call       0x44ced0
00487f2e 50                       push       eax
00487f2f 8d45d4                   lea        eax, [ebp - 0x2c]
00487f32 50                       push       eax
00487f33 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00487f36 51                       push       ecx
00487f37 8d8de0feffff             lea        ecx, [ebp - 0x120]
00487f3d e82eecffff               call       0x486b70
00487f42 894590                   mov        dword ptr [ebp - 0x70], eax
00487f45 8b5590                   mov        edx, dword ptr [ebp - 0x70]
00487f48 89558c                   mov        dword ptr [ebp - 0x74], edx
00487f4b c645fc01                 mov        byte ptr [ebp - 4], 1
00487f4f 8b458c                   mov        eax, dword ptr [ebp - 0x74]
00487f52 50                       push       eax
00487f53 8bce                     mov        ecx, esi
00487f55 e8a6e8ffff               call       0x486800
00487f5a 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00487f60 e81b82feff               call       0x470180
00487f65 c645fc00                 mov        byte ptr [ebp - 4], 0
00487f69 8d8de0feffff             lea        ecx, [ebp - 0x120]
00487f6f e86c2efeff               call       0x46ade0
00487f74 90                       nop        
00487f75 6a00                     push       0
00487f77 6a21                     push       0x21
00487f79 b930a85b00               mov        ecx, 0x5ba830
00487f7e e8ededf9ff               call       0x426d70
00487f83 90                       nop        
00487f84 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00487f87 8b89c0000000             mov        ecx, dword ptr [ecx + 0xc0]
00487f8d e83e4bf9ff               call       0x41cad0
00487f92 894588                   mov        dword ptr [ebp - 0x78], eax
00487f95 6a00                     push       0
00487f97 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00487f9a e86151fdff               call       0x45d100
00487f9f 894584                   mov        dword ptr [ebp - 0x7c], eax
00487fa2 6a00                     push       0
00487fa4 6aff                     push       -1
00487fa6 6a06                     push       6
00487fa8 6884ec5600               push       0x56ec84
00487fad 8d9514ffffff             lea        edx, [ebp - 0xec]
00487fb3 52                       push       edx
00487fb4 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
00487fb7 e8b48cfcff               call       0x450c70
00487fbc 8b00                     mov        eax, dword ptr [eax]
00487fbe 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00487fc1 894120                   mov        dword ptr [ecx + 0x20], eax
00487fc4 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00487fc7 8b8ac0000000             mov        ecx, dword ptr [edx + 0xc0]
00487fcd e85ea7f8ff               call       0x412730
00487fd2 894580                   mov        dword ptr [ebp - 0x80], eax
00487fd5 6a00                     push       0
00487fd7 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00487fda e881d6ffff               call       0x485660
00487fdf 85c0                     test       eax, eax
00487fe1 7443                     je         0x488026
00487fe3 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00487fe6 8b88c0000000             mov        ecx, dword ptr [eax + 0xc0]
00487fec e83fa7f8ff               call       0x412730
00487ff1 89857cffffff             mov        dword ptr [ebp - 0x84], eax
00487ff7 6a00                     push       0
00487ff9 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00487fff e85cd6ffff               call       0x485660
00488004 8bc8                     mov        ecx, eax
00488006 e8b522ffff               call       0x47a2c0
0048800b 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0048800e 81c1ac000000             add        ecx, 0xac
00488014 8b10                     mov        edx, dword ptr [eax]
00488016 8911                     mov        dword ptr [ecx], edx
00488018 8b5004                   mov        edx, dword ptr [eax + 4]
0048801b 895104                   mov        dword ptr [ecx + 4], edx
0048801e 8b4008                   mov        eax, dword ptr [eax + 8]
00488021 894108                   mov        dword ptr [ecx + 8], eax
00488024 eb49                     jmp        0x48806f
00488026 51                       push       ecx
00488027 0f57c0                   xorps      xmm0, xmm0
0048802a f30f110424               movss      dword ptr [esp], xmm0
0048802f 51                       push       ecx
00488030 0f57c0                   xorps      xmm0, xmm0
00488033 f30f110424               movss      dword ptr [esp], xmm0
00488038 51                       push       ecx
00488039 0f57c0                   xorps      xmm0, xmm0
0048803c f30f110424               movss      dword ptr [esp], xmm0
00488041 8d8d38ffffff             lea        ecx, [ebp - 0xc8]
00488047 e884adf9ff               call       0x422dd0
0048804c 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0048804f 81c1ac000000             add        ecx, 0xac
00488055 8b9538ffffff             mov        edx, dword ptr [ebp - 0xc8]
0048805b 8911                     mov        dword ptr [ecx], edx
0048805d 8b853cffffff             mov        eax, dword ptr [ebp - 0xc4]
00488063 894104                   mov        dword ptr [ecx + 4], eax
00488066 8b9540ffffff             mov        edx, dword ptr [ebp - 0xc0]
0048806c 895108                   mov        dword ptr [ecx + 8], edx
0048806f 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00488072 83c020                   add        eax, 0x20
00488075 898578ffffff             mov        dword ptr [ebp - 0x88], eax
0048807b 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0048807e 81c1ac000000             add        ecx, 0xac
00488084 51                       push       ecx
00488085 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0048808b e83082fcff               call       0x4502c0
00488090 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00488093 83c220                   add        edx, 0x20
00488096 899574ffffff             mov        dword ptr [ebp - 0x8c], edx
0048809c 6a00                     push       0
0048809e 6a04                     push       4
004880a0 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
004880a6 e8c545fcff               call       0x44c670
004880ab 8bc8                     mov        ecx, eax
004880ad e80e50fdff               call       0x45d0c0
004880b2 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
004880b5 894808                   mov        dword ptr [eax + 8], ecx
004880b8 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004880bb 83c220                   add        edx, 0x20
004880be 899570ffffff             mov        dword ptr [ebp - 0x90], edx
004880c4 6a00                     push       0
004880c6 6a05                     push       5
004880c8 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
004880ce e89d45fcff               call       0x44c670
004880d3 8bc8                     mov        ecx, eax
004880d5 e8e64ffdff               call       0x45d0c0
004880da 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
004880dd 894808                   mov        dword ptr [eax + 8], ecx
004880e0 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004880e3 8b4510                   mov        eax, dword ptr [ebp + 0x10]
004880e6 898284000000             mov        dword ptr [edx + 0x84], eax
004880ec c78524ffffff20a10700     mov        dword ptr [ebp - 0xdc], 0x7a120
004880f6 c78528ffffff40420f00     mov        dword ptr [ebp - 0xd8], 0xf4240
00488100 c7852cffffff60e31600     mov        dword ptr [ebp - 0xd4], 0x16e360
0048810a c78530ffffff80841e00     mov        dword ptr [ebp - 0xd0], 0x1e8480
00488114 c78534ffffff40420f00     mov        dword ptr [ebp - 0xcc], 0xf4240
0048811e b968a55b00               mov        ecx, 0x5ba568
00488123 e838ccfeff               call       0x474d60
00488128 8bf0                     mov        esi, eax
0048812a b968a55b00               mov        ecx, 0x5ba568
0048812f e86cbffdff               call       0x4640a0
00488134 0fafb48524ffffff         imul       esi, dword ptr [ebp + eax*4 - 0xdc]
0048813c 8975bc                   mov        dword ptr [ebp - 0x44], esi
0048813f 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00488142 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
00488145 89517c                   mov        dword ptr [ecx + 0x7c], edx
00488148 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
0048814b 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
0048814e 898880000000             mov        dword ptr [eax + 0x80], ecx
00488154 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00488157 81ba8000000000ca9a3b     cmp        dword ptr [edx + 0x80], 0x3b9aca00
00488161 7c0d                     jl         0x488170
00488163 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00488166 c78080000000ffc99a3b     mov        dword ptr [eax + 0x80], 0x3b9ac9ff
00488170 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00488173 8b89c0000000             mov        ecx, dword ptr [ecx + 0xc0]
00488179 e85249f9ff               call       0x41cad0
0048817e 89856cffffff             mov        dword ptr [ebp - 0x94], eax
00488184 6a00                     push       0
00488186 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
0048818c e86f4ffdff               call       0x45d100
00488191 898568ffffff             mov        dword ptr [ebp - 0x98], eax
00488197 6a00                     push       0
00488199 6aff                     push       -1
0048819b 6a0d                     push       0xd
0048819d 6884ec5600               push       0x56ec84
004881a2 8d9510ffffff             lea        edx, [ebp - 0xf0]
004881a8 52                       push       edx
004881a9 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
004881af e8bc8afcff               call       0x450c70
004881b4 90                       nop        
004881b5 8b4514                   mov        eax, dword ptr [ebp + 0x14]
004881b8 8945cc                   mov        dword ptr [ebp - 0x34], eax
004881bb 837d0858                 cmp        dword ptr [ebp + 8], 0x58
004881bf 7c0f                     jl         0x4881d0
004881c1 837d085c                 cmp        dword ptr [ebp + 8], 0x5c
004881c5 7f09                     jg         0x4881d0
004881c7 c745b801000000           mov        dword ptr [ebp - 0x48], 1
004881ce eb07                     jmp        0x4881d7
004881d0 c745b800000000           mov        dword ptr [ebp - 0x48], 0
004881d7 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004881da 894dc8                   mov        dword ptr [ebp - 0x38], ecx
004881dd 6b55cc34                 imul       edx, dword ptr [ebp - 0x34], 0x34
004881e1 a110615c00               mov        eax, dword ptr [0x5c6110]
004881e6 8d4c1064                 lea        ecx, [eax + edx + 0x64]
004881ea 6b55c80c                 imul       edx, dword ptr [ebp - 0x38], 0xc
004881ee 03ca                     add        ecx, edx
004881f0 b804000000               mov        eax, 4
004881f5 6bd000                   imul       edx, eax, 0
004881f8 833c1100                 cmp        dword ptr [ecx + edx], 0
004881fc 0f8c99000000             jl         0x48829b
00488202 6a00                     push       0
00488204 e857fefeff               call       0x478060
00488209 83c404                   add        esp, 4
0048820c 898560ffffff             mov        dword ptr [ebp - 0xa0], eax
00488212 6b45cc34                 imul       eax, dword ptr [ebp - 0x34], 0x34
00488216 8b0d10615c00             mov        ecx, dword ptr [0x5c6110]
0048821c 8d540164                 lea        edx, [ecx + eax + 0x64]
00488220 6b45c80c                 imul       eax, dword ptr [ebp - 0x38], 0xc
00488224 03d0                     add        edx, eax
00488226 b904000000               mov        ecx, 4
0048822b 6bc100                   imul       eax, ecx, 0
0048822e 8b0c02                   mov        ecx, dword ptr [edx + eax]
00488231 898d64ffffff             mov        dword ptr [ebp - 0x9c], ecx
00488237 8b9564ffffff             mov        edx, dword ptr [ebp - 0x9c]
0048823d 52                       push       edx
0048823e 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
00488244 e8c72b0200               call       0x4aae10
00488249 898558ffffff             mov        dword ptr [ebp - 0xa8], eax
0048824f 6b45cc34                 imul       eax, dword ptr [ebp - 0x34], 0x34
00488253 8b0d10615c00             mov        ecx, dword ptr [0x5c6110]
00488259 8d540164                 lea        edx, [ecx + eax + 0x64]
0048825d 6b45c80c                 imul       eax, dword ptr [ebp - 0x38], 0xc
00488261 03d0                     add        edx, eax
00488263 b904000000               mov        ecx, 4
00488268 c1e100                   shl        ecx, 0
0048826b 8b140a                   mov        edx, dword ptr [edx + ecx]
0048826e 89955cffffff             mov        dword ptr [ebp - 0xa4], edx
00488274 6a00                     push       0
00488276 6aff                     push       -1
00488278 8b855cffffff             mov        eax, dword ptr [ebp - 0xa4]
0048827e 50                       push       eax
0048827f 6a00                     push       0
00488281 8d8d0cffffff             lea        ecx, [ebp - 0xf4]
00488287 51                       push       ecx
00488288 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
0048828e e8dd89fcff               call       0x450c70
00488293 8b10                     mov        edx, dword ptr [eax]
00488295 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00488298 895010                   mov        dword ptr [eax + 0x10], edx
0048829b 6b4dcc34                 imul       ecx, dword ptr [ebp - 0x34], 0x34
0048829f 8b1510615c00             mov        edx, dword ptr [0x5c6110]
004882a5 8d440a64                 lea        eax, [edx + ecx + 0x64]
004882a9 6b4dc80c                 imul       ecx, dword ptr [ebp - 0x38], 0xc
004882ad 03c1                     add        eax, ecx
004882af ba04000000               mov        edx, 4
004882b4 d1e2                     shl        edx, 1
004882b6 8b0410                   mov        eax, dword ptr [eax + edx]
004882b9 50                       push       eax
004882ba 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004882bd e89e060000               call       0x488960
004882c2 90                       nop        
004882c3 6b4d1434                 imul       ecx, dword ptr [ebp + 0x14], 0x34
004882c7 8b1510615c00             mov        edx, dword ptr [0x5c6110]
004882cd 8d440a64                 lea        eax, [edx + ecx + 0x64]
004882d1 b904000000               mov        ecx, 4
004882d6 6bd100                   imul       edx, ecx, 0
004882d9 837c1018ff               cmp        dword ptr [eax + edx + 0x18], -1
004882de 0f8488000000             je         0x48836c
004882e4 6a00                     push       0
004882e6 e875fdfeff               call       0x478060
004882eb 83c404                   add        esp, 4
004882ee 898550ffffff             mov        dword ptr [ebp - 0xb0], eax
004882f4 6b451434                 imul       eax, dword ptr [ebp + 0x14], 0x34
004882f8 8b0d10615c00             mov        ecx, dword ptr [0x5c6110]
004882fe 8d540164                 lea        edx, [ecx + eax + 0x64]
00488302 b804000000               mov        eax, 4
00488307 6bc800                   imul       ecx, eax, 0
0048830a 8b540a18                 mov        edx, dword ptr [edx + ecx + 0x18]
0048830e 899554ffffff             mov        dword ptr [ebp - 0xac], edx
00488314 8b8554ffffff             mov        eax, dword ptr [ebp - 0xac]
0048831a 50                       push       eax
0048831b 8b8d50ffffff             mov        ecx, dword ptr [ebp - 0xb0]
00488321 e8ea2a0200               call       0x4aae10
00488326 898548ffffff             mov        dword ptr [ebp - 0xb8], eax
0048832c 6b4d1434                 imul       ecx, dword ptr [ebp + 0x14], 0x34
00488330 8b1510615c00             mov        edx, dword ptr [0x5c6110]
00488336 8d440a64                 lea        eax, [edx + ecx + 0x64]
0048833a b904000000               mov        ecx, 4
0048833f c1e100                   shl        ecx, 0
00488342 8b540818                 mov        edx, dword ptr [eax + ecx + 0x18]
00488346 89954cffffff             mov        dword ptr [ebp - 0xb4], edx
0048834c 6a00                     push       0
0048834e 6aff                     push       -1
00488350 8b854cffffff             mov        eax, dword ptr [ebp - 0xb4]
00488356 50                       push       eax
00488357 6a00                     push       0
00488359 8d8d08ffffff             lea        ecx, [ebp - 0xf8]
0048835f 51                       push       ecx
00488360 8b8d48ffffff             mov        ecx, dword ptr [ebp - 0xb8]
00488366 e80589fcff               call       0x450c70
0048836b 90                       nop        
0048836c c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00488373 8d4dd4                   lea        ecx, [ebp - 0x2c]
00488376 e8c50efcff               call       0x449240
0048837b 90                       nop        
0048837c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0048837f 64890d00000000           mov        dword ptr fs:[0], ecx
00488386 59                       pop        ecx
00488387 5e                       pop        esi
00488388 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0048838b 33cd                     xor        ecx, ebp
0048838d e85ca60b00               call       0x5429ee
00488392 8be5                     mov        esp, ebp
00488394 5d                       pop        ebp
00488395 c21000                   ret        0x10
00488398 cc                       int3       
00488399 cc                       int3       
0048839a cc                       int3       
0048839b cc                       int3       
0048839c cc                       int3       
0048839d cc                       int3       
0048839e cc                       int3       
0048839f cc                       int3       
