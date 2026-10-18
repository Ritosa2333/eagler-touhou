00449d80 55                       push       ebp
00449d81 8bec                     mov        ebp, esp
00449d83 83ec44                   sub        esp, 0x44
00449d86 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00449d8b 33c5                     xor        eax, ebp
00449d8d 8945fc                   mov        dword ptr [ebp - 4], eax
00449d90 894dcc                   mov        dword ptr [ebp - 0x34], ecx
00449d93 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
00449d96 8b08                     mov        ecx, dword ptr [eax]
00449d98 894dc8                   mov        dword ptr [ebp - 0x38], ecx
00449d9b 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
00449d9e 8b02                     mov        eax, dword ptr [edx]
00449da0 8b08                     mov        ecx, dword ptr [eax]
00449da2 8b5148                   mov        edx, dword ptr [ecx + 0x48]
00449da5 8955c4                   mov        dword ptr [ebp - 0x3c], edx
00449da8 8d45d8                   lea        eax, [ebp - 0x28]
00449dab 50                       push       eax
00449dac 6a00                     push       0
00449dae 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00449db1 51                       push       ecx
00449db2 ff55c4                   call       dword ptr [ebp - 0x3c]
00449db5 90                       nop        
00449db6 837dd800                 cmp        dword ptr [ebp - 0x28], 0
00449dba 7502                     jne        0x449dbe
00449dbc eb70                     jmp        0x449e2e
00449dbe 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00449dc1 8b02                     mov        eax, dword ptr [edx]
00449dc3 8b4830                   mov        ecx, dword ptr [eax + 0x30]
00449dc6 894dc0                   mov        dword ptr [ebp - 0x40], ecx
00449dc9 8d55dc                   lea        edx, [ebp - 0x24]
00449dcc 52                       push       edx
00449dcd 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00449dd0 50                       push       eax
00449dd1 ff55c0                   call       dword ptr [ebp - 0x40]
00449dd4 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00449dd7 8b11                     mov        edx, dword ptr [ecx]
00449dd9 8b4234                   mov        eax, dword ptr [edx + 0x34]
00449ddc 8945bc                   mov        dword ptr [ebp - 0x44], eax
00449ddf 6a00                     push       0
00449de1 6a00                     push       0
00449de3 8d4dd0                   lea        ecx, [ebp - 0x30]
00449de6 51                       push       ecx
00449de7 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00449dea 52                       push       edx
00449deb ff55bc                   call       dword ptr [ebp - 0x44]
00449dee 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00449df1 0faf45f8                 imul       eax, dword ptr [ebp - 8]
00449df5 50                       push       eax
00449df6 6a00                     push       0
00449df8 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00449dfb 51                       push       ecx
00449dfc e85fa90f00               call       0x544760
00449e01 83c40c                   add        esp, 0xc
00449e04 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00449e07 8b02                     mov        eax, dword ptr [edx]
00449e09 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00449e0c 51                       push       ecx
00449e0d 8b5038                   mov        edx, dword ptr [eax + 0x38]
00449e10 ffd2                     call       edx
00449e12 90                       nop        
00449e13 837dd800                 cmp        dword ptr [ebp - 0x28], 0
00449e17 7415                     je         0x449e2e
00449e19 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00449e1c 8b08                     mov        ecx, dword ptr [eax]
00449e1e 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00449e21 52                       push       edx
00449e22 8b4108                   mov        eax, dword ptr [ecx + 8]
00449e25 ffd0                     call       eax
00449e27 c745d800000000           mov        dword ptr [ebp - 0x28], 0
00449e2e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00449e31 33cd                     xor        ecx, ebp
00449e33 e8b68b0f00               call       0x5429ee
00449e38 8be5                     mov        esp, ebp
00449e3a 5d                       pop        ebp
00449e3b c3                       ret        
00449e3c cc                       int3       
00449e3d cc                       int3       
00449e3e cc                       int3       
00449e3f cc                       int3       
