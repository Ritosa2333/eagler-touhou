#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

mod games;
mod overlay;

use games::{Catalog, GameStatus, Library};
use std::{path::PathBuf, sync::{Arc, Mutex}};
use url::Url;
use tauri::{ipc::Channel, Manager, State, WebviewWindow};
use tauri_plugin_dialog::DialogExt;

type Desktop = Arc<Mutex<Library>>;

fn overlay_site(address: Url) -> Url {
    let allowed = matches!((address.scheme(), address.host_str()),
        ("http", Some("127.0.0.1")) |
        ("https", Some("touhouonline.com" | "test.touhouonline.com")));
    if allowed {
        address.join("/lobby.html").expect("Launcher URL")
    } else {
        Url::parse("https://touhouonline.com/lobby.html").expect("Launcher URL")
    }
}

#[tauri::command]
fn games(state: State<'_, Desktop>) -> Result<Vec<GameStatus>, String> {
    Ok(state.lock().map_err(|error| error.to_string())?.games())
}

#[tauri::command]
async fn swarm_ping(addresses: Vec<std::net::Ipv4Addr>) -> Result<Vec<Option<u32>>, String> {
    if addresses.len() > 24 || addresses.iter().any(|ip| {
        ip.is_private() || ip.is_loopback() || ip.is_link_local() || ip.is_multicast()
            || ip.is_unspecified() || ip.is_broadcast() || ip.is_documentation()
    }) {
        return Err("Invalid Swarm node addresses".into());
    }
    tauri::async_runtime::spawn_blocking(move || {
        #[cfg(windows)]
        {
            use std::{os::windows::process::CommandExt, process::Command};
            let executable = std::env::var_os("SystemRoot").map(PathBuf::from)
                .unwrap_or_else(|| PathBuf::from(r"C:\Windows")).join("System32").join("PING.EXE");
            let probes: Vec<_> = addresses.into_iter().map(|ip| {
                let executable = executable.clone();
                std::thread::spawn(move || {
                    let output = Command::new(executable).creation_flags(0x08000000)
                        .args(["-4", "-n", "2", "-w", "400", &ip.to_string()]).output().ok()?;
                    let data = String::from_utf8_lossy(&output.stdout);
                    data.lines().filter(|line| line.to_ascii_uppercase().contains("TTL="))
                        .filter_map(|line| {
                            let head = line.split("ms").next()?.trim_end();
                            let digits = head.trim_end_matches(|ch: char| ch.is_ascii_digit());
                            head[digits.len()..].parse::<u32>().ok()
                        }).min()
                })
            }).collect();
            probes.into_iter().map(|probe| probe.join().ok().flatten()).collect()
        }
        #[cfg(not(windows))]
        { vec![None; addresses.len()] }
    }).await.map_err(|error| error.to_string())
}

#[tauri::command]
async fn choose_directory(
    window: WebviewWindow,
    state: State<'_, Desktop>,
) -> Result<Option<Vec<GameStatus>>, String> {
    let Some(directory) = window
        .dialog()
        .file()
        .set_parent(&window)
        .set_title("选择游戏目录或上一级目录")
        .blocking_pick_folder()
    else {
        return Ok(None);
    };
    let directory = directory.into_path().map_err(|error| error.to_string())?;
    state
        .lock()
        .map_err(|error| error.to_string())?
        .select(directory)
        .map(Some)
}

#[tauri::command]
async fn prepare_game(
    app: tauri::AppHandle,
    game: String,
    progress: Channel<String>,
) -> Result<GameStatus, String> {
    let library = app.state::<Desktop>().lock().map_err(|error| error.to_string())?.clone();
    tauri::async_runtime::spawn_blocking(move || {
        library.prepare(&game, |file| {
            let _ = progress.send(file);
        })
    })
    .await
    .map_err(|error| error.to_string())?
}

#[tauri::command]
fn launch_game(window: WebviewWindow, state: State<'_, Desktop>, overlay: State<'_, overlay::Overlay>, game: String, room: Option<serde_json::Value>, display_mode: Option<games::DisplayMode>, reuse_overlay: Option<String>) -> Result<games::Launch, String> {
    let site = overlay_site(window.url().map_err(|error| error.to_string())?);
    state.lock().map_err(|error| error.to_string())?.launch(&game, room, display_mode, reuse_overlay.as_deref(), &overlay, &site, window)
}

fn main() {
    tauri::Builder::default()
        .plugin(tauri_plugin_dialog::init())
        .setup(|app| {
            let app_data = std::env::var_os("EAGLER_DESKTOP_DATA")
                .map(PathBuf::from)
                .unwrap_or(app.path().app_local_data_dir()?);
            let config = std::env::var_os("EAGLER_DESKTOP_CATALOG")
                .map(PathBuf::from)
                .unwrap_or_else(|| app_data.join("catalog.json"));
            let library = Arc::new(Mutex::new(Library::open(Catalog::load(&config)?, app_data)?));
            app.manage(overlay::Overlay::new(library.clone()));
            app.manage(library);
            Ok(())
        })
        .invoke_handler(tauri::generate_handler![
            games, choose_directory, prepare_game, launch_game, swarm_ping
        ])
        .run(tauri::generate_context!())
        .expect("start desktop launcher");
}
