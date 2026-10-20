
// block 00491181
00491181  mov        edi, edi
00491183  push       ebp
00491184  mov        ebp, esp
00491186  push       ecx
00491187  wait       
00491188  fnstcw     word ptr [ebp - 4]
0049118b  mov        eax, dword ptr [ebp + 0xc]
0049118e  mov        ecx, dword ptr [ebp + 8]
00491191  and        ecx, dword ptr [ebp + 0xc]
00491194  not        eax
00491196  and        eax, dword ptr [ebp - 4]
00491199  or         eax, ecx
0049119b  movzx      eax, ax
0049119e  mov        dword ptr [ebp + 0xc], eax
004911a1  fldcw      word ptr [ebp + 0xc]
004911a4  movsx      eax, word ptr [ebp - 4]
004911a8  leave      
004911a9  ret        
