
// block 00406090
00406090  fld        dword ptr [ecx]
00406092  fsub       dword ptr [eax]
00406094  fstp       dword ptr [esi]
00406096  fld        dword ptr [ecx + 4]
00406099  fsub       dword ptr [eax + 4]
0040609c  fstp       dword ptr [esi + 4]
0040609f  fld        dword ptr [ecx + 8]
004060a2  fsub       dword ptr [eax + 8]
004060a5  fstp       dword ptr [esi + 8]
004060a8  fld        dword ptr [ecx + 0xc]
004060ab  fsub       dword ptr [eax + 0xc]
004060ae  fstp       dword ptr [esi + 0xc]
004060b1  fld        dword ptr [ecx + 0x10]
004060b4  fsub       dword ptr [eax + 0x10]
004060b7  fstp       dword ptr [esi + 0x10]
004060ba  fld        dword ptr [ecx + 0x14]
004060bd  mov        ecx, esi
004060bf  fsub       dword ptr [eax + 0x14]
004060c2  fstp       dword ptr [esi + 0x14]
004060c5  call       0x406250

// block 004060ca
004060ca  mov        eax, esi
004060cc  ret        
