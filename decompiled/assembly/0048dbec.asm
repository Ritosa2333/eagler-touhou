
// block 0048dbec
0048dbec  mov        edi, edi
0048dbee  push       ebp
0048dbef  mov        ebp, esp
0048dbf1  xor        eax, eax
0048dbf3  cmp        dword ptr [ebp + 0x18], 0xa
0048dbf7  jne        0x48dc08

// block 0048dbf9
0048dbf9  cmp        dword ptr [ebp + 0xc], eax
0048dbfc  jg         0x48dc08

// block 0048dbfe
0048dbfe  jl         0x48dc05

// block 0048dc00
0048dc00  cmp        dword ptr [ebp + 8], eax
0048dc03  jae        0x48dc08

// block 0048dc05
0048dc05  xor        eax, eax
0048dc07  inc        eax

// block 0048dc08
0048dc08  push       edi
0048dc09  mov        edi, dword ptr [ebp + 0x10]
0048dc0c  push       eax
0048dc0d  push       dword ptr [ebp + 0x18]
0048dc10  push       dword ptr [ebp + 0x14]
0048dc13  push       dword ptr [ebp + 0xc]
0048dc16  push       dword ptr [ebp + 8]
0048dc19  call       0x48daf4

// block 0048dc1e
0048dc1e  pop        edi
0048dc1f  pop        ebp
0048dc20  ret        
