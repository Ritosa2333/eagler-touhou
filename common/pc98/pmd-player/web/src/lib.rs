use pmd_core::{FileProvider, Player};

struct NoCompanions;
impl FileProvider for NoCompanions {
    fn get(&self, _: &str) -> Option<&[u8]> { None }
}
pub struct Instance {
    player: Player,
    input: Vec<u8>,
    output: Vec<f32>,
}
#[no_mangle]
pub extern "C" fn pmd_create(rate: u32) -> *mut Instance {
    Box::into_raw(Box::new(Instance { player: Player::new(rate), input: Vec::new(), output: Vec::new() }))
}
#[no_mangle]
pub unsafe extern "C" fn pmd_destroy(instance: *mut Instance) {
    drop(Box::from_raw(instance));
}
#[no_mangle]
pub unsafe extern "C" fn pmd_input(instance: *mut Instance, length: usize) -> *mut u8 {
    let instance = &mut *instance;
    instance.input.resize(length, 0);
    instance.input.as_mut_ptr()
}
#[no_mangle]
pub unsafe extern "C" fn pmd_load(instance: *mut Instance) -> bool {
    let instance = &mut *instance;
    if instance.player.load(&instance.input, &NoCompanions).is_err() { return false; }
    instance.player.set_loop_count(None);
    true
}
#[no_mangle]
pub unsafe extern "C" fn pmd_render(instance: *mut Instance, frames: usize) -> *const f32 {
    let instance = &mut *instance;
    instance.output.resize(frames * 2, 0.0);
    instance.player.render(&mut instance.output);
    instance.output.as_ptr()
}
