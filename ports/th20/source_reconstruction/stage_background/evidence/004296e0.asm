004296e0 55                       push       ebp
004296e1 8bec                     mov        ebp, esp
004296e3 51                       push       ecx
004296e4 894dfc                   mov        dword ptr [ebp - 4], ecx
004296e7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004296ea 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004296ed f30f1000                 movss      xmm0, dword ptr [eax]
004296f1 f30f5801                 addss      xmm0, dword ptr [ecx]
004296f5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004296f8 f30f1102                 movss      dword ptr [edx], xmm0
004296fc 8b45fc                   mov        eax, dword ptr [ebp - 4]
004296ff 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00429702 f30f104004               movss      xmm0, dword ptr [eax + 4]
00429707 f30f584104               addss      xmm0, dword ptr [ecx + 4]
0042970c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0042970f f30f114204               movss      dword ptr [edx + 4], xmm0
00429714 8b45fc                   mov        eax, dword ptr [ebp - 4]
00429717 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0042971a f30f104008               movss      xmm0, dword ptr [eax + 8]
0042971f f30f584108               addss      xmm0, dword ptr [ecx + 8]
00429724 8b55fc                   mov        edx, dword ptr [ebp - 4]
00429727 f30f114208               movss      dword ptr [edx + 8], xmm0
0042972c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0042972f 8be5                     mov        esp, ebp
00429731 5d                       pop        ebp
00429732 c20400                   ret        4
00429735 cc                       int3       
00429736 cc                       int3       
00429737 cc                       int3       
00429738 cc                       int3       
00429739 cc                       int3       
0042973a cc                       int3       
0042973b cc                       int3       
0042973c cc                       int3       
0042973d cc                       int3       
0042973e cc                       int3       
0042973f cc                       int3       
