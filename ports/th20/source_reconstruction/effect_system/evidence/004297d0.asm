004297d0 55                       push       ebp
004297d1 8bec                     mov        ebp, esp
004297d3 83ec0c                   sub        esp, 0xc
004297d6 894dfc                   mov        dword ptr [ebp - 4], ecx
004297d9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004297dc e8ff000000               call       0x4298e0
004297e1 d95df8                   fstp       dword ptr [ebp - 8]
004297e4 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
004297e9 f30f5905f0e05600         mulss      xmm0, dword ptr [0x56e0f0]
004297f1 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004297f6 d945f4                   fld        dword ptr [ebp - 0xc]
004297f9 8be5                     mov        esp, ebp
004297fb 5d                       pop        ebp
004297fc c3                       ret        
004297fd cc                       int3       
004297fe cc                       int3       
004297ff cc                       int3       
