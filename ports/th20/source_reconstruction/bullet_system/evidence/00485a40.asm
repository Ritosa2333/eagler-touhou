00485a40 55                       push       ebp
00485a41 8bec                     mov        ebp, esp
00485a43 51                       push       ecx
00485a44 894dfc                   mov        dword ptr [ebp - 4], ecx
00485a47 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485a4a 0fb680b0000000           movzx      eax, byte ptr [eax + 0xb0]
00485a51 8be5                     mov        esp, ebp
00485a53 5d                       pop        ebp
00485a54 c3                       ret        
00485a55 cc                       int3       
00485a56 cc                       int3       
00485a57 cc                       int3       
00485a58 cc                       int3       
00485a59 cc                       int3       
00485a5a cc                       int3       
00485a5b cc                       int3       
00485a5c cc                       int3       
00485a5d cc                       int3       
00485a5e cc                       int3       
00485a5f cc                       int3       
