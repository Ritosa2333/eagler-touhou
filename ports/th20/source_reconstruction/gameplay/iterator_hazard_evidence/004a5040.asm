004a5040 55                       push       ebp
004a5041 8bec                     mov        ebp, esp
004a5043 6aff                     push       -1
004a5045 684d975600               push       0x56974d
004a504a 64a100000000             mov        eax, dword ptr fs:[0]
004a5050 50                       push       eax
004a5051 83ec58                   sub        esp, 0x58
004a5054 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004a5059 33c5                     xor        eax, ebp
004a505b 8945f0                   mov        dword ptr [ebp - 0x10], eax
004a505e 50                       push       eax
004a505f 8d45f4                   lea        eax, [ebp - 0xc]
004a5062 64a300000000             mov        dword ptr fs:[0], eax
004a5068 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004a506b 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004a506e 8b882c010000             mov        ecx, dword ptr [eax + 0x12c]
004a5074 51                       push       ecx
004a5075 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004a507b e82026fdff               call       0x4776a0
004a5080 c745d400000000           mov        dword ptr [ebp - 0x2c], 0
004a5087 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004a508a c782ac00000000000000     mov        dword ptr [edx + 0xac], 0
004a5094 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004a5097 c780b000000000000000     mov        dword ptr [eax + 0xb0], 0
004a50a1 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a50a4 83b9cc00000000           cmp        dword ptr [ecx + 0xcc], 0
004a50ab 0f8ec0000000             jle        0x4a5171
004a50b1 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004a50b4 81c2d0000000             add        edx, 0xd0
004a50ba 8955d0                   mov        dword ptr [ebp - 0x30], edx
004a50bd 6a00                     push       0
004a50bf 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004a50c2 e8a9bffbff               call       0x461070
004a50c7 0fb6c0                   movzx      eax, al
004a50ca 85c0                     test       eax, eax
004a50cc 7452                     je         0x4a5120
004a50ce 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a50d1 81c1d0000000             add        ecx, 0xd0
004a50d7 894dcc                   mov        dword ptr [ebp - 0x34], ecx
004a50da 6a00                     push       0
004a50dc 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004a50df e83c43f8ff               call       0x429420
004a50e4 90                       nop        
004a50e5 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004a50e8 81c2d0000000             add        edx, 0xd0
004a50ee 8955c8                   mov        dword ptr [ebp - 0x38], edx
004a50f1 6a00                     push       0
004a50f3 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004a50f6 e895e4f7ff               call       0x423590
004a50fb 0fb6c0                   movzx      eax, al
004a50fe 85c0                     test       eax, eax
004a5100 741e                     je         0x4a5120
004a5102 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a5105 c781cc00000000000000     mov        dword ptr [ecx + 0xcc], 0
004a510f 6a00                     push       0
004a5111 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a5114 81c1d0000000             add        ecx, 0xd0
004a511a e801e4f7ff               call       0x423520
004a511f 90                       nop        
004a5120 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004a5123 81c2d0000000             add        edx, 0xd0
004a5129 8955c4                   mov        dword ptr [ebp - 0x3c], edx
004a512c 6a3c                     push       0x3c
004a512e 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004a5131 e80acffcff               call       0x472040
004a5136 6bc064                   imul       eax, eax, 0x64
004a5139 99                       cdq        
004a513a b93c000000               mov        ecx, 0x3c
004a513f f7f9                     idiv       ecx
004a5141 8945bc                   mov        dword ptr [ebp - 0x44], eax
004a5144 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004a5147 81c2d0000000             add        edx, 0xd0
004a514d 8955c0                   mov        dword ptr [ebp - 0x40], edx
004a5150 6a3c                     push       0x3c
004a5152 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004a5155 e8a6edffff               call       0x4a3f00
004a515a 8945b8                   mov        dword ptr [ebp - 0x48], eax
004a515d 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004a5160 50                       push       eax
004a5161 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004a5164 51                       push       ecx
004a5165 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004a516b e840640000               call       0x4ab5b0
004a5170 90                       nop        
004a5171 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004a5174 81c208010000             add        edx, 0x108
004a517a 8955dc                   mov        dword ptr [ebp - 0x24], edx
004a517d 6a08                     push       8
004a517f 8d4de8                   lea        ecx, [ebp - 0x18]
004a5182 e8f96ef6ff               call       0x40c080
004a5187 8d45e8                   lea        eax, [ebp - 0x18]
004a518a 50                       push       eax
004a518b 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004a518e e8edd0f6ff               call       0x412280
004a5193 c745fc00000000           mov        dword ptr [ebp - 4], 0
004a519a 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004a519d e89ed3f6ff               call       0x412540
004a51a2 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004a51a5 eb09                     jmp        0x4a51b0
004a51a7 8d4de8                   lea        ecx, [ebp - 0x18]
004a51aa e881caf6ff               call       0x411c30
004a51af 90                       nop        
004a51b0 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004a51b3 51                       push       ecx
004a51b4 8d4de8                   lea        ecx, [ebp - 0x18]
004a51b7 e8f4c9f6ff               call       0x411bb0
004a51bc 0fb6d0                   movzx      edx, al
004a51bf 85d2                     test       edx, edx
004a51c1 0f8480000000             je         0x4a5247
004a51c7 8d4de8                   lea        ecx, [ebp - 0x18]
004a51ca e83171f6ff               call       0x40c300
004a51cf 8945e0                   mov        dword ptr [ebp - 0x20], eax
004a51d2 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004a51d5 e8869ff6ff               call       0x40f160
004a51da 8b00                     mov        eax, dword ptr [eax]
004a51dc 8b8854030000             mov        ecx, dword ptr [eax + 0x354]
004a51e2 c1e909                   shr        ecx, 9
004a51e5 83e101                   and        ecx, 1
004a51e8 7513                     jne        0x4a51fd
004a51ea 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004a51ed e86e9ff6ff               call       0x40f160
004a51f2 8b08                     mov        ecx, dword ptr [eax]
004a51f4 e867350000               call       0x4a8760
004a51f9 85c0                     test       eax, eax
004a51fb 741d                     je         0x4a521a
004a51fd 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004a5200 e85b9ff6ff               call       0x40f160
004a5205 8945b0                   mov        dword ptr [ebp - 0x50], eax
004a5208 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
004a520b 52                       push       edx
004a520c 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004a5212 e809d5ffff               call       0x4a2720
004a5217 90                       nop        
004a5218 eb1f                     jmp        0x4a5239
004a521a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004a521d e83e9ff6ff               call       0x40f160
004a5222 8b00                     mov        eax, dword ptr [eax]
004a5224 0554030000               add        eax, 0x354
004a5229 8945d8                   mov        dword ptr [ebp - 0x28], eax
004a522c 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004a522f 8b11                     mov        edx, dword ptr [ecx]
004a5231 83e2fb                   and        edx, 0xfffffffb
004a5234 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004a5237 8910                     mov        dword ptr [eax], edx
004a5239 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004a523c 83c101                   add        ecx, 1
004a523f 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
004a5242 e960ffffff               jmp        0x4a51a7
004a5247 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004a524e 8d4de8                   lea        ecx, [ebp - 0x18]
004a5251 e8aac8f6ff               call       0x411b00
004a5256 90                       nop        
004a5257 e874eb0600               call       0x513dd0
004a525c 8bc8                     mov        ecx, eax
004a525e e86dce0600               call       0x5120d0
004a5263 90                       nop        
004a5264 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004a5267 8b8a30010000             mov        ecx, dword ptr [edx + 0x130]
004a526d e81eadf6ff               call       0x40ff90
004a5272 8bc8                     mov        ecx, eax
004a5274 e8c7570000               call       0x4aaa40
004a5279 d95dac                   fstp       dword ptr [ebp - 0x54]
004a527c f30f1045ac               movss      xmm0, dword ptr [ebp - 0x54]
004a5281 0f2f0530e75600           comiss     xmm0, dword ptr [0x56e730]
004a5288 761e                     jbe        0x4a52a8
004a528a 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004a528d 8b8830010000             mov        ecx, dword ptr [eax + 0x130]
004a5293 e8f8acf6ff               call       0x40ff90
004a5298 8945a8                   mov        dword ptr [ebp - 0x58], eax
004a529b 6a01                     push       1
004a529d 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004a52a0 e8ab650000               call       0x4ab850
004a52a5 90                       nop        
004a52a6 eb1c                     jmp        0x4a52c4
004a52a8 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a52ab 8b8930010000             mov        ecx, dword ptr [ecx + 0x130]
004a52b1 e8daacf6ff               call       0x40ff90
004a52b6 8945a4                   mov        dword ptr [ebp - 0x5c], eax
004a52b9 6a00                     push       0
004a52bb 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
004a52be e88d650000               call       0x4ab850
004a52c3 90                       nop        
004a52c4 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004a52c7 8b8a30010000             mov        ecx, dword ptr [edx + 0x130]
004a52cd e8beacf6ff               call       0x40ff90
004a52d2 8945a0                   mov        dword ptr [ebp - 0x60], eax
004a52d5 51                       push       ecx
004a52d6 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004a52de f30f110424               movss      dword ptr [esp], xmm0
004a52e3 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
004a52e6 e8a5620000               call       0x4ab590
004a52eb 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004a52ee 059c000000               add        eax, 0x9c
004a52f3 89459c                   mov        dword ptr [ebp - 0x64], eax
004a52f6 6a00                     push       0
004a52f8 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
004a52fb e840e2f7ff               call       0x423540
004a5300 b801000000               mov        eax, 1
004a5305 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004a5308 64890d00000000           mov        dword ptr fs:[0], ecx
004a530f 59                       pop        ecx
004a5310 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004a5313 33cd                     xor        ecx, ebp
004a5315 e8d4d60900               call       0x5429ee
004a531a 8be5                     mov        esp, ebp
004a531c 5d                       pop        ebp
004a531d c3                       ret        
004a531e cc                       int3       
004a531f cc                       int3       
