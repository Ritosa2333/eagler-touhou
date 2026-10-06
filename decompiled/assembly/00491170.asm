
// block 00491170
00491170  mov        edi, edi
00491172  push       ebp
00491173  mov        ebp, esp
00491175  push       ecx
00491176  fnstsw     word ptr [ebp - 4]
00491179  fnclex     
0049117b  movsx      eax, word ptr [ebp - 4]
0049117f  leave      
00491180  ret        
