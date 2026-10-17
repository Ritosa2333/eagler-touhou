00508b90 55                       push       ebp
00508b91 8bec                     mov        ebp, esp
00508b93 6aff                     push       -1
00508b95 68a5a75600               push       0x56a7a5
00508b9a 64a100000000             mov        eax, dword ptr fs:[0]
00508ba0 50                       push       eax
00508ba1 83ec74                   sub        esp, 0x74
00508ba4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00508ba9 33c5                     xor        eax, ebp
00508bab 8945f0                   mov        dword ptr [ebp - 0x10], eax
00508bae 56                       push       esi
00508baf 50                       push       eax
00508bb0 8d45f4                   lea        eax, [ebp - 0xc]
00508bb3 64a300000000             mov        dword ptr fs:[0], eax
00508bb9 894db8                   mov        dword ptr [ebp - 0x48], ecx
00508bbc 8b4508                   mov        eax, dword ptr [ebp + 8]
00508bbf 50                       push       eax
00508bc0 6800010000               push       0x100
00508bc5 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00508bc8 81c160020000             add        ecx, 0x260
00508bce 51                       push       ecx
00508bcf e83cfa0300               call       0x548610
00508bd4 83c40c                   add        esp, 0xc
00508bd7 6a00                     push       0
00508bd9 68e1675b00               push       0x5b67e1
00508bde 8d4dd4                   lea        ecx, [ebp - 0x2c]
00508be1 e8eaf0f0ff               call       0x417cd0
00508be6 c745fc00000000           mov        dword ptr [ebp - 4], 0
00508bed 68a4ca5600               push       0x56caa4
00508bf2 8d4dd4                   lea        ecx, [ebp - 0x2c]
00508bf5 e846f2f0ff               call       0x417e40
00508bfa 8d5508                   lea        edx, [ebp + 8]
00508bfd 52                       push       edx
00508bfe 8d4dd4                   lea        ecx, [ebp - 0x2c]
00508c01 e81afafcff               call       0x4d8620
00508c06 8d45bc                   lea        eax, [ebp - 0x44]
00508c09 50                       push       eax
00508c0a 8d4dd4                   lea        ecx, [ebp - 0x2c]
00508c0d e87e5bf1ff               call       0x41e790
00508c12 c645fc01                 mov        byte ptr [ebp - 4], 1
00508c16 8d4dbc                   lea        ecx, [ebp - 0x44]
00508c19 e81273f0ff               call       0x40ff30
00508c1e 8945a8                   mov        dword ptr [ebp - 0x58], eax
00508c21 c745b000000000           mov        dword ptr [ebp - 0x50], 0
00508c28 b968a55b00               mov        ecx, 0x5ba568
00508c2d e85ef7faff               call       0x4b8390
00508c32 85c0                     test       eax, eax
00508c34 0f85b9000000             jne        0x508cf3
00508c3a 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00508c3d 51                       push       ecx
00508c3e e85d7bf0ff               call       0x4107a0
00508c43 83c404                   add        esp, 4
00508c46 0fb6d0                   movzx      edx, al
00508c49 85d2                     test       edx, edx
00508c4b 7505                     jne        0x508c52
00508c4d e9d2020000               jmp        0x508f24
00508c52 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
00508c55 50                       push       eax
00508c56 e89580f0ff               call       0x410cf0
00508c5b 83c404                   add        esp, 4
00508c5e 85c0                     test       eax, eax
00508c60 7405                     je         0x508c67
00508c62 e9bd020000               jmp        0x508f24
00508c67 6a30                     push       0x30
00508c69 e8227df0ff               call       0x410990
00508c6e 83c404                   add        esp, 4
00508c71 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00508c74 894118                   mov        dword ptr [ecx + 0x18], eax
00508c77 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
00508c7a 8b4218                   mov        eax, dword ptr [edx + 0x18]
00508c7d 813874323072             cmp        dword ptr [eax], 0x72303274
00508c83 7539                     jne        0x508cbe
00508c85 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00508c88 8b5118                   mov        edx, dword ptr [ecx + 0x18]
00508c8b 0fb74204                 movzx      eax, word ptr [edx + 4]
00508c8f 83f801                   cmp        eax, 1
00508c92 752a                     jne        0x508cbe
00508c94 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00508c97 8b5118                   mov        edx, dword ptr [ecx + 0x18]
00508c9a 837a1c30                 cmp        dword ptr [edx + 0x1c], 0x30
00508c9e 751e                     jne        0x508cbe
00508ca0 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
00508ca3 8b4818                   mov        ecx, dword ptr [eax + 0x18]
00508ca6 81792000010000           cmp        dword ptr [ecx + 0x20], 0x100
00508cad 750f                     jne        0x508cbe
00508caf 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
00508cb2 8b4218                   mov        eax, dword ptr [edx + 0x18]
00508cb5 817824a0020000           cmp        dword ptr [eax + 0x24], 0x2a0
00508cbc 7418                     je         0x508cd6
00508cbe 6838365700               push       0x573638
00508cc3 e8e839f0ff               call       0x40c6b0
00508cc8 83c404                   add        esp, 4
00508ccb e8107af0ff               call       0x4106e0
00508cd0 90                       nop        
00508cd1 e94e020000               jmp        0x508f24
00508cd6 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00508cd9 8b5118                   mov        edx, dword ptr [ecx + 0x18]
00508cdc 8b4228                   mov        eax, dword ptr [edx + 0x28]
00508cdf 50                       push       eax
00508ce0 e8ab7cf0ff               call       0x410990
00508ce5 83c404                   add        esp, 4
00508ce8 8945b0                   mov        dword ptr [ebp - 0x50], eax
00508ceb e8f079f0ff               call       0x4106e0
00508cf0 90                       nop        
00508cf1 eb29                     jmp        0x508d1c
00508cf3 6a00                     push       0
00508cf5 8d4dec                   lea        ecx, [ebp - 0x14]
00508cf8 51                       push       ecx
00508cf9 8b5508                   mov        edx, dword ptr [ebp + 8]
00508cfc 52                       push       edx
00508cfd e89e7df0ff               call       0x410aa0
00508d02 83c40c                   add        esp, 0xc
00508d05 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00508d08 894118                   mov        dword ptr [ecx + 0x18], eax
00508d0b ba30000000               mov        edx, 0x30
00508d10 c1e200                   shl        edx, 0
00508d13 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
00508d16 035018                   add        edx, dword ptr [eax + 0x18]
00508d19 8955b0                   mov        dword ptr [ebp - 0x50], edx
00508d1c 6870365700               push       0x573670
00508d21 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00508d24 8b5118                   mov        edx, dword ptr [ecx + 0x18]
00508d27 8b422c                   mov        eax, dword ptr [edx + 0x2c]
00508d2a 50                       push       eax
00508d2b 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00508d31 e8da68f1ff               call       0x41f610
00508d36 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00508d39 898148020000             mov        dword ptr [ecx + 0x248], eax
00508d3f 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
00508d42 8b4218                   mov        eax, dword ptr [edx + 0x18]
00508d45 8b4828                   mov        ecx, dword ptr [eax + 0x28]
00508d48 51                       push       ecx
00508d49 6800040000               push       0x400
00508d4e 68e1000000               push       0xe1
00508d53 6a5c                     push       0x5c
00508d55 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
00508d58 8b4218                   mov        eax, dword ptr [edx + 0x18]
00508d5b 8b4828                   mov        ecx, dword ptr [eax + 0x28]
00508d5e 51                       push       ecx
00508d5f 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
00508d62 52                       push       edx
00508d63 e87873f0ff               call       0x4100e0
00508d68 83c418                   add        esp, 0x18
00508d6b 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
00508d6e 8b4818                   mov        ecx, dword ptr [eax + 0x18]
00508d71 8b5128                   mov        edx, dword ptr [ecx + 0x28]
00508d74 52                       push       edx
00508d75 6800010000               push       0x100
00508d7a 6a3a                     push       0x3a
00508d7c 6a7d                     push       0x7d
00508d7e 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
00508d81 8b4818                   mov        ecx, dword ptr [eax + 0x18]
00508d84 8b5128                   mov        edx, dword ptr [ecx + 0x28]
00508d87 52                       push       edx
00508d88 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00508d8b 50                       push       eax
00508d8c e84f73f0ff               call       0x4100e0
00508d91 83c418                   add        esp, 0x18
00508d94 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00508d97 8b5118                   mov        edx, dword ptr [ecx + 0x18]
00508d9a 8b422c                   mov        eax, dword ptr [edx + 0x2c]
00508d9d 50                       push       eax
00508d9e 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00508da1 8b9148020000             mov        edx, dword ptr [ecx + 0x248]
00508da7 52                       push       edx
00508da8 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
00508dab 8b4818                   mov        ecx, dword ptr [eax + 0x18]
00508dae 8b5128                   mov        edx, dword ptr [ecx + 0x28]
00508db1 52                       push       edx
00508db2 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00508db5 50                       push       eax
00508db6 e835040300               call       0x5391f0
00508dbb 83c410                   add        esp, 0x10
00508dbe 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00508dc1 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
00508dc4 8b8248020000             mov        eax, dword ptr [edx + 0x248]
00508dca 89411c                   mov        dword ptr [ecx + 0x1c], eax
00508dcd b900010000               mov        ecx, 0x100
00508dd2 c1e100                   shl        ecx, 0
00508dd5 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
00508dd8 034a1c                   add        ecx, dword ptr [edx + 0x1c]
00508ddb 894db4                   mov        dword ptr [ebp - 0x4c], ecx
00508dde c745ac00000000           mov        dword ptr [ebp - 0x54], 0
00508de5 eb09                     jmp        0x508df0
00508de7 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
00508dea 83c001                   add        eax, 1
00508ded 8945ac                   mov        dword ptr [ebp - 0x54], eax
00508df0 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00508df3 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
00508df6 83bad400000008           cmp        dword ptr [edx + 0xd4], 8
00508dfd 7c09                     jl         0x508e08
00508dff c745a406000000           mov        dword ptr [ebp - 0x5c], 6
00508e06 eb0f                     jmp        0x508e17
00508e08 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
00508e0b 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
00508e0e 8b91d4000000             mov        edx, dword ptr [ecx + 0xd4]
00508e14 8955a4                   mov        dword ptr [ebp - 0x5c], edx
00508e17 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
00508e1a 3b45a4                   cmp        eax, dword ptr [ebp - 0x5c]
00508e1d 0f8db0000000             jge        0x508ed3
00508e23 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00508e26 81c1e8000000             add        ecx, 0xe8
00508e2c 894da0                   mov        dword ptr [ebp - 0x60], ecx
00508e2f 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
00508e32 8b02                     mov        eax, dword ptr [edx]
00508e34 50                       push       eax
00508e35 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00508e38 e8f3dff7ff               call       0x486e30
00508e3d 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00508e40 894810                   mov        dword ptr [eax + 0x10], ecx
00508e43 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
00508e46 81c2e8000000             add        edx, 0xe8
00508e4c 89559c                   mov        dword ptr [ebp - 0x64], edx
00508e4f bea0020000               mov        esi, 0x2a0
00508e54 c1e600                   shl        esi, 0
00508e57 0375b4                   add        esi, dword ptr [ebp - 0x4c]
00508e5a 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
00508e5d 8b08                     mov        ecx, dword ptr [eax]
00508e5f 51                       push       ecx
00508e60 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
00508e63 e8c8dff7ff               call       0x486e30
00508e68 8930                     mov        dword ptr [eax], esi
00508e6a 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
00508e6d 81c2e8000000             add        edx, 0xe8
00508e73 895598                   mov        dword ptr [ebp - 0x68], edx
00508e76 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
00508e79 8b08                     mov        ecx, dword ptr [eax]
00508e7b 51                       push       ecx
00508e7c 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00508e7f e8acdff7ff               call       0x486e30
00508e84 8b10                     mov        edx, dword ptr [eax]
00508e86 895594                   mov        dword ptr [ebp - 0x6c], edx
00508e89 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
00508e8c 6b480806                 imul       ecx, dword ptr [eax + 8], 6
00508e90 894d90                   mov        dword ptr [ebp - 0x70], ecx
00508e93 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
00508e96 035590                   add        edx, dword ptr [ebp - 0x70]
00508e99 895588                   mov        dword ptr [ebp - 0x78], edx
00508e9c 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
00508e9f 05e8000000               add        eax, 0xe8
00508ea4 89458c                   mov        dword ptr [ebp - 0x74], eax
00508ea7 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00508eaa 8b11                     mov        edx, dword ptr [ecx]
00508eac 52                       push       edx
00508ead 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00508eb0 e87bdff7ff               call       0x486e30
00508eb5 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00508eb8 894808                   mov        dword ptr [eax + 8], ecx
00508ebb 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
00508ebe 8b420c                   mov        eax, dword ptr [edx + 0xc]
00508ec1 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00508ec4 8d9401a0020000           lea        edx, [ecx + eax + 0x2a0]
00508ecb 8955b4                   mov        dword ptr [ebp - 0x4c], edx
00508ece e914ffffff               jmp        0x508de7
00508ed3 b968a55b00               mov        ecx, 0x5ba568
00508ed8 e8b3f4faff               call       0x4b8390
00508edd 85c0                     test       eax, eax
00508edf 751c                     jne        0x508efd
00508ee1 837db000                 cmp        dword ptr [ebp - 0x50], 0
00508ee5 7416                     je         0x508efd
00508ee7 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00508eea 50                       push       eax
00508eeb 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00508ef1 e87a67f1ff               call       0x41f670
00508ef6 c745b000000000           mov        dword ptr [ebp - 0x50], 0
00508efd c7458400000000           mov        dword ptr [ebp - 0x7c], 0
00508f04 c645fc00                 mov        byte ptr [ebp - 4], 0
00508f08 8d4dbc                   lea        ecx, [ebp - 0x44]
00508f0b e89056f0ff               call       0x40e5a0
00508f10 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00508f17 8d4dd4                   lea        ecx, [ebp - 0x2c]
00508f1a e8a157f0ff               call       0x40e6c0
00508f1f 8b4584                   mov        eax, dword ptr [ebp - 0x7c]
00508f22 eb25                     jmp        0x508f49
00508f24 c74580ffffffff           mov        dword ptr [ebp - 0x80], 0xffffffff
00508f2b c645fc00                 mov        byte ptr [ebp - 4], 0
00508f2f 8d4dbc                   lea        ecx, [ebp - 0x44]
00508f32 e86956f0ff               call       0x40e5a0
00508f37 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00508f3e 8d4dd4                   lea        ecx, [ebp - 0x2c]
00508f41 e87a57f0ff               call       0x40e6c0
00508f46 8b4580                   mov        eax, dword ptr [ebp - 0x80]
00508f49 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00508f4c 64890d00000000           mov        dword ptr fs:[0], ecx
00508f53 59                       pop        ecx
00508f54 5e                       pop        esi
00508f55 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00508f58 33cd                     xor        ecx, ebp
00508f5a e88f9a0300               call       0x5429ee
00508f5f 8be5                     mov        esp, ebp
00508f61 5d                       pop        ebp
00508f62 c20400                   ret        4
00508f65 cc                       int3       
00508f66 cc                       int3       
00508f67 cc                       int3       
00508f68 cc                       int3       
00508f69 cc                       int3       
00508f6a cc                       int3       
00508f6b cc                       int3       
00508f6c cc                       int3       
00508f6d cc                       int3       
00508f6e cc                       int3       
00508f6f cc                       int3       
00508f70 53                       push       ebx
00508f71 8bdc                     mov        ebx, esp
00508f73 83ec08                   sub        esp, 8
00508f76 83e4f8                   and        esp, 0xfffffff8
00508f79 83c404                   add        esp, 4
00508f7c 55                       push       ebp
00508f7d 8b6b04                   mov        ebp, dword ptr [ebx + 4]
00508f80 896c2404                 mov        dword ptr [esp + 4], ebp
00508f84 8bec                     mov        ebp, esp
00508f86 81ec50020000             sub        esp, 0x250
00508f8c 56                       push       esi
00508f8d 57                       push       edi
00508f8e 894dfc                   mov        dword ptr [ebp - 4], ecx
00508f91 8b45fc                   mov        eax, dword ptr [ebp - 4]
00508f94 83780800                 cmp        dword ptr [eax + 8], 0
00508f98 740c                     je         0x508fa6
00508f9a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00508f9d 8b4908                   mov        ecx, dword ptr [ecx + 8]
00508fa0 e8db9df0ff               call       0x412d80
00508fa5 90                       nop        
00508fa6 8b55fc                   mov        edx, dword ptr [ebp - 4]
00508fa9 83ba5402000000           cmp        dword ptr [edx + 0x254], 0
00508fb0 740f                     je         0x508fc1
00508fb2 8b45fc                   mov        eax, dword ptr [ebp - 4]
00508fb5 8b8854020000             mov        ecx, dword ptr [eax + 0x254]
00508fbb e8c09df0ff               call       0x412d80
00508fc0 90                       nop        
00508fc1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00508fc4 83790c00                 cmp        dword ptr [ecx + 0xc], 0
00508fc8 740c                     je         0x508fd6
00508fca 8b55fc                   mov        edx, dword ptr [ebp - 4]
00508fcd 8b4a0c                   mov        ecx, dword ptr [edx + 0xc]
00508fd0 e8ab9df0ff               call       0x412d80
00508fd5 90                       nop        
00508fd6 b8c0020000               mov        eax, 0x2c0
00508fdb 6bc800                   imul       ecx, eax, 0
00508fde 81c1b0885b00             add        ecx, 0x5b88b0
00508fe4 e8e7010000               call       0x5091d0
00508fe9 90                       nop        
00508fea 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00508fed 83791000                 cmp        dword ptr [ecx + 0x10], 0
00508ff1 0f85af000000             jne        0x5090a6
00508ff7 8b55fc                   mov        edx, dword ptr [ebp - 4]
00508ffa 83c220                   add        edx, 0x20
00508ffd 8955ec                   mov        dword ptr [ebp - 0x14], edx
00509000 b968a55b00               mov        ecx, 0x5ba568
00509005 e856bdf6ff               call       0x474d60
0050900a 50                       push       eax
0050900b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0050900e e86db5f0ff               call       0x414580
00509013 8b00                     mov        eax, dword ptr [eax]
00509015 8945f4                   mov        dword ptr [ebp - 0xc], eax
00509018 b968a55b00               mov        ecx, 0x5ba568
0050901d e83ebdf6ff               call       0x474d60
00509022 50                       push       eax
00509023 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00509026 e8750f0000               call       0x509fa0
0050902b b968a55b00               mov        ecx, 0x5ba568
00509030 e82bbdf6ff               call       0x474d60
00509035 50                       push       eax
00509036 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00509039 e8a2120000               call       0x50a2e0
0050903e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00509041 8981e0000000             mov        dword ptr [ecx + 0xe0], eax
00509047 e8f4fef8ff               call       0x498f40
0050904c 8b7df4                   mov        edi, dword ptr [ebp - 0xc]
0050904f 83c770                   add        edi, 0x70
00509052 b98a000000               mov        ecx, 0x8a
00509057 8bf0                     mov        esi, eax
00509059 f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
0050905b 6a00                     push       0
0050905d e8ce77f5ff               call       0x460830
00509062 83c404                   add        esp, 4
00509065 8bc8                     mov        ecx, eax
00509067 e82466ffff               call       0x4ff690
0050906c 8b10                     mov        edx, dword ptr [eax]
0050906e 8b4004                   mov        eax, dword ptr [eax + 4]
00509071 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00509074 895110                   mov        dword ptr [ecx + 0x10], edx
00509077 894114                   mov        dword ptr [ecx + 0x14], eax
0050907a 6a00                     push       0
0050907c e8af77f5ff               call       0x460830
00509081 83c404                   add        esp, 4
00509084 8bc8                     mov        ecx, eax
00509086 e85565ffff               call       0x4ff5e0
0050908b 0fb6d0                   movzx      edx, al
0050908e 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00509091 895018                   mov        dword ptr [eax + 0x18], edx
00509094 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00509097 c7815002000000000000     mov        dword ptr [ecx + 0x250], 0
005090a1 e90a010000               jmp        0x5091b0
005090a6 8b55fc                   mov        edx, dword ptr [ebp - 4]
005090a9 837a1001                 cmp        dword ptr [edx + 0x10], 1
005090ad 0f85fd000000             jne        0x5091b0
005090b3 8b45fc                   mov        eax, dword ptr [ebp - 4]
005090b6 05e8000000               add        eax, 0xe8
005090bb 8945e8                   mov        dword ptr [ebp - 0x18], eax
005090be b968a55b00               mov        ecx, 0x5ba568
005090c3 e898bcf6ff               call       0x474d60
005090c8 50                       push       eax
005090c9 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
005090cc e85fddf7ff               call       0x486e30
005090d1 8b4810                   mov        ecx, dword ptr [eax + 0x10]
005090d4 894df0                   mov        dword ptr [ebp - 0x10], ecx
005090d7 b968a55b00               mov        ecx, 0x5ba568
005090dc e87fbcf6ff               call       0x474d60
005090e1 8b55fc                   mov        edx, dword ptr [ebp - 4]
005090e4 898258020000             mov        dword ptr [edx + 0x258], eax
005090ea 8b75f0                   mov        esi, dword ptr [ebp - 0x10]
005090ed 83c670                   add        esi, 0x70
005090f0 b98a000000               mov        ecx, 0x8a
005090f5 8dbdb0fdffff             lea        edi, [ebp - 0x250]
005090fb f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
005090fd e83efef8ff               call       0x498f40
00509102 b98a000000               mov        ecx, 0x8a
00509107 8db5b0fdffff             lea        esi, [ebp - 0x250]
0050910d 8bf8                     mov        edi, eax
0050910f f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
00509111 6a00                     push       0
00509113 e81877f5ff               call       0x460830
00509118 83c404                   add        esp, 4
0050911b 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0050911e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00509121 83c010                   add        eax, 0x10
00509124 50                       push       eax
00509125 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00509128 e803130000               call       0x50a430
0050912d 6a00                     push       0
0050912f e8fc76f5ff               call       0x460830
00509134 83c404                   add        esp, 4
00509137 8945e0                   mov        dword ptr [ebp - 0x20], eax
0050913a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0050913d 83791800                 cmp        dword ptr [ecx + 0x18], 0
00509141 7406                     je         0x509149
00509143 c645fb01                 mov        byte ptr [ebp - 5], 1
00509147 eb04                     jmp        0x50914d
00509149 c645fb00                 mov        byte ptr [ebp - 5], 0
0050914d 0fb655fb                 movzx      edx, byte ptr [ebp - 5]
00509151 52                       push       edx
00509152 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00509155 e8b6120000               call       0x50a410
0050915a 6a00                     push       0
0050915c e8cf76f5ff               call       0x460830
00509161 83c404                   add        esp, 4
00509164 8bc8                     mov        ecx, eax
00509166 e8d5110000               call       0x50a340
0050916b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050916e 05e8000000               add        eax, 0xe8
00509173 8945dc                   mov        dword ptr [ebp - 0x24], eax
00509176 b968a55b00               mov        ecx, 0x5ba568
0050917b e8e0bbf6ff               call       0x474d60
00509180 50                       push       eax
00509181 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00509184 e8a7dcf7ff               call       0x486e30
00509189 8bc8                     mov        ecx, eax
0050918b e8100c0000               call       0x509da0
00509190 6a00                     push       0
00509192 e89976f5ff               call       0x460830
00509197 83c404                   add        esp, 4
0050919a 8945d8                   mov        dword ptr [ebp - 0x28], eax
0050919d 6aff                     push       -1
0050919f 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
005091a2 e8f91affff               call       0x4faca0
005091a7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005091aa e811120000               call       0x50a3c0
005091af 90                       nop        
005091b0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005091b3 c7815002000000000000     mov        dword ptr [ecx + 0x250], 0
005091bd 5f                       pop        edi
005091be 5e                       pop        esi
005091bf 8be5                     mov        esp, ebp
005091c1 5d                       pop        ebp
005091c2 8be3                     mov        esp, ebx
005091c4 5b                       pop        ebx
005091c5 c3                       ret        
005091c6 cc                       int3       
005091c7 cc                       int3       
005091c8 cc                       int3       
005091c9 cc                       int3       
005091ca cc                       int3       
005091cb cc                       int3       
005091cc cc                       int3       
005091cd cc                       int3       
005091ce cc                       int3       
005091cf cc                       int3       
