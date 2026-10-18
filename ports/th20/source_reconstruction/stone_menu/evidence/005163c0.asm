005163c0 55                       push       ebp
005163c1 8bec                     mov        ebp, esp
005163c3 83ec08                   sub        esp, 8
005163c6 894df8                   mov        dword ptr [ebp - 8], ecx
005163c9 8b45f8                   mov        eax, dword ptr [ebp - 8]
005163cc 8b08                     mov        ecx, dword ptr [eax]
005163ce 3b4d08                   cmp        ecx, dword ptr [ebp + 8]
005163d1 7409                     je         0x5163dc
005163d3 c745fc01000000           mov        dword ptr [ebp - 4], 1
005163da eb07                     jmp        0x5163e3
005163dc c745fc00000000           mov        dword ptr [ebp - 4], 0
005163e3 8b45fc                   mov        eax, dword ptr [ebp - 4]
005163e6 8be5                     mov        esp, ebp
005163e8 5d                       pop        ebp
005163e9 c20400                   ret        4
005163ec cc                       int3       
005163ed cc                       int3       
005163ee cc                       int3       
005163ef cc                       int3       
