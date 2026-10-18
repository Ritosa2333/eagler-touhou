00521060 55                       push       ebp
00521061 8bec                     mov        ebp, esp
00521063 81ec3c010000             sub        esp, 0x13c
00521069 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052106e 33c5                     xor        eax, ebp
00521070 8945fc                   mov        dword ptr [ebp - 4], eax
00521073 894de0                   mov        dword ptr [ebp - 0x20], ecx
00521076 b968a55b00               mov        ecx, 0x5ba568
0052107b e82030f4ff               call       0x4640a0
00521080 83f804                   cmp        eax, 4
00521083 7509                     jne        0x52108e
00521085 c745d80e000000           mov        dword ptr [ebp - 0x28], 0xe
0052108c eb07                     jmp        0x521095
0052108e c745d80c000000           mov        dword ptr [ebp - 0x28], 0xc
00521095 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00521098 8945dc                   mov        dword ptr [ebp - 0x24], eax
0052109b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052109e 8b5120                   mov        edx, dword ptr [ecx + 0x20]
005210a1 8955d4                   mov        dword ptr [ebp - 0x2c], edx
005210a4 837dd404                 cmp        dword ptr [ebp - 0x2c], 4
005210a8 0f87e50a0000             ja         0x521b93
005210ae 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
005210b1 ff2485a81b5200           jmp        dword ptr [eax*4 + 0x521ba8]
005210b8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005210bb 83c124                   add        ecx, 0x24
005210be 894db0                   mov        dword ptr [ebp - 0x50], ecx
005210c1 6a02                     push       2
005210c3 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
005210c6 e815b4f7ff               call       0x49c4e0
005210cb 90                       nop        
005210cc b968a55b00               mov        ecx, 0x5ba568
005210d1 e8ca2ff4ff               call       0x4640a0
005210d6 83f804                   cmp        eax, 4
005210d9 0f8534010000             jne        0x521213
005210df 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005210e2 83c124                   add        ecx, 0x24
005210e5 e816b2eeff               call       0x40c300
005210ea 8945ac                   mov        dword ptr [ebp - 0x54], eax
005210ed 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
005210f0 52                       push       edx
005210f1 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
005210f7 e874b70000               call       0x52c870
005210fc 0fb6c0                   movzx      eax, al
005210ff 85c0                     test       eax, eax
00521101 0f8584000000             jne        0x52118b
00521107 c745e800000000           mov        dword ptr [ebp - 0x18], 0
0052110e 8d4de8                   lea        ecx, [ebp - 0x18]
00521111 894dd0                   mov        dword ptr [ebp - 0x30], ecx
00521114 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00521117 8b02                     mov        eax, dword ptr [edx]
00521119 50                       push       eax
0052111a 8d4df8                   lea        ecx, [ebp - 8]
0052111d 51                       push       ecx
0052111e e8ed03f4ff               call       0x461510
00521123 83c408                   add        esp, 8
00521126 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00521129 8b02                     mov        eax, dword ptr [edx]
0052112b 50                       push       eax
0052112c 8d4de4                   lea        ecx, [ebp - 0x1c]
0052112f 51                       push       ecx
00521130 e82b9dfeff               call       0x50ae60
00521135 83c408                   add        esp, 8
00521138 eb09                     jmp        0x521143
0052113a 8d4df8                   lea        ecx, [ebp - 8]
0052113d e81e07f4ff               call       0x461860
00521142 90                       nop        
00521143 8d55e4                   lea        edx, [ebp - 0x1c]
00521146 52                       push       edx
00521147 8d4df8                   lea        ecx, [ebp - 8]
0052114a e81183f2ff               call       0x449460
0052114f 0fb6c0                   movzx      eax, al
00521152 85c0                     test       eax, eax
00521154 7435                     je         0x52118b
00521156 8d4df8                   lea        ecx, [ebp - 8]
00521159 e8a2b1eeff               call       0x40c300
0052115e 8945cc                   mov        dword ptr [ebp - 0x34], eax
00521161 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00521164 51                       push       ecx
00521165 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052116b e800b70000               call       0x52c870
00521170 0fb6d0                   movzx      edx, al
00521173 85d2                     test       edx, edx
00521175 7412                     je         0x521189
00521177 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
0052117a 50                       push       eax
0052117b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052117e 83c124                   add        ecx, 0x24
00521181 e8cadbf9ff               call       0x4bed50
00521186 90                       nop        
00521187 eb02                     jmp        0x52118b
00521189 ebaf                     jmp        0x52113a
0052118b c745f000000000           mov        dword ptr [ebp - 0x10], 0
00521192 8d4df0                   lea        ecx, [ebp - 0x10]
00521195 894dc8                   mov        dword ptr [ebp - 0x38], ecx
00521198 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
0052119b 8b02                     mov        eax, dword ptr [edx]
0052119d 50                       push       eax
0052119e 8d4df4                   lea        ecx, [ebp - 0xc]
005211a1 51                       push       ecx
005211a2 e86903f4ff               call       0x461510
005211a7 83c408                   add        esp, 8
005211aa 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
005211ad 8b02                     mov        eax, dword ptr [edx]
005211af 50                       push       eax
005211b0 8d4dec                   lea        ecx, [ebp - 0x14]
005211b3 51                       push       ecx
005211b4 e8a79cfeff               call       0x50ae60
005211b9 83c408                   add        esp, 8
005211bc eb09                     jmp        0x5211c7
005211be 8d4df4                   lea        ecx, [ebp - 0xc]
005211c1 e89a06f4ff               call       0x461860
005211c6 90                       nop        
005211c7 8d55ec                   lea        edx, [ebp - 0x14]
005211ca 52                       push       edx
005211cb 8d4df4                   lea        ecx, [ebp - 0xc]
005211ce e88d82f2ff               call       0x449460
005211d3 0fb6c0                   movzx      eax, al
005211d6 85c0                     test       eax, eax
005211d8 7439                     je         0x521213
005211da 8d4df4                   lea        ecx, [ebp - 0xc]
005211dd e81eb1eeff               call       0x40c300
005211e2 8945b4                   mov        dword ptr [ebp - 0x4c], eax
005211e5 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
005211e8 51                       push       ecx
005211e9 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
005211ef e87cb60000               call       0x52c870
005211f4 0fb6d0                   movzx      edx, al
005211f7 85d2                     test       edx, edx
005211f9 7516                     jne        0x521211
005211fb 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
005211fe 83c024                   add        eax, 0x24
00521201 8945a8                   mov        dword ptr [ebp - 0x58], eax
00521204 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00521207 51                       push       ecx
00521208 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0052120b e800c6ffff               call       0x51d810
00521210 90                       nop        
00521211 ebab                     jmp        0x5211be
00521213 6a23                     push       0x23
00521215 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521218 e833b50000               call       0x52c750
0052121d 85c0                     test       eax, eax
0052121f 750b                     jne        0x52122c
00521221 6a23                     push       0x23
00521223 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521226 e855bd0000               call       0x52cf80
0052122b 90                       nop        
0052122c 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0052122f 52                       push       edx
00521230 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521233 e818b50000               call       0x52c750
00521238 85c0                     test       eax, eax
0052123a 7543                     jne        0x52127f
0052123c 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0052123f 50                       push       eax
00521240 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521243 e8e8b90000               call       0x52cc30
00521248 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0052124b 51                       push       ecx
0052124c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052124f e82cbd0000               call       0x52cf80
00521254 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00521257 52                       push       edx
00521258 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052125b e830ba0000               call       0x52cc90
00521260 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521263 83c124                   add        ecx, 0x24
00521266 e895b0eeff               call       0x40c300
0052126b 83c007                   add        eax, 7
0052126e 98                       cwde       
0052126f 50                       push       eax
00521270 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00521273 51                       push       ecx
00521274 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521277 e854ba0000               call       0x52ccd0
0052127c 90                       nop        
0052127d eb29                     jmp        0x5212a8
0052127f 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00521282 52                       push       edx
00521283 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521286 e805ba0000               call       0x52cc90
0052128b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052128e 83c124                   add        ecx, 0x24
00521291 e86ab0eeff               call       0x40c300
00521296 83c007                   add        eax, 7
00521299 98                       cwde       
0052129a 50                       push       eax
0052129b 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0052129e 51                       push       ecx
0052129f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005212a2 e829ba0000               call       0x52ccd0
005212a7 90                       nop        
005212a8 6a01                     push       1
005212aa 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005212ad e89ebb0000               call       0x52ce50
005212b2 90                       nop        
005212b3 b968a55b00               mov        ecx, 0x5ba568
005212b8 e8e32df4ff               call       0x4640a0
005212bd 8945a4                   mov        dword ptr [ebp - 0x5c], eax
005212c0 6a00                     push       0
005212c2 8b55a4                   mov        edx, dword ptr [ebp - 0x5c]
005212c5 52                       push       edx
005212c6 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
005212cc e81fb20000               call       0x52c4f0
005212d1 85c0                     test       eax, eax
005212d3 751f                     jne        0x5212f4
005212d5 6a0f                     push       0xf
005212d7 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
005212da 50                       push       eax
005212db 8d8d00ffffff             lea        ecx, [ebp - 0x100]
005212e1 51                       push       ecx
005212e2 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005212e5 e856af0000               call       0x52c240
005212ea 8bc8                     mov        ecx, eax
005212ec e82feef2ff               call       0x450120
005212f1 90                       nop        
005212f2 eb1d                     jmp        0x521311
005212f4 6a0f                     push       0xf
005212f6 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
005212f9 52                       push       edx
005212fa 8d85fcfeffff             lea        eax, [ebp - 0x104]
00521300 50                       push       eax
00521301 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521304 e837af0000               call       0x52c240
00521309 8bc8                     mov        ecx, eax
0052130b e820e7f2ff               call       0x44fa30
00521310 90                       nop        
00521311 b968a55b00               mov        ecx, 0x5ba568
00521316 e8852df4ff               call       0x4640a0
0052131b 8945a0                   mov        dword ptr [ebp - 0x60], eax
0052131e 6a01                     push       1
00521320 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00521323 51                       push       ecx
00521324 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052132a e8c1b10000               call       0x52c4f0
0052132f 85c0                     test       eax, eax
00521331 751f                     jne        0x521352
00521333 6a10                     push       0x10
00521335 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00521338 52                       push       edx
00521339 8d85f8feffff             lea        eax, [ebp - 0x108]
0052133f 50                       push       eax
00521340 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521343 e8f8ae0000               call       0x52c240
00521348 8bc8                     mov        ecx, eax
0052134a e8d1edf2ff               call       0x450120
0052134f 90                       nop        
00521350 eb1d                     jmp        0x52136f
00521352 6a10                     push       0x10
00521354 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00521357 51                       push       ecx
00521358 8d95f4feffff             lea        edx, [ebp - 0x10c]
0052135e 52                       push       edx
0052135f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521362 e8d9ae0000               call       0x52c240
00521367 8bc8                     mov        ecx, eax
00521369 e8c2e6f2ff               call       0x44fa30
0052136e 90                       nop        
0052136f 833d28615c0004           cmp        dword ptr [0x5c6128], 4
00521376 740d                     je         0x521385
00521378 833d28615c0005           cmp        dword ptr [0x5c6128], 5
0052137f 0f85e3000000             jne        0x521468
00521385 b968a55b00               mov        ecx, 0x5ba568
0052138a e8e16bf9ff               call       0x4b7f70
0052138f 50                       push       eax
00521390 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521393 83c124                   add        ecx, 0x24
00521396 e8b5d9f9ff               call       0x4bed50
0052139b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052139e 83c124                   add        ecx, 0x24
005213a1 e85aafeeff               call       0x40c300
005213a6 83c007                   add        eax, 7
005213a9 98                       cwde       
005213aa 50                       push       eax
005213ab 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
005213ae 51                       push       ecx
005213af 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005213b2 e859b90000               call       0x52cd10
005213b7 90                       nop        
005213b8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005213bb 83c124                   add        ecx, 0x24
005213be e83dafeeff               call       0x40c300
005213c3 83c006                   add        eax, 6
005213c6 50                       push       eax
005213c7 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
005213ca 52                       push       edx
005213cb 8d85f0feffff             lea        eax, [ebp - 0x110]
005213d1 50                       push       eax
005213d2 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005213d5 e866ae0000               call       0x52c240
005213da 89459c                   mov        dword ptr [ebp - 0x64], eax
005213dd 6a06                     push       6
005213df 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
005213e2 e8a9dbf2ff               call       0x44ef90
005213e7 90                       nop        
005213e8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005213eb 83c124                   add        ecx, 0x24
005213ee e80dafeeff               call       0x40c300
005213f3 83c008                   add        eax, 8
005213f6 50                       push       eax
005213f7 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
005213fa 51                       push       ecx
005213fb 8d95ecfeffff             lea        edx, [ebp - 0x114]
00521401 52                       push       edx
00521402 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521405 e836ae0000               call       0x52c240
0052140a 894598                   mov        dword ptr [ebp - 0x68], eax
0052140d 6a06                     push       6
0052140f 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00521412 e879dbf2ff               call       0x44ef90
00521417 90                       nop        
00521418 6a04                     push       4
0052141a 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0052141d 50                       push       eax
0052141e 8d8de8feffff             lea        ecx, [ebp - 0x118]
00521424 51                       push       ecx
00521425 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521428 e813ae0000               call       0x52c240
0052142d 894594                   mov        dword ptr [ebp - 0x6c], eax
00521430 6a06                     push       6
00521432 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00521435 e856dbf2ff               call       0x44ef90
0052143a 90                       nop        
0052143b 6a05                     push       5
0052143d 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00521440 52                       push       edx
00521441 8d85e4feffff             lea        eax, [ebp - 0x11c]
00521447 50                       push       eax
00521448 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052144b e8f0ad0000               call       0x52c240
00521450 894590                   mov        dword ptr [ebp - 0x70], eax
00521453 6a06                     push       6
00521455 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00521458 e833dbf2ff               call       0x44ef90
0052145d 90                       nop        
0052145e e9dc030000               jmp        0x52183f
00521463 e9d7030000               jmp        0x52183f
00521468 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052146b 81c154010000             add        ecx, 0x154
00521471 894d8c                   mov        dword ptr [ebp - 0x74], ecx
00521474 6a06                     push       6
00521476 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00521479 e8f2fbf3ff               call       0x461070
0052147e 0fb6d0                   movzx      edx, al
00521481 85d2                     test       edx, edx
00521483 0f848f000000             je         0x521518
00521489 6a02                     push       2
0052148b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052148e e8bdb90000               call       0x52ce50
00521493 90                       nop        
00521494 6a06                     push       6
00521496 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
00521499 50                       push       eax
0052149a 8d8de0feffff             lea        ecx, [ebp - 0x120]
005214a0 51                       push       ecx
005214a1 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005214a4 e897ad0000               call       0x52c240
005214a9 894584                   mov        dword ptr [ebp - 0x7c], eax
005214ac 6a00                     push       0
005214ae 6a00                     push       0
005214b0 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
005214b6 e8452cf4ff               call       0x464100
005214bb 99                       cdq        
005214bc 2bc2                     sub        eax, edx
005214be d1f8                     sar        eax, 1
005214c0 83c01e                   add        eax, 0x1e
005214c3 894588                   mov        dword ptr [ebp - 0x78], eax
005214c6 8b5588                   mov        edx, dword ptr [ebp - 0x78]
005214c9 52                       push       edx
005214ca 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
005214cd e83e30f4ff               call       0x464510
005214d2 90                       nop        
005214d3 6a07                     push       7
005214d5 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
005214d8 50                       push       eax
005214d9 8d8ddcfeffff             lea        ecx, [ebp - 0x124]
005214df 51                       push       ecx
005214e0 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005214e3 e858ad0000               call       0x52c240
005214e8 89857cffffff             mov        dword ptr [ebp - 0x84], eax
005214ee 6a01                     push       1
005214f0 6a00                     push       0
005214f2 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
005214f8 e8032cf4ff               call       0x464100
005214fd 99                       cdq        
005214fe 2bc2                     sub        eax, edx
00521500 d1f8                     sar        eax, 1
00521502 83c01e                   add        eax, 0x1e
00521505 894580                   mov        dword ptr [ebp - 0x80], eax
00521508 8b5580                   mov        edx, dword ptr [ebp - 0x80]
0052150b 52                       push       edx
0052150c 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00521512 e8f92ff4ff               call       0x464510
00521517 90                       nop        
00521518 e976060000               jmp        0x521b93
0052151d 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521520 83c124                   add        ecx, 0x24
00521523 e8d8e6f9ff               call       0x4bfc00
00521528 90                       nop        
00521529 33c0                     xor        eax, eax
0052152b 7537                     jne        0x521564
0052152d b904000000               mov        ecx, 4
00521532 6bd100                   imul       edx, ecx, 0
00521535 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
0052153c 7426                     je         0x521564
0052153e b804000000               mov        eax, 4
00521543 6bc800                   imul       ecx, eax, 0
00521546 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
0052154c 899578ffffff             mov        dword ptr [ebp - 0x88], edx
00521552 6a40                     push       0x40
00521554 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0052155a e8218defff               call       0x41a280
0052155f 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00521562 eb07                     jmp        0x52156b
00521564 c745c400000000           mov        dword ptr [ebp - 0x3c], 0
0052156b 837dc400                 cmp        dword ptr [ebp - 0x3c], 0
0052156f 7460                     je         0x5215d1
00521571 6a00                     push       0
00521573 6a0a                     push       0xa
00521575 b930a85b00               mov        ecx, 0x5ba830
0052157a e8f157f0ff               call       0x426d70
0052157f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521582 83c124                   add        ecx, 0x24
00521585 e876adeeff               call       0x40c300
0052158a 83c019                   add        eax, 0x19
0052158d 98                       cwde       
0052158e 50                       push       eax
0052158f 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00521592 51                       push       ecx
00521593 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521596 e875b70000               call       0x52cd10
0052159b 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052159e 83c224                   add        edx, 0x24
005215a1 899574ffffff             mov        dword ptr [ebp - 0x8c], edx
005215a7 6a00                     push       0
005215a9 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
005215af e88cdaf9ff               call       0x4bf040
005215b4 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005215b7 83c124                   add        ecx, 0x24
005215ba e841adeeff               call       0x40c300
005215bf 83c00d                   add        eax, 0xd
005215c2 98                       cwde       
005215c3 50                       push       eax
005215c4 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
005215c7 51                       push       ecx
005215c8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005215cb e800b70000               call       0x52ccd0
005215d0 90                       nop        
005215d1 33d2                     xor        edx, edx
005215d3 753a                     jne        0x52160f
005215d5 b804000000               mov        eax, 4
005215da 6bc800                   imul       ecx, eax, 0
005215dd 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
005215e4 7429                     je         0x52160f
005215e6 ba04000000               mov        edx, 4
005215eb 6bc200                   imul       eax, edx, 0
005215ee 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
005215f4 898d70ffffff             mov        dword ptr [ebp - 0x90], ecx
005215fa 6880000000               push       0x80
005215ff 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
00521605 e8768cefff               call       0x41a280
0052160a 8945c0                   mov        dword ptr [ebp - 0x40], eax
0052160d eb07                     jmp        0x521616
0052160f c745c000000000           mov        dword ptr [ebp - 0x40], 0
00521616 837dc000                 cmp        dword ptr [ebp - 0x40], 0
0052161a 7464                     je         0x521680
0052161c 6a00                     push       0
0052161e 6a0a                     push       0xa
00521620 b930a85b00               mov        ecx, 0x5ba830
00521625 e84657f0ff               call       0x426d70
0052162a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052162d 83c124                   add        ecx, 0x24
00521630 e8cbaceeff               call       0x40c300
00521635 83c013                   add        eax, 0x13
00521638 0fbfd0                   movsx      edx, ax
0052163b 52                       push       edx
0052163c 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0052163f 50                       push       eax
00521640 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521643 e8c8b60000               call       0x52cd10
00521648 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052164b 83c124                   add        ecx, 0x24
0052164e 898d6cffffff             mov        dword ptr [ebp - 0x94], ecx
00521654 6a00                     push       0
00521656 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
0052165c e8bfd9f9ff               call       0x4bf020
00521661 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521664 83c124                   add        ecx, 0x24
00521667 e894aceeff               call       0x40c300
0052166c 83c007                   add        eax, 7
0052166f 0fbfd0                   movsx      edx, ax
00521672 52                       push       edx
00521673 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
00521676 50                       push       eax
00521677 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052167a e851b60000               call       0x52ccd0
0052167f 90                       nop        
00521680 33c9                     xor        ecx, ecx
00521682 753a                     jne        0x5216be
00521684 ba04000000               mov        edx, 4
00521689 6bc200                   imul       eax, edx, 0
0052168c 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
00521693 7429                     je         0x5216be
00521695 b904000000               mov        ecx, 4
0052169a 6bd100                   imul       edx, ecx, 0
0052169d 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
005216a3 898568ffffff             mov        dword ptr [ebp - 0x98], eax
005216a9 6801000800               push       0x80001
005216ae 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
005216b4 e84785efff               call       0x419c00
005216b9 8945bc                   mov        dword ptr [ebp - 0x44], eax
005216bc eb07                     jmp        0x5216c5
005216be c745bc00000000           mov        dword ptr [ebp - 0x44], 0
005216c5 837dbc00                 cmp        dword ptr [ebp - 0x44], 0
005216c9 0f84dc000000             je         0x5217ab
005216cf 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005216d2 83c124                   add        ecx, 0x24
005216d5 e826aceeff               call       0x40c300
005216da 83c006                   add        eax, 6
005216dd 50                       push       eax
005216de 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
005216e1 51                       push       ecx
005216e2 8d95d8feffff             lea        edx, [ebp - 0x128]
005216e8 52                       push       edx
005216e9 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005216ec e84fab0000               call       0x52c240
005216f1 898564ffffff             mov        dword ptr [ebp - 0x9c], eax
005216f7 6a06                     push       6
005216f9 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
005216ff e88cd8f2ff               call       0x44ef90
00521704 90                       nop        
00521705 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521708 83c124                   add        ecx, 0x24
0052170b e8f0abeeff               call       0x40c300
00521710 83c008                   add        eax, 8
00521713 50                       push       eax
00521714 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
00521717 50                       push       eax
00521718 8d8dd4feffff             lea        ecx, [ebp - 0x12c]
0052171e 51                       push       ecx
0052171f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521722 e819ab0000               call       0x52c240
00521727 898560ffffff             mov        dword ptr [ebp - 0xa0], eax
0052172d 6a06                     push       6
0052172f 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
00521735 e856d8f2ff               call       0x44ef90
0052173a 90                       nop        
0052173b 6a04                     push       4
0052173d 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00521740 52                       push       edx
00521741 8d85d0feffff             lea        eax, [ebp - 0x130]
00521747 50                       push       eax
00521748 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052174b e8f0aa0000               call       0x52c240
00521750 89855cffffff             mov        dword ptr [ebp - 0xa4], eax
00521756 6a06                     push       6
00521758 8b8d5cffffff             mov        ecx, dword ptr [ebp - 0xa4]
0052175e e82dd8f2ff               call       0x44ef90
00521763 90                       nop        
00521764 6a05                     push       5
00521766 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00521769 51                       push       ecx
0052176a 8d95ccfeffff             lea        edx, [ebp - 0x134]
00521770 52                       push       edx
00521771 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521774 e8c7aa0000               call       0x52c240
00521779 898558ffffff             mov        dword ptr [ebp - 0xa8], eax
0052177f 6a06                     push       6
00521781 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
00521787 e804d8f2ff               call       0x44ef90
0052178c 90                       nop        
0052178d 6a00                     push       0
0052178f 6a07                     push       7
00521791 b930a85b00               mov        ecx, 0x5ba830
00521796 e8d555f0ff               call       0x426d70
0052179b 6a03                     push       3
0052179d 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005217a0 e8abb60000               call       0x52ce50
005217a5 90                       nop        
005217a6 e9e8030000               jmp        0x521b93
005217ab 33c0                     xor        eax, eax
005217ad 753a                     jne        0x5217e9
005217af b904000000               mov        ecx, 4
005217b4 6bd100                   imul       edx, ecx, 0
005217b7 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
005217be 7429                     je         0x5217e9
005217c0 b804000000               mov        eax, 4
005217c5 6bc800                   imul       ecx, eax, 0
005217c8 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
005217ce 899554ffffff             mov        dword ptr [ebp - 0xac], edx
005217d4 6806010000               push       0x106
005217d9 8b8d54ffffff             mov        ecx, dword ptr [ebp - 0xac]
005217df e81c84efff               call       0x419c00
005217e4 8945b8                   mov        dword ptr [ebp - 0x48], eax
005217e7 eb07                     jmp        0x5217f0
005217e9 c745b800000000           mov        dword ptr [ebp - 0x48], 0
005217f0 837db800                 cmp        dword ptr [ebp - 0x48], 0
005217f4 741e                     je         0x521814
005217f6 6a04                     push       4
005217f8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005217fb e850b60000               call       0x52ce50
00521800 6a00                     push       0
00521802 6a09                     push       9
00521804 b930a85b00               mov        ecx, 0x5ba830
00521809 e86255f0ff               call       0x426d70
0052180e 90                       nop        
0052180f e97f030000               jmp        0x521b93
00521814 e97a030000               jmp        0x521b93
00521819 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0052181c 0554010000               add        eax, 0x154
00521821 898550ffffff             mov        dword ptr [ebp - 0xb0], eax
00521827 6a0e                     push       0xe
00521829 8b8d50ffffff             mov        ecx, dword ptr [ebp - 0xb0]
0052182f e88c1df0ff               call       0x4235c0
00521834 0fb6c8                   movzx      ecx, al
00521837 85c9                     test       ecx, ecx
00521839 0f8469020000             je         0x521aa8
0052183f 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00521845 e816defeff               call       0x50f660
0052184a 6a07                     push       7
0052184c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052184f e87cb60000               call       0x52ced0
00521854 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521857 83c124                   add        ecx, 0x24
0052185a e8a1aaeeff               call       0x40c300
0052185f 50                       push       eax
00521860 b968a55b00               mov        ecx, 0x5ba568
00521865 e8768bfeff               call       0x50a3e0
0052186a b968a55b00               mov        ecx, 0x5ba568
0052186f e8fc66f9ff               call       0x4b7f70
00521874 a340615c00               mov        dword ptr [0x5c6140], eax
00521879 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052187c 83c124                   add        ecx, 0x24
0052187f e82c48faff               call       0x4c60b0
00521884 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00521887 83c224                   add        edx, 0x24
0052188a 89954cffffff             mov        dword ptr [ebp - 0xb4], edx
00521890 6a01                     push       1
00521892 8b8d4cffffff             mov        ecx, dword ptr [ebp - 0xb4]
00521898 e843e3f9ff               call       0x4bfbe0
0052189d 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
005218a0 83c024                   add        eax, 0x24
005218a3 898548ffffff             mov        dword ptr [ebp - 0xb8], eax
005218a9 6a09                     push       9
005218ab 8b8d48ffffff             mov        ecx, dword ptr [ebp - 0xb8]
005218b1 e82aacf7ff               call       0x49c4e0
005218b6 8b0d44615c00             mov        ecx, dword ptr [0x5c6144]
005218bc 51                       push       ecx
005218bd 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005218c0 83c124                   add        ecx, 0x24
005218c3 e888d4f9ff               call       0x4bed50
005218c8 6a00                     push       0
005218ca e86129f4ff               call       0x464230
005218cf 83c404                   add        esp, 4
005218d2 898538ffffff             mov        dword ptr [ebp - 0xc8], eax
005218d8 b968a55b00               mov        ecx, 0x5ba568
005218dd e88e66f9ff               call       0x4b7f70
005218e2 898544ffffff             mov        dword ptr [ebp - 0xbc], eax
005218e8 8b9544ffffff             mov        edx, dword ptr [ebp - 0xbc]
005218ee 52                       push       edx
005218ef 6a00                     push       0
005218f1 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
005218f7 e80428f4ff               call       0x464100
005218fc 898540ffffff             mov        dword ptr [ebp - 0xc0], eax
00521902 b968a55b00               mov        ecx, 0x5ba568
00521907 e86466f9ff               call       0x4b7f70
0052190c 89853cffffff             mov        dword ptr [ebp - 0xc4], eax
00521912 8b8540ffffff             mov        eax, dword ptr [ebp - 0xc0]
00521918 50                       push       eax
00521919 8b8d3cffffff             mov        ecx, dword ptr [ebp - 0xc4]
0052191f 51                       push       ecx
00521920 8b8d38ffffff             mov        ecx, dword ptr [ebp - 0xc8]
00521926 e8f52a0100               call       0x534420
0052192b 6a00                     push       0
0052192d e8fe28f4ff               call       0x464230
00521932 83c404                   add        esp, 4
00521935 898528ffffff             mov        dword ptr [ebp - 0xd8], eax
0052193b b968a55b00               mov        ecx, 0x5ba568
00521940 e82b66f9ff               call       0x4b7f70
00521945 898534ffffff             mov        dword ptr [ebp - 0xcc], eax
0052194b 8b9534ffffff             mov        edx, dword ptr [ebp - 0xcc]
00521951 52                       push       edx
00521952 6a01                     push       1
00521954 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052195a e8a127f4ff               call       0x464100
0052195f 898530ffffff             mov        dword ptr [ebp - 0xd0], eax
00521965 b968a55b00               mov        ecx, 0x5ba568
0052196a e80166f9ff               call       0x4b7f70
0052196f 89852cffffff             mov        dword ptr [ebp - 0xd4], eax
00521975 8b8530ffffff             mov        eax, dword ptr [ebp - 0xd0]
0052197b 50                       push       eax
0052197c 8b8d2cffffff             mov        ecx, dword ptr [ebp - 0xd4]
00521982 51                       push       ecx
00521983 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
00521989 e8422e0100               call       0x5347d0
0052198e 6a00                     push       0
00521990 e89b28f4ff               call       0x464230
00521995 83c404                   add        esp, 4
00521998 898518ffffff             mov        dword ptr [ebp - 0xe8], eax
0052199e b968a55b00               mov        ecx, 0x5ba568
005219a3 e8c865f9ff               call       0x4b7f70
005219a8 898524ffffff             mov        dword ptr [ebp - 0xdc], eax
005219ae 8b9524ffffff             mov        edx, dword ptr [ebp - 0xdc]
005219b4 52                       push       edx
005219b5 6a02                     push       2
005219b7 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
005219bd e83e27f4ff               call       0x464100
005219c2 898520ffffff             mov        dword ptr [ebp - 0xe0], eax
005219c8 b968a55b00               mov        ecx, 0x5ba568
005219cd e89e65f9ff               call       0x4b7f70
005219d2 89851cffffff             mov        dword ptr [ebp - 0xe4], eax
005219d8 8b8520ffffff             mov        eax, dword ptr [ebp - 0xe0]
005219de 50                       push       eax
005219df 8b8d1cffffff             mov        ecx, dword ptr [ebp - 0xe4]
005219e5 51                       push       ecx
005219e6 8b8d18ffffff             mov        ecx, dword ptr [ebp - 0xe8]
005219ec e8df2a0100               call       0x5344d0
005219f1 6a00                     push       0
005219f3 e83828f4ff               call       0x464230
005219f8 83c404                   add        esp, 4
005219fb 898508ffffff             mov        dword ptr [ebp - 0xf8], eax
00521a01 b968a55b00               mov        ecx, 0x5ba568
00521a06 e86565f9ff               call       0x4b7f70
00521a0b 898514ffffff             mov        dword ptr [ebp - 0xec], eax
00521a11 8b9514ffffff             mov        edx, dword ptr [ebp - 0xec]
00521a17 52                       push       edx
00521a18 6a03                     push       3
00521a1a 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00521a20 e8db26f4ff               call       0x464100
00521a25 898510ffffff             mov        dword ptr [ebp - 0xf0], eax
00521a2b b968a55b00               mov        ecx, 0x5ba568
00521a30 e83b65f9ff               call       0x4b7f70
00521a35 89850cffffff             mov        dword ptr [ebp - 0xf4], eax
00521a3b 8b8510ffffff             mov        eax, dword ptr [ebp - 0xf0]
00521a41 50                       push       eax
00521a42 8b8d0cffffff             mov        ecx, dword ptr [ebp - 0xf4]
00521a48 51                       push       ecx
00521a49 8b8d08ffffff             mov        ecx, dword ptr [ebp - 0xf8]
00521a4f e8ac2b0100               call       0x534600
00521a54 90                       nop        
00521a55 6a0f                     push       0xf
00521a57 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00521a5a 52                       push       edx
00521a5b 8d85c8feffff             lea        eax, [ebp - 0x138]
00521a61 50                       push       eax
00521a62 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521a65 e8d6a70000               call       0x52c240
00521a6a 8bc8                     mov        ecx, eax
00521a6c e8afe6f2ff               call       0x450120
00521a71 90                       nop        
00521a72 6a10                     push       0x10
00521a74 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00521a77 51                       push       ecx
00521a78 8d95c4feffff             lea        edx, [ebp - 0x13c]
00521a7e 52                       push       edx
00521a7f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521a82 e8b9a70000               call       0x52c240
00521a87 8bc8                     mov        ecx, eax
00521a89 e892e6f2ff               call       0x450120
00521a8e 90                       nop        
00521a8f 6a06                     push       6
00521a91 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
00521a94 50                       push       eax
00521a95 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521a98 e833b20000               call       0x52ccd0
00521a9d 6a23                     push       0x23
00521a9f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521aa2 e889b10000               call       0x52cc30
00521aa7 90                       nop        
00521aa8 e9e6000000               jmp        0x521b93
00521aad 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521ab0 81c154010000             add        ecx, 0x154
00521ab6 898d04ffffff             mov        dword ptr [ebp - 0xfc], ecx
00521abc 6a06                     push       6
00521abe 8b8d04ffffff             mov        ecx, dword ptr [ebp - 0xfc]
00521ac4 e8f71af0ff               call       0x4235c0
00521ac9 0fb6d0                   movzx      edx, al
00521acc 85d2                     test       edx, edx
00521ace 0f84bf000000             je         0x521b93
00521ad4 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00521ada e881dbfeff               call       0x50f660
00521adf 90                       nop        
00521ae0 b968a55b00               mov        ecx, 0x5ba568
00521ae5 e8666df6ff               call       0x488850
00521aea 0fb6c0                   movzx      eax, al
00521aed 85c0                     test       eax, eax
00521aef 7552                     jne        0x521b43
00521af1 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00521af4 51                       push       ecx
00521af5 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521af8 e833b10000               call       0x52cc30
00521afd 6a23                     push       0x23
00521aff 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521b02 e829b10000               call       0x52cc30
00521b07 6a05                     push       5
00521b09 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521b0c e8bfb30000               call       0x52ced0
00521b11 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521b14 83c124                   add        ecx, 0x24
00521b17 e8e4a7eeff               call       0x40c300
00521b1c 50                       push       eax
00521b1d b968a55b00               mov        ecx, 0x5ba568
00521b22 e8b988feff               call       0x50a3e0
00521b27 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521b2a 83c124                   add        ecx, 0x24
00521b2d e8de44faff               call       0x4c6010
00521b32 b968a55b00               mov        ecx, 0x5ba568
00521b37 e83464f9ff               call       0x4b7f70
00521b3c a340615c00               mov        dword ptr [0x5c6140], eax
00521b41 eb50                     jmp        0x521b93
00521b43 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00521b46 52                       push       edx
00521b47 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521b4a e8e1b00000               call       0x52cc30
00521b4f 6a23                     push       0x23
00521b51 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521b54 e8d7b00000               call       0x52cc30
00521b59 6a01                     push       1
00521b5b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521b5e e86db30000               call       0x52ced0
00521b63 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521b66 83c124                   add        ecx, 0x24
00521b69 e892a7eeff               call       0x40c300
00521b6e 50                       push       eax
00521b6f b968a55b00               mov        ecx, 0x5ba568
00521b74 e86788feff               call       0x50a3e0
00521b79 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00521b7c 83c124                   add        ecx, 0x24
00521b7f e88c44faff               call       0x4c6010
00521b84 b968a55b00               mov        ecx, 0x5ba568
00521b89 e8e263f9ff               call       0x4b7f70
00521b8e a340615c00               mov        dword ptr [0x5c6140], eax
00521b93 b801000000               mov        eax, 1
00521b98 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521b9b 33cd                     xor        ecx, ebp
00521b9d e84c0e0200               call       0x5429ee
00521ba2 8be5                     mov        esp, ebp
00521ba4 5d                       pop        ebp
00521ba5 c3                       ret        
00521ba6 6690                     nop        
00521ba8 b810520068               mov        eax, 0x68005210
00521bad 1452                     adc        al, 0x52
00521baf 001d15520019             add        byte ptr [0x19005215], bl
00521bb5 185200                   sbb        byte ptr [edx], dl
00521bb8 ad                       lodsd      eax, dword ptr [esi]
00521bb9 1a5200                   sbb        dl, byte ptr [edx]
00521bbc cc                       int3       
00521bbd cc                       int3       
00521bbe cc                       int3       
00521bbf cc                       int3       
