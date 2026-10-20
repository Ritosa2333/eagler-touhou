
// block 004db651
004db651  mov        ebp, 0x57d3ca8b
004db656  jno        0x4db6bb

// block 004db658
004db658  je         0x4db67a

// block 004db65a
004db65a  jno        0x4db67d

// block 004db65c
004db65c  and        byte ptr [ecx], 0x7b
004db65f  hlt        

// block 004db6bb
004db6bb  shl        dword ptr [edx], 1
004db6bd  push       ebp
004db6be  out        0x60, al
004db6c0  sbb        edi, dword ptr [esp + ecx*8 - 0x38]
004db6c4  mov        ebx, 0x694e6a03
