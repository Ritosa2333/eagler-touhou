0052fbc0 55                       push       ebp
0052fbc1 8bec                     mov        ebp, esp
0052fbc3 51                       push       ecx
0052fbc4 894dfc                   mov        dword ptr [ebp - 4], ecx
0052fbc7 8be5                     mov        esp, ebp
0052fbc9 5d                       pop        ebp
0052fbca c20c00                   ret        0xc
0052fbcd cc                       int3       
0052fbce cc                       int3       
0052fbcf cc                       int3       
