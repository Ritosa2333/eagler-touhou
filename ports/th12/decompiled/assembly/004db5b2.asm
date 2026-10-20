
// block 004db5b2
004db5b2  rcr        edx, 1
004db5b4  je         0x4db56d

// block 004db5b6
004db5b6  clc        
004db5b7  add        ah, byte ptr [edi + 0x77]
004db5ba  sti        

// block 004db5bb
004db5bb  add        byte ptr [ebx], dl
004db5bd  stc        
004db5be  and        byte ptr [esi - 0x67], cl
