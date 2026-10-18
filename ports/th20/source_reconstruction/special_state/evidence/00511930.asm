00511930 55                       push       ebp
00511931 8bec                     mov        ebp, esp
00511933 51                       push       ecx
00511934 894dfc                   mov        dword ptr [ebp - 4], ecx
00511937 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051193a c60000                   mov        byte ptr [eax], 0
0051193d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511940 c6410100                 mov        byte ptr [ecx + 1], 0
00511944 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511947 c6420200                 mov        byte ptr [edx + 2], 0
0051194b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051194e c6400300                 mov        byte ptr [eax + 3], 0
00511952 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511955 c6410400                 mov        byte ptr [ecx + 4], 0
00511959 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051195c 83c108                   add        ecx, 8
0051195f e82c14f1ff               call       0x422d90
00511964 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511967 c7421800000000           mov        dword ptr [edx + 0x18], 0
0051196e 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511971 c7401c00000000           mov        dword ptr [eax + 0x1c], 0
00511978 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051197b 8be5                     mov        esp, ebp
0051197d 5d                       pop        ebp
0051197e c3                       ret        
0051197f cc                       int3       
