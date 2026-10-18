0047b7c0 55                       push       ebp
0047b7c1 8bec                     mov        ebp, esp
0047b7c3 6aff                     push       -1
0047b7c5 6800765600               push       0x567600
0047b7ca 64a100000000             mov        eax, dword ptr fs:[0]
0047b7d0 50                       push       eax
0047b7d1 51                       push       ecx
0047b7d2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047b7d7 33c5                     xor        eax, ebp
0047b7d9 50                       push       eax
0047b7da 8d45f4                   lea        eax, [ebp - 0xc]
0047b7dd 64a300000000             mov        dword ptr fs:[0], eax
0047b7e3 894df0                   mov        dword ptr [ebp - 0x10], ecx
0047b7e6 68c05c4200               push       0x425cc0
0047b7eb 68d1070000               push       0x7d1
0047b7f0 6a04                     push       4
0047b7f2 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047b7f5 50                       push       eax
0047b7f6 e82504f9ff               call       0x40bc20
0047b7fb 90                       nop        
0047b7fc 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047b7ff 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047b802 64890d00000000           mov        dword ptr fs:[0], ecx
0047b809 59                       pop        ecx
0047b80a 8be5                     mov        esp, ebp
0047b80c 5d                       pop        ebp
0047b80d c3                       ret        
0047b80e cc                       int3       
0047b80f cc                       int3       
0047b810 cc                       int3       
0047b811 cc                       int3       
0047b812 cc                       int3       
0047b813 cc                       int3       
0047b814 cc                       int3       
0047b815 cc                       int3       
0047b816 cc                       int3       
0047b817 cc                       int3       
0047b818 cc                       int3       
0047b819 cc                       int3       
0047b81a cc                       int3       
0047b81b cc                       int3       
0047b81c cc                       int3       
0047b81d cc                       int3       
0047b81e cc                       int3       
0047b81f cc                       int3       
