fn main() {
    tauri_build::try_build(tauri_build::Attributes::new().app_manifest(
        tauri_build::AppManifest::new().commands(&[
            "games", "choose_directory", "prepare_game", "launch_game", "swarm_ping",
        ]),
    ))
    .expect("build desktop launcher");
}
