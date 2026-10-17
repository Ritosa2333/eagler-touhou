0044fca0 55                       push       ebp
0044fca1 8bec                     mov        ebp, esp
0044fca3 83ec08                   sub        esp, 8
0044fca6 894dfc                   mov        dword ptr [ebp - 4], ecx
0044fca9 8b4508                   mov        eax, dword ptr [ebp + 8]
0044fcac 50                       push       eax
0044fcad 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044fcb0 e84bd0ffff               call       0x44cd00
0044fcb5 8945f8                   mov        dword ptr [ebp - 8], eax
0044fcb8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0044fcbb 51                       push       ecx
0044fcbc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044fcbf e8bcfeffff               call       0x44fb80
0044fcc4 90                       nop        
0044fcc5 8be5                     mov        esp, ebp
0044fcc7 5d                       pop        ebp
0044fcc8 c20400                   ret        4
0044fccb cc                       int3       
0044fccc cc                       int3       
0044fccd cc                       int3       
0044fcce cc                       int3       
0044fccf cc                       int3       
