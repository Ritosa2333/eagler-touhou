0052fe80 55                       push       ebp
0052fe81 8bec                     mov        ebp, esp
0052fe83 83ec0c                   sub        esp, 0xc
0052fe86 894df4                   mov        dword ptr [ebp - 0xc], ecx
0052fe89 6a00                     push       0
0052fe8b e8f041f3ff               call       0x464080
0052fe90 83c404                   add        esp, 4
0052fe93 8bc8                     mov        ecx, eax
0052fe95 e86645f3ff               call       0x464400
0052fe9a 0fb6c0                   movzx      eax, al
0052fe9d 85c0                     test       eax, eax
0052fe9f 741a                     je         0x52febb
0052fea1 6a00                     push       0
0052fea3 e8d841f3ff               call       0x464080
0052fea8 83c404                   add        esp, 4
0052feab 8945fc                   mov        dword ptr [ebp - 4], eax
0052feae 6a07                     push       7
0052feb0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052feb3 e858edf8ff               call       0x4bec10
0052feb8 90                       nop        
0052feb9 eb18                     jmp        0x52fed3
0052febb 6a00                     push       0
0052febd e8be41f3ff               call       0x464080
0052fec2 83c404                   add        esp, 4
0052fec5 8945f8                   mov        dword ptr [ebp - 8], eax
0052fec8 6a0c                     push       0xc
0052feca 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052fecd e83eedf8ff               call       0x4bec10
0052fed2 90                       nop        
0052fed3 8be5                     mov        esp, ebp
0052fed5 5d                       pop        ebp
0052fed6 c3                       ret        
0052fed7 cc                       int3       
0052fed8 cc                       int3       
0052fed9 cc                       int3       
0052feda cc                       int3       
0052fedb cc                       int3       
0052fedc cc                       int3       
0052fedd cc                       int3       
0052fede cc                       int3       
0052fedf cc                       int3       
