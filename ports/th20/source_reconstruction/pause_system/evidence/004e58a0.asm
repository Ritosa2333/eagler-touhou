004e58a0 55                       push       ebp
004e58a1 8bec                     mov        ebp, esp
004e58a3 51                       push       ecx
004e58a4 894dfc                   mov        dword ptr [ebp - 4], ecx
004e58a7 b968a55b00               mov        ecx, 0x5ba568
004e58ac e82f7efdff               call       0x4bd6e0
004e58b1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e58b4 81c1cc000000             add        ecx, 0xcc
004e58ba e88137f9ff               call       0x479040
004e58bf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e58c2 81c1c8000000             add        ecx, 0xc8
004e58c8 e87337f9ff               call       0x479040
004e58cd 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e58d0 51                       push       ecx
004e58d1 f30f1080c4010000         movss      xmm0, dword ptr [eax + 0x1c4]
004e58d9 f30f110424               movss      dword ptr [esp], xmm0
004e58de b9e4ef5a00               mov        ecx, 0x5aefe4
004e58e3 e8b839f4ff               call       0x4292a0
004e58e8 90                       nop        
004e58e9 8be5                     mov        esp, ebp
004e58eb 5d                       pop        ebp
004e58ec c3                       ret        
004e58ed cc                       int3       
004e58ee cc                       int3       
004e58ef cc                       int3       
