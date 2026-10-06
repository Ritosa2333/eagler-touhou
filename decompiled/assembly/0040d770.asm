
// block 0040d770
0040d770  push       esi
0040d771  lea        esi, [eax + ecx + 0x16]
0040d775  add        eax, 0x42
0040d778  imul       esi, esi, 0x3e8
0040d77e  cdq        
0040d77f  push       edi
0040d780  mov        edi, 0x3e8
0040d785  idiv       edi
0040d787  lea        eax, [ecx + 0x21]
0040d78a  mov        ecx, 0x64
0040d78f  pop        edi
0040d790  add        esi, edx
0040d792  cdq        
0040d793  imul       esi, esi, 0x64
0040d796  idiv       ecx
0040d798  add        esi, edx
0040d79a  mov        edx, dword ptr [esp + 8]
0040d79e  mov        dword ptr [edx + 0xa8], esi
0040d7a4  pop        esi
0040d7a5  ret        4
