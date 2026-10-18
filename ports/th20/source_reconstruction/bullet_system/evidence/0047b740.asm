0047b740 55                       push       ebp
0047b741 8bec                     mov        ebp, esp
0047b743 6aff                     push       -1
0047b745 6800765600               push       0x567600
0047b74a 64a100000000             mov        eax, dword ptr fs:[0]
0047b750 50                       push       eax
0047b751 51                       push       ecx
0047b752 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047b757 33c5                     xor        eax, ebp
0047b759 50                       push       eax
0047b75a 8d45f4                   lea        eax, [ebp - 0xc]
0047b75d 64a300000000             mov        dword ptr fs:[0], eax
0047b763 894df0                   mov        dword ptr [ebp - 0x10], ecx
0047b766 6890bd4700               push       0x47bd90
0047b76b 6a0e                     push       0xe
0047b76d 6a40                     push       0x40
0047b76f 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047b772 50                       push       eax
0047b773 e8a804f9ff               call       0x40bc20
0047b778 90                       nop        
0047b779 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047b77c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047b77f 64890d00000000           mov        dword ptr fs:[0], ecx
0047b786 59                       pop        ecx
0047b787 8be5                     mov        esp, ebp
0047b789 5d                       pop        ebp
0047b78a c3                       ret        
0047b78b cc                       int3       
0047b78c cc                       int3       
0047b78d cc                       int3       
0047b78e cc                       int3       
0047b78f cc                       int3       
