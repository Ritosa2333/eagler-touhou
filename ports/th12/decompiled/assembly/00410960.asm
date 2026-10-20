
// block 00410960
00410960  fld        dword ptr [eax]
00410962  fdiv       qword ptr [0x4a3d18]
00410968  fstp       dword ptr [ecx]
0041096a  fld        dword ptr [eax + 4]
0041096d  fdiv       qword ptr [0x4a3d10]
00410973  fstp       dword ptr [edx]
00410975  fldz       
00410977  fcom       dword ptr [ecx]
00410979  fnstsw     ax
0041097b  test       ah, 0x41
0041097e  jne        0x410982

// block 00410980
00410980  fst        dword ptr [ecx]

// block 00410982
00410982  fcom       dword ptr [edx]
00410984  fnstsw     ax
00410986  test       ah, 0x41
00410989  jne        0x41098e

// block 0041098b
0041098b  fstp       dword ptr [edx]
0041098d  ret        

// block 0041098e
0041098e  fstp       st(0)
00410990  ret        
