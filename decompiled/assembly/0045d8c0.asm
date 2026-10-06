
// block 0045d8c0
0045d8c0  sub        esp, 0x18
0045d8c3  fld        dword ptr [ecx + 0x424]
0045d8c9  fadd       dword ptr [ecx + 0x430]
0045d8cf  fstp       dword ptr [esp]
0045d8d2  fld        dword ptr [ecx + 0x428]
0045d8d8  fadd       dword ptr [ecx + 0x434]
0045d8de  fstp       dword ptr [esp + 4]
0045d8e2  fld        dword ptr [ecx + 0x42c]
0045d8e8  fadd       dword ptr [ecx + 0x438]
0045d8ee  fstp       dword ptr [esp + 8]
0045d8f2  fld        dword ptr [esp]
0045d8f5  fadd       dword ptr [ecx + 0x43c]
0045d8fb  fstp       dword ptr [esp + 0xc]
0045d8ff  fld        dword ptr [ecx + 0x440]
0045d905  fadd       dword ptr [esp + 4]
0045d909  fstp       dword ptr [esp + 0x10]
0045d90d  mov        edx, dword ptr [esp + 0x10]
0045d911  fld        dword ptr [ecx + 0x444]
0045d917  mov        ecx, dword ptr [esp + 0xc]
0045d91b  fadd       dword ptr [esp + 8]
0045d91f  mov        dword ptr [eax], ecx
0045d921  mov        dword ptr [eax + 4], edx
0045d924  fstp       dword ptr [esp + 0x14]
0045d928  mov        ecx, dword ptr [esp + 0x14]
0045d92c  mov        dword ptr [eax + 8], ecx
0045d92f  add        esp, 0x18
0045d932  ret        
