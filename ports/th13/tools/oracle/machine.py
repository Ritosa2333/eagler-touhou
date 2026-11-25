import struct

from unicorn import UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, Uc
from unicorn.x86_const import (UC_X86_REG_EAX, UC_X86_REG_EBP, UC_X86_REG_EBX, UC_X86_REG_ECX,
                               UC_X86_REG_EDI, UC_X86_REG_EDX, UC_X86_REG_EIP, UC_X86_REG_ESI,
                               UC_X86_REG_ESP)

RETURN = 0x100000
STACK = 0x20080000
REGISTERS = {
    'eax': UC_X86_REG_EAX,
    'ecx': UC_X86_REG_ECX,
    'edx': UC_X86_REG_EDX,
    'ebx': UC_X86_REG_EBX,
    'esi': UC_X86_REG_ESI,
    'edi': UC_X86_REG_EDI,
    'ebp': UC_X86_REG_EBP,
    'esp': UC_X86_REG_ESP,
    'eip': UC_X86_REG_EIP,
}


class Machine:
    def __init__(self, exe):
        data = open(exe, 'rb').read()
        pe = struct.unpack_from('<I', data, 0x3c)[0]
        if data[pe:pe + 4] != b'PE\0\0':
            raise ValueError('not a PE image')
        count = struct.unpack_from('<H', data, pe + 6)[0]
        optional = struct.unpack_from('<H', data, pe + 20)[0]
        header = pe + 24
        image = struct.unpack_from('<I', data, header + 56)[0]
        headers = struct.unpack_from('<I', data, header + 60)[0]
        self.base = struct.unpack_from('<I', data, header + 28)[0]
        self.uc = Uc(UC_ARCH_X86, UC_MODE_32)
        self.uc.mem_map(self.base, (image + 0xfff) & ~0xfff)
        self.uc.mem_write(self.base, data[:headers])
        for index in range(count):
            virtual_size, address, raw_size, raw = struct.unpack_from('<IIII', data, header + optional + index * 40 + 8)
            if raw_size:
                self.uc.mem_write(self.base + address, data[raw:raw + raw_size])
        self.heap = 0x10000000
        self.uc.mem_map(self.heap, 0x100000)
        self.uc.mem_map(0x20000000, 0x100000)
        self.uc.mem_map(RETURN, 0x1000)
        self.cursor = self.heap

    def alloc(self, size):
        address = self.cursor
        self.cursor += (size + 15) & ~15
        if self.cursor > self.heap + 0x100000:
            raise MemoryError('probe heap exhausted')
        return address

    def map(self, address, size):
        self.uc.mem_map(address, size)
        return address

    def hook_code(self, address, callback):
        def hook(uc, instruction, size, user_data):
            callback(uc)
        return self.uc.hook_add(UC_HOOK_CODE, hook, begin=address, end=address)

    def reg_read(self, name):
        return self.uc.reg_read(REGISTERS[name])

    def reg_write(self, name, value):
        self.uc.reg_write(REGISTERS[name], value & 0xffffffff)

    def start(self, address, until, **registers):
        self.write(STACK, struct.pack('<I', RETURN))
        for name, value in registers.items():
            self.reg_write(name, value)
        if 'esp' not in registers:
            self.reg_write('esp', STACK)
        self.uc.emu_start(address, until)
        return self.uc.reg_read(UC_X86_REG_EAX)

    def stop(self):
        self.uc.emu_stop()

    def read(self, address, size):
        return bytes(self.uc.mem_read(address, size))

    def write(self, address, data):
        self.uc.mem_write(address, bytes(data))

    def u32(self, address):
        return struct.unpack('<I', self.read(address, 4))[0]

    def f32(self, address):
        return struct.unpack('<f', self.read(address, 4))[0]

    def call(self, address, args=(), eax=None, ecx=None, edx=None, ebx=None, esi=None, edi=None, ebp=None):
        self.write(STACK, struct.pack('<I', RETURN))
        for index, value in enumerate(args):
            self.write(STACK + 4 + 4 * index, struct.pack('<I', value & 0xffffffff))
        self.uc.reg_write(UC_X86_REG_ESP, STACK)
        for register, value in ((UC_X86_REG_EAX, eax), (UC_X86_REG_ECX, ecx), (UC_X86_REG_EDX, edx),
                                (UC_X86_REG_EBX, ebx), (UC_X86_REG_ESI, esi), (UC_X86_REG_EDI, edi),
                                (UC_X86_REG_EBP, ebp)):
            if value is not None:
                self.uc.reg_write(register, value & 0xffffffff)
        self.uc.emu_start(address, RETURN)
        return self.uc.reg_read(UC_X86_REG_EAX)
