0045e6d0 55                       push       ebp
0045e6d1 8bec                     mov        ebp, esp
0045e6d3 51                       push       ecx
0045e6d4 894dfc                   mov        dword ptr [ebp - 4], ecx
0045e6d7 8b4508                   mov        eax, dword ptr [ebp + 8]
0045e6da 50                       push       eax
0045e6db 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045e6de e87d84feff               call       0x446b60
0045e6e3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045e6e6 c701dced5600             mov        dword ptr [ecx], 0x56eddc
0045e6ec 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045e6ef 83c108                   add        ecx, 8
0045e6f2 e879ffffff               call       0x45e670
0045e6f7 8b55fc                   mov        edx, dword ptr [ebp - 4]
0045e6fa 0f57c0                   xorps      xmm0, xmm0
0045e6fd f30f1182480b0000         movss      dword ptr [edx + 0xb48], xmm0
0045e705 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045e708 81c14c0b0000             add        ecx, 0xb4c
0045e70e e87d46fcff               call       0x422d90
0045e713 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045e716 c7805c0b000000000000     mov        dword ptr [eax + 0xb5c], 0
0045e720 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045e723 8be5                     mov        esp, ebp
0045e725 5d                       pop        ebp
0045e726 c20400                   ret        4
0045e729 cc                       int3       
0045e72a cc                       int3       
0045e72b cc                       int3       
0045e72c cc                       int3       
0045e72d cc                       int3       
0045e72e cc                       int3       
0045e72f cc                       int3       
