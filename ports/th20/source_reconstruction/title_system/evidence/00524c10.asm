00524c10 55                       push       ebp
00524c11 8bec                     mov        ebp, esp
00524c13 81eca4000000             sub        esp, 0xa4
00524c19 894dfc                   mov        dword ptr [ebp - 4], ecx
00524c1c 8b45fc                   mov        eax, dword ptr [ebp - 4]
00524c1f 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00524c22 894de4                   mov        dword ptr [ebp - 0x1c], ecx
00524c25 837de403                 cmp        dword ptr [ebp - 0x1c], 3
00524c29 0f87a30b0000             ja         0x5257d2
00524c2f 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
00524c32 ff2495dc575200           jmp        dword ptr [edx*4 + 0x5257dc]
00524c39 e8226dffff               call       0x51b960
00524c3e 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00524c41 6a02                     push       2
00524c43 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00524c46 e86554ffff               call       0x51a0b0
00524c4b e8106dffff               call       0x51b960
00524c50 8945c0                   mov        dword ptr [ebp - 0x40], eax
00524c53 6a00                     push       0
00524c55 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00524c58 e823820000               call       0x52ce80
00524c5d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524c60 83c124                   add        ecx, 0x24
00524c63 e89876eeff               call       0x40c300
00524c68 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524c6b 8981f8580000             mov        dword ptr [ecx + 0x58f8], eax
00524c71 8b55fc                   mov        edx, dword ptr [ebp - 4]
00524c74 83baf858000001           cmp        dword ptr [edx + 0x58f8], 1
00524c7b 755d                     jne        0x524cda
00524c7d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00524c80 0508010000               add        eax, 0x108
00524c85 8945bc                   mov        dword ptr [ebp - 0x44], eax
00524c88 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524c8b 81c1bc000000             add        ecx, 0xbc
00524c91 e86a76eeff               call       0x40c300
00524c96 50                       push       eax
00524c97 e8143af6ff               call       0x4886b0
00524c9c 83c404                   add        esp, 4
00524c9f 83c009                   add        eax, 9
00524ca2 99                       cdq        
00524ca3 b90a000000               mov        ecx, 0xa
00524ca8 f7f9                     idiv       ecx
00524caa 50                       push       eax
00524cab 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00524cae e82d78f7ff               call       0x49c4e0
00524cb3 6a00                     push       0
00524cb5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524cb8 81c108010000             add        ecx, 0x108
00524cbe e88da0f9ff               call       0x4bed50
00524cc3 8b55fc                   mov        edx, dword ptr [ebp - 4]
00524cc6 81c208010000             add        edx, 0x108
00524ccc 8955b8                   mov        dword ptr [ebp - 0x48], edx
00524ccf 6a01                     push       1
00524cd1 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00524cd4 e807aff9ff               call       0x4bfbe0
00524cd9 90                       nop        
00524cda 8b45fc                   mov        eax, dword ptr [ebp - 4]
00524cdd 83c024                   add        eax, 0x24
00524ce0 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00524ce3 6a10                     push       0x10
00524ce5 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00524ce8 e8f377f7ff               call       0x49c4e0
00524ced 6a00                     push       0
00524cef 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524cf2 83c124                   add        ecx, 0x24
00524cf5 e856a0f9ff               call       0x4bed50
00524cfa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524cfd 81c1bc000000             add        ecx, 0xbc
00524d03 894db0                   mov        dword ptr [ebp - 0x50], ecx
00524d06 6a05                     push       5
00524d08 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00524d0b e8d077f7ff               call       0x49c4e0
00524d10 6a01                     push       1
00524d12 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524d15 81c1bc000000             add        ecx, 0xbc
00524d1b e830a0f9ff               call       0x4bed50
00524d20 8b55fc                   mov        edx, dword ptr [ebp - 4]
00524d23 81c2bc000000             add        edx, 0xbc
00524d29 8955ac                   mov        dword ptr [ebp - 0x54], edx
00524d2c 6a01                     push       1
00524d2e 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00524d31 e8aaaef9ff               call       0x4bfbe0
00524d36 6a01                     push       1
00524d38 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524d3b e810810000               call       0x52ce50
00524d40 6a48                     push       0x48
00524d42 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524d45 e836820000               call       0x52cf80
00524d4a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524d4d 81c1bc000000             add        ecx, 0xbc
00524d53 e8a875eeff               call       0x40c300
00524d58 83c040                   add        eax, 0x40
00524d5b 50                       push       eax
00524d5c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524d5f e81c820000               call       0x52cf80
00524d64 6a49                     push       0x49
00524d66 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524d69 e812820000               call       0x52cf80
00524d6e 6a4a                     push       0x4a
00524d70 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524d73 e808820000               call       0x52cf80
00524d78 6a4b                     push       0x4b
00524d7a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524d7d e8fe810000               call       0x52cf80
00524d82 6a4c                     push       0x4c
00524d84 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524d87 e8f4810000               call       0x52cf80
00524d8c 6a45                     push       0x45
00524d8e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524d91 e8ea810000               call       0x52cf80
00524d96 6a46                     push       0x46
00524d98 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524d9b e8e0810000               call       0x52cf80
00524da0 6a47                     push       0x47
00524da2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524da5 e8d6810000               call       0x52cf80
00524daa 6a27                     push       0x27
00524dac 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524daf e8cc810000               call       0x52cf80
00524db4 90                       nop        
00524db5 8b45fc                   mov        eax, dword ptr [ebp - 4]
00524db8 0554010000               add        eax, 0x154
00524dbd 8945a8                   mov        dword ptr [ebp - 0x58], eax
00524dc0 6a06                     push       6
00524dc2 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00524dc5 e8a6c2f3ff               call       0x461070
00524dca 0fb6c8                   movzx      ecx, al
00524dcd 85c9                     test       ecx, ecx
00524dcf 740b                     je         0x524ddc
00524dd1 6a02                     push       2
00524dd3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524dd6 e875800000               call       0x52ce50
00524ddb 90                       nop        
00524ddc e9f1090000               jmp        0x5257d2
00524de1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524de4 83c124                   add        ecx, 0x24
00524de7 e814aef9ff               call       0x4bfc00
00524dec 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524def 81c1bc000000             add        ecx, 0xbc
00524df5 e806aef9ff               call       0x4bfc00
00524dfa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524dfd 81c108010000             add        ecx, 0x108
00524e03 e8f8adf9ff               call       0x4bfc00
00524e08 90                       nop        
00524e09 33d2                     xor        edx, edx
00524e0b 7531                     jne        0x524e3e
00524e0d b804000000               mov        eax, 4
00524e12 6bc800                   imul       ecx, eax, 0
00524e15 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
00524e1c 7420                     je         0x524e3e
00524e1e ba04000000               mov        edx, 4
00524e23 6bc200                   imul       eax, edx, 0
00524e26 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
00524e2c 894da4                   mov        dword ptr [ebp - 0x5c], ecx
00524e2f 6a10                     push       0x10
00524e31 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00524e34 e84754efff               call       0x41a280
00524e39 8945e0                   mov        dword ptr [ebp - 0x20], eax
00524e3c eb07                     jmp        0x524e45
00524e3e c745e000000000           mov        dword ptr [ebp - 0x20], 0
00524e45 837de000                 cmp        dword ptr [ebp - 0x20], 0
00524e49 7421                     je         0x524e6c
00524e4b 8b55fc                   mov        edx, dword ptr [ebp - 4]
00524e4e 81c2bc000000             add        edx, 0xbc
00524e54 8955a0                   mov        dword ptr [ebp - 0x60], edx
00524e57 6a00                     push       0
00524e59 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00524e5c e8dfa1f9ff               call       0x4bf040
00524e61 6a4b                     push       0x4b
00524e63 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524e66 e8857d0000               call       0x52cbf0
00524e6b 90                       nop        
00524e6c 33c0                     xor        eax, eax
00524e6e 7531                     jne        0x524ea1
00524e70 b904000000               mov        ecx, 4
00524e75 6bd100                   imul       edx, ecx, 0
00524e78 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00524e7f 7420                     je         0x524ea1
00524e81 b804000000               mov        eax, 4
00524e86 6bc800                   imul       ecx, eax, 0
00524e89 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00524e8f 89559c                   mov        dword ptr [ebp - 0x64], edx
00524e92 6a20                     push       0x20
00524e94 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
00524e97 e8e453efff               call       0x41a280
00524e9c 8945dc                   mov        dword ptr [ebp - 0x24], eax
00524e9f eb07                     jmp        0x524ea8
00524ea1 c745dc00000000           mov        dword ptr [ebp - 0x24], 0
00524ea8 837ddc00                 cmp        dword ptr [ebp - 0x24], 0
00524eac 7420                     je         0x524ece
00524eae 8b45fc                   mov        eax, dword ptr [ebp - 4]
00524eb1 05bc000000               add        eax, 0xbc
00524eb6 894598                   mov        dword ptr [ebp - 0x68], eax
00524eb9 6a00                     push       0
00524ebb 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00524ebe e85da1f9ff               call       0x4bf020
00524ec3 6a4c                     push       0x4c
00524ec5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524ec8 e8237d0000               call       0x52cbf0
00524ecd 90                       nop        
00524ece 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524ed1 81c1bc000000             add        ecx, 0xbc
00524ed7 e85481f3ff               call       0x45d030
00524edc 85c0                     test       eax, eax
00524ede 0f84a0000000             je         0x524f84
00524ee4 6a00                     push       0
00524ee6 6a0a                     push       0xa
00524ee8 b930a85b00               mov        ecx, 0x5ba830
00524eed e87e1ef0ff               call       0x426d70
00524ef2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524ef5 81c1bc000000             add        ecx, 0xbc
00524efb e890b0eeff               call       0x40ff90
00524f00 83c040                   add        eax, 0x40
00524f03 50                       push       eax
00524f04 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524f07 e8247d0000               call       0x52cc30
00524f0c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524f0f 81c1bc000000             add        ecx, 0xbc
00524f15 e8e673eeff               call       0x40c300
00524f1a 83c040                   add        eax, 0x40
00524f1d 50                       push       eax
00524f1e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524f21 e85a800000               call       0x52cf80
00524f26 90                       nop        
00524f27 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524f2a 83b9f858000001           cmp        dword ptr [ecx + 0x58f8], 1
00524f31 7519                     jne        0x524f4c
00524f33 6a00                     push       0
00524f35 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524f38 81c108010000             add        ecx, 0x108
00524f3e e80d9ef9ff               call       0x4bed50
00524f43 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524f46 e8f5d5eeff               call       0x412540
00524f4b 90                       nop        
00524f4c 8b55fc                   mov        edx, dword ptr [ebp - 4]
00524f4f 81c208010000             add        edx, 0x108
00524f55 895594                   mov        dword ptr [ebp - 0x6c], edx
00524f58 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524f5b 81c1bc000000             add        ecx, 0xbc
00524f61 e89a73eeff               call       0x40c300
00524f66 50                       push       eax
00524f67 e84437f6ff               call       0x4886b0
00524f6c 83c404                   add        esp, 4
00524f6f 83c009                   add        eax, 9
00524f72 99                       cdq        
00524f73 b90a000000               mov        ecx, 0xa
00524f78 f7f9                     idiv       ecx
00524f7a 50                       push       eax
00524f7b 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00524f7e e85d75f7ff               call       0x49c4e0
00524f83 90                       nop        
00524f84 33d2                     xor        edx, edx
00524f86 7531                     jne        0x524fb9
00524f88 b804000000               mov        eax, 4
00524f8d 6bc800                   imul       ecx, eax, 0
00524f90 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
00524f97 7420                     je         0x524fb9
00524f99 ba04000000               mov        edx, 4
00524f9e 6bc200                   imul       eax, edx, 0
00524fa1 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
00524fa7 894d90                   mov        dword ptr [ebp - 0x70], ecx
00524faa 6a40                     push       0x40
00524fac 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00524faf e8cc52efff               call       0x41a280
00524fb4 8945d8                   mov        dword ptr [ebp - 0x28], eax
00524fb7 eb07                     jmp        0x524fc0
00524fb9 c745d800000000           mov        dword ptr [ebp - 0x28], 0
00524fc0 837dd800                 cmp        dword ptr [ebp - 0x28], 0
00524fc4 741e                     je         0x524fe4
00524fc6 8b55fc                   mov        edx, dword ptr [ebp - 4]
00524fc9 83c224                   add        edx, 0x24
00524fcc 89558c                   mov        dword ptr [ebp - 0x74], edx
00524fcf 6a00                     push       0
00524fd1 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00524fd4 e867a0f9ff               call       0x4bf040
00524fd9 6a49                     push       0x49
00524fdb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524fde e80d7c0000               call       0x52cbf0
00524fe3 90                       nop        
00524fe4 33c0                     xor        eax, eax
00524fe6 7534                     jne        0x52501c
00524fe8 b904000000               mov        ecx, 4
00524fed 6bd100                   imul       edx, ecx, 0
00524ff0 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00524ff7 7423                     je         0x52501c
00524ff9 b804000000               mov        eax, 4
00524ffe 6bc800                   imul       ecx, eax, 0
00525001 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00525007 895588                   mov        dword ptr [ebp - 0x78], edx
0052500a 6880000000               push       0x80
0052500f 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00525012 e86952efff               call       0x41a280
00525017 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0052501a eb07                     jmp        0x525023
0052501c c745d400000000           mov        dword ptr [ebp - 0x2c], 0
00525023 837dd400                 cmp        dword ptr [ebp - 0x2c], 0
00525027 741e                     je         0x525047
00525029 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052502c 83c024                   add        eax, 0x24
0052502f 894584                   mov        dword ptr [ebp - 0x7c], eax
00525032 6a00                     push       0
00525034 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
00525037 e8e49ff9ff               call       0x4bf020
0052503c 6a4a                     push       0x4a
0052503e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00525041 e8aa7b0000               call       0x52cbf0
00525046 90                       nop        
00525047 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052504a 83c124                   add        ecx, 0x24
0052504d e8de7ff3ff               call       0x45d030
00525052 85c0                     test       eax, eax
00525054 0f8491000000             je         0x5250eb
0052505a 6a00                     push       0
0052505c 6a0a                     push       0xa
0052505e b930a85b00               mov        ecx, 0x5ba830
00525063 e8081df0ff               call       0x426d70
00525068 e8f368ffff               call       0x51b960
0052506d 898578ffffff             mov        dword ptr [ebp - 0x88], eax
00525073 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00525076 83c124                   add        ecx, 0x24
00525079 894d80                   mov        dword ptr [ebp - 0x80], ecx
0052507c 6a08                     push       8
0052507e 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00525081 e83a90ffff               call       0x51e0c0
00525086 89857cffffff             mov        dword ptr [ebp - 0x84], eax
0052508c 8b957cffffff             mov        edx, dword ptr [ebp - 0x84]
00525092 52                       push       edx
00525093 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00525099 e8e27d0000               call       0x52ce80
0052509e 8b45fc                   mov        eax, dword ptr [ebp - 4]
005250a1 83c024                   add        eax, 0x24
005250a4 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
005250aa 6a02                     push       2
005250ac 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
005250b2 e8e98fffff               call       0x51e0a0
005250b7 83c025                   add        eax, 0x25
005250ba 50                       push       eax
005250bb 6a48                     push       0x48
005250bd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005250c0 e84b7c0000               call       0x52cd10
005250c5 6a00                     push       0
005250c7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005250ca 81c154010000             add        ecx, 0x154
005250d0 e84be4efff               call       0x423520
005250d5 90                       nop        
005250d6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005250d9 83b9f858000001           cmp        dword ptr [ecx + 0x58f8], 1
005250e0 7509                     jne        0x5250eb
005250e2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005250e5 e856d4eeff               call       0x412540
005250ea 90                       nop        
005250eb 8b55fc                   mov        edx, dword ptr [ebp - 4]
005250ee 83baf858000001           cmp        dword ptr [edx + 0x58f8], 1
005250f5 757d                     jne        0x525174
005250f7 33c0                     xor        eax, eax
005250f9 753a                     jne        0x525135
005250fb b904000000               mov        ecx, 4
00525100 6bd100                   imul       edx, ecx, 0
00525103 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
0052510a 7429                     je         0x525135
0052510c b804000000               mov        eax, 4
00525111 6bc800                   imul       ecx, eax, 0
00525114 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
0052511a 899570ffffff             mov        dword ptr [ebp - 0x90], edx
00525120 6801000800               push       0x80001
00525125 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
0052512b e8d04aefff               call       0x419c00
00525130 8945d0                   mov        dword ptr [ebp - 0x30], eax
00525133 eb07                     jmp        0x52513c
00525135 c745d000000000           mov        dword ptr [ebp - 0x30], 0
0052513c 837dd000                 cmp        dword ptr [ebp - 0x30], 0
00525140 7432                     je         0x525174
00525142 8b45fc                   mov        eax, dword ptr [ebp - 4]
00525145 0508010000               add        eax, 0x108
0052514a 89856cffffff             mov        dword ptr [ebp - 0x94], eax
00525150 6a00                     push       0
00525152 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00525158 e8c39ef9ff               call       0x4bf020
0052515d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00525160 e8dbd3eeff               call       0x412540
00525165 6a00                     push       0
00525167 6a07                     push       7
00525169 b930a85b00               mov        ecx, 0x5ba830
0052516e e8fd1bf0ff               call       0x426d70
00525173 90                       nop        
00525174 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00525177 81c1bc000000             add        ecx, 0xbc
0052517d e87e71eeff               call       0x40c300
00525182 83f803                   cmp        eax, 3
00525185 0f85d9040000             jne        0x525664
0052518b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052518e 83c124                   add        ecx, 0x24
00525191 e86a71eeff               call       0x40c300
00525196 83f807                   cmp        eax, 7
00525199 0f85c5040000             jne        0x525664
0052519f 33c9                     xor        ecx, ecx
005251a1 753a                     jne        0x5251dd
005251a3 ba04000000               mov        edx, 4
005251a8 6bc200                   imul       eax, edx, 0
005251ab 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
005251b2 7429                     je         0x5251dd
005251b4 b904000000               mov        ecx, 4
005251b9 6bd100                   imul       edx, ecx, 0
005251bc 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
005251c2 898568ffffff             mov        dword ptr [ebp - 0x98], eax
005251c8 680f010800               push       0x8010f
005251cd 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
005251d3 e8284aefff               call       0x419c00
005251d8 8945cc                   mov        dword ptr [ebp - 0x34], eax
005251db eb07                     jmp        0x5251e4
005251dd c745cc00000000           mov        dword ptr [ebp - 0x34], 0
005251e4 837dcc00                 cmp        dword ptr [ebp - 0x34], 0
005251e8 7414                     je         0x5251fe
005251ea c70548655c0000000000     mov        dword ptr [0x5c6548], 0
005251f4 c7054c655c0000000000     mov        dword ptr [0x5c654c], 0
005251fe 6800010000               push       0x100
00525203 6848645c00               push       0x5c6448
00525208 6848635c00               push       0x5c6348
0052520d e80eec0100               call       0x543e20
00525212 83c40c                   add        esp, 0xc
00525215 6848645c00               push       0x5c6448
0052521a e8f1a0ffff               call       0x51f310
0052521f 83c404                   add        esp, 4
00525222 8945e8                   mov        dword ptr [ebp - 0x18], eax
00525225 837de802                 cmp        dword ptr [ebp - 0x18], 2
00525229 0f850a030000             jne        0x525539
0052522f 6800010000               push       0x100
00525234 6a00                     push       0
00525236 6848625c00               push       0x5c6248
0052523b e820f50100               call       0x544760
00525240 83c40c                   add        esp, 0xc
00525243 b901000000               mov        ecx, 1
00525248 6bd141                   imul       edx, ecx, 0x41
0052524b b801000000               mov        eax, 1
00525250 6bc81e                   imul       ecx, eax, 0x1e
00525253 8a9248645c00             mov        dl, byte ptr [edx + 0x5c6448]
00525259 889148625c00             mov        byte ptr [ecx + 0x5c6248], dl
0052525f b801000000               mov        eax, 1
00525264 6bc842                   imul       ecx, eax, 0x42
00525267 ba01000000               mov        edx, 1
0052526c 6bc230                   imul       eax, edx, 0x30
0052526f 8a8948645c00             mov        cl, byte ptr [ecx + 0x5c6448]
00525275 888848625c00             mov        byte ptr [eax + 0x5c6248], cl
0052527b ba01000000               mov        edx, 1
00525280 6bc243                   imul       eax, edx, 0x43
00525283 b901000000               mov        ecx, 1
00525288 6bd12e                   imul       edx, ecx, 0x2e
0052528b 8a8048645c00             mov        al, byte ptr [eax + 0x5c6448]
00525291 888248625c00             mov        byte ptr [edx + 0x5c6248], al
00525297 b901000000               mov        ecx, 1
0052529c 6bd144                   imul       edx, ecx, 0x44
0052529f b801000000               mov        eax, 1
005252a4 c1e005                   shl        eax, 5
005252a7 8a8a48645c00             mov        cl, byte ptr [edx + 0x5c6448]
005252ad 888848625c00             mov        byte ptr [eax + 0x5c6248], cl
005252b3 ba01000000               mov        edx, 1
005252b8 6bc245                   imul       eax, edx, 0x45
005252bb b901000000               mov        ecx, 1
005252c0 6bd112                   imul       edx, ecx, 0x12
005252c3 8a8048645c00             mov        al, byte ptr [eax + 0x5c6448]
005252c9 888248625c00             mov        byte ptr [edx + 0x5c6248], al
005252cf b901000000               mov        ecx, 1
005252d4 6bd146                   imul       edx, ecx, 0x46
005252d7 b801000000               mov        eax, 1
005252dc 6bc821                   imul       ecx, eax, 0x21
005252df 8a9248645c00             mov        dl, byte ptr [edx + 0x5c6448]
005252e5 889148625c00             mov        byte ptr [ecx + 0x5c6248], dl
005252eb b801000000               mov        eax, 1
005252f0 6bc847                   imul       ecx, eax, 0x47
005252f3 ba01000000               mov        edx, 1
005252f8 6bc222                   imul       eax, edx, 0x22
005252fb 8a8948645c00             mov        cl, byte ptr [ecx + 0x5c6448]
00525301 888848625c00             mov        byte ptr [eax + 0x5c6248], cl
00525307 ba01000000               mov        edx, 1
0052530c 6bc248                   imul       eax, edx, 0x48
0052530f b901000000               mov        ecx, 1
00525314 6bd123                   imul       edx, ecx, 0x23
00525317 8a8048645c00             mov        al, byte ptr [eax + 0x5c6448]
0052531d 888248625c00             mov        byte ptr [edx + 0x5c6248], al
00525323 b901000000               mov        ecx, 1
00525328 6bd149                   imul       edx, ecx, 0x49
0052532b b801000000               mov        eax, 1
00525330 6bc817                   imul       ecx, eax, 0x17
00525333 8a9248645c00             mov        dl, byte ptr [edx + 0x5c6448]
00525339 889148625c00             mov        byte ptr [ecx + 0x5c6248], dl
0052533f b801000000               mov        eax, 1
00525344 6bc84a                   imul       ecx, eax, 0x4a
00525347 ba01000000               mov        edx, 1
0052534c 6bc224                   imul       eax, edx, 0x24
0052534f 8a8948645c00             mov        cl, byte ptr [ecx + 0x5c6448]
00525355 888848625c00             mov        byte ptr [eax + 0x5c6248], cl
0052535b ba01000000               mov        edx, 1
00525360 6bc24b                   imul       eax, edx, 0x4b
00525363 b901000000               mov        ecx, 1
00525368 6bd125                   imul       edx, ecx, 0x25
0052536b 8a8048645c00             mov        al, byte ptr [eax + 0x5c6448]
00525371 888248625c00             mov        byte ptr [edx + 0x5c6248], al
00525377 b901000000               mov        ecx, 1
0052537c 6bd14c                   imul       edx, ecx, 0x4c
0052537f b801000000               mov        eax, 1
00525384 6bc826                   imul       ecx, eax, 0x26
00525387 8a9248645c00             mov        dl, byte ptr [edx + 0x5c6448]
0052538d 889148625c00             mov        byte ptr [ecx + 0x5c6248], dl
00525393 b801000000               mov        eax, 1
00525398 6bc84d                   imul       ecx, eax, 0x4d
0052539b ba01000000               mov        edx, 1
005253a0 6bc232                   imul       eax, edx, 0x32
005253a3 8a8948645c00             mov        cl, byte ptr [ecx + 0x5c6448]
005253a9 888848625c00             mov        byte ptr [eax + 0x5c6248], cl
005253af ba01000000               mov        edx, 1
005253b4 6bc24e                   imul       eax, edx, 0x4e
005253b7 b901000000               mov        ecx, 1
005253bc 6bd131                   imul       edx, ecx, 0x31
005253bf 8a8048645c00             mov        al, byte ptr [eax + 0x5c6448]
005253c5 888248625c00             mov        byte ptr [edx + 0x5c6248], al
005253cb b901000000               mov        ecx, 1
005253d0 6bd14f                   imul       edx, ecx, 0x4f
005253d3 b801000000               mov        eax, 1
005253d8 6bc818                   imul       ecx, eax, 0x18
005253db 8a9248645c00             mov        dl, byte ptr [edx + 0x5c6448]
005253e1 889148625c00             mov        byte ptr [ecx + 0x5c6248], dl
005253e7 b801000000               mov        eax, 1
005253ec 6bc850                   imul       ecx, eax, 0x50
005253ef ba01000000               mov        edx, 1
005253f4 6bc219                   imul       eax, edx, 0x19
005253f7 8a8948645c00             mov        cl, byte ptr [ecx + 0x5c6448]
005253fd 888848625c00             mov        byte ptr [eax + 0x5c6248], cl
00525403 ba01000000               mov        edx, 1
00525408 6bc251                   imul       eax, edx, 0x51
0052540b b901000000               mov        ecx, 1
00525410 c1e104                   shl        ecx, 4
00525413 8a9048645c00             mov        dl, byte ptr [eax + 0x5c6448]
00525419 889148625c00             mov        byte ptr [ecx + 0x5c6248], dl
0052541f b801000000               mov        eax, 1
00525424 6bc852                   imul       ecx, eax, 0x52
00525427 ba01000000               mov        edx, 1
0052542c 6bc213                   imul       eax, edx, 0x13
0052542f 8a8948645c00             mov        cl, byte ptr [ecx + 0x5c6448]
00525435 888848625c00             mov        byte ptr [eax + 0x5c6248], cl
0052543b ba01000000               mov        edx, 1
00525440 6bc253                   imul       eax, edx, 0x53
00525443 b901000000               mov        ecx, 1
00525448 6bd11f                   imul       edx, ecx, 0x1f
0052544b 8a8048645c00             mov        al, byte ptr [eax + 0x5c6448]
00525451 888248625c00             mov        byte ptr [edx + 0x5c6248], al
00525457 b901000000               mov        ecx, 1
0052545c 6bd154                   imul       edx, ecx, 0x54
0052545f b801000000               mov        eax, 1
00525464 6bc814                   imul       ecx, eax, 0x14
00525467 8a9248645c00             mov        dl, byte ptr [edx + 0x5c6448]
0052546d 889148625c00             mov        byte ptr [ecx + 0x5c6248], dl
00525473 b801000000               mov        eax, 1
00525478 6bc855                   imul       ecx, eax, 0x55
0052547b ba01000000               mov        edx, 1
00525480 6bc216                   imul       eax, edx, 0x16
00525483 8a8948645c00             mov        cl, byte ptr [ecx + 0x5c6448]
00525489 888848625c00             mov        byte ptr [eax + 0x5c6248], cl
0052548f ba01000000               mov        edx, 1
00525494 6bc256                   imul       eax, edx, 0x56
00525497 b901000000               mov        ecx, 1
0052549c 6bd12f                   imul       edx, ecx, 0x2f
0052549f 8a8048645c00             mov        al, byte ptr [eax + 0x5c6448]
005254a5 888248625c00             mov        byte ptr [edx + 0x5c6248], al
005254ab b901000000               mov        ecx, 1
005254b0 6bd157                   imul       edx, ecx, 0x57
005254b3 b801000000               mov        eax, 1
005254b8 6bc811                   imul       ecx, eax, 0x11
005254bb 8a9248645c00             mov        dl, byte ptr [edx + 0x5c6448]
005254c1 889148625c00             mov        byte ptr [ecx + 0x5c6248], dl
005254c7 b801000000               mov        eax, 1
005254cc 6bc858                   imul       ecx, eax, 0x58
005254cf ba01000000               mov        edx, 1
005254d4 6bc22d                   imul       eax, edx, 0x2d
005254d7 8a8948645c00             mov        cl, byte ptr [ecx + 0x5c6448]
005254dd 888848625c00             mov        byte ptr [eax + 0x5c6248], cl
005254e3 ba01000000               mov        edx, 1
005254e8 6bc259                   imul       eax, edx, 0x59
005254eb b901000000               mov        ecx, 1
005254f0 6bd115                   imul       edx, ecx, 0x15
005254f3 8a8048645c00             mov        al, byte ptr [eax + 0x5c6448]
005254f9 888248625c00             mov        byte ptr [edx + 0x5c6248], al
005254ff b901000000               mov        ecx, 1
00525504 6bd15a                   imul       edx, ecx, 0x5a
00525507 b801000000               mov        eax, 1
0052550c 6bc82c                   imul       ecx, eax, 0x2c
0052550f 8a9248645c00             mov        dl, byte ptr [edx + 0x5c6448]
00525515 889148625c00             mov        byte ptr [ecx + 0x5c6248], dl
0052551b 6800010000               push       0x100
00525520 6848625c00               push       0x5c6248
00525525 6848645c00               push       0x5c6448
0052552a e8f1e80100               call       0x543e20
0052552f 83c40c                   add        esp, 0xc
00525532 c745e801000000           mov        dword ptr [ebp - 0x18], 1
00525539 837de801                 cmp        dword ptr [ebp - 0x18], 1
0052553d 0f85f2000000             jne        0x525635
00525543 c745f400000000           mov        dword ptr [ebp - 0xc], 0
0052554a eb09                     jmp        0x525555
0052554c 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0052554f 83c001                   add        eax, 1
00525552 8945f4                   mov        dword ptr [ebp - 0xc], eax
00525555 817df400010000           cmp        dword ptr [ebp - 0xc], 0x100
0052555c 7d2d                     jge        0x52558b
0052555e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00525561 0fb69148645c00           movzx      edx, byte ptr [ecx + 0x5c6448]
00525568 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0052556b 0fb68848635c00           movzx      ecx, byte ptr [eax + 0x5c6348]
00525572 33d1                     xor        edx, ecx
00525574 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00525577 0fb68848645c00           movzx      ecx, byte ptr [eax + 0x5c6448]
0052557e 23d1                     and        edx, ecx
00525580 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00525583 889048625c00             mov        byte ptr [eax + 0x5c6248], dl
00525589 ebc1                     jmp        0x52554c
0052558b 833d48655c000c           cmp        dword ptr [0x5c6548], 0xc
00525592 7225                     jb         0x5255b9
00525594 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052559a e8218cffff               call       0x51e1c0
0052559f 6a00                     push       0
005255a1 6a11                     push       0x11
005255a3 b930a85b00               mov        ecx, 0x5ba830
005255a8 e8c317f0ff               call       0x426d70
005255ad c70548655c0000000000     mov        dword ptr [0x5c6548], 0
005255b7 eb7c                     jmp        0x525635
005255b9 8b0d48655c00             mov        ecx, dword ptr [0x5c6548]
005255bf 8b148d98525700           mov        edx, dword ptr [ecx*4 + 0x575298]
005255c6 0fb68248625c00           movzx      eax, byte ptr [edx + 0x5c6248]
005255cd 2580000000               and        eax, 0x80
005255d2 741b                     je         0x5255ef
005255d4 8b0d48655c00             mov        ecx, dword ptr [0x5c6548]
005255da 83c101                   add        ecx, 1
005255dd 890d48655c00             mov        dword ptr [0x5c6548], ecx
005255e3 c7054c655c0000000000     mov        dword ptr [0x5c654c], 0
005255ed eb46                     jmp        0x525635
005255ef c645fb00                 mov        byte ptr [ebp - 5], 0
005255f3 c745f000000000           mov        dword ptr [ebp - 0x10], 0
005255fa eb09                     jmp        0x525605
005255fc 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
005255ff 83c201                   add        edx, 1
00525602 8955f0                   mov        dword ptr [ebp - 0x10], edx
00525605 837df039                 cmp        dword ptr [ebp - 0x10], 0x39
00525609 7d15                     jge        0x525620
0052560b 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0052560e 0fb68848625c00           movzx      ecx, byte ptr [eax + 0x5c6248]
00525615 0fb655fb                 movzx      edx, byte ptr [ebp - 5]
00525619 0bd1                     or         edx, ecx
0052561b 8855fb                   mov        byte ptr [ebp - 5], dl
0052561e ebdc                     jmp        0x5255fc
00525620 0fb645fb                 movzx      eax, byte ptr [ebp - 5]
00525624 2580000000               and        eax, 0x80
00525629 740a                     je         0x525635
0052562b c70548655c0000000000     mov        dword ptr [0x5c6548], 0
00525635 8b0d4c655c00             mov        ecx, dword ptr [0x5c654c]
0052563b 83c101                   add        ecx, 1
0052563e 890d4c655c00             mov        dword ptr [0x5c654c], ecx
00525644 813d4c655c002c010000     cmp        dword ptr [0x5c654c], 0x12c
0052564e 7e14                     jle        0x525664
00525650 c70548655c0000000000     mov        dword ptr [0x5c6548], 0
0052565a c7054c655c0000000000     mov        dword ptr [0x5c654c], 0
00525664 33d2                     xor        edx, edx
00525666 753a                     jne        0x5256a2
00525668 b804000000               mov        eax, 4
0052566d 6bc800                   imul       ecx, eax, 0
00525670 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
00525677 7429                     je         0x5256a2
00525679 ba04000000               mov        edx, 4
0052567e 6bc200                   imul       eax, edx, 0
00525681 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
00525687 898d64ffffff             mov        dword ptr [ebp - 0x9c], ecx
0052568d 6806010000               push       0x106
00525692 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
00525698 e86345efff               call       0x419c00
0052569d 8945c8                   mov        dword ptr [ebp - 0x38], eax
005256a0 eb07                     jmp        0x5256a9
005256a2 c745c800000000           mov        dword ptr [ebp - 0x38], 0
005256a9 837dc800                 cmp        dword ptr [ebp - 0x38], 0
005256ad 0f84ce000000             je         0x525781
005256b3 6a03                     push       3
005256b5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005256b8 e893770000               call       0x52ce50
005256bd 6a00                     push       0
005256bf 6a09                     push       9
005256c1 b930a85b00               mov        ecx, 0x5ba830
005256c6 e8a516f0ff               call       0x426d70
005256cb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005256ce 81c1bc000000             add        ecx, 0xbc
005256d4 e8276ceeff               call       0x40c300
005256d9 83c040                   add        eax, 0x40
005256dc 50                       push       eax
005256dd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005256e0 e84b750000               call       0x52cc30
005256e5 6a49                     push       0x49
005256e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005256ea e841750000               call       0x52cc30
005256ef 6a4a                     push       0x4a
005256f1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005256f4 e837750000               call       0x52cc30
005256f9 6a4b                     push       0x4b
005256fb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005256fe e82d750000               call       0x52cc30
00525703 6a4c                     push       0x4c
00525705 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00525708 e823750000               call       0x52cc30
0052570d 6a45                     push       0x45
0052570f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00525712 e819750000               call       0x52cc30
00525717 6a46                     push       0x46
00525719 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052571c e80f750000               call       0x52cc30
00525721 6a47                     push       0x47
00525723 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00525726 e805750000               call       0x52cc30
0052572b 6a4d                     push       0x4d
0052572d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00525730 e8fb740000               call       0x52cc30
00525735 6a48                     push       0x48
00525737 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052573a e8f1740000               call       0x52cc30
0052573f 90                       nop        
00525740 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
00525747 eb09                     jmp        0x525752
00525749 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0052574c 83c201                   add        edx, 1
0052574f 8955ec                   mov        dword ptr [ebp - 0x14], edx
00525752 837dec0a                 cmp        dword ptr [ebp - 0x14], 0xa
00525756 7d27                     jge        0x52577f
00525758 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052575b 0594030000               add        eax, 0x394
00525760 898560ffffff             mov        dword ptr [ebp - 0xa0], eax
00525766 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00525769 51                       push       ecx
0052576a 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
00525770 e80beeeeff               call       0x414580
00525775 8bc8                     mov        ecx, eax
00525777 e8c438f5ff               call       0x479040
0052577c 90                       nop        
0052577d ebca                     jmp        0x525749
0052577f eb51                     jmp        0x5257d2
00525781 eb4f                     jmp        0x5257d2
00525783 8b55fc                   mov        edx, dword ptr [ebp - 4]
00525786 81c254010000             add        edx, 0x154
0052578c 89955cffffff             mov        dword ptr [ebp - 0xa4], edx
00525792 6a06                     push       6
00525794 8b8d5cffffff             mov        ecx, dword ptr [ebp - 0xa4]
0052579a e821deefff               call       0x4235c0
0052579f 0fb6c0                   movzx      eax, al
005257a2 85c0                     test       eax, eax
005257a4 742c                     je         0x5257d2
005257a6 e8b561ffff               call       0x51b960
005257ab 8bc8                     mov        ecx, eax
005257ad e88e40ffff               call       0x519840
005257b2 6a27                     push       0x27
005257b4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005257b7 e874740000               call       0x52cc30
005257bc 6a0a                     push       0xa
005257be 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005257c1 e80a770000               call       0x52ced0
005257c6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005257c9 83c124                   add        ecx, 0x24
005257cc e83f08faff               call       0x4c6010
005257d1 90                       nop        
005257d2 b801000000               mov        eax, 1
005257d7 8be5                     mov        esp, ebp
005257d9 5d                       pop        ebp
005257da c3                       ret        
005257db 90                       nop        
005257dc 394c5200                 cmp        dword ptr [edx + edx*2], ecx
005257e0 b54d                     mov        ch, 0x4d
005257e2 52                       push       edx
005257e3 00e1                     add        cl, ah
005257e5 4d                       dec        ebp
005257e6 52                       push       edx
005257e7 0083575200cc             add        byte ptr [ebx - 0x33ffada9], al
005257ed cc                       int3       
005257ee cc                       int3       
005257ef cc                       int3       
