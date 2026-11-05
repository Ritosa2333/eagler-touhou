# pmd-core

`pmd-core` is the filesystem-free PMD player used by the native and WebAssembly
frontends.

It is `no_std` with `alloc`, uses no threads or filesystem access, and forbids
`unsafe`. The caller provides the main file and companion files through
`FileProvider`, then renders interleaved stereo samples with `Player`.

```rust
use pmd_core::{FileProvider, Player};

struct Files;

impl FileProvider for Files {
    fn get(&self, _name: &str) -> Option<&[u8]> {
        None
    }
}

fn play(main_file: &[u8]) -> Result<(), pmd_core::Error> {
    let mut player = Player::new(44_100);
    player.load(main_file, &Files)?;

    let mut samples = [0.0_f32; 2048];
    player.render(&mut samples);
    Ok(())
}
```

The crate exposes PMD metadata, channel snapshots, voice peaks, and playback
status for frontends.

See the [repository README](../README.md) for build and run instructions.

## License

AGPL-3.0-only. Third-party attributions and terms are listed in
[`NOTICE`](../NOTICE).
