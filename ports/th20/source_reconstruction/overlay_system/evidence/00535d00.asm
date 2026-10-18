00535d00 55                       push       ebp
00535d01 8bec                     mov        ebp, esp
00535d03 83ec30                   sub        esp, 0x30
00535d06 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00535d0b 33c5                     xor        eax, ebp
00535d0d 8945fc                   mov        dword ptr [ebp - 4], eax
00535d10 894df4                   mov        dword ptr [ebp - 0xc], ecx
00535d13 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00535d16 83c048                   add        eax, 0x48
00535d19 8945ec                   mov        dword ptr [ebp - 0x14], eax
00535d1c 6a00                     push       0
00535d1e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00535d21 e84ab3f2ff               call       0x461070
00535d26 0fb6c8                   movzx      ecx, al
00535d29 85c9                     test       ecx, ecx
00535d2b 0f84d8000000             je         0x535e09
00535d31 6a04                     push       4
00535d33 8d4df8                   lea        ecx, [ebp - 8]
00535d36 e84563edff               call       0x40c080
00535d3b 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00535d3e 52                       push       edx
00535d3f 8d4df8                   lea        ecx, [ebp - 8]
00535d42 e859e5edff               call       0x4142a0
00535d47 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00535d4a 83c148                   add        ecx, 0x48
00535d4d e87edeeeff               call       0x423bd0
00535d52 d95de8                   fstp       dword ptr [ebp - 0x18]
00535d55 f30f100548e05600         movss      xmm0, dword ptr [0x56e048]
00535d5d f30f5c45e8               subss      xmm0, dword ptr [ebp - 0x18]
00535d62 f30f590534e75600         mulss      xmm0, dword ptr [0x56e734]
00535d6a f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00535d6f 6a00                     push       0
00535d71 e80a31f4ff               call       0x478e80
00535d76 83c404                   add        esp, 4
00535d79 8945e0                   mov        dword ptr [ebp - 0x20], eax
00535d7c 6a00                     push       0
00535d7e e8adaaf2ff               call       0x460830
00535d83 83c404                   add        esp, 4
00535d86 8bc8                     mov        ecx, eax
00535d88 e8c3aaf2ff               call       0x460850
00535d8d 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00535d90 83ec28                   sub        esp, 0x28
00535d93 8bcc                     mov        ecx, esp
00535d95 8965d0                   mov        dword ptr [ebp - 0x30], esp
00535d98 8d45f8                   lea        eax, [ebp - 8]
00535d9b 50                       push       eax
00535d9c e8affaffff               call       0x535850
00535da1 51                       push       ecx
00535da2 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00535da7 f30f110424               movss      dword ptr [esp], xmm0
00535dac 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00535daf 51                       push       ecx
00535db0 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00535db3 e88874f4ff               call       0x47d240
00535db8 90                       nop        
00535db9 6a00                     push       0
00535dbb e82031f4ff               call       0x478ee0
00535dc0 83c404                   add        esp, 4
00535dc3 8945d8                   mov        dword ptr [ebp - 0x28], eax
00535dc6 6a00                     push       0
00535dc8 e863aaf2ff               call       0x460830
00535dcd 83c404                   add        esp, 4
00535dd0 8bc8                     mov        ecx, eax
00535dd2 e879aaf2ff               call       0x460850
00535dd7 8945dc                   mov        dword ptr [ebp - 0x24], eax
00535dda 6a01                     push       1
00535ddc 6a00                     push       0
00535dde 51                       push       ecx
00535ddf f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00535de4 f30f110424               movss      dword ptr [esp], xmm0
00535de9 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00535dec 52                       push       edx
00535ded 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00535df0 e8db4cf9ff               call       0x4caad0
00535df5 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00535df8 83c048                   add        eax, 0x48
00535dfb 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00535dfe 6a00                     push       0
00535e00 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00535e03 e81836efff               call       0x429420
00535e08 90                       nop        
00535e09 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00535e0c 33cd                     xor        ecx, ebp
00535e0e e8dbcb0000               call       0x5429ee
00535e13 8be5                     mov        esp, ebp
00535e15 5d                       pop        ebp
00535e16 c3                       ret        
00535e17 cc                       int3       
00535e18 cc                       int3       
00535e19 cc                       int3       
00535e1a cc                       int3       
00535e1b cc                       int3       
00535e1c cc                       int3       
00535e1d cc                       int3       
00535e1e cc                       int3       
00535e1f cc                       int3       
