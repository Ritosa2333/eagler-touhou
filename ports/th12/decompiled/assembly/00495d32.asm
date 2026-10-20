
// block 00495d32
00495d32  mov        edi, edi
00495d34  lea        esp, [esp]
00495d3b  add        eax, 0
00495d40  call       0x495d7c

// block 00495d45
00495d45  xchg       cl, ch
00495d47  jmp        0x495d6b

// block 00495d6b
00495d6b  fpatan     
00495d6d  or         cl, cl
00495d6f  je         0x495d75

// block 00495d71
00495d71  fldpi      
00495d73  fsubrp     st(1)

// block 00495d75
00495d75  or         ch, ch
00495d77  je         0x495d7b

// block 00495d79
00495d79  fchs       

// block 00495d7b
00495d7b  ret        
