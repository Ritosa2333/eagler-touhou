005321a0 55                       push       ebp
005321a1 8bec                     mov        ebp, esp
005321a3 51                       push       ecx
005321a4 894dfc                   mov        dword ptr [ebp - 4], ecx
005321a7 8b45fc                   mov        eax, dword ptr [ebp - 4]
005321aa 058c000000               add        eax, 0x8c
005321af 8be5                     mov        esp, ebp
005321b1 5d                       pop        ebp
005321b2 c3                       ret        
005321b3 cc                       int3       
005321b4 cc                       int3       
005321b5 cc                       int3       
005321b6 cc                       int3       
005321b7 cc                       int3       
005321b8 cc                       int3       
005321b9 cc                       int3       
005321ba cc                       int3       
005321bb cc                       int3       
005321bc cc                       int3       
005321bd cc                       int3       
005321be cc                       int3       
005321bf cc                       int3       
