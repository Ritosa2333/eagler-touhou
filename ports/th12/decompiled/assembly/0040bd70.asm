
// block 0040bd70
0040bd70  push       ecx
0040bd71  fld        dword ptr [0x4a3d98]
0040bd77  fcomp      dword ptr [esi + 0x4bc]
0040bd7d  fnstsw     ax
0040bd7f  test       ah, 0x41
0040bd82  jne        0x40bdd3

// block 0040bd84
0040bd84  test       byte ptr [esi + 0x800], 0x10
0040bd8b  jne        0x40bdcc

// block 0040bd8d
0040bd8d  fldz       
0040bd8f  sub        esp, 8
0040bd92  fstp       dword ptr [esp + 4]
0040bd96  fld        dword ptr [esi + 0x4d8]
0040bd9c  fchs       
0040bd9e  fsub       qword ptr [0x4a3cd8]
0040bda4  fstp       dword ptr [esp + 8]
0040bda8  fld        dword ptr [esp + 8]
0040bdac  fstp       dword ptr [esp]
0040bdaf  call       0x464640

// block 0040bdb4
0040bdb4  fstp       dword ptr [esi + 0x4d8]
0040bdba  fld        dword ptr [esi + 0x4bc]
0040bdc0  fsubr      qword ptr [0x4a3d90]
0040bdc6  fstp       dword ptr [esi + 0x4bc]

// block 0040bdcc
0040bdcc  mov        eax, 1
0040bdd1  pop        ecx
0040bdd2  ret        

// block 0040bdd3
0040bdd3  xor        eax, eax
0040bdd5  pop        ecx
0040bdd6  ret        
