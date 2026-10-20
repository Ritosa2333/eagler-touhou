
// block 0041c530
0041c530  push       ebp
0041c531  mov        ebp, esp
0041c533  and        esp, 0xfffffff8
0041c536  sub        esp, 8
0041c539  fld        dword ptr [eax + 0x10]
0041c53c  fld        dword ptr [eax + 0xc]
0041c53f  call       0x4937aa

// block 0041c544
0041c544  fstp       dword ptr [esp + 4]
0041c548  fld        dword ptr [esp + 4]
0041c54c  mov        esp, ebp
0041c54e  pop        ebp
0041c54f  ret        
