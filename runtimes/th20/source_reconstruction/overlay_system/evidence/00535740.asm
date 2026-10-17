00535740 55                       push       ebp
00535741 8bec                     mov        ebp, esp
00535743 51                       push       ecx
00535744 894dfc                   mov        dword ptr [ebp - 4], ecx
00535747 b970695c00               mov        ecx, 0x5c6970
0053574c e82f9bf1ff               call       0x44f280
00535751 b970695c00               mov        ecx, 0x5c6970
00535756 e8c5a9f1ff               call       0x450120
0053575b 33c0                     xor        eax, eax
0053575d 8be5                     mov        esp, ebp
0053575f 5d                       pop        ebp
00535760 c3                       ret        
00535761 cc                       int3       
00535762 cc                       int3       
00535763 cc                       int3       
00535764 cc                       int3       
00535765 cc                       int3       
00535766 cc                       int3       
00535767 cc                       int3       
00535768 cc                       int3       
00535769 cc                       int3       
0053576a cc                       int3       
0053576b cc                       int3       
0053576c cc                       int3       
0053576d cc                       int3       
0053576e cc                       int3       
0053576f cc                       int3       
