004bd610 55                       push       ebp
004bd611 8bec                     mov        ebp, esp
004bd613 6aff                     push       -1
004bd615 688d805600               push       0x56808d
004bd61a 64a100000000             mov        eax, dword ptr fs:[0]
004bd620 50                       push       eax
004bd621 83ec14                   sub        esp, 0x14
004bd624 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004bd629 33c5                     xor        eax, ebp
004bd62b 8945f0                   mov        dword ptr [ebp - 0x10], eax
004bd62e 50                       push       eax
004bd62f 8d45f4                   lea        eax, [ebp - 0xc]
004bd632 64a300000000             mov        dword ptr fs:[0], eax
004bd638 894de8                   mov        dword ptr [ebp - 0x18], ecx
004bd63b 6a14                     push       0x14
004bd63d b940025c00               mov        ecx, 0x5c0240
004bd642 e8c9ecf4ff               call       0x40c310
004bd647 50                       push       eax
004bd648 8d4dec                   lea        ecx, [ebp - 0x14]
004bd64b e8e0e0f4ff               call       0x40b730
004bd650 c745fc00000000           mov        dword ptr [ebp - 4], 0
004bd657 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004bd65a e85169faff               call       0x463fb0
004bd65f 90                       nop        
004bd660 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004bd663 e8786afaff               call       0x4640e0
004bd668 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004bd66b 8b4508                   mov        eax, dword ptr [ebp + 8]
004bd66e 50                       push       eax
004bd66f 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004bd672 e839000000               call       0x4bd6b0
004bd677 8945e0                   mov        dword ptr [ebp - 0x20], eax
004bd67a c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004bd681 8d4dec                   lea        ecx, [ebp - 0x14]
004bd684 e877e2f4ff               call       0x40b900
004bd689 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004bd68c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004bd68f 64890d00000000           mov        dword ptr fs:[0], ecx
004bd696 59                       pop        ecx
004bd697 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bd69a 33cd                     xor        ecx, ebp
004bd69c e84d530800               call       0x5429ee
004bd6a1 8be5                     mov        esp, ebp
004bd6a3 5d                       pop        ebp
004bd6a4 c20400                   ret        4
004bd6a7 cc                       int3       
004bd6a8 cc                       int3       
004bd6a9 cc                       int3       
004bd6aa cc                       int3       
004bd6ab cc                       int3       
004bd6ac cc                       int3       
004bd6ad cc                       int3       
004bd6ae cc                       int3       
004bd6af cc                       int3       
