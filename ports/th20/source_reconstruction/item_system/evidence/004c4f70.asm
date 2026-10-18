004c4f70 55                       push       ebp
004c4f71 8bec                     mov        ebp, esp
004c4f73 51                       push       ecx
004c4f74 894dfc                   mov        dword ptr [ebp - 4], ecx
004c4f77 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4f7a f30f104508               movss      xmm0, dword ptr [ebp + 8]
004c4f7f f30f118060c84900         movss      dword ptr [eax + 0x49c860], xmm0
004c4f87 8be5                     mov        esp, ebp
004c4f89 5d                       pop        ebp
004c4f8a c20400                   ret        4
004c4f8d cc                       int3       
004c4f8e cc                       int3       
004c4f8f cc                       int3       
