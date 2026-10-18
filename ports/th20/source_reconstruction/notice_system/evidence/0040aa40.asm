0040aa40 55                       push       ebp
0040aa41 8bec                     mov        ebp, esp
0040aa43 6aff                     push       -1
0040aa45 682fa05600               push       0x56a02f
0040aa4a 64a100000000             mov        eax, dword ptr fs:[0]
0040aa50 50                       push       eax
0040aa51 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0040aa56 33c5                     xor        eax, ebp
0040aa58 50                       push       eax
0040aa59 8d45f4                   lea        eax, [ebp - 0xc]
0040aa5c 64a300000000             mov        dword ptr fs:[0], eax
0040aa62 68d41e5700               push       0x571ed4
0040aa67 b9405b5c00               mov        ecx, 0x5c5b40
0040aa6c e8bffd0500               call       0x46a830
0040aa71 c745fc00000000           mov        dword ptr [ebp - 4], 0
0040aa78 68d41e5700               push       0x571ed4
0040aa7d b95c5b5c00               mov        ecx, 0x5c5b5c
0040aa82 e8a9fd0500               call       0x46a830
0040aa87 c645fc01                 mov        byte ptr [ebp - 4], 1
0040aa8b 68e01e5700               push       0x571ee0
0040aa90 b9785b5c00               mov        ecx, 0x5c5b78
0040aa95 e896fd0500               call       0x46a830
0040aa9a c645fc02                 mov        byte ptr [ebp - 4], 2
0040aa9e 68f81e5700               push       0x571ef8
0040aaa3 b9945b5c00               mov        ecx, 0x5c5b94
0040aaa8 e883fd0500               call       0x46a830
0040aaad c645fc03                 mov        byte ptr [ebp - 4], 3
0040aab1 68e01e5700               push       0x571ee0
0040aab6 b9b05b5c00               mov        ecx, 0x5c5bb0
0040aabb e870fd0500               call       0x46a830
0040aac0 c645fc04                 mov        byte ptr [ebp - 4], 4
0040aac4 68f81e5700               push       0x571ef8
0040aac9 b9cc5b5c00               mov        ecx, 0x5c5bcc
0040aace e85dfd0500               call       0x46a830
0040aad3 c645fc05                 mov        byte ptr [ebp - 4], 5
0040aad7 68e01e5700               push       0x571ee0
0040aadc b9e85b5c00               mov        ecx, 0x5c5be8
0040aae1 e84afd0500               call       0x46a830
0040aae6 c645fc06                 mov        byte ptr [ebp - 4], 6
0040aaea 68f81e5700               push       0x571ef8
0040aaef b9045c5c00               mov        ecx, 0x5c5c04
0040aaf4 e837fd0500               call       0x46a830
0040aaf9 c645fc07                 mov        byte ptr [ebp - 4], 7
0040aafd 68e01e5700               push       0x571ee0
0040ab02 b9205c5c00               mov        ecx, 0x5c5c20
0040ab07 e824fd0500               call       0x46a830
0040ab0c c645fc08                 mov        byte ptr [ebp - 4], 8
0040ab10 68f81e5700               push       0x571ef8
0040ab15 b93c5c5c00               mov        ecx, 0x5c5c3c
0040ab1a e811fd0500               call       0x46a830
0040ab1f c645fc09                 mov        byte ptr [ebp - 4], 9
0040ab23 68e01e5700               push       0x571ee0
0040ab28 b9585c5c00               mov        ecx, 0x5c5c58
0040ab2d e8fefc0500               call       0x46a830
0040ab32 c645fc0a                 mov        byte ptr [ebp - 4], 0xa
0040ab36 68f81e5700               push       0x571ef8
0040ab3b b9745c5c00               mov        ecx, 0x5c5c74
0040ab40 e8ebfc0500               call       0x46a830
0040ab45 c645fc0b                 mov        byte ptr [ebp - 4], 0xb
0040ab49 68e01e5700               push       0x571ee0
0040ab4e b9905c5c00               mov        ecx, 0x5c5c90
0040ab53 e8d8fc0500               call       0x46a830
0040ab58 c645fc0c                 mov        byte ptr [ebp - 4], 0xc
0040ab5c 68f81e5700               push       0x571ef8
0040ab61 b9ac5c5c00               mov        ecx, 0x5c5cac
0040ab66 e8c5fc0500               call       0x46a830
0040ab6b c645fc0d                 mov        byte ptr [ebp - 4], 0xd
0040ab6f 68e01e5700               push       0x571ee0
0040ab74 b9c85c5c00               mov        ecx, 0x5c5cc8
0040ab79 e8b2fc0500               call       0x46a830
0040ab7e c645fc0e                 mov        byte ptr [ebp - 4], 0xe
0040ab82 68f81e5700               push       0x571ef8
0040ab87 b9e45c5c00               mov        ecx, 0x5c5ce4
0040ab8c e89ffc0500               call       0x46a830
0040ab91 c645fc0f                 mov        byte ptr [ebp - 4], 0xf
0040ab95 68e01e5700               push       0x571ee0
0040ab9a b9005d5c00               mov        ecx, 0x5c5d00
0040ab9f e88cfc0500               call       0x46a830
0040aba4 c645fc10                 mov        byte ptr [ebp - 4], 0x10
0040aba8 68f81e5700               push       0x571ef8
0040abad b91c5d5c00               mov        ecx, 0x5c5d1c
0040abb2 e879fc0500               call       0x46a830
0040abb7 c645fc11                 mov        byte ptr [ebp - 4], 0x11
0040abbb 681c1f5700               push       0x571f1c
0040abc0 b9385d5c00               mov        ecx, 0x5c5d38
0040abc5 e866fc0500               call       0x46a830
0040abca c645fc12                 mov        byte ptr [ebp - 4], 0x12
0040abce 68281f5700               push       0x571f28
0040abd3 b9545d5c00               mov        ecx, 0x5c5d54
0040abd8 e853fc0500               call       0x46a830
0040abdd c645fc13                 mov        byte ptr [ebp - 4], 0x13
0040abe1 681c1f5700               push       0x571f1c
0040abe6 b9705d5c00               mov        ecx, 0x5c5d70
0040abeb e840fc0500               call       0x46a830
0040abf0 c645fc14                 mov        byte ptr [ebp - 4], 0x14
0040abf4 68281f5700               push       0x571f28
0040abf9 b98c5d5c00               mov        ecx, 0x5c5d8c
0040abfe e82dfc0500               call       0x46a830
0040ac03 c645fc15                 mov        byte ptr [ebp - 4], 0x15
0040ac07 681c1f5700               push       0x571f1c
0040ac0c b9a85d5c00               mov        ecx, 0x5c5da8
0040ac11 e81afc0500               call       0x46a830
0040ac16 c645fc16                 mov        byte ptr [ebp - 4], 0x16
0040ac1a 68281f5700               push       0x571f28
0040ac1f b9c45d5c00               mov        ecx, 0x5c5dc4
0040ac24 e807fc0500               call       0x46a830
0040ac29 c645fc17                 mov        byte ptr [ebp - 4], 0x17
0040ac2d 681c1f5700               push       0x571f1c
0040ac32 b9e05d5c00               mov        ecx, 0x5c5de0
0040ac37 e8f4fb0500               call       0x46a830
0040ac3c c645fc18                 mov        byte ptr [ebp - 4], 0x18
0040ac40 68281f5700               push       0x571f28
0040ac45 b9fc5d5c00               mov        ecx, 0x5c5dfc
0040ac4a e8e1fb0500               call       0x46a830
0040ac4f c645fc19                 mov        byte ptr [ebp - 4], 0x19
0040ac53 681c1f5700               push       0x571f1c
0040ac58 b9185e5c00               mov        ecx, 0x5c5e18
0040ac5d e8cefb0500               call       0x46a830
0040ac62 c645fc1a                 mov        byte ptr [ebp - 4], 0x1a
0040ac66 68281f5700               push       0x571f28
0040ac6b b9345e5c00               mov        ecx, 0x5c5e34
0040ac70 e8bbfb0500               call       0x46a830
0040ac75 c645fc1b                 mov        byte ptr [ebp - 4], 0x1b
0040ac79 681c1f5700               push       0x571f1c
0040ac7e b9505e5c00               mov        ecx, 0x5c5e50
0040ac83 e8a8fb0500               call       0x46a830
0040ac88 c645fc1c                 mov        byte ptr [ebp - 4], 0x1c
0040ac8c 68281f5700               push       0x571f28
0040ac91 b96c5e5c00               mov        ecx, 0x5c5e6c
0040ac96 e895fb0500               call       0x46a830
0040ac9b c645fc1d                 mov        byte ptr [ebp - 4], 0x1d
0040ac9f 681c1f5700               push       0x571f1c
0040aca4 b9885e5c00               mov        ecx, 0x5c5e88
0040aca9 e882fb0500               call       0x46a830
0040acae c645fc1e                 mov        byte ptr [ebp - 4], 0x1e
0040acb2 68281f5700               push       0x571f28
0040acb7 b9a45e5c00               mov        ecx, 0x5c5ea4
0040acbc e86ffb0500               call       0x46a830
0040acc1 c645fc1f                 mov        byte ptr [ebp - 4], 0x1f
0040acc5 681c1f5700               push       0x571f1c
0040acca b9c05e5c00               mov        ecx, 0x5c5ec0
0040accf e85cfb0500               call       0x46a830
0040acd4 c645fc20                 mov        byte ptr [ebp - 4], 0x20
0040acd8 68281f5700               push       0x571f28
0040acdd b9dc5e5c00               mov        ecx, 0x5c5edc
0040ace2 e849fb0500               call       0x46a830
0040ace7 c645fc21                 mov        byte ptr [ebp - 4], 0x21
0040aceb 68401f5700               push       0x571f40
0040acf0 b9f85e5c00               mov        ecx, 0x5c5ef8
0040acf5 e836fb0500               call       0x46a830
0040acfa c645fc22                 mov        byte ptr [ebp - 4], 0x22
0040acfe 68281f5700               push       0x571f28
0040ad03 b9145f5c00               mov        ecx, 0x5c5f14
0040ad08 e823fb0500               call       0x46a830
0040ad0d c645fc23                 mov        byte ptr [ebp - 4], 0x23
0040ad11 68401f5700               push       0x571f40
0040ad16 b9305f5c00               mov        ecx, 0x5c5f30
0040ad1b e810fb0500               call       0x46a830
0040ad20 c645fc24                 mov        byte ptr [ebp - 4], 0x24
0040ad24 68281f5700               push       0x571f28
0040ad29 b94c5f5c00               mov        ecx, 0x5c5f4c
0040ad2e e8fdfa0500               call       0x46a830
0040ad33 c645fc25                 mov        byte ptr [ebp - 4], 0x25
0040ad37 68401f5700               push       0x571f40
0040ad3c b9685f5c00               mov        ecx, 0x5c5f68
0040ad41 e8eafa0500               call       0x46a830
0040ad46 c645fc26                 mov        byte ptr [ebp - 4], 0x26
0040ad4a 68281f5700               push       0x571f28
0040ad4f b9845f5c00               mov        ecx, 0x5c5f84
0040ad54 e8d7fa0500               call       0x46a830
0040ad59 c645fc27                 mov        byte ptr [ebp - 4], 0x27
0040ad5d 68401f5700               push       0x571f40
0040ad62 b9a05f5c00               mov        ecx, 0x5c5fa0
0040ad67 e8c4fa0500               call       0x46a830
0040ad6c c645fc28                 mov        byte ptr [ebp - 4], 0x28
0040ad70 68281f5700               push       0x571f28
0040ad75 b9bc5f5c00               mov        ecx, 0x5c5fbc
0040ad7a e8b1fa0500               call       0x46a830
0040ad7f c645fc29                 mov        byte ptr [ebp - 4], 0x29
0040ad83 68401f5700               push       0x571f40
0040ad88 b9d85f5c00               mov        ecx, 0x5c5fd8
0040ad8d e89efa0500               call       0x46a830
0040ad92 c645fc2a                 mov        byte ptr [ebp - 4], 0x2a
0040ad96 68281f5700               push       0x571f28
0040ad9b b9f45f5c00               mov        ecx, 0x5c5ff4
0040ada0 e88bfa0500               call       0x46a830
0040ada5 c645fc2b                 mov        byte ptr [ebp - 4], 0x2b
0040ada9 68401f5700               push       0x571f40
0040adae b910605c00               mov        ecx, 0x5c6010
0040adb3 e878fa0500               call       0x46a830
0040adb8 c645fc2c                 mov        byte ptr [ebp - 4], 0x2c
0040adbc 68281f5700               push       0x571f28
0040adc1 b92c605c00               mov        ecx, 0x5c602c
0040adc6 e865fa0500               call       0x46a830
0040adcb c645fc2d                 mov        byte ptr [ebp - 4], 0x2d
0040adcf 68401f5700               push       0x571f40
0040add4 b948605c00               mov        ecx, 0x5c6048
0040add9 e852fa0500               call       0x46a830
0040adde c645fc2e                 mov        byte ptr [ebp - 4], 0x2e
0040ade2 68281f5700               push       0x571f28
0040ade7 b964605c00               mov        ecx, 0x5c6064
0040adec e83ffa0500               call       0x46a830
0040adf1 c645fc2f                 mov        byte ptr [ebp - 4], 0x2f
0040adf5 68401f5700               push       0x571f40
0040adfa b980605c00               mov        ecx, 0x5c6080
0040adff e82cfa0500               call       0x46a830
0040ae04 c645fc30                 mov        byte ptr [ebp - 4], 0x30
0040ae08 68281f5700               push       0x571f28
0040ae0d b99c605c00               mov        ecx, 0x5c609c
0040ae12 e819fa0500               call       0x46a830
0040ae17 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0040ae1e 6800b45600               push       0x56b400
0040ae23 e8b17b1300               call       0x5429d9
0040ae28 83c404                   add        esp, 4
0040ae2b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0040ae2e 64890d00000000           mov        dword ptr fs:[0], ecx
0040ae35 59                       pop        ecx
0040ae36 8be5                     mov        esp, ebp
0040ae38 5d                       pop        ebp
0040ae39 c3                       ret        
0040ae3a cc                       int3       
0040ae3b cc                       int3       
0040ae3c cc                       int3       
0040ae3d cc                       int3       
0040ae3e cc                       int3       
0040ae3f cc                       int3       
