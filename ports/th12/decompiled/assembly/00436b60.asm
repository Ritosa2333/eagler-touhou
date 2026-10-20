
// block 00436b60
00436b60  mov        ecx, dword ptr [esi + 0x10]
00436b63  push       0
00436b65  lea        eax, [esi + 0x14]
00436b68  push       eax
00436b69  call       0x454d10

// block 00436b6e
00436b6e  and        dword ptr [esi + 0xc414], 0xfffffffd
00436b75  mov        dword ptr [esi + 0xa14], 0
00436b7f  mov        dword ptr [esi + 0xa18], 0
00436b89  ret        
