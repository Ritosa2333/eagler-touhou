
// block 004db244
004db244  or         byte ptr [esi - 0x4849f508], cl
004db24a  and        esp, dword ptr [esi + edx*4 - 0x43]
004db24e  push       ebx
004db24f  in         al, 0x95
004db251  rcr        dword ptr [esi - 0x7bb967db], 1
004db257  nop        
004db258  push       esp
004db259  js         0x4db2bf
