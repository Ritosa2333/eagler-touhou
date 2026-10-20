
// block 0040bc20
0040bc20  sub        esp, 8
0040bc23  push       esi
0040bc24  mov        esi, eax
0040bc26  mov        eax, dword ptr [esi + 0x7c4]
0040bc2c  cmp        dword ptr [esi + 0x7a0], eax
0040bc32  mov        dword ptr [esp + 8], eax
0040bc36  jl         0x40bd20

// block 0040bc3c
0040bc3c  mov        edx, dword ptr [esi + 0x544]
0040bc42  test       edx, edx
0040bc44  jl         0x40bc4b

// block 0040bc46
0040bc46  call       0x453d90

// block 0040bc4b
0040bc4b  fld        dword ptr [esi + 0x7b4]
0040bc51  inc        dword ptr [esi + 0x7cc]
0040bc57  sub        esp, 8
0040bc5a  lea        ecx, [esi + 0x4bc]
0040bc60  fstp       dword ptr [esp + 4]
0040bc64  call       0x4377a0

// block 0040bc69
0040bc69  fstp       dword ptr [esp]
0040bc6c  call       0x464640

// block 0040bc71
0040bc71  fstp       dword ptr [esi + 0x4d8]
0040bc77  fld        dword ptr [esi + 0x7b0]
0040bc7d  fstp       dword ptr [esp + 8]
0040bc81  fld        dword ptr [esp + 8]
0040bc85  fst        dword ptr [esi + 0x4d4]
0040bc8b  mov        eax, dword ptr [esi + 0x7ac]
0040bc91  fstp       dword ptr [esp + 8]
0040bc95  fldz       
0040bc97  test       al, 1
0040bc99  jne        0x40bcc8

// block 0040bc9b
0040bc9b  or         eax, 1
0040bc9e  fst        dword ptr [esi + 0x7a4]
0040bca4  mov        dword ptr [esi + 0x7a0], 0
0040bcae  mov        dword ptr [esi + 0x79c], 0xfff0bdc1
0040bcb8  mov        dword ptr [esi + 0x7a8], 0x4b2ed0
0040bcc2  mov        dword ptr [esi + 0x7ac], eax

// block 0040bcc8
0040bcc8  fstp       dword ptr [esi + 0x7a4]
0040bcce  mov        dword ptr [esi + 0x7a0], 0
0040bcd8  mov        dword ptr [esi + 0x79c], 0xffffffff
0040bce2  mov        eax, dword ptr [esi + 0x7cc]
0040bce8  cmp        eax, dword ptr [esi + 0x7c8]
0040bcee  jl         0x40bd38

// block 0040bcf0
0040bcf0  fld        dword ptr [esp + 8]
0040bcf4  sub        esp, 8
0040bcf7  fstp       dword ptr [esp + 4]
0040bcfb  lea        ecx, [esi + 0x4c8]
0040bd01  fld        dword ptr [esi + 0x4d8]
0040bd07  fstp       dword ptr [esp]
0040bd0a  call       0x40d640

// block 0040bd0f
0040bd0f  and        dword ptr [esi + 0x528], 0xffffffdf
0040bd16  mov        eax, 1
0040bd1b  pop        esi
0040bd1c  add        esp, 8
0040bd1f  ret        

// block 0040bd20
0040bd20  fld        dword ptr [esi + 0x4d4]
0040bd26  fld        dword ptr [esi + 0x7a4]
0040bd2c  fmul       st(1)
0040bd2e  fidiv      dword ptr [esp + 8]
0040bd32  fsubp      st(1)
0040bd34  fstp       dword ptr [esp + 8]

// block 0040bd38
0040bd38  fld        dword ptr [esp + 8]
0040bd3c  sub        esp, 8
0040bd3f  fstp       dword ptr [esp + 4]
0040bd43  lea        ecx, [esi + 0x4c8]
0040bd49  fld        dword ptr [esi + 0x4d8]
0040bd4f  fstp       dword ptr [esp]
0040bd52  call       0x40d640

// block 0040bd57
0040bd57  add        esi, 0x79c
0040bd5d  call       0x464a80

// block 0040bd62
0040bd62  xor        eax, eax
0040bd64  pop        esi
0040bd65  add        esp, 8
0040bd68  ret        
