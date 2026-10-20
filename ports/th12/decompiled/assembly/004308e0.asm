
// block 004308e0
004308e0  and        dword ptr [eax + 0x590], 0xffff7fff
004308ea  push       ebx
004308eb  mov        ebx, dword ptr [0x498094]
004308f1  push       esi
004308f2  push       edi
004308f3  lea        esi, [eax + 0x810]
004308f9  mov        edi, 0xc
004308fe  mov        edi, edi

// block 00430900
00430900  push       esi
00430901  call       ebx

// block 00430903
00430903  add        esi, 0x18
00430906  sub        edi, 1
00430909  jne        0x430900

// block 0043090b
0043090b  pop        edi
0043090c  pop        esi
0043090d  pop        ebx
0043090e  ret        
