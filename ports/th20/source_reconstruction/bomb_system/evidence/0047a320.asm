0047a320 55                       push       ebp
0047a321 8bec                     mov        ebp, esp
0047a323 51                       push       ecx
0047a324 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a327 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a32a d94018                   fld        dword ptr [eax + 0x18]
0047a32d 8be5                     mov        esp, ebp
0047a32f 5d                       pop        ebp
0047a330 c3                       ret        
0047a331 cc                       int3       
0047a332 cc                       int3       
0047a333 cc                       int3       
0047a334 cc                       int3       
0047a335 cc                       int3       
0047a336 cc                       int3       
0047a337 cc                       int3       
0047a338 cc                       int3       
0047a339 cc                       int3       
0047a33a cc                       int3       
0047a33b cc                       int3       
0047a33c cc                       int3       
0047a33d cc                       int3       
0047a33e cc                       int3       
0047a33f cc                       int3       
