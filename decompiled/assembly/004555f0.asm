
// block 004555f0
004555f0  fld        dword ptr [eax]
004555f2  fdiv       qword ptr [0x4a3d18]
004555f8  fstp       dword ptr [ecx]
004555fa  fld        dword ptr [eax + 4]
004555fd  fdiv       qword ptr [0x4a3d10]
00455603  fstp       dword ptr [ecx + 4]
00455606  fldz       
00455608  fcom       dword ptr [ecx]
0045560a  fnstsw     ax
0045560c  test       ah, 0x41
0045560f  jne        0x455613

// block 00455611
00455611  fst        dword ptr [ecx]

// block 00455613
00455613  fcom       dword ptr [ecx + 4]
00455616  fnstsw     ax
00455618  test       ah, 0x41
0045561b  jne        0x455621

// block 0045561d
0045561d  fstp       dword ptr [ecx + 4]
00455620  ret        

// block 00455621
00455621  fstp       st(0)
00455623  ret        
