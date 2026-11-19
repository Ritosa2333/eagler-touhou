// SPDX-License-Identifier: AGPL-3.0-only

const FIXED_TIME_SHIFT: u32 = 48;
const FIXED_TIME_ONE: u128 = 1u128 << FIXED_TIME_SHIFT;
const MICROSECONDS_PER_SECOND: u128 = 1_000_000;
const REDUCED_MICROSECONDS_PER_SECOND: u128 = MICROSECONDS_PER_SECOND >> 6;
const TIMER_OPERATORS: u64 = 24;
const DEFAULT_PRESCALE: u32 = 6;
const DEFAULT_TIMER_B: u8 = 200;
const DEFAULT_MEASURE_LENGTH: u32 = 96;
const TEMPO_CONSTANT: u32 = 0x112c;

pub(crate) const OPNA_CLOCK_HZ: u32 = 7_987_200;

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) struct Tempo {
    timer_b: u8,
    timer_b_saved: u8,
    quarter: u8,
    quarter_saved: u8,
}

impl Tempo {
    pub(crate) fn new() -> Self {
        let timer_b = DEFAULT_TIMER_B;
        let quarter = timer_b_to_quarter(timer_b);
        Self {
            timer_b,
            timer_b_saved: timer_b,
            quarter,
            quarter_saved: quarter,
        }
    }

    pub(crate) fn timer_b(self) -> u8 {
        self.timer_b
    }

    pub(crate) fn quarter(self) -> u8 {
        self.quarter
    }

    pub(crate) fn set_timer_b(&mut self, value: u8) {
        let value = value.min(250);
        self.timer_b = value;
        self.timer_b_saved = value;
        self.quarter = timer_b_to_quarter(value);
        self.quarter_saved = self.quarter;
    }

    pub(crate) fn set_quarter(&mut self, value: u8) {
        let value = value.max(18);
        self.quarter = value;
        self.quarter_saved = value;
        self.timer_b = quarter_to_timer_b(value);
        self.timer_b_saved = self.timer_b;
    }

    pub(crate) fn adjust_timer_b(&mut self, delta: i8) {
        let value = (i16::from(self.timer_b_saved) + i16::from(delta)).clamp(0, 250) as u8;
        self.timer_b = value;
        self.timer_b_saved = value;
        self.quarter = timer_b_to_quarter(value);
        self.quarter_saved = self.quarter;
    }

    pub(crate) fn adjust_quarter(&mut self, delta: i8) {
        let value = (i16::from(self.quarter_saved) + i16::from(delta)).clamp(18, 255) as u8;
        self.quarter = value;
        self.quarter_saved = value;
        self.timer_b = quarter_to_timer_b(value);
        self.timer_b_saved = self.timer_b;
    }
}

pub(crate) fn timer_b_to_quarter(timer_b: u8) -> u8 {
    let denominator = 256u32 - u32::from(timer_b);
    if denominator == 0 {
        return 255;
    }

    let value = (TEMPO_CONSTANT * 2 / denominator).div_ceil(2);
    value.min(255) as u8
}

pub(crate) fn quarter_to_timer_b(quarter: u8) -> u8 {
    if quarter < 18 {
        return 0;
    }

    let quarter = u32::from(quarter);
    let mut value = 256 - TEMPO_CONSTANT / quarter;
    if TEMPO_CONSTANT % quarter >= 128 {
        value -= 1;
    }
    value as u8
}

#[derive(Clone, Copy, Debug, Default, Eq, PartialEq)]
pub(crate) struct TimerStatus {
    pub(crate) timer_a: bool,
    pub(crate) timer_b: bool,
}

impl TimerStatus {
    fn from_bits(bits: u8) -> Self {
        Self {
            timer_a: bits & 0x01 != 0,
            timer_b: bits & 0x02 != 0,
        }
    }
}

#[derive(Clone, Copy, Debug, Default, Eq, PartialEq)]
pub(crate) struct SongPosition {
    pub(crate) measure: u32,
    pub(crate) tick: u32,
}

impl SongPosition {
    fn advance(&mut self, measure_length: u32) {
        if self.tick + 1 == measure_length {
            self.measure = self.measure.wrapping_add(1);
            self.tick = 0;
        } else {
            self.tick += 1;
        }
    }

    pub(crate) fn count(self, measure_length: u32) -> u32 {
        self.measure
            .wrapping_mul(measure_length)
            .wrapping_add(self.tick)
    }
}

