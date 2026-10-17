004dea60 55                       push       ebp
004dea61 8bec                     mov        ebp, esp
004dea63 6aff                     push       -1
004dea65 6800765600               push       0x567600
004dea6a 64a100000000             mov        eax, dword ptr fs:[0]
004dea70 50                       push       eax
004dea71 51                       push       ecx
004dea72 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004dea77 33c5                     xor        eax, ebp
004dea79 50                       push       eax
004dea7a 8d45f4                   lea        eax, [ebp - 0xc]
004dea7d 64a300000000             mov        dword ptr fs:[0], eax
004dea83 894df0                   mov        dword ptr [ebp - 0x10], ecx
004dea86 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dea89 e89212f4ff               call       0x41fd20
004dea8e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004dea91 c700a81e5700             mov        dword ptr [eax], 0x571ea8
004dea97 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dea9a c7411000000000           mov        dword ptr [ecx + 0x10], 0
004deaa1 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004deaa4 83c114                   add        ecx, 0x14
004deaa7 e8e442f4ff               call       0x422d90
004deaac 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004deaaf 83c124                   add        ecx, 0x24
004deab2 e84904fdff               call       0x4aef00
004deab7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004deaba 83c170                   add        ecx, 0x70
004deabd e82e03fdff               call       0x4aedf0
004deac2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004deac5 81c18c000000             add        ecx, 0x8c
004deacb e8f071f4ff               call       0x425cc0
004dead0 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004dead3 c7829000000000000000     mov        dword ptr [edx + 0x90], 0
004deadd 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004deae0 0f57c0                   xorps      xmm0, xmm0
004deae3 f30f118094000000         movss      dword ptr [eax + 0x94], xmm0
004deaeb 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004deaee c7819800000000000000     mov        dword ptr [ecx + 0x98], 0
004deaf8 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004deafb c7829c00000000000000     mov        dword ptr [edx + 0x9c], 0
004deb05 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004deb08 c780a000000000000000     mov        dword ptr [eax + 0xa0], 0
004deb12 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004deb15 c781a400000000000000     mov        dword ptr [ecx + 0xa4], 0
004deb1f 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004deb22 c782a800000000000000     mov        dword ptr [edx + 0xa8], 0
004deb2c 6880000000               push       0x80
004deb31 6a00                     push       0
004deb33 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004deb36 05ac000000               add        eax, 0xac
004deb3b 50                       push       eax
004deb3c e81f5c0600               call       0x544760
004deb41 83c40c                   add        esp, 0xc
004deb44 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004deb47 c7812c01000000000000     mov        dword ptr [ecx + 0x12c], 0
004deb51 68b41e5700               push       0x571eb4
004deb56 e855dbf2ff               call       0x40c6b0
004deb5b 83c404                   add        esp, 4
004deb5e 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004deb61 8915385b5c00             mov        dword ptr [0x5c5b38], edx
004deb67 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004deb6a 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004deb6d 64890d00000000           mov        dword ptr fs:[0], ecx
004deb74 59                       pop        ecx
004deb75 8be5                     mov        esp, ebp
004deb77 5d                       pop        ebp
004deb78 c3                       ret        
004deb79 cc                       int3       
004deb7a cc                       int3       
004deb7b cc                       int3       
004deb7c cc                       int3       
004deb7d cc                       int3       
004deb7e cc                       int3       
004deb7f cc                       int3       
