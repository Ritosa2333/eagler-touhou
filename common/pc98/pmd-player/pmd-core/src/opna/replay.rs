// SPDX-License-Identifier: AGPL-3.0-only

use super::fm::FmEngine;
use super::ssg::Ssg;

const ALL_CHANNELS: u32 = 0x3f;

#[derive(Clone, Debug)]
pub struct RegisterReplay {
    fm: FmEngine,
    ssg: Ssg,
}

impl RegisterReplay {
    pub fn new() -> Self {
        Self {
            fm: FmEngine::new(),
            ssg: Ssg::new(),
        }
    }

    pub fn reset(&mut self) {
        self.fm.reset();
        self.ssg.reset();
    }

    pub fn write(&mut self, address: u16, value: u8) {
        match address {
            0x00..=0x0f => self.ssg.write(address as usize, value),
            0x10..=0x1f => {}
            0x20..=0xff | 0x111..=0x1ff => self.fm.write(address, value),
            0x100..=0x10f => {}
            0x110 => {}
            _ => {}
        }
    }

    pub fn clock(&mut self) -> [i32; 3] {
        self.fm.clock(ALL_CHANNELS);
        let fm = self.fm.output(1, 32767, ALL_CHANNELS);
        self.ssg.clock();
        let mut ssg = [0; 3];
        self.ssg.output(&mut ssg);
        [fm[0], fm[1], ssg[0] + ssg[1] + ssg[2]]
    }
}

impl Default for RegisterReplay {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::RegisterReplay;

    #[test]
    fn routes_ssg_registers_and_clocks_them_alongside_fm() {
        let mut replay = RegisterReplay::new();
        replay.write(0x07, 0x08);
        replay.write(0x08, 0x0f);
        replay.write(0x00, 2);
        assert_eq!(replay.clock()[2], 0);
        assert_eq!(replay.clock()[2], 16382);
        assert_eq!(replay.clock()[2], 16382);
    }
}
