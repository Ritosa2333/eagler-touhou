
// block 00408820
00408820  push       ebp
00408821  mov        ebp, esp
00408823  and        esp, 0xfffffff8
00408826  sub        esp, 8
00408829  fld        dword ptr [eax + 4]
0040882c  fsub       dword ptr [ecx + 4]
0040882f  fstp       dword ptr [esp + 4]
00408833  fld        dword ptr [esp + 4]
00408837  fld        dword ptr [eax]
00408839  fsub       dword ptr [ecx]
0040883b  fstp       dword ptr [esp + 4]
0040883f  fld        dword ptr [esp + 4]
00408843  call       0x4937aa

// block 00408848
00408848  fstp       dword ptr [esp + 4]
0040884c  fld        dword ptr [esp + 4]
00408850  mov        esp, ebp
00408852  pop        ebp
00408853  ret        
