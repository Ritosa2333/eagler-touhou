004297a0 55                       push       ebp
004297a1 8bec                     mov        ebp, esp
004297a3 51                       push       ecx
004297a4 894dfc                   mov        dword ptr [ebp - 4], ecx
004297a7 f30f2a4508               cvtsi2ss   xmm0, dword ptr [ebp + 8]
004297ac 51                       push       ecx
004297ad f30f110424               movss      dword ptr [esp], xmm0
004297b2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004297b5 e836990200               call       0x4530f0
004297ba 90                       nop        
004297bb 8be5                     mov        esp, ebp
004297bd 5d                       pop        ebp
004297be c20400                   ret        4
004297c1 cc                       int3       
004297c2 cc                       int3       
004297c3 cc                       int3       
004297c4 cc                       int3       
004297c5 cc                       int3       
004297c6 cc                       int3       
004297c7 cc                       int3       
004297c8 cc                       int3       
004297c9 cc                       int3       
004297ca cc                       int3       
004297cb cc                       int3       
004297cc cc                       int3       
004297cd cc                       int3       
004297ce cc                       int3       
004297cf cc                       int3       
