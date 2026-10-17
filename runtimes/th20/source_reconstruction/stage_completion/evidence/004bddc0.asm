004bddc0 55                       push       ebp
004bddc1 8bec                     mov        ebp, esp
004bddc3 6aff                     push       -1
004bddc5 688d805600               push       0x56808d
004bddca 64a100000000             mov        eax, dword ptr fs:[0]
004bddd0 50                       push       eax
004bddd1 83ec14                   sub        esp, 0x14
004bddd4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004bddd9 33c5                     xor        eax, ebp
004bdddb 8945f0                   mov        dword ptr [ebp - 0x10], eax
004bddde 50                       push       eax
004bdddf 8d45f4                   lea        eax, [ebp - 0xc]
004bdde2 64a300000000             mov        dword ptr fs:[0], eax
004bdde8 894de8                   mov        dword ptr [ebp - 0x18], ecx
004bddeb 6a14                     push       0x14
004bdded b940025c00               mov        ecx, 0x5c0240
004bddf2 e819e5f4ff               call       0x40c310
004bddf7 50                       push       eax
004bddf8 8d4dec                   lea        ecx, [ebp - 0x14]
004bddfb e830d9f4ff               call       0x40b730
004bde00 c745fc00000000           mov        dword ptr [ebp - 4], 0
004bde07 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004bde0a 0584421200               add        eax, 0x124284
004bde0f 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004bde12 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004bde15 8b9180421200             mov        edx, dword ptr [ecx + 0x124280]
004bde1b 52                       push       edx
004bde1c 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004bde1f e85c6bfcff               call       0x484980
004bde24 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004bde27 8908                     mov        dword ptr [eax], ecx
004bde29 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004bde2c 8b8280421200             mov        eax, dword ptr [edx + 0x124280]
004bde32 83c001                   add        eax, 1
004bde35 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004bde38 898180421200             mov        dword ptr [ecx + 0x124280], eax
004bde3e 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004bde41 e89a62faff               call       0x4640e0
004bde46 05e8000000               add        eax, 0xe8
004bde4b 8945e0                   mov        dword ptr [ebp - 0x20], eax
004bde4e 8b5508                   mov        edx, dword ptr [ebp + 8]
004bde51 52                       push       edx
004bde52 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004bde55 e806f2ffff               call       0x4bd060
004bde5a c60001                   mov        byte ptr [eax], 1
004bde5d 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004bde60 e8db0c0000               call       0x4beb40
004bde65 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004bde6c 8d4dec                   lea        ecx, [ebp - 0x14]
004bde6f e88cdaf4ff               call       0x40b900
004bde74 90                       nop        
004bde75 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004bde78 64890d00000000           mov        dword ptr fs:[0], ecx
004bde7f 59                       pop        ecx
004bde80 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bde83 33cd                     xor        ecx, ebp
004bde85 e8644b0800               call       0x5429ee
004bde8a 8be5                     mov        esp, ebp
004bde8c 5d                       pop        ebp
004bde8d c20400                   ret        4
