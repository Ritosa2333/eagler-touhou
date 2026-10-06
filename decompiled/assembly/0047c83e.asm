
// block 0047c83e
0047c83e  mov        edi, edi
0047c840  push       ebp
0047c841  mov        ebp, esp
0047c843  push       dword ptr [ebp + 0xc]
0047c846  push       0x103
0047c84b  push       dword ptr [ebp + 8]
0047c84e  call       0x48d524

// block 0047c853
0047c853  add        esp, 0xc
0047c856  test       eax, eax
0047c858  jne        0x47c863

// block 0047c85a
0047c85a  cmp        word ptr [ebp + 8], 0x5f
0047c85f  je         0x47c863

// block 0047c861
0047c861  pop        ebp
0047c862  ret        

// block 0047c863
0047c863  xor        eax, eax
0047c865  inc        eax
0047c866  pop        ebp
0047c867  ret        
