
// block 00406500
00406500  push       ecx
00406501  neg        eax
00406503  mov        dword ptr [esp], eax
00406506  fild       dword ptr [esp]
00406509  push       ecx
0040650a  fstp       dword ptr [esp]
0040650d  call       0x464a20

// block 00406512
00406512  pop        ecx
00406513  ret        
