00512aa0 55                       push       ebp
00512aa1 8bec                     mov        ebp, esp
00512aa3 83ec64                   sub        esp, 0x64
00512aa6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00512aab 33c5                     xor        eax, ebp
00512aad 8945fc                   mov        dword ptr [ebp - 4], eax
00512ab0 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00512ab3 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00512ab6 e8852ff7ff               call       0x485a40
00512abb 0fb6c0                   movzx      eax, al
00512abe 85c0                     test       eax, eax
00512ac0 0f8458040000             je         0x512f1e
00512ac6 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00512ac9 e8222ff7ff               call       0x4859f0
00512ace 85c0                     test       eax, eax
00512ad0 0f8448040000             je         0x512f1e
00512ad6 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00512ad9 83c118                   add        ecx, 0x18
00512adc 894dc8                   mov        dword ptr [ebp - 0x38], ecx
00512adf 68c8000000               push       0xc8
00512ae4 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00512ae7 e8740af1ff               call       0x423560
00512aec 0fb6d0                   movzx      edx, al
00512aef 85d2                     test       edx, edx
00512af1 0f8427040000             je         0x512f1e
00512af7 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00512afa e8f12ef7ff               call       0x4859f0
00512aff 8bc8                     mov        ecx, eax
00512b01 e8fa97efff               call       0x40c300
00512b06 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00512b09 c7459c4040ffff           mov        dword ptr [ebp - 0x64], 0xffff4040
00512b10 c745a0ff4040ff           mov        dword ptr [ebp - 0x60], 0xff4040ff
00512b17 c745a400ffffff           mov        dword ptr [ebp - 0x5c], 0xffffff00
00512b1e c745a840ff40ff           mov        dword ptr [ebp - 0x58], 0xff40ff40
00512b25 6a00                     push       0
00512b27 6a00                     push       0
00512b29 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512b2f e8ec5afaff               call       0x4b8620
00512b34 6a0c                     push       0xc
00512b36 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512b3c e8bf5ff7ff               call       0x488b00
00512b41 90                       nop        
00512b42 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
00512b45 83c018                   add        eax, 0x18
00512b48 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00512b4b 6a08                     push       8
00512b4d 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00512b50 e8ebf4f5ff               call       0x472040
00512b55 83f805                   cmp        eax, 5
00512b58 7f19                     jg         0x512b73
00512b5a 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00512b5d 8b512c                   mov        edx, dword ptr [ecx + 0x2c]
00512b60 8b44959c                 mov        eax, dword ptr [ebp + edx*4 - 0x64]
00512b64 50                       push       eax
00512b65 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512b6b e860dff5ff               call       0x470ad0
00512b70 90                       nop        
00512b71 eb0e                     jmp        0x512b81
00512b73 6aff                     push       -1
00512b75 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512b7b e850dff5ff               call       0x470ad0
00512b80 90                       nop        
00512b81 51                       push       ecx
00512b82 0f57c0                   xorps      xmm0, xmm0
00512b85 f30f110424               movss      dword ptr [esp], xmm0
00512b8a 51                       push       ecx
00512b8b f30f100580fb5600         movss      xmm0, dword ptr [0x56fb80]
00512b93 f30f110424               movss      dword ptr [esp], xmm0
00512b98 51                       push       ecx
00512b99 f30f100514ed5600         movss      xmm0, dword ptr [0x56ed14]
00512ba1 f30f110424               movss      dword ptr [esp], xmm0
00512ba6 8d4df0                   lea        ecx, [ebp - 0x10]
00512ba9 e82202f1ff               call       0x422dd0
00512bae 6a00                     push       0
00512bb0 e8ab54f6ff               call       0x478060
00512bb5 83c404                   add        esp, 4
00512bb8 8945bc                   mov        dword ptr [ebp - 0x44], eax
00512bbb 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00512bbe e8dde1f8ff               call       0x4a0da0
00512bc3 8bc8                     mov        ecx, eax
00512bc5 e83697efff               call       0x40c300
00512bca 8945c0                   mov        dword ptr [ebp - 0x40], eax
00512bcd 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00512bd0 51                       push       ecx
00512bd1 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00512bd4 e8a75ef8ff               call       0x498a80
00512bd9 8bc8                     mov        ecx, eax
00512bdb e8e076f6ff               call       0x47a2c0
00512be0 8b10                     mov        edx, dword ptr [eax]
00512be2 8955e4                   mov        dword ptr [ebp - 0x1c], edx
00512be5 8b4804                   mov        ecx, dword ptr [eax + 4]
00512be8 894de8                   mov        dword ptr [ebp - 0x18], ecx
00512beb 8b5008                   mov        edx, dword ptr [eax + 8]
00512bee 8955ec                   mov        dword ptr [ebp - 0x14], edx
00512bf1 f30f1005d8425700         movss      xmm0, dword ptr [0x5742d8]
00512bf9 0f2f45e4                 comiss     xmm0, dword ptr [ebp - 0x1c]
00512bfd 760f                     jbe        0x512c0e
00512bff f30f1005d8425700         movss      xmm0, dword ptr [0x5742d8]
00512c07 f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
00512c0c eb31                     jmp        0x512c3f
00512c0e f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00512c13 0f2f0590f05600           comiss     xmm0, dword ptr [0x56f090]
00512c1a 760f                     jbe        0x512c2b
00512c1c f30f100590f05600         movss      xmm0, dword ptr [0x56f090]
00512c24 f30f1145d0               movss      dword ptr [ebp - 0x30], xmm0
00512c29 eb0a                     jmp        0x512c35
00512c2b f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00512c30 f30f1145d0               movss      dword ptr [ebp - 0x30], xmm0
00512c35 f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
00512c3a f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
00512c3f f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
00512c44 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00512c49 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00512c4e f30f580514ed5600         addss      xmm0, dword ptr [0x56ed14]
00512c56 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00512c5b 83ec08                   sub        esp, 8
00512c5e 8bcc                     mov        ecx, esp
00512c60 6888425700               push       0x574288
00512c65 e866b1efff               call       0x40ddd0
00512c6a 8d45f0                   lea        eax, [ebp - 0x10]
00512c6d 50                       push       eax
00512c6e 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512c74 e817a2f5ff               call       0x46ce90
00512c79 90                       nop        
00512c7a f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00512c7f f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
00512c87 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00512c8c 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00512c8f 8b5130                   mov        edx, dword ptr [ecx + 0x30]
00512c92 83c201                   add        edx, 1
00512c95 8955b8                   mov        dword ptr [ebp - 0x48], edx
00512c98 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
00512c9b 50                       push       eax
00512c9c 83ec08                   sub        esp, 8
00512c9f 8bcc                     mov        ecx, esp
00512ca1 68a0425700               push       0x5742a0
00512ca6 e825b1efff               call       0x40ddd0
00512cab 8d4df0                   lea        ecx, [ebp - 0x10]
00512cae 51                       push       ecx
00512caf 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512cb5 e8e6a2f5ff               call       0x46cfa0
00512cba 90                       nop        
00512cbb f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00512cc0 f30f5c0590cd5600         subss      xmm0, dword ptr [0x56cd90]
00512cc8 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00512ccd 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00512cd0 83c138                   add        ecx, 0x38
00512cd3 e818f0ffff               call       0x511cf0
00512cd8 d95dd8                   fstp       dword ptr [ebp - 0x28]
00512cdb 6a01                     push       1
00512cdd 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512ce3 e8383cfdff               call       0x4e6920
00512ce8 f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
00512ced f30f5805ccc85600         addss      xmm0, dword ptr [0x56c8cc]
00512cf5 51                       push       ecx
00512cf6 f30f110424               movss      dword ptr [esp], xmm0
00512cfb f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
00512d00 f30f5805ccc85600         addss      xmm0, dword ptr [0x56c8cc]
00512d08 51                       push       ecx
00512d09 f30f110424               movss      dword ptr [esp], xmm0
00512d0e 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512d14 e87760faff               call       0x4b8d90
00512d19 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00512d1c 83c164                   add        ecx, 0x64
00512d1f e8ccedffff               call       0x511af0
00512d24 8845e3                   mov        byte ptr [ebp - 0x1d], al
00512d27 0fb655e3                 movzx      edx, byte ptr [ebp - 0x1d]
00512d2b 52                       push       edx
00512d2c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512d32 e8095cf7ff               call       0x488940
00512d37 90                       nop        
00512d38 83ec08                   sub        esp, 8
00512d3b 8bcc                     mov        ecx, esp
00512d3d 6888425700               push       0x574288
00512d42 e889b0efff               call       0x40ddd0
00512d47 8d45f0                   lea        eax, [ebp - 0x10]
00512d4a 50                       push       eax
00512d4b 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512d51 e83aa1f5ff               call       0x46ce90
00512d56 90                       nop        
00512d57 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00512d5c f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
00512d64 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00512d69 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00512d6c 8b5130                   mov        edx, dword ptr [ecx + 0x30]
00512d6f 83c201                   add        edx, 1
00512d72 8955b4                   mov        dword ptr [ebp - 0x4c], edx
00512d75 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
00512d78 50                       push       eax
00512d79 83ec08                   sub        esp, 8
00512d7c 8bcc                     mov        ecx, esp
00512d7e 68a0425700               push       0x5742a0
00512d83 e848b0efff               call       0x40ddd0
00512d88 8d4df0                   lea        ecx, [ebp - 0x10]
00512d8b 51                       push       ecx
00512d8c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512d92 e809a2f5ff               call       0x46cfa0
00512d97 90                       nop        
00512d98 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00512d9d f30f5c0590cd5600         subss      xmm0, dword ptr [0x56cd90]
00512da5 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00512daa f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
00512daf f30f5905d0425700         mulss      xmm0, dword ptr [0x5742d0]
00512db7 f30f5805ccc85600         addss      xmm0, dword ptr [0x56c8cc]
00512dbf 51                       push       ecx
00512dc0 f30f110424               movss      dword ptr [esp], xmm0
00512dc5 f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
00512dca f30f5905d0425700         mulss      xmm0, dword ptr [0x5742d0]
00512dd2 f30f5805ccc85600         addss      xmm0, dword ptr [0x56c8cc]
00512dda 51                       push       ecx
00512ddb f30f110424               movss      dword ptr [esp], xmm0
00512de0 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512de6 e8a55ffaff               call       0x4b8d90
00512deb 90                       nop        
00512dec 83ec08                   sub        esp, 8
00512def 8bcc                     mov        ecx, esp
00512df1 6888425700               push       0x574288
00512df6 e8d5afefff               call       0x40ddd0
00512dfb 8d55f0                   lea        edx, [ebp - 0x10]
00512dfe 52                       push       edx
00512dff 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512e05 e886a0f5ff               call       0x46ce90
00512e0a 90                       nop        
00512e0b f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00512e10 f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
00512e18 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00512e1d 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00512e20 8b4830                   mov        ecx, dword ptr [eax + 0x30]
00512e23 83c101                   add        ecx, 1
00512e26 894db0                   mov        dword ptr [ebp - 0x50], ecx
00512e29 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
00512e2c 52                       push       edx
00512e2d 83ec08                   sub        esp, 8
00512e30 8bcc                     mov        ecx, esp
00512e32 68a0425700               push       0x5742a0
00512e37 e894afefff               call       0x40ddd0
00512e3c 8d45f0                   lea        eax, [ebp - 0x10]
00512e3f 50                       push       eax
00512e40 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512e46 e855a1f5ff               call       0x46cfa0
00512e4b 90                       nop        
00512e4c f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00512e51 f30f5c0590cd5600         subss      xmm0, dword ptr [0x56cd90]
00512e59 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00512e5e f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
00512e63 f30f59055c055700         mulss      xmm0, dword ptr [0x57055c]
00512e6b f30f5805ccc85600         addss      xmm0, dword ptr [0x56c8cc]
00512e73 51                       push       ecx
00512e74 f30f110424               movss      dword ptr [esp], xmm0
00512e79 f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
00512e7e f30f59055c055700         mulss      xmm0, dword ptr [0x57055c]
00512e86 f30f5805ccc85600         addss      xmm0, dword ptr [0x56c8cc]
00512e8e 51                       push       ecx
00512e8f f30f110424               movss      dword ptr [esp], xmm0
00512e94 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512e9a e8f15efaff               call       0x4b8d90
00512e9f 90                       nop        
00512ea0 83ec08                   sub        esp, 8
00512ea3 8bcc                     mov        ecx, esp
00512ea5 6888425700               push       0x574288
00512eaa e821afefff               call       0x40ddd0
00512eaf 8d4df0                   lea        ecx, [ebp - 0x10]
00512eb2 51                       push       ecx
00512eb3 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512eb9 e8d29ff5ff               call       0x46ce90
00512ebe 90                       nop        
00512ebf f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00512ec4 f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
00512ecc f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00512ed1 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00512ed4 8b4230                   mov        eax, dword ptr [edx + 0x30]
00512ed7 83c001                   add        eax, 1
00512eda 8945ac                   mov        dword ptr [ebp - 0x54], eax
00512edd 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00512ee0 51                       push       ecx
00512ee1 83ec08                   sub        esp, 8
00512ee4 8bcc                     mov        ecx, esp
00512ee6 68a0425700               push       0x5742a0
00512eeb e8e0aeefff               call       0x40ddd0
00512ef0 8d55f0                   lea        edx, [ebp - 0x10]
00512ef3 52                       push       edx
00512ef4 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512efa e8a1a0f5ff               call       0x46cfa0
00512eff 90                       nop        
00512f00 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00512f05 f30f5c0590cd5600         subss      xmm0, dword ptr [0x56cd90]
00512f0d f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00512f12 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00512f18 e8c338fdff               call       0x4e67e0
00512f1d 90                       nop        
00512f1e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00512f21 33cd                     xor        ecx, ebp
00512f23 e8c6fa0200               call       0x5429ee
00512f28 8be5                     mov        esp, ebp
00512f2a 5d                       pop        ebp
00512f2b c3                       ret        
00512f2c cc                       int3       
00512f2d cc                       int3       
00512f2e cc                       int3       
00512f2f cc                       int3       
