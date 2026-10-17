00484850 55                       push       ebp
00484851 8bec                     mov        ebp, esp
00484853 83ec0c                   sub        esp, 0xc
00484856 894dfc                   mov        dword ptr [ebp - 4], ecx
00484859 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048485c 8b88b4000000             mov        ecx, dword ptr [eax + 0xb4]
00484862 034d08                   add        ecx, dword ptr [ebp + 8]
00484865 8b55fc                   mov        edx, dword ptr [ebp - 4]
00484868 898ab4000000             mov        dword ptr [edx + 0xb4], ecx
0048486e c745f864000000           mov        dword ptr [ebp - 8], 0x64
00484875 c745f400000000           mov        dword ptr [ebp - 0xc], 0
0048487c 8d45f8                   lea        eax, [ebp - 8]
0048487f 50                       push       eax
00484880 8d4df4                   lea        ecx, [ebp - 0xc]
00484883 51                       push       ecx
00484884 8b55fc                   mov        edx, dword ptr [ebp - 4]
00484887 81c2b4000000             add        edx, 0xb4
0048488d 52                       push       edx
0048488e e8ddc8feff               call       0x471170
00484893 83c40c                   add        esp, 0xc
00484896 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00484899 8b10                     mov        edx, dword ptr [eax]
0048489b 8991b4000000             mov        dword ptr [ecx + 0xb4], edx
004848a1 8be5                     mov        esp, ebp
004848a3 5d                       pop        ebp
004848a4 c20400                   ret        4
004848a7 cc                       int3       
004848a8 cc                       int3       
004848a9 cc                       int3       
004848aa cc                       int3       
004848ab cc                       int3       
004848ac cc                       int3       
004848ad cc                       int3       
004848ae cc                       int3       
004848af cc                       int3       
