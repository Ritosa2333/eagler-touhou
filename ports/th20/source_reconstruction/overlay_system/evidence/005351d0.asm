005351d0 55                       push       ebp
005351d1 8bec                     mov        ebp, esp
005351d3 51                       push       ecx
005351d4 894dfc                   mov        dword ptr [ebp - 4], ecx
005351d7 8b4508                   mov        eax, dword ptr [ebp + 8]
005351da c7801c010000e0e54000     mov        dword ptr [eax + 0x11c], 0x40e5e0
005351e4 8be5                     mov        esp, ebp
005351e6 5d                       pop        ebp
005351e7 c20800                   ret        8
005351ea cc                       int3       
005351eb cc                       int3       
005351ec cc                       int3       
005351ed cc                       int3       
005351ee cc                       int3       
005351ef cc                       int3       
