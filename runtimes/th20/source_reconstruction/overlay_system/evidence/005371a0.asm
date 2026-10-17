005371a0 55                       push       ebp
005371a1 8bec                     mov        ebp, esp
005371a3 83ec30                   sub        esp, 0x30
005371a6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
005371ab 33c5                     xor        eax, ebp
005371ad 8945fc                   mov        dword ptr [ebp - 4], eax
005371b0 894df4                   mov        dword ptr [ebp - 0xc], ecx
005371b3 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
005371b6 83c048                   add        eax, 0x48
005371b9 8945ec                   mov        dword ptr [ebp - 0x14], eax
005371bc 6a00                     push       0
005371be 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
005371c1 e8aa9ef2ff               call       0x461070
005371c6 0fb6c8                   movzx      ecx, al
005371c9 85c9                     test       ecx, ecx
005371cb 0f84d8000000             je         0x5372a9
005371d1 6a04                     push       4
005371d3 8d4df8                   lea        ecx, [ebp - 8]
005371d6 e8a54eedff               call       0x40c080
005371db 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
005371de 52                       push       edx
005371df 8d4df8                   lea        ecx, [ebp - 8]
005371e2 e8b9d0edff               call       0x4142a0
005371e7 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005371ea 83c148                   add        ecx, 0x48
005371ed e8dec9eeff               call       0x423bd0
005371f2 d95de8                   fstp       dword ptr [ebp - 0x18]
005371f5 f30f100548e05600         movss      xmm0, dword ptr [0x56e048]
005371fd f30f5c45e8               subss      xmm0, dword ptr [ebp - 0x18]
00537202 f30f590534e75600         mulss      xmm0, dword ptr [0x56e734]
0053720a f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
0053720f 6a00                     push       0
00537211 e86a1cf4ff               call       0x478e80
00537216 83c404                   add        esp, 4
00537219 8945e0                   mov        dword ptr [ebp - 0x20], eax
0053721c 6a00                     push       0
0053721e e80d96f2ff               call       0x460830
00537223 83c404                   add        esp, 4
00537226 8bc8                     mov        ecx, eax
00537228 e82396f2ff               call       0x460850
0053722d 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00537230 83ec28                   sub        esp, 0x28
00537233 8bcc                     mov        ecx, esp
00537235 8965d0                   mov        dword ptr [ebp - 0x30], esp
00537238 8d45f8                   lea        eax, [ebp - 8]
0053723b 50                       push       eax
0053723c e86ffbffff               call       0x536db0
00537241 51                       push       ecx
00537242 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00537247 f30f110424               movss      dword ptr [esp], xmm0
0053724c 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0053724f 51                       push       ecx
00537250 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00537253 e8e85ff4ff               call       0x47d240
00537258 90                       nop        
00537259 6a00                     push       0
0053725b e8801cf4ff               call       0x478ee0
00537260 83c404                   add        esp, 4
00537263 8945d8                   mov        dword ptr [ebp - 0x28], eax
00537266 6a00                     push       0
00537268 e8c395f2ff               call       0x460830
0053726d 83c404                   add        esp, 4
00537270 8bc8                     mov        ecx, eax
00537272 e8d995f2ff               call       0x460850
00537277 8945dc                   mov        dword ptr [ebp - 0x24], eax
0053727a 6a01                     push       1
0053727c 6a00                     push       0
0053727e 51                       push       ecx
0053727f f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00537284 f30f110424               movss      dword ptr [esp], xmm0
00537289 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0053728c 52                       push       edx
0053728d 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00537290 e83b38f9ff               call       0x4caad0
00537295 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00537298 83c048                   add        eax, 0x48
0053729b 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0053729e 6a00                     push       0
005372a0 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
005372a3 e87821efff               call       0x429420
005372a8 90                       nop        
005372a9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005372ac 33cd                     xor        ecx, ebp
005372ae e83bb70000               call       0x5429ee
005372b3 8be5                     mov        esp, ebp
005372b5 5d                       pop        ebp
005372b6 c3                       ret        
005372b7 cc                       int3       
005372b8 cc                       int3       
005372b9 cc                       int3       
005372ba cc                       int3       
005372bb cc                       int3       
005372bc cc                       int3       
005372bd cc                       int3       
005372be cc                       int3       
005372bf cc                       int3       
