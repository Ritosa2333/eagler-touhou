
// block 00439a40
00439a40  push       ecx
00439a41  push       ebp
00439a42  mov        ebp, dword ptr [esp + 0xc]
00439a46  cmp        dword ptr [ebp + 0xa28], 1
00439a4d  push       esi
00439a4e  push       edi
00439a4f  jne        0x439af0

// block 00439a55
00439a55  cmp        dword ptr [ebp + 0xc424], 0
00439a5c  jge        0x439a85

// block 00439a5e
00439a5e  cmp        dword ptr [ebp + 0xa20], 0x63
00439a65  jge        0x439b01

// block 00439a6b
00439a6b  test       byte ptr [0x4d49d0], 1
00439a72  je         0x439b01

// block 00439a78
00439a78  push       0
00439a7a  lea        eax, [ebp + 0xc420]
00439a80  call       0x4067e0

// block 00439a85
00439a85  mov        eax, dword ptr [ebp + 0xc424]
00439a8b  cmp        eax, dword ptr [ebp + 0xc420]
00439a91  lea        esi, [ebp + 0xc420]
00439a97  je         0x439aa6

// block 00439a99
00439a99  mov        edi, dword ptr [ebp + 0xc424]
00439a9f  mov        eax, ebp
00439aa1  call       0x4399d0

// block 00439aa6
00439aa6  cmp        dword ptr [ebp + 0xc424], 0xe
00439aad  jl         0x439ae2

// block 00439aaf
00439aaf  test       byte ptr [0x4d49d0], 1
00439ab6  je         0x439ad0

// block 00439ab8
00439ab8  fld        dword ptr [0x4a43c8]
00439abe  push       ecx
00439abf  fstp       dword ptr [esp]
00439ac2  call       0x464a20

// block 00439ac7
00439ac7  xor        eax, eax
00439ac9  pop        edi
00439aca  pop        esi
00439acb  pop        ebp
00439acc  pop        ecx
00439acd  ret        4

// block 00439ad0
00439ad0  push       -1
00439ad2  mov        eax, esi
00439ad4  call       0x4067e0

// block 00439ad9
00439ad9  xor        eax, eax
00439adb  pop        edi
00439adc  pop        esi
00439add  pop        ebp
00439ade  pop        ecx
00439adf  ret        4

// block 00439ae2
00439ae2  call       0x464a80

// block 00439ae7
00439ae7  xor        eax, eax
00439ae9  pop        edi
00439aea  pop        esi
00439aeb  pop        ebp
00439aec  pop        ecx
00439aed  ret        4

// block 00439af0
00439af0  mov        dword ptr [ebp + 0x8980], 0
00439afa  mov        byte ptr [ebp + 0x8984], 0

// block 00439b01
00439b01  pop        edi
00439b02  pop        esi
00439b03  xor        eax, eax
00439b05  pop        ebp
00439b06  pop        ecx
00439b07  ret        4
