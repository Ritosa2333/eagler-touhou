
// block 004779b0
004779b0  mov        edi, edi
004779b2  push       ebp
004779b3  mov        ebp, esp
004779b5  mov        ecx, dword ptr [ebp + 8]
004779b8  mov        eax, 0x5a4d
004779bd  cmp        word ptr [ecx], ax
004779c0  je         0x4779c6

// block 004779c2
004779c2  xor        eax, eax
004779c4  pop        ebp
004779c5  ret        

// block 004779c6
004779c6  mov        eax, dword ptr [ecx + 0x3c]
004779c9  add        eax, ecx
004779cb  cmp        dword ptr [eax], 0x4550
004779d1  jne        0x4779c2

// block 004779d3
004779d3  xor        edx, edx
004779d5  mov        ecx, 0x10b
004779da  cmp        word ptr [eax + 0x18], cx
004779de  sete       dl
004779e1  mov        eax, edx
004779e3  pop        ebp
004779e4  ret        
