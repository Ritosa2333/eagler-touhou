00410e70 55                       push       ebp
00410e71 8bec                     mov        ebp, esp
00410e73 6aff                     push       -1
00410e75 68107b5600               push       0x567b10
00410e7a 64a100000000             mov        eax, dword ptr fs:[0]
00410e80 50                       push       eax
00410e81 81ec2c020000             sub        esp, 0x22c
00410e87 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00410e8c 33c5                     xor        eax, ebp
00410e8e 8945f0                   mov        dword ptr [ebp - 0x10], eax
00410e91 50                       push       eax
00410e92 8d45f4                   lea        eax, [ebp - 0xc]
00410e95 64a300000000             mov        dword ptr fs:[0], eax
00410e9b 6a02                     push       2
00410e9d b940025c00               mov        ecx, 0x5c0240
00410ea2 e869b4ffff               call       0x40c310
00410ea7 50                       push       eax
00410ea8 8d8de0fdffff             lea        ecx, [ebp - 0x220]
00410eae e87da8ffff               call       0x40b730
00410eb3 c745fc00000000           mov        dword ptr [ebp - 4], 0
00410eba 680a020000               push       0x20a
00410ebf 6a00                     push       0
00410ec1 8d85e4fdffff             lea        eax, [ebp - 0x21c]
00410ec7 50                       push       eax
00410ec8 e893381300               call       0x544760
00410ecd 83c40c                   add        esp, 0xc
00410ed0 6804010000               push       0x104
00410ed5 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
00410edb 51                       push       ecx
00410edc 6aff                     push       -1
00410ede 8b5508                   mov        edx, dword ptr [ebp + 8]
00410ee1 52                       push       edx
00410ee2 6a00                     push       0
00410ee4 68a4030000               push       0x3a4
00410ee9 ff1524c15600             call       dword ptr [0x56c124]
00410eef 6a00                     push       0
00410ef1 6880000000               push       0x80
00410ef6 6a02                     push       2
00410ef8 6a00                     push       0
00410efa 6a01                     push       1
00410efc 6800000040               push       0x40000000
00410f01 8d85e4fdffff             lea        eax, [ebp - 0x21c]
00410f07 50                       push       eax
00410f08 ff1564c15600             call       dword ptr [0x56c164]
00410f0e 8985d4fdffff             mov        dword ptr [ebp - 0x22c], eax
00410f14 83bdd4fdffffff           cmp        dword ptr [ebp - 0x22c], -1
00410f1b 757b                     jne        0x410f98
00410f1d c785d8fdffff00000000     mov        dword ptr [ebp - 0x228], 0
00410f27 6a00                     push       0
00410f29 6a00                     push       0
00410f2b 8d8dd8fdffff             lea        ecx, [ebp - 0x228]
00410f31 51                       push       ecx
00410f32 6800040000               push       0x400
00410f37 ff1530c15600             call       dword ptr [0x56c130]
00410f3d 50                       push       eax
00410f3e 6a00                     push       0
00410f40 6800130000               push       0x1300
00410f45 ff1528c15600             call       dword ptr [0x56c128]
00410f4b 8b95d8fdffff             mov        edx, dword ptr [ebp - 0x228]
00410f51 52                       push       edx
00410f52 8b4508                   mov        eax, dword ptr [ebp + 8]
00410f55 50                       push       eax
00410f56 6858c65600               push       0x56c658
00410f5b e850b7ffff               call       0x40c6b0
00410f60 83c40c                   add        esp, 0xc
00410f63 8b8dd8fdffff             mov        ecx, dword ptr [ebp - 0x228]
00410f69 51                       push       ecx
00410f6a ff152cc15600             call       dword ptr [0x56c12c]
00410f70 90                       nop        
00410f71 c785d0fdffffffffffff     mov        dword ptr [ebp - 0x230], 0xffffffff
00410f7b c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00410f82 8d8de0fdffff             lea        ecx, [ebp - 0x220]
00410f88 e873a9ffff               call       0x40b900
00410f8d 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
00410f93 e9ac000000               jmp        0x411044
00410f98 6a00                     push       0
00410f9a 8d95dcfdffff             lea        edx, [ebp - 0x224]
00410fa0 52                       push       edx
00410fa1 8b4510                   mov        eax, dword ptr [ebp + 0x10]
00410fa4 50                       push       eax
00410fa5 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00410fa8 51                       push       ecx
00410fa9 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
00410faf 52                       push       edx
00410fb0 ff1538c15600             call       dword ptr [0x56c138]
00410fb6 90                       nop        
00410fb7 8b4510                   mov        eax, dword ptr [ebp + 0x10]
00410fba 3b85dcfdffff             cmp        eax, dword ptr [ebp - 0x224]
00410fc0 7442                     je         0x411004
00410fc2 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
00410fc8 51                       push       ecx
00410fc9 ff1534c15600             call       dword ptr [0x56c134]
00410fcf 8b5508                   mov        edx, dword ptr [ebp + 8]
00410fd2 52                       push       edx
00410fd3 6874c65600               push       0x56c674
00410fd8 e8d3b6ffff               call       0x40c6b0
00410fdd 83c408                   add        esp, 8
00410fe0 c785ccfdfffffeffffff     mov        dword ptr [ebp - 0x234], 0xfffffffe
00410fea c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00410ff1 8d8de0fdffff             lea        ecx, [ebp - 0x220]
00410ff7 e804a9ffff               call       0x40b900
00410ffc 8b85ccfdffff             mov        eax, dword ptr [ebp - 0x234]
00411002 eb40                     jmp        0x411044
00411004 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
0041100a 50                       push       eax
0041100b ff1534c15600             call       dword ptr [0x56c134]
00411011 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00411014 51                       push       ecx
00411015 6890c65600               push       0x56c690
0041101a e891b6ffff               call       0x40c6b0
0041101f 83c408                   add        esp, 8
00411022 c785c8fdffff00000000     mov        dword ptr [ebp - 0x238], 0
0041102c c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00411033 8d8de0fdffff             lea        ecx, [ebp - 0x220]
00411039 e8c2a8ffff               call       0x40b900
0041103e 8b85c8fdffff             mov        eax, dword ptr [ebp - 0x238]
00411044 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00411047 64890d00000000           mov        dword ptr fs:[0], ecx
0041104e 59                       pop        ecx
0041104f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00411052 33cd                     xor        ecx, ebp
00411054 e895191300               call       0x5429ee
00411059 8be5                     mov        esp, ebp
0041105b 5d                       pop        ebp
0041105c c3                       ret        
0041105d cc                       int3       
0041105e cc                       int3       
0041105f cc                       int3       
