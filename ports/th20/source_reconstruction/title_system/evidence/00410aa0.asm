00410aa0 55                       push       ebp
00410aa1 8bec                     mov        ebp, esp
00410aa3 6aff                     push       -1
00410aa5 688d7a5600               push       0x567a8d
00410aaa 64a100000000             mov        eax, dword ptr fs:[0]
00410ab0 50                       push       eax
00410ab1 83ec38                   sub        esp, 0x38
00410ab4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00410ab9 33c5                     xor        eax, ebp
00410abb 8945f0                   mov        dword ptr [ebp - 0x10], eax
00410abe 50                       push       eax
00410abf 8d45f4                   lea        eax, [ebp - 0xc]
00410ac2 64a300000000             mov        dword ptr fs:[0], eax
00410ac8 6a02                     push       2
00410aca b940025c00               mov        ecx, 0x5c0240
00410acf e83cb8ffff               call       0x40c310
00410ad4 50                       push       eax
00410ad5 8d4dec                   lea        ecx, [ebp - 0x14]
00410ad8 e853acffff               call       0x40b730
00410add c745fc00000000           mov        dword ptr [ebp - 4], 0
00410ae4 837d1000                 cmp        dword ptr [ebp + 0x10], 0
00410ae8 0f85ca000000             jne        0x410bb8
00410aee 6a5c                     push       0x5c
00410af0 8b4508                   mov        eax, dword ptr [ebp + 8]
00410af3 50                       push       eax
00410af4 e8c73d1300               call       0x5448c0
00410af9 83c408                   add        esp, 8
00410afc 8945cc                   mov        dword ptr [ebp - 0x34], eax
00410aff 837dcc00                 cmp        dword ptr [ebp - 0x34], 0
00410b03 7508                     jne        0x410b0d
00410b05 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00410b08 894dcc                   mov        dword ptr [ebp - 0x34], ecx
00410b0b eb09                     jmp        0x410b16
00410b0d 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
00410b10 83c201                   add        edx, 1
00410b13 8955cc                   mov        dword ptr [ebp - 0x34], edx
00410b16 6a2f                     push       0x2f
00410b18 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
00410b1b 50                       push       eax
00410b1c e89f3d1300               call       0x5448c0
00410b21 83c408                   add        esp, 8
00410b24 8945cc                   mov        dword ptr [ebp - 0x34], eax
00410b27 837dcc00                 cmp        dword ptr [ebp - 0x34], 0
00410b2b 7508                     jne        0x410b35
00410b2d 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00410b30 894dcc                   mov        dword ptr [ebp - 0x34], ecx
00410b33 eb09                     jmp        0x410b3e
00410b35 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
00410b38 83c201                   add        edx, 1
00410b3b 8955cc                   mov        dword ptr [ebp - 0x34], edx
00410b3e 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
00410b41 50                       push       eax
00410b42 b9c8665b00               mov        ecx, 0x5b66c8
00410b47 e8d49a1200               call       0x53a620
00410b4c 8945d0                   mov        dword ptr [ebp - 0x30], eax
00410b4f 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
00410b53 7408                     je         0x410b5d
00410b55 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00410b58 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00410b5b 8911                     mov        dword ptr [ecx], edx
00410b5d 837dd000                 cmp        dword ptr [ebp - 0x30], 0
00410b61 7505                     jne        0x410b68
00410b63 e94d010000               jmp        0x410cb5
00410b68 837dd000                 cmp        dword ptr [ebp - 0x30], 0
00410b6c 744a                     je         0x410bb8
00410b6e 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
00410b71 50                       push       eax
00410b72 68f8c55600               push       0x56c5f8
00410b77 e834bbffff               call       0x40c6b0
00410b7c 83c408                   add        esp, 8
00410b7f 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00410b82 51                       push       ecx
00410b83 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00410b86 52                       push       edx
00410b87 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00410b8d e87eea0000               call       0x41f610
00410b92 8945c8                   mov        dword ptr [ebp - 0x38], eax
00410b95 837dc800                 cmp        dword ptr [ebp - 0x38], 0
00410b99 7505                     jne        0x410ba0
00410b9b e915010000               jmp        0x410cb5
00410ba0 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
00410ba3 50                       push       eax
00410ba4 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00410ba7 51                       push       ecx
00410ba8 b9c8665b00               mov        ecx, 0x5b66c8
00410bad e80e981200               call       0x53a3c0
00410bb2 90                       nop        
00410bb3 e9e3000000               jmp        0x410c9b
00410bb8 8b5508                   mov        edx, dword ptr [ebp + 8]
00410bbb 52                       push       edx
00410bbc 680cc65600               push       0x56c60c
00410bc1 e8eabaffff               call       0x40c6b0
00410bc6 83c408                   add        esp, 8
00410bc9 6a00                     push       0
00410bcb 8d4508                   lea        eax, [ebp + 8]
00410bce 50                       push       eax
00410bcf 8d4dd4                   lea        ecx, [ebp - 0x2c]
00410bd2 e849bbffff               call       0x40c720
00410bd7 6a00                     push       0
00410bd9 6880000008               push       0x8000080
00410bde 6a03                     push       3
00410be0 6a00                     push       0
00410be2 6a01                     push       1
00410be4 6800000080               push       0x80000000
00410be9 8d4dd4                   lea        ecx, [ebp - 0x2c]
00410bec e87ff3ffff               call       0x40ff70
00410bf1 50                       push       eax
00410bf2 ff1564c15600             call       dword ptr [0x56c164]
00410bf8 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00410bfb 8d4dd4                   lea        ecx, [ebp - 0x2c]
00410bfe e8bddaffff               call       0x40e6c0
00410c03 90                       nop        
00410c04 837dc4ff                 cmp        dword ptr [ebp - 0x3c], -1
00410c08 7516                     jne        0x410c20
00410c0a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00410c0d 51                       push       ecx
00410c0e 681cc65600               push       0x56c61c
00410c13 e898baffff               call       0x40c6b0
00410c18 83c408                   add        esp, 8
00410c1b e995000000               jmp        0x410cb5
00410c20 6a00                     push       0
00410c22 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
00410c25 52                       push       edx
00410c26 ff1560c15600             call       dword ptr [0x56c160]
00410c2c 8945d0                   mov        dword ptr [ebp - 0x30], eax
00410c2f 8b4508                   mov        eax, dword ptr [ebp + 8]
00410c32 50                       push       eax
00410c33 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00410c36 51                       push       ecx
00410c37 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00410c3d e8cee90000               call       0x41f610
00410c42 8945c8                   mov        dword ptr [ebp - 0x38], eax
00410c45 837dc800                 cmp        dword ptr [ebp - 0x38], 0
00410c49 751e                     jne        0x410c69
00410c4b 8b5508                   mov        edx, dword ptr [ebp + 8]
00410c4e 52                       push       edx
00410c4f 6838c65600               push       0x56c638
00410c54 e857baffff               call       0x40c6b0
00410c59 83c408                   add        esp, 8
00410c5c 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
00410c5f 50                       push       eax
00410c60 ff1534c15600             call       dword ptr [0x56c134]
00410c66 90                       nop        
00410c67 eb4c                     jmp        0x410cb5
00410c69 6a00                     push       0
00410c6b 8d4dd0                   lea        ecx, [ebp - 0x30]
00410c6e 51                       push       ecx
00410c6f 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00410c72 52                       push       edx
00410c73 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
00410c76 50                       push       eax
00410c77 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00410c7a 51                       push       ecx
00410c7b ff15b0c05600             call       dword ptr [0x56c0b0]
00410c81 90                       nop        
00410c82 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
00410c86 7408                     je         0x410c90
00410c88 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00410c8b 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00410c8e 8902                     mov        dword ptr [edx], eax
00410c90 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00410c93 51                       push       ecx
00410c94 ff1534c15600             call       dword ptr [0x56c134]
00410c9a 90                       nop        
00410c9b 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
00410c9e 8955c0                   mov        dword ptr [ebp - 0x40], edx
00410ca1 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00410ca8 8d4dec                   lea        ecx, [ebp - 0x14]
00410cab e850acffff               call       0x40b900
00410cb0 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
00410cb3 eb19                     jmp        0x410cce
00410cb5 c745bc00000000           mov        dword ptr [ebp - 0x44], 0
00410cbc c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00410cc3 8d4dec                   lea        ecx, [ebp - 0x14]
00410cc6 e835acffff               call       0x40b900
00410ccb 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
00410cce 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00410cd1 64890d00000000           mov        dword ptr fs:[0], ecx
00410cd8 59                       pop        ecx
00410cd9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00410cdc 33cd                     xor        ecx, ebp
00410cde e80b1d1300               call       0x5429ee
00410ce3 8be5                     mov        esp, ebp
00410ce5 5d                       pop        ebp
00410ce6 c3                       ret        
00410ce7 cc                       int3       
00410ce8 cc                       int3       
00410ce9 cc                       int3       
00410cea cc                       int3       
00410ceb cc                       int3       
00410cec cc                       int3       
00410ced cc                       int3       
00410cee cc                       int3       
00410cef cc                       int3       
