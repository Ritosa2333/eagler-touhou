005361f0 55                       push       ebp
005361f1 8bec                     mov        ebp, esp
005361f3 51                       push       ecx
005361f4 894dfc                   mov        dword ptr [ebp - 4], ecx
005361f7 8b4508                   mov        eax, dword ptr [ebp + 8]
005361fa c7801c010000c06a5300     mov        dword ptr [eax + 0x11c], 0x536ac0
00536204 8be5                     mov        esp, ebp
00536206 5d                       pop        ebp
00536207 c20800                   ret        8
0053620a cc                       int3       
0053620b cc                       int3       
0053620c cc                       int3       
0053620d cc                       int3       
0053620e cc                       int3       
0053620f cc                       int3       
