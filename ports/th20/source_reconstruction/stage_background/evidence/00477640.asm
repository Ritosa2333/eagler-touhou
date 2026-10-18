00477640 55                       push       ebp
00477641 8bec                     mov        ebp, esp
00477643 51                       push       ecx
00477644 894dfc                   mov        dword ptr [ebp - 4], ecx
00477647 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047764a c780900ed40701000000     mov        dword ptr [eax + 0x7d40e90], 1
00477654 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477657 8b5508                   mov        edx, dword ptr [ebp + 8]
0047765a 89918c0ed407             mov        dword ptr [ecx + 0x7d40e8c], edx
00477660 8be5                     mov        esp, ebp
00477662 5d                       pop        ebp
00477663 c20400                   ret        4
00477666 cc                       int3       
00477667 cc                       int3       
00477668 cc                       int3       
00477669 cc                       int3       
0047766a cc                       int3       
0047766b cc                       int3       
0047766c cc                       int3       
0047766d cc                       int3       
0047766e cc                       int3       
0047766f cc                       int3       
