
// block 004da0aa
004da0aa  add        byte ptr [0xde60e7f9], al

// block 004da0f6
004da0f6  or         al, 0xd7

// block 004da110
004da110  jb         0x4da0aa

// block 004da112
004da112  jge        0x4da0f6

// block 004da114
004da114  push       cs
004da115  xor        cl, byte ptr ss:[ebx]
004da118  fdiv       dword ptr [edi + 0x229b86f6]
004da11f  std        
004da120  sub        dword ptr [edi], -0x26
004da123  mov        dh, 0x2e
004da125  lodsd      eax, dword ptr [esi]
004da126  dec        ebp
004da127  inc        eax
004da128  lea        edi, [edi]
004da12a  or         cl, byte ptr [ebx + ecx*2 - 0x6f]
004da12e  in         al, dx
004da12f  pop        ebx
004da130  and        edi, dword ptr [edx + 0x289ebd46]
