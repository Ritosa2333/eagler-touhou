0052fb80 55                       push       ebp
0052fb81 8bec                     mov        ebp, esp
0052fb83 51                       push       ecx
0052fb84 894dfc                   mov        dword ptr [ebp - 4], ecx
0052fb87 6a00                     push       0
0052fb89 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052fb8c 83c114                   add        ecx, 0x14
0052fb8f e88c39efff               call       0x423520
0052fb94 6a00                     push       0
0052fb96 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052fb99 83c124                   add        ecx, 0x24
0052fb9c e87f39efff               call       0x423520
0052fba1 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052fba4 c6403400                 mov        byte ptr [eax + 0x34], 0
0052fba8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052fbab c6413500                 mov        byte ptr [ecx + 0x35], 0
0052fbaf 8be5                     mov        esp, ebp
0052fbb1 5d                       pop        ebp
0052fbb2 c3                       ret        
0052fbb3 cc                       int3       
0052fbb4 cc                       int3       
0052fbb5 cc                       int3       
0052fbb6 cc                       int3       
0052fbb7 cc                       int3       
0052fbb8 cc                       int3       
0052fbb9 cc                       int3       
0052fbba cc                       int3       
0052fbbb cc                       int3       
0052fbbc cc                       int3       
0052fbbd cc                       int3       
0052fbbe cc                       int3       
0052fbbf cc                       int3       
