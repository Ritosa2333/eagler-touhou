
// block 00495db5
00495db5  fstp       st(0)
00495db7  or         cl, cl
00495db9  je         0x495dc6

// block 00495dbb
00495dbb  fstp       st(0)
00495dbd  fldpi      
00495dbf  or         ch, ch
00495dc1  je         0x495dc5

// block 00495dc3
00495dc3  fchs       

// block 00495dc5
00495dc5  ret        

// block 00495dc6
00495dc6  fstp       st(0)
00495dc8  fldz       
00495dca  or         ch, ch
00495dcc  je         0x495dc5

// block 00495dce
00495dce  fchs       
00495dd0  ret        
