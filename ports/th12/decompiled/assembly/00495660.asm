
// block 00495660
00495660  fld        st(0)
00495662  frndint    
00495664  fsubr      st(1), st(0)
00495666  fxch       st(1)
00495668  fchs       
0049566a  f2xm1      
0049566c  fld1       
0049566e  faddp      st(1)
00495670  fscale     
00495672  fstp       st(1)
00495674  ret        