#[derive(Clone, Debug)]
pub(crate) struct TimerClock {
    clock_hz: u32,
    prescale: u32,
    timer_a_value: u16,
    timer_b_value: u8,
    mode: u8,
    status: u8,
    running: [bool; 2],
    periods: [u64; 2],
    counts: [i64; 2],
}

impl TimerClock {
    pub(crate) fn new(clock_hz: u32, prescale: u32) -> Option<Self> {
        if clock_hz == 0 || !matches!(prescale, 2 | 3 | 6) {
            return None;
        }
        Some(Self {
            clock_hz,
            prescale,
            timer_a_value: 0,
            timer_b_value: 0,
            mode: 0,
            status: 0,
            running: [false; 2],
            periods: [0; 2],
            counts: [0; 2],
        })
    }

    pub(crate) fn reset(&mut self) {
        self.timer_a_value = 0;
        self.timer_b_value = 0;
        self.mode = 0;
        self.status = 0;
        self.running = [false; 2];
        self.periods = [0; 2];
        self.counts = [0; 2];
    }

    pub(crate) fn set_timer_a_value(&mut self, value: u16) {
        self.timer_a_value = value & 0x03ff;
    }

    pub(crate) fn set_timer_b_value(&mut self, value: u8) {
        self.timer_b_value = value;
    }

    pub(crate) fn timer_a_value(&self) -> u16 {
        self.timer_a_value
    }

    pub(crate) fn timer_b_value(&self) -> u8 {
        self.timer_b_value
    }

    pub(crate) fn mode(&self) -> u8 {
        self.mode
    }

    pub(crate) fn status(&self) -> TimerStatus {
        TimerStatus::from_bits(self.status)
    }

    pub(crate) fn period(&self, timer: usize) -> u64 {
        self.periods[timer]
    }

    pub(crate) fn write_mode(&mut self, mode: u8, total_clocks: u8) {
        self.mode = mode;
        if mode & 0x20 != 0 {
            self.status &= !0x02;
        }
        if mode & 0x10 != 0 {
            self.status &= !0x01;
        }

        self.update_timer(1, mode & 0x02 != 0, -(i64::from(total_clocks & 0x0f)));
        self.update_timer(0, mode & 0x01 != 0, 0);
    }

    pub(crate) fn count(&mut self, microseconds: u32) -> TimerStatus {
        let delta = fixed_microseconds(microseconds) as i64;
        if self.mode & 0x01 != 0 && self.counts[0] > 0 {
            self.counts[0] -= delta;
        }
        if self.mode & 0x02 != 0 && self.counts[1] > 0 {
            self.counts[1] -= delta;
        }

        if self.mode & 0x04 != 0 && self.counts[0] < 0 {
            self.status |= 0x01;
            self.reload(0);
        }
        if self.mode & 0x08 != 0 && self.counts[1] < 0 {
            self.status |= 0x02;
            self.reload(1);
        }
        self.status()
    }

    pub(crate) fn next_event_microseconds(&self) -> u32 {
        let count = self.counts.iter().copied().filter(|value| *value > 0).min();
        let Some(count) = count else {
            return 0;
        };

        let fixed = ((count as u128 + FIXED_TIME_ONE / MICROSECONDS_PER_SECOND)
            * REDUCED_MICROSECONDS_PER_SECOND)
            >> (FIXED_TIME_SHIFT - 6);
        fixed.min(u128::from(u32::MAX)) as u32
    }

    fn update_timer(&mut self, timer: usize, load: bool, delta_clocks: i64) {
        if load && !self.running[timer] {
            let period = self.period_for(timer, delta_clocks);
            self.periods[timer] = period.min(u128::from(i64::MAX as u64)) as u64;
            self.counts[timer] = self.periods[timer] as i64;
            self.running[timer] = true;
        } else if !load {
            self.periods[timer] = 0;
            self.counts[timer] = 0;
            self.running[timer] = false;
        }
    }

    fn reload(&mut self, timer: usize) {
        let period = self.period_for(timer, 0);
        self.periods[timer] = period.min(u128::from(i64::MAX as u64)) as u64;
        if self.periods[timer] == 0 {
            self.counts[timer] = 0;
            return;
        }

        self.counts[timer] = self.periods[timer] as i64;
    }

    fn period_for(&self, timer: usize, delta_clocks: i64) -> u128 {
        let base = if timer == 0 {
            1024u64 - u64::from(self.timer_a_value)
        } else {
            16 * (256u64 - u64::from(self.timer_b_value))
        };
        let duration = (base as i64 + delta_clocks) as u64;
        let clocks = duration * TIMER_OPERATORS * u64::from(self.prescale);
        ((u128::from(clocks) << 43) / u128::from(self.clock_hz)) << 5
    }
}

