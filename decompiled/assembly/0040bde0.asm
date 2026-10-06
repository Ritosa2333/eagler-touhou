
// block 0040bde0
0040bde0  push       ecx
0040bde1  fld        dword ptr [0x4a3d88]
0040bde7  fcomp      dword ptr [esi + 0x4bc]
0040bded  fnstsw     ax
0040bdef  test       ah, 0x41
0040bdf2  jp         0x40be43

// block 0040bdf4
0040bdf4  test       byte ptr [esi + 0x800], 0x10
0040bdfb  jne        0x40be3c

// block 0040bdfd
0040bdfd  fldz       
0040bdff  sub        esp, 8
0040be02  fstp       dword ptr [esp + 4]
0040be06  fld        dword ptr [esi + 0x4d8]
0040be0c  fchs       
0040be0e  fsub       qword ptr [0x4a3cd8]
0040be14  fstp       dword ptr [esp + 8]
0040be18  fld        dword ptr [esp + 8]
0040be1c  fstp       dword ptr [esp]
0040be1f  call       0x464640

// block 0040be24
0040be24  fstp       dword ptr [esi + 0x4d8]
0040be2a  fld        dword ptr [esi + 0x4bc]
0040be30  fsubr      qword ptr [0x4a3d80]
0040be36  fstp       dword ptr [esi + 0x4bc]

// block 0040be3c
0040be3c  mov        eax, 1
0040be41  pop        ecx
0040be42  ret        

// block 0040be43
0040be43  xor        eax, eax
0040be45  pop        ecx
0040be46  ret        
