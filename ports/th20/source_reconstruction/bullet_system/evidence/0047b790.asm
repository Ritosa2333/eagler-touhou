0047b790 55                       push       ebp
0047b791 8bec                     mov        ebp, esp
0047b793 51                       push       ecx
0047b794 894dfc                   mov        dword ptr [ebp - 4], ecx
0047b797 6870c44700               push       0x47c470
0047b79c 6800be4700               push       0x47be00
0047b7a1 68d1070000               push       0x7d1
0047b7a6 6828050000               push       0x528
0047b7ab 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047b7ae 50                       push       eax
0047b7af e8fb760c00               call       0x542eaf
0047b7b4 90                       nop        
0047b7b5 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047b7b8 8be5                     mov        esp, ebp
0047b7ba 5d                       pop        ebp
0047b7bb c3                       ret        
0047b7bc cc                       int3       
0047b7bd cc                       int3       
0047b7be cc                       int3       
0047b7bf cc                       int3       
