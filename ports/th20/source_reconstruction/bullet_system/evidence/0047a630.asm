0047a630 55                       push       ebp
0047a631 8bec                     mov        ebp, esp
0047a633 51                       push       ecx
0047a634 f30f5a4508               cvtss2sd   xmm0, dword ptr [ebp + 8]
0047a639 83ec08                   sub        esp, 8
0047a63c f20f110424               movsd      qword ptr [esp], xmm0
0047a641 e8ea5b0d00               call       0x550230
0047a646 83c408                   add        esp, 8
0047a649 d95dfc                   fstp       dword ptr [ebp - 4]
0047a64c d945fc                   fld        dword ptr [ebp - 4]
0047a64f 8be5                     mov        esp, ebp
0047a651 5d                       pop        ebp
0047a652 c3                       ret        
0047a653 cc                       int3       
0047a654 cc                       int3       
0047a655 cc                       int3       
0047a656 cc                       int3       
0047a657 cc                       int3       
0047a658 cc                       int3       
0047a659 cc                       int3       
0047a65a cc                       int3       
0047a65b cc                       int3       
0047a65c cc                       int3       
0047a65d cc                       int3       
0047a65e cc                       int3       
0047a65f cc                       int3       