fn fixed_microseconds(microseconds: u32) -> u128 {
    (u128::from(microseconds) << (FIXED_TIME_SHIFT - 6)) / REDUCED_MICROSECONDS_PER_SECOND
}

#[derive(Clone, Debug)]
pub(crate) struct PmdTiming {
    pub(crate) tempo: Tempo,
    pub(crate) timers: TimerClock,
    position: SongPosition,
    measure_length: u32,
    timer_a_time: u32,
    last_timer_a_time: u32,
    timer_b_ticks: u64,
    elapsed_us: u64,
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) struct TimerBTick {
    pub(crate) position: SongPosition,
    pub(crate) timer_a_delta: u32,
    pub(crate) tick: u64,
    pub(crate) time_us: u64,
    pub(crate) timer_a_count: u8,
    pub(crate) timer_a_times: [u64; 8],
}

impl PmdTiming {
    pub(crate) fn new(clock_hz: u32) -> Option<Self> {
        Some(Self {
            tempo: Tempo::new(),
            timers: TimerClock::new(clock_hz, DEFAULT_PRESCALE)?,
            position: SongPosition::default(),
            measure_length: DEFAULT_MEASURE_LENGTH,
            timer_a_time: 0,
            last_timer_a_time: 0,
            timer_b_ticks: 0,
            elapsed_us: 0,
        })
    }

    pub(crate) fn start(&mut self) {
        self.start_with_timer_b_delta(0);
    }

    pub(crate) fn start_with_timer_b_delta(&mut self, delta_clocks: u8) {
        self.tempo = Tempo::new();
        self.timers.reset();
        self.timers.set_timer_a_value(0);
        self.timers.set_timer_b_value(self.tempo.timer_b());
        self.timers.write_mode(0x3f, delta_clocks);
        self.position = SongPosition::default();
        self.measure_length = DEFAULT_MEASURE_LENGTH;
        self.timer_a_time = 0;
        self.last_timer_a_time = 0;
        self.timer_b_ticks = 0;
        self.elapsed_us = 0;
    }

    pub(crate) fn position(&self) -> SongPosition {
        self.position
    }

    pub(crate) fn timer_a_time(&self) -> u32 {
        self.timer_a_time
    }

    pub(crate) fn timer_a_tick(&mut self) {
        self.timer_a_time = self.timer_a_time.wrapping_add(1);
    }

