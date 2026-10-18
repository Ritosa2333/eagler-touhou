0052e210 55                       push       ebp
0052e211 8bec                     mov        ebp, esp
0052e213 6aff                     push       -1
0052e215 68c6af5600               push       0x56afc6
0052e21a 64a100000000             mov        eax, dword ptr fs:[0]
0052e220 50                       push       eax
0052e221 81eca4010000             sub        esp, 0x1a4
0052e227 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052e22c 33c5                     xor        eax, ebp
0052e22e 8945f0                   mov        dword ptr [ebp - 0x10], eax
0052e231 50                       push       eax
0052e232 8d45f4                   lea        eax, [ebp - 0xc]
0052e235 64a300000000             mov        dword ptr fs:[0], eax
0052e23b 6800575700               push       0x575700
0052e240 6a14                     push       0x14
0052e242 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0052e248 e8030cf2ff               call       0x44ee50
0052e24d 85c0                     test       eax, eax
0052e24f 751a                     jne        0x52e26b
0052e251 685c1f5700               push       0x571f5c
0052e256 6878065c00               push       0x5c0678
0052e25b e8f05ef2ff               call       0x454150
0052e260 83c408                   add        esp, 8
0052e263 83c8ff                   or         eax, 0xffffffff
0052e266 e9a8040000               jmp        0x52e713
0052e26b 6880000000               push       0x80
0052e270 6820575700               push       0x575720
0052e275 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0052e27b e840f3ffff               call       0x52d5c0
0052e280 a35c685c00               mov        dword ptr [0x5c685c], eax
0052e285 6a00                     push       0
0052e287 8d45b4                   lea        eax, [ebp - 0x4c]
0052e28a 50                       push       eax
0052e28b 6864575700               push       0x575764
0052e290 e80b28eeff               call       0x410aa0
0052e295 83c40c                   add        esp, 0xc
0052e298 8985f8feffff             mov        dword ptr [ebp - 0x108], eax
0052e29e 6a1c                     push       0x1c
0052e2a0 8d4db8                   lea        ecx, [ebp - 0x48]
0052e2a3 e8d8ddedff               call       0x40c080
0052e2a8 8b8df8feffff             mov        ecx, dword ptr [ebp - 0x108]
0052e2ae 51                       push       ecx
0052e2af 8d4db8                   lea        ecx, [ebp - 0x48]
0052e2b2 e879c5f3ff               call       0x46a830
0052e2b7 c745fc00000000           mov        dword ptr [ebp - 4], 0
0052e2be 8d958cfeffff             lea        edx, [ebp - 0x174]
0052e2c4 52                       push       edx
0052e2c5 8d4db8                   lea        ecx, [ebp - 0x48]
0052e2c8 e833d2feff               call       0x51b500
0052e2cd 8b00                     mov        eax, dword ptr [eax]
0052e2cf 8985f0feffff             mov        dword ptr [ebp - 0x110], eax
0052e2d5 8b8df0feffff             mov        ecx, dword ptr [ebp - 0x110]
0052e2db 898de0feffff             mov        dword ptr [ebp - 0x120], ecx
0052e2e1 c6850affffff0d           mov        byte ptr [ebp - 0xf6], 0xd
0052e2e8 8d9588feffff             lea        edx, [ebp - 0x178]
0052e2ee 52                       push       edx
0052e2ef 8d4db8                   lea        ecx, [ebp - 0x48]
0052e2f2 e809d2feff               call       0x51b500
0052e2f7 8b00                     mov        eax, dword ptr [eax]
0052e2f9 8985ecfeffff             mov        dword ptr [ebp - 0x114], eax
0052e2ff 8d8d84feffff             lea        ecx, [ebp - 0x17c]
0052e305 51                       push       ecx
0052e306 8d4db8                   lea        ecx, [ebp - 0x48]
0052e309 e8f209fdff               call       0x4fed00
0052e30e 8b10                     mov        edx, dword ptr [eax]
0052e310 8995e8feffff             mov        dword ptr [ebp - 0x118], edx
0052e316 8d850affffff             lea        eax, [ebp - 0xf6]
0052e31c 50                       push       eax
0052e31d 8b8decfeffff             mov        ecx, dword ptr [ebp - 0x114]
0052e323 51                       push       ecx
0052e324 8b95e8feffff             mov        edx, dword ptr [ebp - 0x118]
0052e32a 52                       push       edx
0052e32b 8d85b0feffff             lea        eax, [ebp - 0x150]
0052e331 50                       push       eax
0052e332 e81972feff               call       0x515550
0052e337 83c410                   add        esp, 0x10
0052e33a 8b08                     mov        ecx, dword ptr [eax]
0052e33c 898de4feffff             mov        dword ptr [ebp - 0x11c], ecx
0052e342 8b95e4feffff             mov        edx, dword ptr [ebp - 0x11c]
0052e348 8995dcfeffff             mov        dword ptr [ebp - 0x124], edx
0052e34e 8b85e0feffff             mov        eax, dword ptr [ebp - 0x120]
0052e354 50                       push       eax
0052e355 8b8ddcfeffff             mov        ecx, dword ptr [ebp - 0x124]
0052e35b 51                       push       ecx
0052e35c 8d95acfeffff             lea        edx, [ebp - 0x154]
0052e362 52                       push       edx
0052e363 8d4db8                   lea        ecx, [ebp - 0x48]
0052e366 e875d2feff               call       0x51b5e0
0052e36b 90                       nop        
0052e36c 8d85a8feffff             lea        eax, [ebp - 0x158]
0052e372 50                       push       eax
0052e373 8d4db8                   lea        ecx, [ebp - 0x48]
0052e376 e885d1feff               call       0x51b500
0052e37b 8b08                     mov        ecx, dword ptr [eax]
0052e37d 898dd8feffff             mov        dword ptr [ebp - 0x128], ecx
0052e383 8b95d8feffff             mov        edx, dword ptr [ebp - 0x128]
0052e389 8995b8feffff             mov        dword ptr [ebp - 0x148], edx
0052e38f c6850bffffff0c           mov        byte ptr [ebp - 0xf5], 0xc
0052e396 8d85a4feffff             lea        eax, [ebp - 0x15c]
0052e39c 50                       push       eax
0052e39d 8d4db8                   lea        ecx, [ebp - 0x48]
0052e3a0 e85bd1feff               call       0x51b500
0052e3a5 8b08                     mov        ecx, dword ptr [eax]
0052e3a7 898dd4feffff             mov        dword ptr [ebp - 0x12c], ecx
0052e3ad 8d95a0feffff             lea        edx, [ebp - 0x160]
0052e3b3 52                       push       edx
0052e3b4 8d4db8                   lea        ecx, [ebp - 0x48]
0052e3b7 e84409fdff               call       0x4fed00
0052e3bc 8b00                     mov        eax, dword ptr [eax]
0052e3be 8985d0feffff             mov        dword ptr [ebp - 0x130], eax
0052e3c4 8d8d0bffffff             lea        ecx, [ebp - 0xf5]
0052e3ca 51                       push       ecx
0052e3cb 8b95d4feffff             mov        edx, dword ptr [ebp - 0x12c]
0052e3d1 52                       push       edx
0052e3d2 8b85d0feffff             mov        eax, dword ptr [ebp - 0x130]
0052e3d8 50                       push       eax
0052e3d9 8d8d9cfeffff             lea        ecx, [ebp - 0x164]
0052e3df 51                       push       ecx
0052e3e0 e86b71feff               call       0x515550
0052e3e5 83c410                   add        esp, 0x10
0052e3e8 8b10                     mov        edx, dword ptr [eax]
0052e3ea 8995ccfeffff             mov        dword ptr [ebp - 0x134], edx
0052e3f0 8b85ccfeffff             mov        eax, dword ptr [ebp - 0x134]
0052e3f6 8985bcfeffff             mov        dword ptr [ebp - 0x144], eax
0052e3fc 8b8db8feffff             mov        ecx, dword ptr [ebp - 0x148]
0052e402 51                       push       ecx
0052e403 8b95bcfeffff             mov        edx, dword ptr [ebp - 0x144]
0052e409 52                       push       edx
0052e40a 8d8598feffff             lea        eax, [ebp - 0x168]
0052e410 50                       push       eax
0052e411 8d4db8                   lea        ecx, [ebp - 0x48]
0052e414 e8c7d1feff               call       0x51b5e0
0052e419 90                       nop        
0052e41a 68a8000000               push       0xa8
0052e41f 8d8d0cffffff             lea        ecx, [ebp - 0xf4]
0052e425 e856dcedff               call       0x40c080
0052e42a 6a01                     push       1
0052e42c 6a01                     push       1
0052e42e 8d4db8                   lea        ecx, [ebp - 0x48]
0052e431 51                       push       ecx
0052e432 8d8d0cffffff             lea        ecx, [ebp - 0xf4]
0052e438 e8735efeff               call       0x5142b0
0052e43d c645fc01                 mov        byte ptr [ebp - 4], 1
0052e441 6a1c                     push       0x1c
0052e443 8d4dd4                   lea        ecx, [ebp - 0x2c]
0052e446 e835dcedff               call       0x40c080
0052e44b 8d4dd4                   lea        ecx, [ebp - 0x2c]
0052e44e e86d98f1ff               call       0x447cc0
0052e453 c645fc02                 mov        byte ptr [ebp - 4], 2
0052e457 8d55d4                   lea        edx, [ebp - 0x2c]
0052e45a 52                       push       edx
0052e45b 8d850cffffff             lea        eax, [ebp - 0xf4]
0052e461 50                       push       eax
0052e462 e89970feff               call       0x515500
0052e467 83c408                   add        esp, 8
0052e46a 8985f4feffff             mov        dword ptr [ebp - 0x10c], eax
0052e470 8b8df4feffff             mov        ecx, dword ptr [ebp - 0x10c]
0052e476 8b11                     mov        edx, dword ptr [ecx]
0052e478 8b8df4feffff             mov        ecx, dword ptr [ebp - 0x10c]
0052e47e 034a04                   add        ecx, dword ptr [edx + 4]
0052e481 e89a7ffeff               call       0x516420
0052e486 0fb6c0                   movzx      eax, al
0052e489 85c0                     test       eax, eax
0052e48b 0f8430020000             je         0x52e6c1
0052e491 8d4dd4                   lea        ecx, [ebp - 0x2c]
0052e494 e8f7e5eeff               call       0x41ca90
0052e499 85c0                     test       eax, eax
0052e49b 7412                     je         0x52e4af
0052e49d 6a00                     push       0
0052e49f 8d4dd4                   lea        ecx, [ebp - 0x2c]
0052e4a2 e8190df7ff               call       0x49f1c0
0052e4a7 0fbe08                   movsx      ecx, byte ptr [eax]
0052e4aa 83f923                   cmp        ecx, 0x23
0052e4ad 7507                     jne        0x52e4b6
0052e4af eba6                     jmp        0x52e457
0052e4b1 e906020000               jmp        0x52e6bc
0052e4b6 6a00                     push       0
0052e4b8 8d4dd4                   lea        ecx, [ebp - 0x2c]
0052e4bb e8000df7ff               call       0x49f1c0
0052e4c0 0fbe10                   movsx      edx, byte ptr [eax]
0052e4c3 83fa5c                   cmp        edx, 0x5c
0052e4c6 750a                     jne        0x52e4d2
0052e4c8 e9f4010000               jmp        0x52e6c1
0052e4cd e9ea010000               jmp        0x52e6bc
0052e4d2 6a00                     push       0
0052e4d4 8d4dd4                   lea        ecx, [ebp - 0x2c]
0052e4d7 e8e40cf7ff               call       0x49f1c0
0052e4dc 0fbe00                   movsx      eax, byte ptr [eax]
0052e4df 83f840                   cmp        eax, 0x40
0052e4e2 0f85d4010000             jne        0x52e6bc
0052e4e8 6aff                     push       -1
0052e4ea 6a01                     push       1
0052e4ec 8d8d50feffff             lea        ecx, [ebp - 0x1b0]
0052e4f2 51                       push       ecx
0052e4f3 8d4dd4                   lea        ecx, [ebp - 0x2c]
0052e4f6 e885e7feff               call       0x51cc80
0052e4fb 8985c0feffff             mov        dword ptr [ebp - 0x140], eax
0052e501 8b95c0feffff             mov        edx, dword ptr [ebp - 0x140]
0052e507 8995b4feffff             mov        dword ptr [ebp - 0x14c], edx
0052e50d c645fc03                 mov        byte ptr [ebp - 4], 3
0052e511 8b8db4feffff             mov        ecx, dword ptr [ebp - 0x14c]
0052e517 e8d4d5f1ff               call       0x44baf0
0052e51c 50                       push       eax
0052e51d 8d8d6cfeffff             lea        ecx, [ebp - 0x194]
0052e523 e848f6edff               call       0x40db70
0052e528 c645fc04                 mov        byte ptr [ebp - 4], 4
0052e52c 6a0a                     push       0xa
0052e52e 6a00                     push       0
0052e530 8d856cfeffff             lea        eax, [ebp - 0x194]
0052e536 50                       push       eax
0052e537 e874140000               call       0x52f9b0
0052e53c 83c40c                   add        esp, 0xc
0052e53f 898504ffffff             mov        dword ptr [ebp - 0xfc], eax
0052e545 c645fc03                 mov        byte ptr [ebp - 4], 3
0052e549 8d8d6cfeffff             lea        ecx, [ebp - 0x194]
0052e54f e84c00eeff               call       0x40e5a0
0052e554 c645fc02                 mov        byte ptr [ebp - 4], 2
0052e558 8d8d50feffff             lea        ecx, [ebp - 0x1b0]
0052e55e e8ddacf1ff               call       0x449240
0052e563 90                       nop        
0052e564 698d04ffffff04070000     imul       ecx, dword ptr [ebp - 0xfc], 0x704
0052e56e 030d5c685c00             add        ecx, dword ptr [0x5c685c]
0052e574 e817100000               call       0x52f590
0052e579 698d04ffffff04070000     imul       ecx, dword ptr [ebp - 0xfc], 0x704
0052e583 8b155c685c00             mov        edx, dword ptr [0x5c685c]
0052e589 8b8504ffffff             mov        eax, dword ptr [ebp - 0xfc]
0052e58f 89040a                   mov        dword ptr [edx + ecx], eax
0052e592 8d4dd4                   lea        ecx, [ebp - 0x2c]
0052e595 51                       push       ecx
0052e596 8d950cffffff             lea        edx, [ebp - 0xf4]
0052e59c 52                       push       edx
0052e59d e85e6ffeff               call       0x515500
0052e5a2 83c408                   add        esp, 8
0052e5a5 698504ffffff04070000     imul       eax, dword ptr [ebp - 0xfc], 0x704
0052e5af 8b0d5c685c00             mov        ecx, dword ptr [0x5c685c]
0052e5b5 8d540104                 lea        edx, [ecx + eax + 4]
0052e5b9 8995c8feffff             mov        dword ptr [ebp - 0x138], edx
0052e5bf 83ec1c                   sub        esp, 0x1c
0052e5c2 8bcc                     mov        ecx, esp
0052e5c4 89a594feffff             mov        dword ptr [ebp - 0x16c], esp
0052e5ca 8d45d4                   lea        eax, [ebp - 0x2c]
0052e5cd 50                       push       eax
0052e5ce e8adc1f3ff               call       0x46a780
0052e5d3 8b8dc8feffff             mov        ecx, dword ptr [ebp - 0x138]
0052e5d9 51                       push       ecx
0052e5da e8210b0000               call       0x52f100
0052e5df 83c420                   add        esp, 0x20
0052e5e2 c78500ffffff00000000     mov        dword ptr [ebp - 0x100], 0
0052e5ec eb0f                     jmp        0x52e5fd
0052e5ee 8b9500ffffff             mov        edx, dword ptr [ebp - 0x100]
0052e5f4 83c201                   add        edx, 1
0052e5f7 899500ffffff             mov        dword ptr [ebp - 0x100], edx
0052e5fd 83bd00ffffff02           cmp        dword ptr [ebp - 0x100], 2
0052e604 0f8db2000000             jge        0x52e6bc
0052e60a c785fcfeffff00000000     mov        dword ptr [ebp - 0x104], 0
0052e614 eb0f                     jmp        0x52e625
0052e616 8b85fcfeffff             mov        eax, dword ptr [ebp - 0x104]
0052e61c 83c001                   add        eax, 1
0052e61f 8985fcfeffff             mov        dword ptr [ebp - 0x104], eax
0052e625 83bdfcfeffff03           cmp        dword ptr [ebp - 0x104], 3
0052e62c 0f8d85000000             jge        0x52e6b7
0052e632 8d4dd4                   lea        ecx, [ebp - 0x2c]
0052e635 51                       push       ecx
0052e636 8d950cffffff             lea        edx, [ebp - 0xf4]
0052e63c 52                       push       edx
0052e63d e8be6efeff               call       0x515500
0052e642 83c408                   add        esp, 8
0052e645 68fc0a5700               push       0x570afc
0052e64a 8d45d4                   lea        eax, [ebp - 0x2c]
0052e64d 50                       push       eax
0052e64e e88d87f1ff               call       0x446de0
0052e653 83c408                   add        esp, 8
0052e656 0fb6c8                   movzx      ecx, al
0052e659 85c9                     test       ecx, ecx
0052e65b 75d5                     jne        0x52e632
0052e65d 699504ffffff04070000     imul       edx, dword ptr [ebp - 0xfc], 0x704
0052e667 03155c685c00             add        edx, dword ptr [0x5c685c]
0052e66d 698500ffffff00030000     imul       eax, dword ptr [ebp - 0x100], 0x300
0052e677 8d8c0204010000           lea        ecx, [edx + eax + 0x104]
0052e67e 8b95fcfeffff             mov        edx, dword ptr [ebp - 0x104]
0052e684 c1e208                   shl        edx, 8
0052e687 03ca                     add        ecx, edx
0052e689 898dc4feffff             mov        dword ptr [ebp - 0x13c], ecx
0052e68f 83ec1c                   sub        esp, 0x1c
0052e692 8bcc                     mov        ecx, esp
0052e694 89a590feffff             mov        dword ptr [ebp - 0x170], esp
0052e69a 8d45d4                   lea        eax, [ebp - 0x2c]
0052e69d 50                       push       eax
0052e69e e8ddc0f3ff               call       0x46a780
0052e6a3 8b8dc4feffff             mov        ecx, dword ptr [ebp - 0x13c]
0052e6a9 51                       push       ecx
0052e6aa e8510a0000               call       0x52f100
0052e6af 83c420                   add        esp, 0x20
0052e6b2 e95fffffff               jmp        0x52e616
0052e6b7 e932ffffff               jmp        0x52e5ee
0052e6bc e996fdffff               jmp        0x52e457
0052e6c1 83bdf8feffff00           cmp        dword ptr [ebp - 0x108], 0
0052e6c8 741c                     je         0x52e6e6
0052e6ca 8b95f8feffff             mov        edx, dword ptr [ebp - 0x108]
0052e6d0 52                       push       edx
0052e6d1 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0052e6d7 e8940fefff               call       0x41f670
0052e6dc c785f8feffff00000000     mov        dword ptr [ebp - 0x108], 0
0052e6e6 c645fc01                 mov        byte ptr [ebp - 4], 1
0052e6ea 8d4dd4                   lea        ecx, [ebp - 0x2c]
0052e6ed e84eabf1ff               call       0x449240
0052e6f2 c645fc00                 mov        byte ptr [ebp - 4], 0
0052e6f6 8d8d0cffffff             lea        ecx, [ebp - 0xf4]
0052e6fc e82f7efeff               call       0x516530
0052e701 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0052e708 8d4db8                   lea        ecx, [ebp - 0x48]
0052e70b e830abf1ff               call       0x449240
0052e710 90                       nop        
0052e711 33c0                     xor        eax, eax
0052e713 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052e716 64890d00000000           mov        dword ptr fs:[0], ecx
0052e71d 59                       pop        ecx
0052e71e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052e721 33cd                     xor        ecx, ebp
0052e723 e8c6420100               call       0x5429ee
0052e728 8be5                     mov        esp, ebp
0052e72a 5d                       pop        ebp
0052e72b c3                       ret        
0052e72c cc                       int3       
0052e72d cc                       int3       
0052e72e cc                       int3       
0052e72f cc                       int3       
