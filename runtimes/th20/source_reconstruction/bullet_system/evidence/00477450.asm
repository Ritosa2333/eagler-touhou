00477450 55                       push       ebp
00477451 8bec                     mov        ebp, esp
00477453 51                       push       ecx
00477454 894dfc                   mov        dword ptr [ebp - 4], ecx
00477457 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047745a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047745d 898838040000             mov        dword ptr [eax + 0x438], ecx
00477463 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477466 e86541fbff               call       0x42b5d0
0047746b 90                       nop        
0047746c 8be5                     mov        esp, ebp
0047746e 5d                       pop        ebp
0047746f c20400                   ret        4
00477472 cc                       int3       
00477473 cc                       int3       
00477474 cc                       int3       
00477475 cc                       int3       
00477476 cc                       int3       
00477477 cc                       int3       
00477478 cc                       int3       
00477479 cc                       int3       
0047747a cc                       int3       
0047747b cc                       int3       
0047747c cc                       int3       
0047747d cc                       int3       
0047747e cc                       int3       
0047747f cc                       int3       
