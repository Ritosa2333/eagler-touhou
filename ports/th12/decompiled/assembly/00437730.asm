
// block 00437730
00437730  push       ebp
00437731  mov        ebp, esp
00437733  and        esp, 0xfffffff8
00437736  sub        esp, 8
00437739  fld        dword ptr [eax]
0043773b  fsub       dword ptr [ecx + 0x97c]
00437741  fstp       dword ptr [esp + 4]
00437745  fld        dword ptr [eax + 4]
00437748  fsub       dword ptr [ecx + 0x980]
0043774e  fstp       dword ptr [esp]
00437751  fldz       
00437753  fld        st(0)
00437755  fld        dword ptr [esp]
00437758  fucom      st(1)
0043775a  fnstsw     ax
0043775c  fstp       st(1)
0043775e  fld        dword ptr [esp + 4]
00437762  test       ah, 0x44
00437765  jp         0x437780

// block 00437767
00437767  fucom      st(2)
00437769  fnstsw     ax
0043776b  fstp       st(2)
0043776d  test       ah, 0x44
00437770  jp         0x437782

// block 00437772
00437772  fstp       st(0)
00437774  fstp       st(0)
00437776  fld        dword ptr [0x4a3e08]
0043777c  mov        esp, ebp
0043777e  pop        ebp
0043777f  ret        

// block 00437780
00437780  fstp       st(2)

// block 00437782
00437782  fxch       st(1)
00437784  call       0x4937aa

// block 00437789
00437789  fstp       dword ptr [esp + 4]
0043778d  fld        dword ptr [esp + 4]
00437791  mov        esp, ebp
00437793  pop        ebp
00437794  ret        
