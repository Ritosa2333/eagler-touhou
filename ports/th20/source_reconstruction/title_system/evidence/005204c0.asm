005204c0 55                       push       ebp
005204c1 8bec                     mov        ebp, esp
005204c3 83ec14                   sub        esp, 0x14
005204c6 894dfc                   mov        dword ptr [ebp - 4], ecx
005204c9 8b45fc                   mov        eax, dword ptr [ebp - 4]
005204cc 8b4820                   mov        ecx, dword ptr [eax + 0x20]
005204cf 894df8                   mov        dword ptr [ebp - 8], ecx
005204d2 837df800                 cmp        dword ptr [ebp - 8], 0
005204d6 740f                     je         0x5204e7
005204d8 837df801                 cmp        dword ptr [ebp - 8], 1
005204dc 0f8494000000             je         0x520576
005204e2 e9df000000               jmp        0x5205c6
005204e7 8b55fc                   mov        edx, dword ptr [ebp - 4]
005204ea 81c290030000             add        edx, 0x390
005204f0 8955f4                   mov        dword ptr [ebp - 0xc], edx
005204f3 6a00                     push       0
005204f5 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005204f8 e8338ff2ff               call       0x449430
005204fd 0fb6c0                   movzx      eax, al
00520500 85c0                     test       eax, eax
00520502 742d                     je         0x520531
00520504 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052050a e84174f1ff               call       0x437950
0052050f 8945f0                   mov        dword ptr [ebp - 0x10], eax
00520512 6a00                     push       0
00520514 6aff                     push       -1
00520516 6a13                     push       0x13
00520518 6a00                     push       0
0052051a 8d4dec                   lea        ecx, [ebp - 0x14]
0052051d 51                       push       ecx
0052051e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00520521 e84a07f3ff               call       0x450c70
00520526 8b10                     mov        edx, dword ptr [eax]
00520528 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052052b 899090030000             mov        dword ptr [eax + 0x390], edx
00520531 6a2d                     push       0x2d
00520533 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520536 e845ca0000               call       0x52cf80
0052053b e830f7f9ff               call       0x4bfc70
00520540 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520543 c7412001000000           mov        dword ptr [ecx + 0x20], 1
0052054a 6a00                     push       0
0052054c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052054f 81c154010000             add        ecx, 0x154
00520555 e8c62ff0ff               call       0x423520
0052055a 51                       push       ecx
0052055b f30f100550e05600         movss      xmm0, dword ptr [0x56e050]
00520563 f30f110424               movss      dword ptr [esp], xmm0
00520568 8b0d244d5c00             mov        ecx, dword ptr [0x5c4d24]
0052056e e89d65fcff               call       0x4e6b10
00520573 90                       nop        
00520574 eb50                     jmp        0x5205c6
00520576 8b0d244d5c00             mov        ecx, dword ptr [0x5c4d24]
0052057c e8ef61fcff               call       0x4e6770
00520581 85c0                     test       eax, eax
00520583 7441                     je         0x5205c6
00520585 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520588 81c190030000             add        ecx, 0x390
0052058e e8ad8af5ff               call       0x479040
00520593 6a00                     push       0
00520595 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520598 81c190030000             add        ecx, 0x390
0052059e e8fd11efff               call       0x4117a0
005205a3 6a2d                     push       0x2d
005205a5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005205a8 e883c60000               call       0x52cc30
005205ad 6a01                     push       1
005205af 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005205b2 e819c90000               call       0x52ced0
005205b7 8b15244d5c00             mov        edx, dword ptr [0x5c4d24]
005205bd 52                       push       edx
005205be e8fd11f0ff               call       0x4217c0
005205c3 83c404                   add        esp, 4
005205c6 33c0                     xor        eax, eax
005205c8 8be5                     mov        esp, ebp
005205ca 5d                       pop        ebp
005205cb c3                       ret        
005205cc cc                       int3       
005205cd cc                       int3       
005205ce cc                       int3       
005205cf cc                       int3       
