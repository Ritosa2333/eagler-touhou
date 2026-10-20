
// block 004377a0
004377a0  push       ebp
004377a1  mov        ebp, esp
004377a3  and        esp, 0xfffffff8
004377a6  sub        esp, 8
004377a9  mov        eax, dword ptr [0x4b4514]
004377ae  fld        dword ptr [eax + 0x97c]
004377b4  fsub       dword ptr [ecx]
004377b6  fstp       dword ptr [esp + 4]
004377ba  fld        dword ptr [eax + 0x980]
004377c0  fsub       dword ptr [ecx + 4]
004377c3  fstp       dword ptr [esp]
004377c6  fldz       
004377c8  fld        st(0)
004377ca  fld        dword ptr [esp]
004377cd  fucom      st(1)
004377cf  fnstsw     ax
004377d1  fstp       st(1)
004377d3  fld        dword ptr [esp + 4]
004377d7  test       ah, 0x44
004377da  jp         0x4377f5

// block 004377dc
004377dc  fucom      st(2)
004377de  fnstsw     ax
004377e0  fstp       st(2)
004377e2  test       ah, 0x44
004377e5  jp         0x4377f7

// block 004377e7
004377e7  fstp       st(0)
004377e9  fstp       st(0)
004377eb  fld        dword ptr [0x4a3e08]
004377f1  mov        esp, ebp
004377f3  pop        ebp
004377f4  ret        

// block 004377f5
004377f5  fstp       st(2)

// block 004377f7
004377f7  fxch       st(1)
004377f9  call       0x4937aa

// block 004377fe
004377fe  fstp       dword ptr [esp + 4]
00437802  fld        dword ptr [esp + 4]
00437806  mov        esp, ebp
00437808  pop        ebp
00437809  ret        
