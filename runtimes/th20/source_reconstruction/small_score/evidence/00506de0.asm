00506de0 55                       push       ebp
00506de1 8bec                     mov        ebp, esp
00506de3 51                       push       ecx
00506de4 894dfc                   mov        dword ptr [ebp - 4], ecx
00506de7 8b45fc                   mov        eax, dword ptr [ebp - 4]
00506dea f30f104508               movss      xmm0, dword ptr [ebp + 8]
00506def f30f114070               movss      dword ptr [eax + 0x70], xmm0
00506df4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00506df7 8b919c040000             mov        edx, dword ptr [ecx + 0x49c]
00506dfd 83ca04                   or         edx, 4
00506e00 8b45fc                   mov        eax, dword ptr [ebp - 4]
00506e03 89909c040000             mov        dword ptr [eax + 0x49c], edx
00506e09 8be5                     mov        esp, ebp
00506e0b 5d                       pop        ebp
00506e0c c20400                   ret        4
00506e0f cc                       int3       
