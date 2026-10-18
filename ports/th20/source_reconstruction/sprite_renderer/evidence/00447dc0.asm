00447dc0 55                       push       ebp
00447dc1 8bec                     mov        ebp, esp
00447dc3 51                       push       ecx
00447dc4 894dfc                   mov        dword ptr [ebp - 4], ecx
00447dc7 6a40                     push       0x40
00447dc9 6a00                     push       0
00447dcb 8b45fc                   mov        eax, dword ptr [ebp - 4]
00447dce 50                       push       eax
00447dcf e88cc90f00               call       0x544760
00447dd4 83c40c                   add        esp, 0xc
00447dd7 8b45fc                   mov        eax, dword ptr [ebp - 4]
00447dda 8be5                     mov        esp, ebp
00447ddc 5d                       pop        ebp
00447ddd c3                       ret        
00447dde cc                       int3       
00447ddf cc                       int3       
