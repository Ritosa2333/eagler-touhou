00479fd0 55                       push       ebp
00479fd1 8bec                     mov        ebp, esp
00479fd3 83ec14                   sub        esp, 0x14
00479fd6 894dfc                   mov        dword ptr [ebp - 4], ecx
00479fd9 6a00                     push       0
00479fdb e85068feff               call       0x460830
00479fe0 83c404                   add        esp, 4
00479fe3 8bc8                     mov        ecx, eax
00479fe5 e86668feff               call       0x460850
00479fea 50                       push       eax
00479feb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00479fee 83c128                   add        ecx, 0x28
00479ff1 e88adbffff               call       0x477b80
00479ff6 6a00                     push       0
00479ff8 6a31                     push       0x31
00479ffa b930a85b00               mov        ecx, 0x5ba830
00479fff e86ccdfaff               call       0x426d70
0047a004 6a00                     push       0
0047a006 e895eeffff               call       0x478ea0
0047a00b 83c404                   add        esp, 4
0047a00e 8bc8                     mov        ecx, eax
0047a010 e89bdb0000               call       0x487bb0
0047a015 6a00                     push       0
0047a017 e844e0ffff               call       0x478060
0047a01c 83c404                   add        esp, 4
0047a01f 8945f8                   mov        dword ptr [ebp - 8], eax
0047a022 6a01                     push       1
0047a024 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0047a027 e814dfffff               call       0x477f40
0047a02c 90                       nop        
0047a02d 6a00                     push       0
0047a02f e8fc67feff               call       0x460830
0047a034 83c404                   add        esp, 4
0047a037 8bc8                     mov        ecx, eax
0047a039 e8d277faff               call       0x421810
0047a03e 8945f0                   mov        dword ptr [ebp - 0x10], eax
0047a041 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a044 83c128                   add        ecx, 0x28
0047a047 e8541df9ff               call       0x40bda0
0047a04c 8945f4                   mov        dword ptr [ebp - 0xc], eax
0047a04f 6a00                     push       0
0047a051 6aff                     push       -1
0047a053 51                       push       ecx
0047a054 0f57c0                   xorps      xmm0, xmm0
0047a057 f30f110424               movss      dword ptr [esp], xmm0
0047a05c 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0047a05f 50                       push       eax
0047a060 6a3d                     push       0x3d
0047a062 688cfb5600               push       0x56fb8c
0047a067 8d4dec                   lea        ecx, [ebp - 0x14]
0047a06a 51                       push       ecx
0047a06b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047a06e e86df0ffff               call       0x4790e0
0047a073 8b10                     mov        edx, dword ptr [eax]
0047a075 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a078 895074                   mov        dword ptr [eax + 0x74], edx
0047a07b 6a00                     push       0
0047a07d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a080 83c108                   add        ecx, 8
0047a083 e89894faff               call       0x423520
0047a088 33c0                     xor        eax, eax
0047a08a 8be5                     mov        esp, ebp
0047a08c 5d                       pop        ebp
0047a08d c20400                   ret        4
