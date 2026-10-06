
// block 0042d890
0042d890  push       ebp
0042d891  mov        ebp, esp
0042d893  and        esp, 0xfffffff8
0042d896  sub        esp, 0x14
0042d899  push       esi
0042d89a  mov        esi, ecx
0042d89c  mov        eax, dword ptr [esi + 0xbc]
0042d8a2  cmp        eax, dword ptr [esi + 0xe0]
0042d8a8  jl         0x42d8bb

// block 0042d8aa
0042d8aa  and        dword ptr [esi + 0x430], 0xfffffffb
0042d8b1  mov        eax, 1
0042d8b6  pop        esi
0042d8b7  mov        esp, ebp
0042d8b9  pop        ebp
0042d8ba  ret        

// block 0042d8bb
0042d8bb  fld        dword ptr [esi + 0xcc]
0042d8c1  fmul       dword ptr [0x4b2ed0]
0042d8c7  fadd       dword ptr [esi + 0x74]
0042d8ca  fstp       dword ptr [esi + 0x74]
0042d8cd  fld        dword ptr [esi + 0xd4]
0042d8d3  fld        dword ptr [0x4b2ed0]
0042d8d9  fld        st(0)
0042d8db  fmulp      st(2)
0042d8dd  fxch       st(1)
0042d8df  fstp       dword ptr [esp + 0xc]
0042d8e3  fld        dword ptr [esi + 0xd8]
0042d8e9  fmul       st(1)
0042d8eb  fstp       dword ptr [esp + 0x10]
0042d8ef  fmul       dword ptr [esi + 0xdc]
0042d8f5  fstp       dword ptr [esp + 0x14]
0042d8f9  fld        dword ptr [esi + 0x5c]
0042d8fc  fadd       dword ptr [esp + 0xc]
0042d900  fstp       dword ptr [esi + 0x5c]
0042d903  fld        dword ptr [esi + 0x60]
0042d906  fadd       dword ptr [esp + 0x10]
0042d90a  fstp       dword ptr [esi + 0x60]
0042d90d  fld        dword ptr [esp + 0x14]
0042d911  fadd       dword ptr [esi + 0x64]
0042d914  fstp       dword ptr [esi + 0x64]
0042d917  fld        dword ptr [esi + 0x5c]
0042d91a  fabs       
0042d91c  fstp       dword ptr [esp + 8]
0042d920  fld        dword ptr [esp + 8]
0042d924  fld        qword ptr [0x4a3ff0]
0042d92a  fcom       st(1)
0042d92c  fnstsw     ax
0042d92e  fstp       st(1)
0042d930  test       ah, 5
0042d933  jnp        0x42d94d

// block 0042d935
0042d935  fld        dword ptr [esi + 0x60]
0042d938  fabs       
0042d93a  fstp       dword ptr [esp + 8]
0042d93e  fld        dword ptr [esp + 8]
0042d942  fcompp     
0042d944  fnstsw     ax
0042d946  test       ah, 0x41
0042d949  jne        0x42d965

// block 0042d94b
0042d94b  jmp        0x42d94f

// block 0042d94d
0042d94d  fstp       st(0)

// block 0042d94f
0042d94f  fld        dword ptr [esi + 0x60]
0042d952  fld        dword ptr [esi + 0x5c]
0042d955  call       0x4937aa

// block 0042d95a
0042d95a  fstp       dword ptr [esp + 8]
0042d95e  fld        dword ptr [esp + 8]
0042d962  fstp       dword ptr [esi + 0x68]

// block 0042d965
0042d965  add        esi, 0xb8
0042d96b  call       0x464a80

// block 0042d970
0042d970  xor        eax, eax
0042d972  pop        esi
0042d973  mov        esp, ebp
0042d975  pop        ebp
0042d976  ret        
