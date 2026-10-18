0051f890 55                       push       ebp
0051f891 8bec                     mov        ebp, esp
0051f893 6aff                     push       -1
0051f895 6860af5600               push       0x56af60
0051f89a 64a100000000             mov        eax, dword ptr fs:[0]
0051f8a0 50                       push       eax
0051f8a1 81ec80010000             sub        esp, 0x180
0051f8a7 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0051f8ac 33c5                     xor        eax, ebp
0051f8ae 8945f0                   mov        dword ptr [ebp - 0x10], eax
0051f8b1 50                       push       eax
0051f8b2 8d45f4                   lea        eax, [ebp - 0xc]
0051f8b5 64a300000000             mov        dword ptr fs:[0], eax
0051f8bb 898d0cffffff             mov        dword ptr [ebp - 0xf4], ecx
0051f8c1 6a00                     push       0
0051f8c3 6a00                     push       0
0051f8c5 68e0535700               push       0x5753e0
0051f8ca e8d111efff               call       0x410aa0
0051f8cf 83c40c                   add        esp, 0xc
0051f8d2 8985f8feffff             mov        dword ptr [ebp - 0x108], eax
0051f8d8 6a1c                     push       0x1c
0051f8da 8d4db8                   lea        ecx, [ebp - 0x48]
0051f8dd e89ec7eeff               call       0x40c080
0051f8e2 8b85f8feffff             mov        eax, dword ptr [ebp - 0x108]
0051f8e8 50                       push       eax
0051f8e9 8d4db8                   lea        ecx, [ebp - 0x48]
0051f8ec e83faff4ff               call       0x46a830
0051f8f1 c745fc00000000           mov        dword ptr [ebp - 4], 0
0051f8f8 8d8db4feffff             lea        ecx, [ebp - 0x14c]
0051f8fe 51                       push       ecx
0051f8ff 8d4db8                   lea        ecx, [ebp - 0x48]
0051f902 e8f9bbffff               call       0x51b500
0051f907 8b10                     mov        edx, dword ptr [eax]
0051f909 8995f0feffff             mov        dword ptr [ebp - 0x110], edx
0051f90f 8b85f0feffff             mov        eax, dword ptr [ebp - 0x110]
0051f915 8985e0feffff             mov        dword ptr [ebp - 0x120], eax
0051f91b c68503ffffff0d           mov        byte ptr [ebp - 0xfd], 0xd
0051f922 8d8db0feffff             lea        ecx, [ebp - 0x150]
0051f928 51                       push       ecx
0051f929 8d4db8                   lea        ecx, [ebp - 0x48]
0051f92c e8cfbbffff               call       0x51b500
0051f931 8b10                     mov        edx, dword ptr [eax]
0051f933 8995ecfeffff             mov        dword ptr [ebp - 0x114], edx
0051f939 8d85acfeffff             lea        eax, [ebp - 0x154]
0051f93f 50                       push       eax
0051f940 8d4db8                   lea        ecx, [ebp - 0x48]
0051f943 e8b8f3fdff               call       0x4fed00
0051f948 8b08                     mov        ecx, dword ptr [eax]
0051f94a 898de8feffff             mov        dword ptr [ebp - 0x118], ecx
0051f950 8d9503ffffff             lea        edx, [ebp - 0xfd]
0051f956 52                       push       edx
0051f957 8b85ecfeffff             mov        eax, dword ptr [ebp - 0x114]
0051f95d 50                       push       eax
0051f95e 8b8de8feffff             mov        ecx, dword ptr [ebp - 0x118]
0051f964 51                       push       ecx
0051f965 8d95a8feffff             lea        edx, [ebp - 0x158]
0051f96b 52                       push       edx
0051f96c e8df5bffff               call       0x515550
0051f971 83c410                   add        esp, 0x10
0051f974 8b00                     mov        eax, dword ptr [eax]
0051f976 8985e4feffff             mov        dword ptr [ebp - 0x11c], eax
0051f97c 8b8de4feffff             mov        ecx, dword ptr [ebp - 0x11c]
0051f982 898ddcfeffff             mov        dword ptr [ebp - 0x124], ecx
0051f988 8b95e0feffff             mov        edx, dword ptr [ebp - 0x120]
0051f98e 52                       push       edx
0051f98f 8b85dcfeffff             mov        eax, dword ptr [ebp - 0x124]
0051f995 50                       push       eax
0051f996 8d8da4feffff             lea        ecx, [ebp - 0x15c]
0051f99c 51                       push       ecx
0051f99d 8d4db8                   lea        ecx, [ebp - 0x48]
0051f9a0 e83bbcffff               call       0x51b5e0
0051f9a5 90                       nop        
0051f9a6 8d95a0feffff             lea        edx, [ebp - 0x160]
0051f9ac 52                       push       edx
0051f9ad 8d4db8                   lea        ecx, [ebp - 0x48]
0051f9b0 e84bbbffff               call       0x51b500
0051f9b5 8b00                     mov        eax, dword ptr [eax]
0051f9b7 8985d8feffff             mov        dword ptr [ebp - 0x128], eax
0051f9bd 8b8dd8feffff             mov        ecx, dword ptr [ebp - 0x128]
0051f9c3 898dc0feffff             mov        dword ptr [ebp - 0x140], ecx
0051f9c9 c6850bffffff0c           mov        byte ptr [ebp - 0xf5], 0xc
0051f9d0 8d959cfeffff             lea        edx, [ebp - 0x164]
0051f9d6 52                       push       edx
0051f9d7 8d4db8                   lea        ecx, [ebp - 0x48]
0051f9da e821bbffff               call       0x51b500
0051f9df 8b00                     mov        eax, dword ptr [eax]
0051f9e1 8985d4feffff             mov        dword ptr [ebp - 0x12c], eax
0051f9e7 8d8d98feffff             lea        ecx, [ebp - 0x168]
0051f9ed 51                       push       ecx
0051f9ee 8d4db8                   lea        ecx, [ebp - 0x48]
0051f9f1 e80af3fdff               call       0x4fed00
0051f9f6 8b10                     mov        edx, dword ptr [eax]
0051f9f8 8995d0feffff             mov        dword ptr [ebp - 0x130], edx
0051f9fe 8d850bffffff             lea        eax, [ebp - 0xf5]
0051fa04 50                       push       eax
0051fa05 8b8dd4feffff             mov        ecx, dword ptr [ebp - 0x12c]
0051fa0b 51                       push       ecx
0051fa0c 8b95d0feffff             mov        edx, dword ptr [ebp - 0x130]
0051fa12 52                       push       edx
0051fa13 8d8594feffff             lea        eax, [ebp - 0x16c]
0051fa19 50                       push       eax
0051fa1a e8315bffff               call       0x515550
0051fa1f 83c410                   add        esp, 0x10
0051fa22 8b08                     mov        ecx, dword ptr [eax]
0051fa24 898dbcfeffff             mov        dword ptr [ebp - 0x144], ecx
0051fa2a 8b95bcfeffff             mov        edx, dword ptr [ebp - 0x144]
0051fa30 8995b8feffff             mov        dword ptr [ebp - 0x148], edx
0051fa36 8b85c0feffff             mov        eax, dword ptr [ebp - 0x140]
0051fa3c 50                       push       eax
0051fa3d 8b8db8feffff             mov        ecx, dword ptr [ebp - 0x148]
0051fa43 51                       push       ecx
0051fa44 8d9590feffff             lea        edx, [ebp - 0x170]
0051fa4a 52                       push       edx
0051fa4b 8d4db8                   lea        ecx, [ebp - 0x48]
0051fa4e e88dbbffff               call       0x51b5e0
0051fa53 90                       nop        
0051fa54 68a8000000               push       0xa8
0051fa59 8d8d10ffffff             lea        ecx, [ebp - 0xf0]
0051fa5f e81cc6eeff               call       0x40c080
0051fa64 6a01                     push       1
0051fa66 6a01                     push       1
0051fa68 8d45b8                   lea        eax, [ebp - 0x48]
0051fa6b 50                       push       eax
0051fa6c 8d8d10ffffff             lea        ecx, [ebp - 0xf0]
0051fa72 e83948ffff               call       0x5142b0
0051fa77 c645fc01                 mov        byte ptr [ebp - 4], 1
0051fa7b 6a1c                     push       0x1c
0051fa7d 8d4dd4                   lea        ecx, [ebp - 0x2c]
0051fa80 e8fbc5eeff               call       0x40c080
0051fa85 8d4dd4                   lea        ecx, [ebp - 0x2c]
0051fa88 e83382f2ff               call       0x447cc0
0051fa8d c645fc02                 mov        byte ptr [ebp - 4], 2
0051fa91 c78504ffffff00000000     mov        dword ptr [ebp - 0xfc], 0
0051fa9b 8d4dd4                   lea        ecx, [ebp - 0x2c]
0051fa9e 51                       push       ecx
0051fa9f 8d9510ffffff             lea        edx, [ebp - 0xf0]
0051faa5 52                       push       edx
0051faa6 e8555affff               call       0x515500
0051faab 83c408                   add        esp, 8
0051faae 8985f4feffff             mov        dword ptr [ebp - 0x10c], eax
0051fab4 8b85f4feffff             mov        eax, dword ptr [ebp - 0x10c]
0051faba 8b08                     mov        ecx, dword ptr [eax]
0051fabc 8b95f4feffff             mov        edx, dword ptr [ebp - 0x10c]
0051fac2 035104                   add        edx, dword ptr [ecx + 4]
0051fac5 8bca                     mov        ecx, edx
0051fac7 e85469ffff               call       0x516420
0051facc 0fb6c0                   movzx      eax, al
0051facf 85c0                     test       eax, eax
0051fad1 0f8465010000             je         0x51fc3c
0051fad7 8d4dd4                   lea        ecx, [ebp - 0x2c]
0051fada e8b1cfefff               call       0x41ca90
0051fadf 85c0                     test       eax, eax
0051fae1 7412                     je         0x51faf5
0051fae3 6a00                     push       0
0051fae5 8d4dd4                   lea        ecx, [ebp - 0x2c]
0051fae8 e8d3f6f7ff               call       0x49f1c0
0051faed 0fbe08                   movsx      ecx, byte ptr [eax]
0051faf0 83f923                   cmp        ecx, 0x23
0051faf3 7507                     jne        0x51fafc
0051faf5 eba4                     jmp        0x51fa9b
0051faf7 e93b010000               jmp        0x51fc37
0051fafc 6a00                     push       0
0051fafe 8d4dd4                   lea        ecx, [ebp - 0x2c]
0051fb01 e8baf6f7ff               call       0x49f1c0
0051fb06 0fbe10                   movsx      edx, byte ptr [eax]
0051fb09 83fa5c                   cmp        edx, 0x5c
0051fb0c 750a                     jne        0x51fb18
0051fb0e e929010000               jmp        0x51fc3c
0051fb13 e91f010000               jmp        0x51fc37
0051fb18 6a00                     push       0
0051fb1a 8d4dd4                   lea        ecx, [ebp - 0x2c]
0051fb1d e89ef6f7ff               call       0x49f1c0
0051fb22 0fbe00                   movsx      eax, byte ptr [eax]
0051fb25 83f840                   cmp        eax, 0x40
0051fb28 0f8509010000             jne        0x51fc37
0051fb2e 6aff                     push       -1
0051fb30 6a01                     push       1
0051fb32 8d8d74feffff             lea        ecx, [ebp - 0x18c]
0051fb38 51                       push       ecx
0051fb39 8d4dd4                   lea        ecx, [ebp - 0x2c]
0051fb3c e83fd1ffff               call       0x51cc80
0051fb41 8985ccfeffff             mov        dword ptr [ebp - 0x134], eax
0051fb47 8b8dccfeffff             mov        ecx, dword ptr [ebp - 0x134]
0051fb4d e89ebff2ff               call       0x44baf0
0051fb52 50                       push       eax
0051fb53 6a40                     push       0x40
0051fb55 8b9504ffffff             mov        edx, dword ptr [ebp - 0xfc]
0051fb5b c1e206                   shl        edx, 6
0051fb5e 8b850cffffff             mov        eax, dword ptr [ebp - 0xf4]
0051fb64 8d8c1090040000           lea        ecx, [eax + edx + 0x490]
0051fb6b 51                       push       ecx
0051fb6c e89f8a0200               call       0x548610
0051fb71 83c40c                   add        esp, 0xc
0051fb74 8d8d74feffff             lea        ecx, [ebp - 0x18c]
0051fb7a e8c196f2ff               call       0x449240
0051fb7f 90                       nop        
0051fb80 8d55d4                   lea        edx, [ebp - 0x2c]
0051fb83 52                       push       edx
0051fb84 8d8510ffffff             lea        eax, [ebp - 0xf0]
0051fb8a 50                       push       eax
0051fb8b e87059ffff               call       0x515500
0051fb90 83c408                   add        esp, 8
0051fb93 8d4dd4                   lea        ecx, [ebp - 0x2c]
0051fb96 e855bff2ff               call       0x44baf0
0051fb9b 50                       push       eax
0051fb9c 6a42                     push       0x42
0051fb9e 6b8d04ffffff42           imul       ecx, dword ptr [ebp - 0xfc], 0x42
0051fba5 8b950cffffff             mov        edx, dword ptr [ebp - 0xf4]
0051fbab 8d840a900c0000           lea        eax, [edx + ecx + 0xc90]
0051fbb2 50                       push       eax
0051fbb3 e8588a0200               call       0x548610
0051fbb8 83c40c                   add        esp, 0xc
0051fbbb c785fcfeffff00000000     mov        dword ptr [ebp - 0x104], 0
0051fbc5 eb0f                     jmp        0x51fbd6
0051fbc7 8b8dfcfeffff             mov        ecx, dword ptr [ebp - 0x104]
0051fbcd 83c101                   add        ecx, 1
0051fbd0 898dfcfeffff             mov        dword ptr [ebp - 0x104], ecx
0051fbd6 83bdfcfeffff08           cmp        dword ptr [ebp - 0x104], 8
0051fbdd 7d49                     jge        0x51fc28
0051fbdf 8d55d4                   lea        edx, [ebp - 0x2c]
0051fbe2 52                       push       edx
0051fbe3 8d8510ffffff             lea        eax, [ebp - 0xf0]
0051fbe9 50                       push       eax
0051fbea e81159ffff               call       0x515500
0051fbef 83c408                   add        esp, 8
0051fbf2 8d4dd4                   lea        ecx, [ebp - 0x2c]
0051fbf5 e8f6bef2ff               call       0x44baf0
0051fbfa 50                       push       eax
0051fbfb 6a42                     push       0x42
0051fbfd 698d04ffffff10020000     imul       ecx, dword ptr [ebp - 0xfc], 0x210
0051fc07 8b950cffffff             mov        edx, dword ptr [ebp - 0xf4]
0051fc0d 8d840ad0140000           lea        eax, [edx + ecx + 0x14d0]
0051fc14 6b8dfcfeffff42           imul       ecx, dword ptr [ebp - 0x104], 0x42
0051fc1b 03c1                     add        eax, ecx
0051fc1d 50                       push       eax
0051fc1e e8ed890200               call       0x548610
0051fc23 83c40c                   add        esp, 0xc
0051fc26 eb9f                     jmp        0x51fbc7
0051fc28 8b9504ffffff             mov        edx, dword ptr [ebp - 0xfc]
0051fc2e 83c201                   add        edx, 1
0051fc31 899504ffffff             mov        dword ptr [ebp - 0xfc], edx
0051fc37 e95ffeffff               jmp        0x51fa9b
0051fc3c 83bdf8feffff00           cmp        dword ptr [ebp - 0x108], 0
0051fc43 741c                     je         0x51fc61
0051fc45 8b85f8feffff             mov        eax, dword ptr [ebp - 0x108]
0051fc4b 50                       push       eax
0051fc4c 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0051fc52 e819faefff               call       0x41f670
0051fc57 c785f8feffff00000000     mov        dword ptr [ebp - 0x108], 0
0051fc61 8b8d0cffffff             mov        ecx, dword ptr [ebp - 0xf4]
0051fc67 83c124                   add        ecx, 0x24
0051fc6a 898dc8feffff             mov        dword ptr [ebp - 0x138], ecx
0051fc70 8b9504ffffff             mov        edx, dword ptr [ebp - 0xfc]
0051fc76 52                       push       edx
0051fc77 8b8dc8feffff             mov        ecx, dword ptr [ebp - 0x138]
0051fc7d e85ec8f7ff               call       0x49c4e0
0051fc82 6a00                     push       0
0051fc84 8b8d0cffffff             mov        ecx, dword ptr [ebp - 0xf4]
0051fc8a 83c124                   add        ecx, 0x24
0051fc8d e8bef0f9ff               call       0x4bed50
0051fc92 8b850cffffff             mov        eax, dword ptr [ebp - 0xf4]
0051fc98 c780d056000000000000     mov        dword ptr [eax + 0x56d0], 0
0051fca2 8b8d0cffffff             mov        ecx, dword ptr [ebp - 0xf4]
0051fca8 8b9504ffffff             mov        edx, dword ptr [ebp - 0xfc]
0051fcae 89917c040000             mov        dword ptr [ecx + 0x47c], edx
0051fcb4 6a01                     push       1
0051fcb6 8b8d0cffffff             mov        ecx, dword ptr [ebp - 0xf4]
0051fcbc 81c178040000             add        ecx, 0x478
0051fcc2 e8d9bdeeff               call       0x40baa0
0051fcc7 90                       nop        
0051fcc8 c785c4feffff00000000     mov        dword ptr [ebp - 0x13c], 0
0051fcd2 c645fc01                 mov        byte ptr [ebp - 4], 1
0051fcd6 8d4dd4                   lea        ecx, [ebp - 0x2c]
0051fcd9 e86295f2ff               call       0x449240
0051fcde c645fc00                 mov        byte ptr [ebp - 4], 0
0051fce2 8d8d10ffffff             lea        ecx, [ebp - 0xf0]
0051fce8 e84368ffff               call       0x516530
0051fced c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0051fcf4 8d4db8                   lea        ecx, [ebp - 0x48]
0051fcf7 e84495f2ff               call       0x449240
0051fcfc 8b85c4feffff             mov        eax, dword ptr [ebp - 0x13c]
0051fd02 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0051fd05 64890d00000000           mov        dword ptr fs:[0], ecx
0051fd0c 59                       pop        ecx
0051fd0d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051fd10 33cd                     xor        ecx, ebp
0051fd12 e8d72c0200               call       0x5429ee
0051fd17 8be5                     mov        esp, ebp
0051fd19 5d                       pop        ebp
0051fd1a c20400                   ret        4
0051fd1d cc                       int3       
0051fd1e cc                       int3       
0051fd1f cc                       int3       
