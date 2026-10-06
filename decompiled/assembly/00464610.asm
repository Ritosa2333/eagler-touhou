
// block 00464610
00464610  push       ecx
00464611  call       0x464440

// block 00464616
00464616  mov        dword ptr [esp], eax
00464619  fild       dword ptr [esp]
0046461c  test       eax, eax
0046461e  jge        0x464626

// block 00464620
00464620  fadd       dword ptr [0x4a3e10]

// block 00464626
00464626  fdiv       qword ptr [0x4a3ea0]
0046462c  fsub       qword ptr [0x4a3cd8]
00464632  fstp       dword ptr [esp]
00464635  fld        dword ptr [esp]
00464638  pop        ecx
00464639  ret        
