0047a560 55                       push       ebp
0047a561 8bec                     mov        ebp, esp
0047a563 6aff                     push       -1
0047a565 6800765600               push       0x567600
0047a56a 64a100000000             mov        eax, dword ptr fs:[0]
0047a570 50                       push       eax
0047a571 51                       push       ecx
0047a572 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047a577 33c5                     xor        eax, ebp
0047a579 50                       push       eax
0047a57a 8d45f4                   lea        eax, [ebp - 0xc]
0047a57d 64a300000000             mov        dword ptr fs:[0], eax
0047a583 894df0                   mov        dword ptr [ebp - 0x10], ecx
0047a586 51                       push       ecx
0047a587 f30f104508               movss      xmm0, dword ptr [ebp + 8]
0047a58c f30f110424               movss      dword ptr [esp], xmm0
0047a591 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047a594 83c11c                   add        ecx, 0x1c
0047a597 e88489fdff               call       0x452f20
0047a59c 90                       nop        
0047a59d 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047a5a0 64890d00000000           mov        dword ptr fs:[0], ecx
0047a5a7 59                       pop        ecx
0047a5a8 8be5                     mov        esp, ebp
0047a5aa 5d                       pop        ebp
0047a5ab c20400                   ret        4
0047a5ae cc                       int3       
0047a5af cc                       int3       
0047a5b0 cc                       int3       
0047a5b1 cc                       int3       
0047a5b2 cc                       int3       
0047a5b3 cc                       int3       
0047a5b4 cc                       int3       
0047a5b5 cc                       int3       
0047a5b6 cc                       int3       
0047a5b7 cc                       int3       
0047a5b8 cc                       int3       
0047a5b9 cc                       int3       
0047a5ba cc                       int3       
0047a5bb cc                       int3       
0047a5bc cc                       int3       
0047a5bd cc                       int3       
0047a5be cc                       int3       
0047a5bf cc                       int3       
