00474e70 55                       push       ebp
00474e71 8bec                     mov        ebp, esp
00474e73 83ec0c                   sub        esp, 0xc
00474e76 894dfc                   mov        dword ptr [ebp - 4], ecx
00474e79 8b45fc                   mov        eax, dword ptr [ebp - 4]
00474e7c 83b86033000000           cmp        dword ptr [eax + 0x3360], 0
00474e83 753e                     jne        0x474ec3
00474e85 6a00                     push       0
00474e87 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00474e8a 81c164330000             add        ecx, 0x3364
00474e90 51                       push       ecx
00474e91 8b5508                   mov        edx, dword ptr [ebp + 8]
00474e94 52                       push       edx
00474e95 e806860400               call       0x4bd4a0
00474e9a 83c404                   add        esp, 4
00474e9d 50                       push       eax
00474e9e e8fdbbf9ff               call       0x410aa0
00474ea3 83c40c                   add        esp, 0xc
00474ea6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00474ea9 898160330000             mov        dword ptr [ecx + 0x3360], eax
00474eaf 8b55fc                   mov        edx, dword ptr [ebp - 4]
00474eb2 83ba6033000000           cmp        dword ptr [edx + 0x3360], 0
00474eb9 7508                     jne        0x474ec3
00474ebb 83c8ff                   or         eax, 0xffffffff
00474ebe e9d2010000               jmp        0x475095
00474ec3 68d0f85600               push       0x56f8d0
00474ec8 8b45fc                   mov        eax, dword ptr [ebp - 4]
00474ecb 8b8864330000             mov        ecx, dword ptr [eax + 0x3364]
00474ed1 51                       push       ecx
00474ed2 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00474ed8 e833a7faff               call       0x41f610
00474edd 8b55fc                   mov        edx, dword ptr [ebp - 4]
00474ee0 898224330000             mov        dword ptr [edx + 0x3324], eax
00474ee6 8b45fc                   mov        eax, dword ptr [ebp - 4]
00474ee9 8b8864330000             mov        ecx, dword ptr [eax + 0x3364]
00474eef 51                       push       ecx
00474ef0 8b55fc                   mov        edx, dword ptr [ebp - 4]
00474ef3 8b8260330000             mov        eax, dword ptr [edx + 0x3360]
00474ef9 50                       push       eax
00474efa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00474efd 8b9124330000             mov        edx, dword ptr [ecx + 0x3324]
00474f03 52                       push       edx
00474f04 e817ef0c00               call       0x543e20
00474f09 83c40c                   add        esp, 0xc
00474f0c 8b45fc                   mov        eax, dword ptr [ebp - 4]
00474f0f 8b8824330000             mov        ecx, dword ptr [eax + 0x3324]
00474f15 83c110                   add        ecx, 0x10
00474f18 51                       push       ecx
00474f19 8b55fc                   mov        edx, dword ptr [ebp - 4]
00474f1c 8b8258330000             mov        eax, dword ptr [edx + 0x3358]
00474f22 83e001                   and        eax, 1
00474f25 83c003                   add        eax, 3
00474f28 50                       push       eax
00474f29 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00474f2f e8fc9bfdff               call       0x44eb30
00474f34 8945f4                   mov        dword ptr [ebp - 0xc], eax
00474f37 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00474f3a 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00474f3d 899134330000             mov        dword ptr [ecx + 0x3334], edx
00474f43 837df400                 cmp        dword ptr [ebp - 0xc], 0
00474f47 751a                     jne        0x474f63
00474f49 681cf95600               push       0x56f91c
00474f4e 6878065c00               push       0x5c0678
00474f53 e8f8f1fdff               call       0x454150
00474f58 83c408                   add        esp, 8
00474f5b 83c8ff                   or         eax, 0xffffffff
00474f5e e932010000               jmp        0x475095
00474f63 b890000000               mov        eax, 0x90
00474f68 c1e000                   shl        eax, 0
00474f6b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00474f6e 038124330000             add        eax, dword ptr [ecx + 0x3324]
00474f74 50                       push       eax
00474f75 8b55fc                   mov        edx, dword ptr [ebp - 4]
00474f78 8b8228330000             mov        eax, dword ptr [edx + 0x3328]
00474f7e 50                       push       eax
00474f7f e87c97f9ff               call       0x40e700
00474f84 83c408                   add        esp, 8
00474f87 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00474f8a 898128330000             mov        dword ptr [ecx + 0x3328], eax
00474f90 8b55fc                   mov        edx, dword ptr [ebp - 4]
00474f93 8b8224330000             mov        eax, dword ptr [edx + 0x3324]
00474f99 8b4804                   mov        ecx, dword ptr [eax + 4]
00474f9c 8b55fc                   mov        edx, dword ptr [ebp - 4]
00474f9f 038a24330000             add        ecx, dword ptr [edx + 0x3324]
00474fa5 51                       push       ecx
00474fa6 8b45fc                   mov        eax, dword ptr [ebp - 4]
00474fa9 8b882c330000             mov        ecx, dword ptr [eax + 0x332c]
00474faf 51                       push       ecx
00474fb0 e84b97f9ff               call       0x40e700
00474fb5 83c408                   add        esp, 8
00474fb8 8b55fc                   mov        edx, dword ptr [ebp - 4]
00474fbb 89822c330000             mov        dword ptr [edx + 0x332c], eax
00474fc1 8b45fc                   mov        eax, dword ptr [ebp - 4]
00474fc4 8b8824330000             mov        ecx, dword ptr [eax + 0x3324]
00474fca 8b5108                   mov        edx, dword ptr [ecx + 8]
00474fcd 8b45fc                   mov        eax, dword ptr [ebp - 4]
00474fd0 039024330000             add        edx, dword ptr [eax + 0x3324]
00474fd6 52                       push       edx
00474fd7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00474fda 8b9130330000             mov        edx, dword ptr [ecx + 0x3330]
00474fe0 52                       push       edx
00474fe1 e81a97f9ff               call       0x40e700
00474fe6 83c408                   add        esp, 8
00474fe9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00474fec 898130330000             mov        dword ptr [ecx + 0x3330], eax
00474ff2 c745f800000000           mov        dword ptr [ebp - 8], 0
00474ff9 eb09                     jmp        0x475004
00474ffb 8b55f8                   mov        edx, dword ptr [ebp - 8]
00474ffe 83c201                   add        edx, 1
00475001 8955f8                   mov        dword ptr [ebp - 8], edx
00475004 8b45fc                   mov        eax, dword ptr [ebp - 4]
00475007 8b8824330000             mov        ecx, dword ptr [eax + 0x3324]
0047500d 0fbf11                   movsx      edx, word ptr [ecx]
00475010 3955f8                   cmp        dword ptr [ebp - 8], edx
00475013 7d29                     jge        0x47503e
00475015 8b45fc                   mov        eax, dword ptr [ebp - 4]
00475018 8b8828330000             mov        ecx, dword ptr [eax + 0x3328]
0047501e 8b55f8                   mov        edx, dword ptr [ebp - 8]
00475021 8b0491                   mov        eax, dword ptr [ecx + edx*4]
00475024 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00475027 038124330000             add        eax, dword ptr [ecx + 0x3324]
0047502d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00475030 8b8a28330000             mov        ecx, dword ptr [edx + 0x3328]
00475036 8b55f8                   mov        edx, dword ptr [ebp - 8]
00475039 890491                   mov        dword ptr [ecx + edx*4], eax
0047503c ebbd                     jmp        0x474ffb
0047503e 6854f95600               push       0x56f954
00475043 8b45fc                   mov        eax, dword ptr [ebp - 4]
00475046 8b8824330000             mov        ecx, dword ptr [eax + 0x3324]
0047504c 0fbf5102                 movsx      edx, word ptr [ecx + 2]
00475050 69c2e4050000             imul       eax, edx, 0x5e4
00475056 50                       push       eax
00475057 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0047505d e8aea5faff               call       0x41f610
00475062 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00475065 898120330000             mov        dword ptr [ecx + 0x3320], eax
0047506b 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047506e 8b8224330000             mov        eax, dword ptr [edx + 0x3324]
00475074 0fbf4802                 movsx      ecx, word ptr [eax + 2]
00475078 69d1e4050000             imul       edx, ecx, 0x5e4
0047507e 52                       push       edx
0047507f 6a00                     push       0
00475081 8b45fc                   mov        eax, dword ptr [ebp - 4]
00475084 8b8820330000             mov        ecx, dword ptr [eax + 0x3320]
0047508a 51                       push       ecx
0047508b e8d0f60c00               call       0x544760
00475090 83c40c                   add        esp, 0xc
00475093 33c0                     xor        eax, eax
00475095 8be5                     mov        esp, ebp
00475097 5d                       pop        ebp
00475098 c20400                   ret        4
0047509b cc                       int3       
0047509c cc                       int3       
0047509d cc                       int3       
0047509e cc                       int3       
0047509f cc                       int3       
