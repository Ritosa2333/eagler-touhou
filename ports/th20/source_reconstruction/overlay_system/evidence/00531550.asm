00531550 55                       push       ebp
00531551 8bec                     mov        ebp, esp
00531553 51                       push       ecx
00531554 894dfc                   mov        dword ptr [ebp - 4], ecx
00531557 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053155a 83c138                   add        ecx, 0x38
0053155d e81eddf1ff               call       0x44f280
00531562 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531565 83c138                   add        ecx, 0x38
00531568 e8b3ebf1ff               call       0x450120
0053156d 33c0                     xor        eax, eax
0053156f 8be5                     mov        esp, ebp
00531571 5d                       pop        ebp
00531572 c3                       ret        
00531573 cc                       int3       
00531574 cc                       int3       
00531575 cc                       int3       
00531576 cc                       int3       
00531577 cc                       int3       
00531578 cc                       int3       
00531579 cc                       int3       
0053157a cc                       int3       
0053157b cc                       int3       
0053157c cc                       int3       
0053157d cc                       int3       
0053157e cc                       int3       
0053157f cc                       int3       
