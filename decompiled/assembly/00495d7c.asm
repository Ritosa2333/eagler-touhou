
// block 00495d7c
00495d7c  fabs       
00495d7e  fld        st(0)
00495d80  fld        st(0)
00495d82  fld1       
00495d84  fsubrp     st(1)
00495d86  fxch       st(1)
00495d88  fld1       
00495d8a  faddp      st(1)
00495d8c  fmulp      st(1)
00495d8e  ftst       
00495d90  wait       
00495d91  fnstsw     word ptr [ebp - 0xa0]
00495d97  wait       
00495d98  test       byte ptr [ebp - 0x9f], 1
00495d9f  jne        0x495da6

// block 00495da1
00495da1  xor        ch, ch
00495da3  fsqrt      
00495da5  ret        

// block 00495da6
00495da6  pop        eax
00495da7  jmp        0x4942e6
