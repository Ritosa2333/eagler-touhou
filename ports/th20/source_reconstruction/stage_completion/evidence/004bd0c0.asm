004bd0c0 55                       push       ebp
004bd0c1 8bec                     mov        ebp, esp
004bd0c3 83ec1c                   sub        esp, 0x1c
004bd0c6 894dfc                   mov        dword ptr [ebp - 4], ecx
004bd0c9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bd0cc 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004bd0cf 83c902                   or         ecx, 2
004bd0d2 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bd0d5 894a14                   mov        dword ptr [edx + 0x14], ecx
004bd0d8 c745f800000000           mov        dword ptr [ebp - 8], 0
004bd0df eb09                     jmp        0x4bd0ea
004bd0e1 8b45f8                   mov        eax, dword ptr [ebp - 8]
004bd0e4 83c001                   add        eax, 1
004bd0e7 8945f8                   mov        dword ptr [ebp - 8], eax
004bd0ea 837df80a                 cmp        dword ptr [ebp - 8], 0xa
004bd0ee 7d4a                     jge        0x4bd13a
004bd0f0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bd0f3 81c184060000             add        ecx, 0x684
004bd0f9 894df0                   mov        dword ptr [ebp - 0x10], ecx
004bd0fc 8b55f8                   mov        edx, dword ptr [ebp - 8]
004bd0ff 52                       push       edx
004bd100 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bd103 e8d8d2ffff               call       0x4ba3e0
004bd108 05dc000000               add        eax, 0xdc
004bd10d 8bc8                     mov        ecx, eax
004bd10f e83c92fcff               call       0x486350
004bd114 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bd117 0584060000               add        eax, 0x684
004bd11c 8945ec                   mov        dword ptr [ebp - 0x14], eax
004bd11f 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004bd122 51                       push       ecx
004bd123 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bd126 e8b5d2ffff               call       0x4ba3e0
004bd12b 05e0000000               add        eax, 0xe0
004bd130 8bc8                     mov        ecx, eax
004bd132 e81992fcff               call       0x486350
004bd137 90                       nop        
004bd138 eba7                     jmp        0x4bd0e1
004bd13a c745f400000000           mov        dword ptr [ebp - 0xc], 0
004bd141 eb09                     jmp        0x4bd14c
004bd143 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004bd146 83c201                   add        edx, 1
004bd149 8955f4                   mov        dword ptr [ebp - 0xc], edx
004bd14c 837df40c                 cmp        dword ptr [ebp - 0xc], 0xc
004bd150 7d4a                     jge        0x4bd19c
004bd152 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bd155 053c120000               add        eax, 0x123c
004bd15a 8945e8                   mov        dword ptr [ebp - 0x18], eax
004bd15d 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004bd160 51                       push       ecx
004bd161 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004bd164 e877d2ffff               call       0x4ba3e0
004bd169 05dc000000               add        eax, 0xdc
004bd16e 8bc8                     mov        ecx, eax
004bd170 e8db91fcff               call       0x486350
004bd175 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bd178 81c23c120000             add        edx, 0x123c
004bd17e 8955e4                   mov        dword ptr [ebp - 0x1c], edx
004bd181 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004bd184 50                       push       eax
004bd185 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004bd188 e853d2ffff               call       0x4ba3e0
004bd18d 05e0000000               add        eax, 0xe0
004bd192 8bc8                     mov        ecx, eax
004bd194 e8b791fcff               call       0x486350
004bd199 90                       nop        
004bd19a eba7                     jmp        0x4bd143
004bd19c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bd19f c781e820000000000000     mov        dword ptr [ecx + 0x20e8], 0
004bd1a9 8be5                     mov        esp, ebp
004bd1ab 5d                       pop        ebp
004bd1ac c3                       ret        
004bd1ad cc                       int3       
004bd1ae cc                       int3       
004bd1af cc                       int3       
