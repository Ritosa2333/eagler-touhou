
// block 00429150
00429150  sub        esp, 0x10
00429153  push       esi
00429154  mov        esi, ecx
00429156  fld        dword ptr [esi + 0x6c]
00429159  push       edi
0042915a  sub        esp, 8
0042915d  fstp       dword ptr [esp + 4]
00429161  lea        ecx, [esp + 0x14]
00429165  fld        dword ptr [esi + 0x68]
00429168  fstp       dword ptr [esp]
0042916b  call       0x42e880

// block 00429170
00429170  fld        dword ptr [esi + 0x50]
00429173  fadd       dword ptr [esp + 0xc]
00429177  fstp       dword ptr [esp + 0xc]
0042917b  fld        dword ptr [esi + 0x54]
0042917e  fadd       dword ptr [esp + 0x10]
00429182  fstp       dword ptr [esp + 0x10]
00429186  fldz       
00429188  fst        dword ptr [esp + 0x14]
0042918c  fld        dword ptr [esp + 0xc]
00429190  fld        st(0)
00429192  fldz       
00429194  fadd       st(1), st(0)
00429196  fxch       st(1)
00429198  fcomp      qword ptr [0x4a3d60]
0042919e  fnstsw     ax
004291a0  fld        dword ptr [esp + 0x10]
004291a4  test       ah, 0x41
004291a7  jnp        0x4291e2

// block 004291a9
004291a9  fxch       st(2)
004291ab  fsub       st(1)
004291ad  fcomp      qword ptr [0x4a3d28]
004291b3  fnstsw     ax
004291b5  test       ah, 1
004291b8  je         0x4291e8

// block 004291ba
004291ba  fld        st(1)
004291bc  fadd       st(1)
004291be  fcomp      st(1)
004291c0  fnstsw     ax
004291c2  test       ah, 0x41
004291c5  jnp        0x4291e8

// block 004291c7
004291c7  fsubr      st(1)
004291c9  fcomp      qword ptr [0x4a3d50]
004291cf  fnstsw     ax
004291d1  test       ah, 1
004291d4  je         0x4291ea

// block 004291d6
004291d6  fstp       st(0)
004291d8  fstp       st(0)

// block 004291da
004291da  pop        edi
004291db  xor        eax, eax
004291dd  pop        esi
004291de  add        esp, 0x10
004291e1  ret        

// block 004291e2
004291e2  fstp       st(1)
004291e4  fstp       st(1)
004291e6  jmp        0x4291ea

// block 004291e8
004291e8  fstp       st(0)

// block 004291ea
004291ea  mov        ecx, dword ptr [esi + 0x184]
004291f0  xor        edx, edx
004291f2  test       cl, 1
004291f5  je         0x42925a

// block 004291f7
004291f7  fcom       st(1)
004291f9  fnstsw     ax
004291fb  fstp       st(1)
004291fd  test       ah, 5
00429200  jp         0x42925c

// block 00429202
00429202  test       cl, 0x10
00429205  jne        0x429253

// block 00429207
00429207  mov        eax, dword ptr [esp + 0xc]
0042920b  fstp       st(0)
0042920d  mov        ecx, dword ptr [esp + 0x10]
00429211  mov        edx, dword ptr [esp + 0x14]
00429215  lea        edi, [esi + 0x454]
0042921b  mov        dword ptr [edi], eax
0042921d  mov        dword ptr [edi + 4], ecx
00429220  mov        dword ptr [edi + 8], edx
00429223  fld        dword ptr [esi + 0x458]
00429229  fchs       
0042922b  push       0
0042922d  fstp       dword ptr [esi + 0x458]
00429233  fld        dword ptr [esi + 0x68]
00429236  fchs       
00429238  fstp       dword ptr [esi + 0x460]
0042923e  fld        dword ptr [esi + 0x168]
00429244  fstp       dword ptr [esi + 0x474]
0042924a  call       0x428450

// block 0042924f
0042924f  fld        dword ptr [esp + 0x10]

// block 00429253
00429253  mov        edx, 1
00429258  jmp        0x42925c

// block 0042925a
0042925a  fstp       st(1)

// block 0042925c
0042925c  mov        ecx, dword ptr [esi + 0x184]
00429262  test       cl, 2
00429265  je         0x4292ca

// block 00429267
00429267  fcomp      qword ptr [0x4a3d50]
0042926d  fnstsw     ax
0042926f  test       ah, 0x41
00429272  jne        0x4292cc

// block 00429274
00429274  test       cl, 0x10
00429277  jne        0x4292c3

