
// block 0048a22e
0048a22e  mov        edi, edi
0048a230  push       ebp
0048a231  mov        ebp, esp
0048a233  fldz       
0048a235  mov        eax, dword ptr [ebp + 8]
0048a238  fcomp      qword ptr [eax]
0048a23a  fnstsw     ax
0048a23c  test       ah, 0x41
0048a23f  jp         0x48a246

// block 0048a241
0048a241  xor        eax, eax
0048a243  inc        eax
0048a244  pop        ebp
0048a245  ret        

// block 0048a246
0048a246  xor        eax, eax
0048a248  pop        ebp
0048a249  ret        
