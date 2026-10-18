00478260 55                       push       ebp
00478261 8bec                     mov        ebp, esp
00478263 51                       push       ecx
00478264 894dfc                   mov        dword ptr [ebp - 4], ecx
00478267 8b4508                   mov        eax, dword ptr [ebp + 8]
0047826a 83e001                   and        eax, 1
0047826d c1e005                   shl        eax, 5
00478270 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478273 8b9158030000             mov        edx, dword ptr [ecx + 0x358]
00478279 83e2df                   and        edx, 0xffffffdf
0047827c 0bd0                     or         edx, eax
0047827e 8b45fc                   mov        eax, dword ptr [ebp - 4]
00478281 899058030000             mov        dword ptr [eax + 0x358], edx
00478287 8be5                     mov        esp, ebp
00478289 5d                       pop        ebp
0047828a c20400                   ret        4
0047828d cc                       int3       
0047828e cc                       int3       
0047828f cc                       int3       
