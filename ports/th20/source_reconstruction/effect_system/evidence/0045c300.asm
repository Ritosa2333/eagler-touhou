0045c300 55                       push       ebp
0045c301 8bec                     mov        ebp, esp
0045c303 51                       push       ecx
0045c304 894dfc                   mov        dword ptr [ebp - 4], ecx
0045c307 8b4508                   mov        eax, dword ptr [ebp + 8]
0045c30a 50                       push       eax
0045c30b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045c30e e88dd4fcff               call       0x4297a0
0045c313 90                       nop        
0045c314 8be5                     mov        esp, ebp
0045c316 5d                       pop        ebp
0045c317 c20400                   ret        4
0045c31a cc                       int3       
0045c31b cc                       int3       
0045c31c cc                       int3       
0045c31d cc                       int3       
0045c31e cc                       int3       
0045c31f cc                       int3       
