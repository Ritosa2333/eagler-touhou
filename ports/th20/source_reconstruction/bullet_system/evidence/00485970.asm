00485970 55                       push       ebp
00485971 8bec                     mov        ebp, esp
00485973 51                       push       ecx
00485974 894dfc                   mov        dword ptr [ebp - 4], ecx
00485977 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048597a 05646d2800               add        eax, 0x286d64
0048597f 8be5                     mov        esp, ebp
00485981 5d                       pop        ebp
00485982 c3                       ret        
00485983 cc                       int3       
00485984 cc                       int3       
00485985 cc                       int3       
00485986 cc                       int3       
00485987 cc                       int3       
00485988 cc                       int3       
00485989 cc                       int3       
0048598a cc                       int3       
0048598b cc                       int3       
0048598c cc                       int3       
0048598d cc                       int3       
0048598e cc                       int3       
0048598f cc                       int3       
