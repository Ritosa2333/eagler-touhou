0047a5c0 55                       push       ebp
0047a5c1 8bec                     mov        ebp, esp
0047a5c3 51                       push       ecx
0047a5c4 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a5c7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a5ca f30f104508               movss      xmm0, dword ptr [ebp + 8]
0047a5cf f30f114018               movss      dword ptr [eax + 0x18], xmm0
0047a5d4 8be5                     mov        esp, ebp
0047a5d6 5d                       pop        ebp
0047a5d7 c20400                   ret        4
0047a5da cc                       int3       
0047a5db cc                       int3       
0047a5dc cc                       int3       
0047a5dd cc                       int3       
0047a5de cc                       int3       
0047a5df cc                       int3       
