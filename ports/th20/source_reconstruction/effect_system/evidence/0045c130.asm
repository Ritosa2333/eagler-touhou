0045c130 55                       push       ebp
0045c131 8bec                     mov        ebp, esp
0045c133 6aff                     push       -1
0045c135 6800765600               push       0x567600
0045c13a 64a100000000             mov        eax, dword ptr fs:[0]
0045c140 50                       push       eax
0045c141 51                       push       ecx
0045c142 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0045c147 33c5                     xor        eax, ebp
0045c149 50                       push       eax
0045c14a 8d45f4                   lea        eax, [ebp - 0xc]
0045c14d 64a300000000             mov        dword ptr fs:[0], eax
0045c153 894df0                   mov        dword ptr [ebp - 0x10], ecx
0045c156 68102e4200               push       0x422e10
0045c15b 68c8000000               push       0xc8
0045c160 6a0c                     push       0xc
0045c162 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0045c165 50                       push       eax
0045c166 e8b5fafaff               call       0x40bc20
0045c16b 90                       nop        
0045c16c 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0045c16f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045c172 64890d00000000           mov        dword ptr fs:[0], ecx
0045c179 59                       pop        ecx
0045c17a 8be5                     mov        esp, ebp
0045c17c 5d                       pop        ebp
0045c17d c3                       ret        
0045c17e cc                       int3       
0045c17f cc                       int3       
0045c180 cc                       int3       
0045c181 cc                       int3       
0045c182 cc                       int3       
0045c183 cc                       int3       
0045c184 cc                       int3       
0045c185 cc                       int3       
0045c186 cc                       int3       
0045c187 cc                       int3       
0045c188 cc                       int3       
0045c189 cc                       int3       
0045c18a cc                       int3       
0045c18b cc                       int3       
0045c18c cc                       int3       
0045c18d cc                       int3       
0045c18e cc                       int3       
0045c18f cc                       int3       
