
// block 004da47e
004da47e  std        
004da47f  xor        dl, byte ptr [esi - 0x35806743]

// block 004da4a2
004da4a2  salc       
004da4a3  xor        al, 0x19
004da4a5  je         0x4da51a

// block 004da4a7
004da4a7  shr        byte ptr [0xdbd911db], 1
004da4ad  push       ebp
004da4ae  jbe        0x4da47e

// block 004da4b0
004da4b0  xor        al, 0xd0
004da4b2  or         al, 0xdb
004da4b4  out        0x47, al

// block 004da4bb
004da4bb  cmp        byte ptr [ebp + 0xc6ce610], ah
004da4c1  lahf       
004da4c2  fsub       st(2), st(0)
004da4c4  movsb      byte ptr es:[edi], byte ptr [esi]
004da4c5  shl        esp, cl
004da4c7  and        ah, byte ptr [ebp - 0x14ed9bb2]

// block 004da4cd
004da4cd  out        0x5f, al
004da4cf  mov        ecx, 0x80ab644a
004da4d5  scasb      al, byte ptr es:[edi]
004da4d6  salc       

// block 004da511
004da511  jl         0x4da4cd

// block 004da513
004da513  xor        byte ptr [edi - 0x16], bl
004da516  pop        ebp
004da517  out        0xf8, al
004da519  in         al, dx

// block 004da51a
004da51a  div        edi
004da51c  popfd      
004da51d  out        0xff, eax
004da51f  jge        0x4da4bb

// block 004da521
004da521  xchg       ebp, eax
004da522  mov        edx, 0xe5febe14
004da527  jge        0x4da511
