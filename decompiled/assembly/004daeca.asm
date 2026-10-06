
// block 004daeca
004daeca  fsubr      qword ptr [edx]
004daecc  shr        dword ptr [ebx - 0x68b697b5], cl
004daed2  pop        esi
004daed3  and        cl, byte ptr [edi + ecx*4]
004daed6  push       -5
004daed8  out        dx, al
004daed9  pop        edx
004daeda  ljmp       0x5a37:0x9e01da63
