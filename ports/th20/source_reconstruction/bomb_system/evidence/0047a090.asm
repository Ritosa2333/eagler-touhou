0047a090 55                       push       ebp
0047a091 8bec                     mov        ebp, esp
0047a093 83ec24                   sub        esp, 0x24
0047a096 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a099 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a09c 83c004                   add        eax, 4
0047a09f 8945f4                   mov        dword ptr [ebp - 0xc], eax
0047a0a2 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0047a0a5 51                       push       ecx
0047a0a6 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047a0a9 e832050000               call       0x47a5e0
0047a0ae 90                       nop        
0047a0af 6a00                     push       0
0047a0b1 e87a67feff               call       0x460830
0047a0b6 83c404                   add        esp, 4
0047a0b9 8bc8                     mov        ecx, eax
0047a0bb e85077faff               call       0x421810
0047a0c0 8945ec                   mov        dword ptr [ebp - 0x14], eax
0047a0c3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a0c6 83c104                   add        ecx, 4
0047a0c9 e8d21cf9ff               call       0x40bda0
0047a0ce 8945f0                   mov        dword ptr [ebp - 0x10], eax
0047a0d1 6a00                     push       0
0047a0d3 6aff                     push       -1
0047a0d5 51                       push       ecx
0047a0d6 0f57c0                   xorps      xmm0, xmm0
0047a0d9 f30f110424               movss      dword ptr [esp], xmm0
0047a0de 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0047a0e1 52                       push       edx
0047a0e2 6a2e                     push       0x2e
0047a0e4 688cfb5600               push       0x56fb8c
0047a0e9 8d45e0                   lea        eax, [ebp - 0x20]
0047a0ec 50                       push       eax
0047a0ed 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047a0f0 e8ebefffff               call       0x4790e0
0047a0f5 8b08                     mov        ecx, dword ptr [eax]
0047a0f7 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047a0fa 890a                     mov        dword ptr [edx], ecx
0047a0fc 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a0ff c780a000000001000000     mov        dword ptr [eax + 0xa0], 1
0047a109 6a00                     push       0
0047a10b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a10e 81c1a4000000             add        ecx, 0xa4
0047a114 e80794faff               call       0x423520
0047a119 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a11c 8b5508                   mov        edx, dword ptr [ebp + 8]
0047a11f 8991c8000000             mov        dword ptr [ecx + 0xc8], edx
0047a125 6a00                     push       0
0047a127 e894edffff               call       0x478ec0
0047a12c 83c404                   add        esp, 4
0047a12f 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0047a132 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a135 83c104                   add        ecx, 4
0047a138 e8631cf9ff               call       0x40bda0
0047a13d 8945e8                   mov        dword ptr [ebp - 0x18], eax
0047a140 8b4510                   mov        eax, dword ptr [ebp + 0x10]
0047a143 50                       push       eax
0047a144 680f270000               push       0x270f
0047a149 51                       push       ecx
0047a14a 0f57c0                   xorps      xmm0, xmm0
0047a14d f30f110424               movss      dword ptr [esp], xmm0
0047a152 51                       push       ecx
0047a153 f30f100504fc5600         movss      xmm0, dword ptr [0x56fc04]
0047a15b f30f110424               movss      dword ptr [esp], xmm0
0047a160 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0047a163 51                       push       ecx
0047a164 8d55dc                   lea        edx, [ebp - 0x24]
0047a167 52                       push       edx
0047a168 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0047a16b e8107a0400               call       0x4c1b80
0047a170 8b00                     mov        eax, dword ptr [eax]
0047a172 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a175 8981cc000000             mov        dword ptr [ecx + 0xcc], eax
0047a17b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a17e 81c1cc000000             add        ecx, 0xcc
0047a184 e8876c0400               call       0x4c0e10
0047a189 8945f8                   mov        dword ptr [ebp - 8], eax
0047a18c 6a03                     push       3
0047a18e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0047a191 e84a030000               call       0x47a4e0
0047a196 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0047a199 e802030000               call       0x47a4a0
0047a19e 90                       nop        
0047a19f 8be5                     mov        esp, ebp
0047a1a1 5d                       pop        ebp
0047a1a2 c20c00                   ret        0xc
0047a1a5 cc                       int3       
0047a1a6 cc                       int3       
0047a1a7 cc                       int3       
0047a1a8 cc                       int3       
0047a1a9 cc                       int3       
0047a1aa cc                       int3       
0047a1ab cc                       int3       
0047a1ac cc                       int3       
0047a1ad cc                       int3       
0047a1ae cc                       int3       
0047a1af cc                       int3       
