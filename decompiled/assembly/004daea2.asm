
// block 004daea2
004daea2  in         eax, dx
004daea3  stosb      byte ptr es:[edi], al
004daea4  fldln2     
004daea6  mov        ds, word ptr [ebx - 0x4e71f2fa]
004daeac  std        
004daead  sbb        esp, dword ptr [eax - 0x13]
004daeb0  jg         0x4daee3

// block 004daeb2
004daeb2  push       edx
004daeb3  das        

// block 004daeb4
004daeb4  mov        ch, 0x38
004daeb6  push       esp
004daeb7  push       ebp
004daeb8  or         eax, 0x27f72a20
004daebd  loopne     0x4daeb2

// block 004daebf
004daebf  xchg       edi, eax
004daec0  dec        ecx
004daec1  adc        al, 0x66
004daec3  loop       0x4daee7

// block 004daec5
004daec5  dec        esp
004daec6  adc        al, 0x53
004daec9  int3       
