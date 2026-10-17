0045c190 55                       push       ebp
0045c191 8bec                     mov        ebp, esp
0045c193 6aff                     push       -1
0045c195 6800765600               push       0x567600
0045c19a 64a100000000             mov        eax, dword ptr fs:[0]
0045c1a0 50                       push       eax
0045c1a1 51                       push       ecx
0045c1a2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0045c1a7 33c5                     xor        eax, ebp
0045c1a9 50                       push       eax
0045c1aa 8d45f4                   lea        eax, [ebp - 0xc]
0045c1ad 64a300000000             mov        dword ptr fs:[0], eax
0045c1b3 894df0                   mov        dword ptr [ebp - 0x10], ecx
0045c1b6 68c05c4200               push       0x425cc0
0045c1bb 68c8000000               push       0xc8
0045c1c0 6a04                     push       4
0045c1c2 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0045c1c5 50                       push       eax
0045c1c6 e855fafaff               call       0x40bc20
0045c1cb 90                       nop        
0045c1cc 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0045c1cf 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045c1d2 64890d00000000           mov        dword ptr fs:[0], ecx
0045c1d9 59                       pop        ecx
0045c1da 8be5                     mov        esp, ebp
0045c1dc 5d                       pop        ebp
0045c1dd c3                       ret        
0045c1de cc                       int3       
0045c1df cc                       int3       
0045c1e0 cc                       int3       
0045c1e1 cc                       int3       
0045c1e2 cc                       int3       
0045c1e3 cc                       int3       
0045c1e4 cc                       int3       
0045c1e5 cc                       int3       
0045c1e6 cc                       int3       
0045c1e7 cc                       int3       
0045c1e8 cc                       int3       
0045c1e9 cc                       int3       
0045c1ea cc                       int3       
0045c1eb cc                       int3       
0045c1ec cc                       int3       
0045c1ed cc                       int3       
0045c1ee cc                       int3       
0045c1ef cc                       int3       
