00429740 55                       push       ebp
00429741 8bec                     mov        ebp, esp
00429743 51                       push       ecx
00429744 894dfc                   mov        dword ptr [ebp - 4], ecx
00429747 8b45fc                   mov        eax, dword ptr [ebp - 4]
0042974a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0042974d f30f1000                 movss      xmm0, dword ptr [eax]
00429751 f30f5c01                 subss      xmm0, dword ptr [ecx]
00429755 8b55fc                   mov        edx, dword ptr [ebp - 4]
00429758 f30f1102                 movss      dword ptr [edx], xmm0
0042975c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0042975f 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00429762 f30f104004               movss      xmm0, dword ptr [eax + 4]
00429767 f30f5c4104               subss      xmm0, dword ptr [ecx + 4]
0042976c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0042976f f30f114204               movss      dword ptr [edx + 4], xmm0
00429774 8b45fc                   mov        eax, dword ptr [ebp - 4]
00429777 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0042977a f30f104008               movss      xmm0, dword ptr [eax + 8]
0042977f f30f5c4108               subss      xmm0, dword ptr [ecx + 8]
00429784 8b55fc                   mov        edx, dword ptr [ebp - 4]
00429787 f30f114208               movss      dword ptr [edx + 8], xmm0
0042978c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0042978f 8be5                     mov        esp, ebp
00429791 5d                       pop        ebp
00429792 c20400                   ret        4
00429795 cc                       int3       
00429796 cc                       int3       
00429797 cc                       int3       
00429798 cc                       int3       
00429799 cc                       int3       
0042979a cc                       int3       
0042979b cc                       int3       
0042979c cc                       int3       
0042979d cc                       int3       
0042979e cc                       int3       
0042979f cc                       int3       
