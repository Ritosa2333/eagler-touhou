
// block 004daa96
004daa96  cmp        esp, ebx
004daa98  cdq        
004daa99  xchg       ecx, eax
004daa9a  cmpsb      byte ptr [esi], byte ptr es:[edi]
004daa9b  and        al, 0xbd
004daa9d  int3       

// block 004daaa2
004daaa2  out        dx, al
004daaa3  ja         0x4daa62

// block 004daaa5
004daaa5  cmp        eax, 0xc7cc72ef
004daaaa  loop       0x4daa96

// block 004daaac
004daaac  jne        0x4daad3

// block 004daaae
004daaae  inc        ecx

// block 004daaaf
004daaaf  rol        byte ptr [ebx - 0x7bbdc8a3], 1
004daab5  push       ss
004daab6  mov        byte ptr [eax - 0x2f5abb75], ah
004daabc  xchg       ebp, eax
004daabd  sub        esp, dword ptr [ecx + 0x7eb4d015]
004daac3  add        dword ptr [0x7441e816], eax
004daac9  mov        al, byte ptr [0x3d97ae88]
004daace  ror        cl, cl
004daad0  mov        al, ch

// block 004daad3
004daad3  xchg       ebp, eax
004daad4  sub        al, ch
004daad6  mov        dh, 0xeb
004daad8  jno        0x4daaaf

// block 004daada
004daada  pushal     
004daadb  mov        edx, 0x63868225
004daae0  adc        dword ptr [ecx - 0x4d1bb79b], edi
004daae6  add        eax, 0x65268acc
004daaeb  clc        
004daaec  mov        cs, word ptr [ecx + 0x29]
004daaef  imul       esp, dword ptr [ebx], 0x430c08f1
004daaf5  adc        byte ptr [ebp + eax*8 + 8], dl
004daaf9  sbb        dword ptr [edx + 0x14], esp
