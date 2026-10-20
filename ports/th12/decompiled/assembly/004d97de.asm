
// block 004d97de
004d97de  push       es
004d97df  dec        ebx
004d97e0  mov        edx, 0xf628a050
004d97e5  xor        eax, 0x78cc001a
004d97ea  inc        edx
004d97eb  cmpsd      dword ptr [esi], dword ptr es:[edi]
004d97ec  fsub       dword ptr [edx]
004d97ee  adc        al, 0xb
004d97f0  sub        eax, 0x87ebb635
004d97f5  je         0x4d978d

// block 004d97f7
004d97f7  dec        esp
004d97f8  ficom      word ptr [eax - 0x5d]
004d97fb  sti        

// block 004d97fc
004d97fc  pop        eax
004d97fd  add        edx, dword ptr [ebx]
004d97ff  cli        

// block 004d9800
004d9800  dec        ebx
004d9801  dec        edx
004d9802  inc        esp
004d9803  js         0x4d980d

// block 004d9805
004d9805  add        dword ptr [eax], 0x51ae9824
004d980b  pop        eax
004d980c  push       -0x67
004d980e  and        ebx, dword ptr [ebp + 0x51]
004d9811  sub        dword ptr [edi], ebp
004d9813  cmp        eax, 0xb8cff34a
004d9818  and        byte ptr [edi], dl
004d981a  sbb        dh, byte ptr [edx - 0x21]

// block 004d981d
004d981d  fdivr      qword ptr [ecx - 0x5d5e52c6]
004d9823  imul       esi, dword ptr [ebx + 0x2c], 0x9c260e7
004d982a  xor        ch, byte ptr [ebp + 0x48]
004d982d  cmp        cl, ch
004d982f  mov        ebp, 0xae1201c1
