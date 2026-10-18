00510e80 55                       push       ebp
00510e81 8bec                     mov        ebp, esp
00510e83 81ec70010000             sub        esp, 0x170
00510e89 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00510e8e 33c5                     xor        eax, ebp
00510e90 8945fc                   mov        dword ptr [ebp - 4], eax
00510e93 898de8feffff             mov        dword ptr [ebp - 0x118], ecx
00510e99 8b8de8feffff             mov        ecx, dword ptr [ebp - 0x118]
00510e9f 83c114                   add        ecx, 0x14
00510ea2 e89994f6ff               call       0x47a340
00510ea7 0fb6c0                   movzx      eax, al
00510eaa 85c0                     test       eax, eax
00510eac 0f8455030000             je         0x511207
00510eb2 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
00510eb8 e8b3060000               call       0x511570
00510ebd 0fb6c8                   movzx      ecx, al
00510ec0 85c9                     test       ecx, ecx
00510ec2 0f853f030000             jne        0x511207
00510ec8 51                       push       ecx
00510ec9 0f57c0                   xorps      xmm0, xmm0
00510ecc f30f110424               movss      dword ptr [esp], xmm0
00510ed1 51                       push       ecx
00510ed2 f30f100598ec5600         movss      xmm0, dword ptr [0x56ec98]
00510eda f30f110424               movss      dword ptr [esp], xmm0
00510edf 51                       push       ecx
00510ee0 f30f100514ed5600         movss      xmm0, dword ptr [0x56ed14]
00510ee8 f30f110424               movss      dword ptr [esp], xmm0
00510eed 8d8df0feffff             lea        ecx, [ebp - 0x110]
00510ef3 e8d81ef1ff               call       0x422dd0
00510ef8 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00510efe e8dd58fdff               call       0x4e67e0
00510f03 8b8de8feffff             mov        ecx, dword ptr [ebp - 0x118]
00510f09 83c114                   add        ecx, 0x14
00510f0c e8bfbff3ff               call       0x44ced0
00510f11 8bc8                     mov        ecx, eax
00510f13 e8c8c1f4ff               call       0x45d0e0
00510f18 8a5003                   mov        dl, byte ptr [eax + 3]
00510f1b 8895effeffff             mov        byte ptr [ebp - 0x111], dl
00510f21 0fb685effeffff           movzx      eax, byte ptr [ebp - 0x111]
00510f28 50                       push       eax
00510f29 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00510f2f e80c7af7ff               call       0x488940
00510f34 6a04                     push       4
00510f36 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00510f3c e83f52fbff               call       0x4c6180
00510f41 6a00                     push       0
00510f43 6a00                     push       0
00510f45 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00510f4b e8d076faff               call       0x4b8620
00510f50 90                       nop        
00510f51 51                       push       ecx
00510f52 0f57c0                   xorps      xmm0, xmm0
00510f55 f30f110424               movss      dword ptr [esp], xmm0
00510f5a 51                       push       ecx
00510f5b f30f100598ec5600         movss      xmm0, dword ptr [0x56ec98]
00510f63 f30f110424               movss      dword ptr [esp], xmm0
00510f68 51                       push       ecx
00510f69 f30f100514ed5600         movss      xmm0, dword ptr [0x56ed14]
00510f71 f30f110424               movss      dword ptr [esp], xmm0
00510f76 8d8dccfeffff             lea        ecx, [ebp - 0x134]
00510f7c e84f1ef1ff               call       0x422dd0
00510f81 83ec08                   sub        esp, 8
00510f84 8bcc                     mov        ecx, esp
00510f86 68503f5700               push       0x573f50
00510f8b e840ceefff               call       0x40ddd0
00510f90 8d8dccfeffff             lea        ecx, [ebp - 0x134]
00510f96 51                       push       ecx
00510f97 8b1598065c00             mov        edx, dword ptr [0x5c0698]
00510f9d 52                       push       edx
00510f9e e86db2f5ff               call       0x46c210
00510fa3 83c410                   add        esp, 0x10
00510fa6 6a00                     push       0
00510fa8 e8d330f5ff               call       0x464080
00510fad 83c404                   add        esp, 4
00510fb0 8bc8                     mov        ecx, eax
00510fb2 e809040000               call       0x5113c0
00510fb7 8985e4feffff             mov        dword ptr [ebp - 0x11c], eax
00510fbd 51                       push       ecx
00510fbe 0f57c0                   xorps      xmm0, xmm0
00510fc1 f30f110424               movss      dword ptr [esp], xmm0
00510fc6 51                       push       ecx
00510fc7 f30f1005dc3f5700         movss      xmm0, dword ptr [0x573fdc]
00510fcf f30f110424               movss      dword ptr [esp], xmm0
00510fd4 51                       push       ecx
00510fd5 f30f100514ed5600         movss      xmm0, dword ptr [0x56ed14]
00510fdd f30f110424               movss      dword ptr [esp], xmm0
00510fe2 8d8dc0feffff             lea        ecx, [ebp - 0x140]
00510fe8 e8e31df1ff               call       0x422dd0
00510fed 8b85e4feffff             mov        eax, dword ptr [ebp - 0x11c]
00510ff3 50                       push       eax
00510ff4 83ec08                   sub        esp, 8
00510ff7 8bcc                     mov        ecx, esp
00510ff9 68643f5700               push       0x573f64
00510ffe e8cdcdefff               call       0x40ddd0
00511003 8d8dc0feffff             lea        ecx, [ebp - 0x140]
00511009 51                       push       ecx
0051100a 8b1598065c00             mov        edx, dword ptr [0x5c0698]
00511010 52                       push       edx
00511011 e8fab1f5ff               call       0x46c210
00511016 83c414                   add        esp, 0x14
00511019 6a00                     push       0
0051101b e86030f5ff               call       0x464080
00511020 83c404                   add        esp, 4
00511023 8bc8                     mov        ecx, eax
00511025 e8f6020000               call       0x511320
0051102a 8985e0feffff             mov        dword ptr [ebp - 0x120], eax
00511030 51                       push       ecx
00511031 0f57c0                   xorps      xmm0, xmm0
00511034 f30f110424               movss      dword ptr [esp], xmm0
00511039 51                       push       ecx
0051103a f30f1005e03f5700         movss      xmm0, dword ptr [0x573fe0]
00511042 f30f110424               movss      dword ptr [esp], xmm0
00511047 51                       push       ecx
00511048 f30f100514ed5600         movss      xmm0, dword ptr [0x56ed14]
00511050 f30f110424               movss      dword ptr [esp], xmm0
00511055 8d8db4feffff             lea        ecx, [ebp - 0x14c]
0051105b e8701df1ff               call       0x422dd0
00511060 8b85e0feffff             mov        eax, dword ptr [ebp - 0x120]
00511066 50                       push       eax
00511067 83ec08                   sub        esp, 8
0051106a 8bcc                     mov        ecx, esp
0051106c 687c3f5700               push       0x573f7c
00511071 e85acdefff               call       0x40ddd0
00511076 8d8db4feffff             lea        ecx, [ebp - 0x14c]
0051107c 51                       push       ecx
0051107d 8b1598065c00             mov        edx, dword ptr [0x5c0698]
00511083 52                       push       edx
00511084 e887b1f5ff               call       0x46c210
00511089 83c414                   add        esp, 0x14
0051108c 6a00                     push       0
0051108e e8ed2ff5ff               call       0x464080
00511093 83c404                   add        esp, 4
00511096 8bc8                     mov        ecx, eax
00511098 e873030000               call       0x511410
0051109d 8985dcfeffff             mov        dword ptr [ebp - 0x124], eax
005110a3 51                       push       ecx
005110a4 0f57c0                   xorps      xmm0, xmm0
005110a7 f30f110424               movss      dword ptr [esp], xmm0
005110ac 51                       push       ecx
005110ad f30f1005e43f5700         movss      xmm0, dword ptr [0x573fe4]
005110b5 f30f110424               movss      dword ptr [esp], xmm0
005110ba 51                       push       ecx
005110bb f30f100514ed5600         movss      xmm0, dword ptr [0x56ed14]
005110c3 f30f110424               movss      dword ptr [esp], xmm0
005110c8 8d8da8feffff             lea        ecx, [ebp - 0x158]
005110ce e8fd1cf1ff               call       0x422dd0
005110d3 8b85dcfeffff             mov        eax, dword ptr [ebp - 0x124]
005110d9 50                       push       eax
005110da 83ec08                   sub        esp, 8
005110dd 8bcc                     mov        ecx, esp
005110df 68943f5700               push       0x573f94
005110e4 e8e7ccefff               call       0x40ddd0
005110e9 8d8da8feffff             lea        ecx, [ebp - 0x158]
005110ef 51                       push       ecx
005110f0 8b1598065c00             mov        edx, dword ptr [0x5c0698]
005110f6 52                       push       edx
005110f7 e814b1f5ff               call       0x46c210
005110fc 83c414                   add        esp, 0x14
005110ff 6a00                     push       0
00511101 e87a2ff5ff               call       0x464080
00511106 83c404                   add        esp, 4
00511109 8bc8                     mov        ecx, eax
0051110b e860020000               call       0x511370
00511110 8985d8feffff             mov        dword ptr [ebp - 0x128], eax
00511116 51                       push       ecx
00511117 0f57c0                   xorps      xmm0, xmm0
0051111a f30f110424               movss      dword ptr [esp], xmm0
0051111f 51                       push       ecx
00511120 f30f1005e83f5700         movss      xmm0, dword ptr [0x573fe8]
00511128 f30f110424               movss      dword ptr [esp], xmm0
0051112d 51                       push       ecx
0051112e f30f100514ed5600         movss      xmm0, dword ptr [0x56ed14]
00511136 f30f110424               movss      dword ptr [esp], xmm0
0051113b 8d8d9cfeffff             lea        ecx, [ebp - 0x164]
00511141 e88a1cf1ff               call       0x422dd0
00511146 8b85d8feffff             mov        eax, dword ptr [ebp - 0x128]
0051114c 50                       push       eax
0051114d 83ec08                   sub        esp, 8
00511150 8bcc                     mov        ecx, esp
00511152 68ac3f5700               push       0x573fac
00511157 e874ccefff               call       0x40ddd0
0051115c 8d8d9cfeffff             lea        ecx, [ebp - 0x164]
00511162 51                       push       ecx
00511163 8b1598065c00             mov        edx, dword ptr [0x5c0698]
00511169 52                       push       edx
0051116a e8a1b0f5ff               call       0x46c210
0051116f 83c414                   add        esp, 0x14
00511172 8b85e8feffff             mov        eax, dword ptr [ebp - 0x118]
00511178 8b4028                   mov        eax, dword ptr [eax + 0x28]
0051117b 99                       cdq        
0051117c 52                       push       edx
0051117d 50                       push       eax
0051117e 6800010000               push       0x100
00511183 8d8dfcfeffff             lea        ecx, [ebp - 0x104]
00511189 51                       push       ecx
0051118a e87123f4ff               call       0x453500
0051118f 83c410                   add        esp, 0x10
00511192 51                       push       ecx
00511193 0f57c0                   xorps      xmm0, xmm0
00511196 f30f110424               movss      dword ptr [esp], xmm0
0051119b 51                       push       ecx
0051119c f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
005111a4 f30f110424               movss      dword ptr [esp], xmm0
005111a9 51                       push       ecx
005111aa f30f100514ed5600         movss      xmm0, dword ptr [0x56ed14]
005111b2 f30f110424               movss      dword ptr [esp], xmm0
005111b7 8d8d90feffff             lea        ecx, [ebp - 0x170]
005111bd e80e1cf1ff               call       0x422dd0
005111c2 8d95fcfeffff             lea        edx, [ebp - 0x104]
005111c8 52                       push       edx
005111c9 83ec08                   sub        esp, 8
005111cc 8bcc                     mov        ecx, esp
005111ce 68c43f5700               push       0x573fc4
005111d3 e8f8cbefff               call       0x40ddd0
005111d8 8d8590feffff             lea        eax, [ebp - 0x170]
005111de 50                       push       eax
005111df 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005111e5 51                       push       ecx
005111e6 e825b0f5ff               call       0x46c210
005111eb 83c414                   add        esp, 0x14
005111ee 6a04                     push       4
005111f0 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005111f6 e80579f7ff               call       0x488b00
005111fb 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00511201 e8da55fdff               call       0x4e67e0
00511206 90                       nop        
00511207 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
0051120e 7507                     jne        0x511217
00511210 b801000000               mov        eax, 1
00511215 eb49                     jmp        0x511260
00511217 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
0051121d e84e030000               call       0x511570
00511222 0fb6d0                   movzx      edx, al
00511225 85d2                     test       edx, edx
00511227 750c                     jne        0x511235
00511229 8b85e8feffff             mov        eax, dword ptr [ebp - 0x118]
0051122f 83782404                 cmp        dword ptr [eax + 0x24], 4
00511233 7417                     je         0x51124c
00511235 8b8de8feffff             mov        ecx, dword ptr [ebp - 0x118]
0051123b 83c114                   add        ecx, 0x14
0051123e e8ddeef3ff               call       0x450120
00511243 b801000000               mov        eax, 1
00511248 eb16                     jmp        0x511260
0051124a eb0f                     jmp        0x51125b
0051124c 8b8de8feffff             mov        ecx, dword ptr [ebp - 0x118]
00511252 83c114                   add        ecx, 0x14
00511255 e8d6e7f3ff               call       0x44fa30
0051125a 90                       nop        
0051125b b801000000               mov        eax, 1
00511260 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511263 33cd                     xor        ecx, ebp
00511265 e884170300               call       0x5429ee
0051126a 8be5                     mov        esp, ebp
0051126c 5d                       pop        ebp
0051126d c3                       ret        
0051126e cc                       int3       
0051126f cc                       int3       