// block 00429279
00429279  mov        eax, dword ptr [esp + 0xc]
0042927d  mov        ecx, dword ptr [esp + 0x10]
00429281  mov        edx, dword ptr [esp + 0x14]
00429285  lea        edi, [esi + 0x454]
0042928b  mov        dword ptr [edi], eax
0042928d  mov        dword ptr [edi + 4], ecx
00429290  mov        dword ptr [edi + 8], edx
00429293  fld        dword ptr [esi + 0x458]
00429299  fsubr      qword ptr [0x4a4180]
0042929f  push       0
004292a1  fstp       dword ptr [esi + 0x458]
004292a7  fld        dword ptr [esi + 0x68]
004292aa  fchs       
004292ac  fstp       dword ptr [esi + 0x460]
004292b2  fld        dword ptr [esi + 0x168]
004292b8  fstp       dword ptr [esi + 0x474]
004292be  call       0x428450

// block 004292c3
004292c3  mov        edx, 1
004292c8  jmp        0x4292cc

// block 004292ca
004292ca  fstp       st(0)

// block 004292cc
004292cc  mov        ecx, dword ptr [esi + 0x184]
004292d2  test       cl, 4
004292d5  je         0x42934d

// block 004292d7
004292d7  fld        dword ptr [0x4a3d98]
004292dd  fcomp      dword ptr [esp + 0xc]
004292e1  fnstsw     ax
004292e3  test       ah, 0x41
004292e6  jne        0x42934d

// block 004292e8
004292e8  test       cl, 0x10
004292eb  jne        0x429348

// block 004292ed
004292ed  mov        eax, dword ptr [esp + 0xc]
004292f1  mov        ecx, dword ptr [esp + 0x10]
004292f5  mov        edx, dword ptr [esp + 0x14]
004292f9  lea        edi, [esi + 0x454]
004292ff  mov        dword ptr [edi], eax
00429301  mov        dword ptr [edi + 4], ecx
00429304  mov        dword ptr [edi + 8], edx
00429307  fld        dword ptr [edi]
00429309  fchs       
0042930b  push       ecx
0042930c  fsub       qword ptr [0x4a3d80]
00429312  fstp       dword ptr [edi]
00429314  fld        dword ptr [esi + 0x68]
00429317  fchs       
00429319  fsub       qword ptr [0x4a3cd8]
0042931f  fstp       dword ptr [esp + 0xc]
00429323  fld        dword ptr [esp + 0xc]
00429327  fstp       dword ptr [esp]
0042932a  call       0x4646e0

// block 0042932f
0042932f  fstp       dword ptr [esi + 0x460]
00429335  push       0
00429337  fld        dword ptr [esi + 0x168]
0042933d  fstp       dword ptr [esi + 0x474]
00429343  call       0x428450

// block 00429348
00429348  mov        edx, 1

// block 0042934d
0042934d  mov        ecx, dword ptr [esi + 0x184]
00429353  test       cl, 8
00429356  je         0x4293c9

// block 00429358
00429358  fld        dword ptr [0x4a3d88]
0042935e  fcomp      dword ptr [esp + 0xc]
00429362  fnstsw     ax
00429364  test       ah, 5
00429367  jp         0x4293c9

// block 00429369
00429369  test       cl, 0x10
0042936c  jne        0x4293d1

// block 0042936e
0042936e  mov        eax, dword ptr [esp + 0xc]
00429372  mov        ecx, dword ptr [esp + 0x10]
00429376  mov        edx, dword ptr [esp + 0x14]
0042937a  lea        edi, [esi + 0x454]
00429380  mov        dword ptr [edi], eax
00429382  mov        dword ptr [edi + 4], ecx
00429385  mov        dword ptr [edi + 8], edx
00429388  fld        dword ptr [edi]
0042938a  fsubr      qword ptr [0x4a3d80]
00429390  push       ecx
00429391  fstp       dword ptr [edi]
00429393  fld        dword ptr [esi + 0x68]
00429396  fchs       
00429398  fsub       qword ptr [0x4a3cd8]
0042939e  fstp       dword ptr [esp + 0xc]
004293a2  fld        dword ptr [esp + 0xc]
004293a6  fstp       dword ptr [esp]
004293a9  call       0x4646e0

// block 004293ae
004293ae  fstp       dword ptr [esi + 0x460]
004293b4  push       0
004293b6  fld        dword ptr [esi + 0x168]
004293bc  fstp       dword ptr [esi + 0x474]
004293c2  call       0x428450

// block 004293c7
004293c7  jmp        0x4293d1

// block 004293c9
004293c9  test       edx, edx
004293cb  je         0x4291da

// block 004293d1
004293d1  mov        edx, dword ptr [esi + 0x638]
004293d7  and        dword ptr [esi + 0x430], 0xfffffeff
004293e1  test       edx, edx
004293e3  jl         0x4293ea

// block 004293e5
004293e5  call       0x453d90

// block 004293ea
004293ea  pop        edi
004293eb  mov        eax, 1
004293f0  pop        esi
004293f1  add        esp, 0x10
004293f4  ret        
