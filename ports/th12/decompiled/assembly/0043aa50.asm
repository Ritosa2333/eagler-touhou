
// block 0043aa50
0043aa50  push       ebx
0043aa51  push       esi
0043aa52  mov        esi, edx
0043aa54  mov        ebx, 2
0043aa59  cmp        dword ptr [esi + 0x48], ebx
0043aa5c  je         0x43aaaf

// block 0043aa5e
0043aa5e  fldz       
0043aa60  fcomp      dword ptr [esi + 0x2c]
0043aa63  fnstsw     ax
0043aa65  test       ah, 5
0043aa68  jp         0x43aa7b

// block 0043aa6a
0043aa6a  fld        dword ptr [esi + 0x2c]
0043aa6d  xor        eax, eax
0043aa6f  fsub       qword ptr [0x4a3e28]
0043aa75  fstp       dword ptr [esi + 0x2c]
0043aa78  pop        esi
0043aa79  pop        ebx
0043aa7a  ret        

// block 0043aa7b
0043aa7b  mov        eax, dword ptr [esi + 0x4c]
0043aa7e  push       eax
0043aa7f  mov        dword ptr [esi + 0x48], ebx
0043aa82  call       0x461970

// block 0043aa87
0043aa87  fld        dword ptr [0x4a4138]
0043aa8d  mov        eax, dword ptr [0x4b4514]
0043aa92  push       4
0043aa94  push       0xf
0043aa96  sub        esp, 8
0043aa99  fstp       dword ptr [esp + 4]
0043aa9d  add        esi, 0x14
0043aaa0  fld        dword ptr [0x4a404c]
0043aaa6  fstp       dword ptr [esp]
0043aaa9  push       esi
0043aaaa  call       0x4390f0

// block 0043aaaf
0043aaaf  pop        esi
0043aab0  xor        eax, eax
0043aab2  pop        ebx
0043aab3  ret        