    pub(crate) fn timer_b_tick(&mut self) -> TimerBTick {
        let mut timer_a_times = [0; 8];
        let mut timer_a_count = 0usize;
        loop {
            let interval = self.timers.next_event_microseconds();
            if interval == 0 {
                break;
            }
            self.elapsed_us = self.elapsed_us.saturating_add(u64::from(interval));
            let status = self.timers.count(interval);
            if status.timer_a {
                self.timer_a_time = self.timer_a_time.wrapping_add(1);
                if let Some(slot) = timer_a_times.get_mut(timer_a_count) {
                    *slot = self.elapsed_us;
                }
                timer_a_count = timer_a_count.saturating_add(1).min(timer_a_times.len());
            }
            if status.timer_b {
                self.timer_b_ticks = self.timer_b_ticks.saturating_add(1);
                self.position.advance(self.measure_length);
                let timer_a_delta = self.timer_a_time.wrapping_sub(self.last_timer_a_time);
                self.last_timer_a_time = self.timer_a_time;

                self.timers.write_mode(0x3f, 0);
                return TimerBTick {
                    position: self.position,
                    timer_a_delta,
                    tick: self.timer_b_ticks,
                    time_us: self.elapsed_us,
                    timer_a_count: timer_a_count as u8,
                    timer_a_times,
                };
            }
            self.timers.write_mode(0x3f, 0);
        }

        self.timer_b_ticks = self.timer_b_ticks.saturating_add(1);
        self.position.advance(self.measure_length);
        let timer_a_delta = self.timer_a_time.wrapping_sub(self.last_timer_a_time);
        self.last_timer_a_time = self.timer_a_time;
        TimerBTick {
            position: self.position,
            timer_a_delta,
            tick: self.timer_b_ticks,
            time_us: self.elapsed_us,
            timer_a_count: timer_a_count as u8,
            timer_a_times,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::{
        quarter_to_timer_b, timer_b_to_quarter, PmdTiming, SongPosition, Tempo, TimerClock,
        TimerStatus, OPNA_CLOCK_HZ,
    };

    #[test]
    fn tempo_conversions_match_pmd_integer_arithmetic() {
        assert_eq!(timer_b_to_quarter(0), 17);
        assert_eq!(timer_b_to_quarter(200), 79);
        assert_eq!(timer_b_to_quarter(250), 255);
        assert_eq!(quarter_to_timer_b(17), 0);
        assert_eq!(quarter_to_timer_b(18), 12);
        assert_eq!(quarter_to_timer_b(79), 201);
        assert_eq!(quarter_to_timer_b(255), 239);
    }

    #[test]
    fn tempo_absolute_and_relative_commands_keep_saved_values() {
        let mut tempo = Tempo::new();
        assert_eq!(tempo.timer_b(), 200);
        assert_eq!(tempo.quarter(), 79);

        tempo.adjust_timer_b(-5);
        assert_eq!(tempo.timer_b(), 195);
        tempo.adjust_timer_b(100);
        assert_eq!(tempo.timer_b(), 250);
        tempo.adjust_timer_b(-127);
        assert_eq!(tempo.timer_b(), 123);

        tempo.set_quarter(10);
        assert_eq!(tempo.quarter(), 18);
        tempo.adjust_quarter(-127);
        assert_eq!(tempo.quarter(), 18);
        tempo.adjust_quarter(127);
        assert_eq!(tempo.quarter(), 145);
        tempo.set_timer_b(255);
        assert_eq!(tempo.timer_b(), 250);
    }

    #[test]
    fn timer_periods_and_next_events_match_ymfm_vectors() {
        let mut timers = TimerClock::new(OPNA_CLOCK_HZ, 6).expect("valid OPNA clock");
        timers.set_timer_a_value(0);
        timers.set_timer_b_value(200);
        timers.write_mode(0x3f, 0);
        assert_eq!(timers.period(0), 5_196_461_108_480);
        assert_eq!(timers.period(1), 4_546_903_469_920);
        assert_eq!(timers.next_event_microseconds(), 16_154);

        let status = timers.count(16_154);
        assert_eq!(
            status,
            TimerStatus {
                timer_a: false,
                timer_b: true
            }
        );
        timers.write_mode(0x3f, 0);
        assert_eq!(timers.next_event_microseconds(), 2_308);
        let status = timers.count(2_308);
        assert_eq!(
            status,
            TimerStatus {
                timer_a: true,
                timer_b: false
            }
        );
    }

    #[test]
    fn mode_reset_disables_countdown_until_load_bits_return() {
        let mut timers = TimerClock::new(OPNA_CLOCK_HZ, 6).expect("valid OPNA clock");
        timers.set_timer_b_value(200);
        timers.write_mode(0x3f, 0);
        timers.write_mode(0x30, 0);
        assert_eq!(timers.next_event_microseconds(), 0);
        assert_eq!(timers.status(), TimerStatus::default());
        timers.write_mode(0x3f, 0);
        assert_eq!(timers.next_event_microseconds(), 16_154);
    }

    #[test]
    fn timer_b_position_and_timer_a_delta_match_driver_order() {
        let mut timing = PmdTiming::new(OPNA_CLOCK_HZ).expect("valid OPNA clock");
        timing.start();
        assert_eq!(timing.position(), SongPosition::default());
        timing.timer_a_tick();
        timing.timer_a_tick();
        let first = timing.timer_b_tick();
        assert_eq!(
            first.position,
            SongPosition {
                measure: 0,
                tick: 1
            }
        );
        assert_eq!(first.timer_a_delta, 2);
        assert_eq!(first.tick, 1);
        assert_eq!(first.time_us, 16_154);
        let second = timing.timer_b_tick();
        assert_eq!(
            second.position,
            SongPosition {
                measure: 0,
                tick: 2
            }
        );
        assert_eq!(second.timer_a_delta, 1);
        assert_eq!(second.timer_a_count, 1);
        assert_eq!(second.timer_a_times[0], 18_462);
        assert_eq!(second.tick, 2);
        assert_eq!(second.time_us, 32_308);
        assert_eq!(timing.position().count(96), 2);
    }

    #[test]
    fn timer_b_register_changes_apply_on_the_following_reload() {
        let mut timing = PmdTiming::new(OPNA_CLOCK_HZ).expect("valid OPNA clock");
        timing.start();

        assert_eq!(timing.timer_b_tick().time_us, 16_154);
        timing.timers.set_timer_b_value(197);
        assert_eq!(timing.timer_b_tick().time_us, 32_308);
        assert_eq!(timing.timer_b_tick().time_us, 49_328);
    }
}
