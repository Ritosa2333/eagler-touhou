0045d5b0 55                       push       ebp
0045d5b1 8bec                     mov        ebp, esp
0045d5b3 51                       push       ecx
0045d5b4 894dfc                   mov        dword ptr [ebp - 4], ecx
0045d5b7 8b4508                   mov        eax, dword ptr [ebp + 8]
0045d5ba 50                       push       eax
0045d5bb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045d5be e89d95feff               call       0x446b60
0045d5c3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045d5c6 c701a4ec5600             mov        dword ptr [ecx], 0x56eca4
0045d5cc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045d5cf 83c108                   add        ecx, 8
0045d5d2 e879ffffff               call       0x45d550
0045d5d7 8b55fc                   mov        edx, dword ptr [ebp - 4]
0045d5da 0f57c0                   xorps      xmm0, xmm0
0045d5dd f30f1182c8030000         movss      dword ptr [edx + 0x3c8], xmm0
0045d5e5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045d5e8 81c1cc030000             add        ecx, 0x3cc
0045d5ee e89d57fcff               call       0x422d90
0045d5f3 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045d5f6 c780dc03000000000000     mov        dword ptr [eax + 0x3dc], 0
0045d600 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045d603 8be5                     mov        esp, ebp
0045d605 5d                       pop        ebp
0045d606 c20400                   ret        4
0045d609 cc                       int3       
0045d60a cc                       int3       
0045d60b cc                       int3       
0045d60c cc                       int3       
0045d60d cc                       int3       
0045d60e cc                       int3       
0045d60f cc                       int3       
