
// block 00491160
00491160  mov        edi, edi
00491162  push       ebp
00491163  mov        ebp, esp
00491165  push       ecx
00491166  wait       
00491167  fnstsw     word ptr [ebp - 4]
0049116a  movsx      eax, word ptr [ebp - 4]
0049116e  leave      
0049116f  ret        
