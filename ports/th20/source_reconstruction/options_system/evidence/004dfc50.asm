004dfc50 55                       push       ebp
004dfc51 8bec                     mov        ebp, esp
004dfc53 6aff                     push       -1
004dfc55 6800765600               push       0x567600
004dfc5a 64a100000000             mov        eax, dword ptr fs:[0]
004dfc60 50                       push       eax
004dfc61 51                       push       ecx
004dfc62 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004dfc67 33c5                     xor        eax, ebp
004dfc69 50                       push       eax
004dfc6a 8d45f4                   lea        eax, [ebp - 0xc]
004dfc6d 64a300000000             mov        dword ptr fs:[0], eax
004dfc73 894df0                   mov        dword ptr [ebp - 0x10], ecx
004dfc76 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dfc79 e8a200f4ff               call       0x41fd20
004dfc7e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004dfc81 c70090205700             mov        dword ptr [eax], 0x572090
004dfc87 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dfc8a c7411000000000           mov        dword ptr [ecx + 0x10], 0
004dfc91 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dfc94 83c114                   add        ecx, 0x14
004dfc97 e864f2fcff               call       0x4aef00
004dfc9c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dfc9f 83c160                   add        ecx, 0x60
004dfca2 e8e930f4ff               call       0x422d90
004dfca7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dfcaa 83c170                   add        ecx, 0x70
004dfcad e85e31f4ff               call       0x422e10
004dfcb2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dfcb5 83c17c                   add        ecx, 0x7c
004dfcb8 e8d330f4ff               call       0x422d90
004dfcbd 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dfcc0 81c18c000000             add        ecx, 0x8c
004dfcc6 e8c530f4ff               call       0x422d90
004dfccb 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004dfcce c7829c00000000000000     mov        dword ptr [edx + 0x9c], 0
004dfcd8 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004dfcdb c780a000000000000000     mov        dword ptr [eax + 0xa0], 0
004dfce5 689c205700               push       0x57209c
004dfcea e8c1c9f2ff               call       0x40c6b0
004dfcef 83c404                   add        esp, 4
004dfcf2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dfcf5 890db8605c00             mov        dword ptr [0x5c60b8], ecx
004dfcfb 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004dfcfe 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004dfd01 64890d00000000           mov        dword ptr fs:[0], ecx
004dfd08 59                       pop        ecx
004dfd09 8be5                     mov        esp, ebp
004dfd0b 5d                       pop        ebp
004dfd0c c3                       ret        
004dfd0d cc                       int3       
004dfd0e cc                       int3       
004dfd0f cc                       int3       
004dfd10 cc                       int3       
004dfd11 cc                       int3       
004dfd12 cc                       int3       
004dfd13 cc                       int3       
004dfd14 cc                       int3       
004dfd15 cc                       int3       
004dfd16 cc                       int3       
004dfd17 cc                       int3       
004dfd18 cc                       int3       
004dfd19 cc                       int3       
004dfd1a cc                       int3       
004dfd1b cc                       int3       
004dfd1c cc                       int3       
004dfd1d cc                       int3       
004dfd1e cc                       int3       
004dfd1f cc                       int3       
