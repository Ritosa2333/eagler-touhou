
// block 00429520
00429520  push       ebp
00429521  mov        ebp, esp
00429523  and        esp, 0xfffffff8
00429526  sub        esp, 0x14
00429529  push       esi
0042952a  mov        esi, ecx
0042952c  mov        eax, dword ptr [esi + 0xbc]
00429532  cmp        eax, dword ptr [esi + 0xe0]
00429538  jl         0x42954b

// block 0042953a
0042953a  and        dword ptr [esi + 0x430], 0xfffffffb
00429541  mov        eax, 1
00429546  pop        esi
00429547  mov        esp, ebp
00429549  pop        ebp
0042954a  ret        

// block 0042954b
0042954b  fld        dword ptr [esi + 0xcc]
00429551  fmul       dword ptr [0x4b2ed0]
00429557  fadd       dword ptr [esi + 0x74]
0042955a  fstp       dword ptr [esi + 0x74]
0042955d  fld        dword ptr [esi + 0xd4]
00429563  fld        dword ptr [0x4b2ed0]
00429569  fld        st(0)
0042956b  fmulp      st(2)
0042956d  fxch       st(1)
0042956f  fstp       dword ptr [esp + 0xc]
00429573  fld        dword ptr [esi + 0xd8]
00429579  fmul       st(1)
0042957b  fstp       dword ptr [esp + 0x10]
0042957f  fmul       dword ptr [esi + 0xdc]
00429585  fstp       dword ptr [esp + 0x14]
00429589  fld        dword ptr [esi + 0x5c]
0042958c  fadd       dword ptr [esp + 0xc]
00429590  fstp       dword ptr [esi + 0x5c]
00429593  fld        dword ptr [esi + 0x60]
00429596  fadd       dword ptr [esp + 0x10]
0042959a  fstp       dword ptr [esi + 0x60]
0042959d  fld        dword ptr [esp + 0x14]
004295a1  fadd       dword ptr [esi + 0x64]
004295a4  fstp       dword ptr [esi + 0x64]
004295a7  fld        dword ptr [esi + 0x5c]
004295aa  fabs       
004295ac  fstp       dword ptr [esp + 8]
004295b0  fld        dword ptr [esp + 8]
004295b4  fld        qword ptr [0x4a3ff0]
004295ba  fcom       st(1)
004295bc  fnstsw     ax
004295be  fstp       st(1)
004295c0  test       ah, 5
004295c3  jnp        0x4295dd

// block 004295c5
004295c5  fld        dword ptr [esi + 0x60]
004295c8  fabs       
004295ca  fstp       dword ptr [esp + 8]
004295ce  fld        dword ptr [esp + 8]
004295d2  fcompp     
004295d4  fnstsw     ax
004295d6  test       ah, 0x41
004295d9  jne        0x4295f5

// block 004295db
004295db  jmp        0x4295df

// block 004295dd
004295dd  fstp       st(0)

// block 004295df
004295df  fld        dword ptr [esi + 0x60]
004295e2  fld        dword ptr [esi + 0x5c]
004295e5  call       0x4937aa

// block 004295ea
004295ea  fstp       dword ptr [esp + 8]
004295ee  fld        dword ptr [esp + 8]
004295f2  fstp       dword ptr [esi + 0x68]

// block 004295f5
004295f5  add        esi, 0xb8
004295fb  call       0x464a80

// block 00429600
00429600  xor        eax, eax
00429602  pop        esi
00429603  mov        esp, ebp
00429605  pop        ebp
00429606  ret        
