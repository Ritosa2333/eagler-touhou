use reqwest::{blocking::Client, redirect::Policy, Url};
use serde::{Deserialize, Serialize};
use sha2::{Digest, Sha256};
use std::{
    collections::BTreeMap,
    fs::{self, File, OpenOptions},
    io::{self, Read, Write},
    path::{Component, Path, PathBuf},
    process::{Command, Stdio},
    time::Duration,
};
use tempfile::NamedTempFile;

#[derive(Default, Deserialize)]
#[serde(deny_unknown_fields)]
pub struct Catalog {
    games: BTreeMap<String, Game>,
}

#[derive(Deserialize)]
#[serde(deny_unknown_fields)]
struct Game {
    title: String,
    executable: PathBuf,
    archives: Vec<PathBuf>,
    #[serde(default)]
    arguments: Vec<String>,
    #[serde(default)]
    library_paths: Vec<PathBuf>,
    #[serde(default)]
    environment: BTreeMap<String, String>,
    #[serde(default)]
    display_mode: bool,
    #[serde(default)]
    native_overlay: bool,
    #[serde(default)]
    required_files: Vec<PathBuf>,
    #[serde(default)]
    music: Music,
    package: Option<Url>,
    room: Option<RoomBuild>,
}

#[derive(Default, Deserialize)]
#[serde(deny_unknown_fields)]
struct Music {
    #[serde(default)]
    ogg: Vec<PathBuf>,
    packed_wav: Option<PathBuf>,
    #[serde(default)]
    loose_wav: Vec<PathBuf>,
}

#[derive(Deserialize, Serialize)]
#[serde(deny_unknown_fields)]
struct RoomBuild {
    build: String,
    generation: String,
}

#[derive(Deserialize)]
struct Package {
    schema: String,
    game: String,
    #[serde(rename = "runtimeRequirement")]
    requirement: Requirement,
    files: BTreeMap<String, Asset>,
    components: BTreeMap<String, PackageComponent>,
}

#[derive(Deserialize)]
struct Requirement {
    #[serde(rename = "dataFile")]
    data_file: String,
}

#[derive(Deserialize)]
struct PackageComponent {
    files: Option<Vec<String>>,
}

#[derive(Deserialize)]
struct Asset {
    source: String,
    target: String,
    bytes: u64,
    sha256: String,
}

#[derive(Serialize)]
#[serde(rename_all = "camelCase")]
pub struct GameStatus {
    id: String,
    title: String,
    directory: PathBuf,
    missing: Vec<PathBuf>,
    music: &'static str,
    can_download: bool,
    room_generation: Option<String>,
    swarm_direct: bool,
    display_mode: bool,
}

#[derive(Deserialize)]
#[serde(rename_all = "kebab-case")]
pub enum DisplayMode {
    Windowed,
    Borderless,
    Exclusive,
}

#[derive(Clone)]
pub struct Library {
    catalog: std::sync::Arc<Catalog>,
    app_data: PathBuf,
    directories: BTreeMap<String, PathBuf>,
}

fn relative(path: &Path) -> bool {
    !path.as_os_str().is_empty()
        && path.components().all(|part| matches!(part, Component::Normal(_)))
}

fn has_file(path: &Path, magic: &[u8]) -> bool {
    let Ok(mut file) = File::open(path) else { return false };
    let mut header = [0; 4];
    file.metadata().is_ok_and(|info| info.is_file() && info.len() > magic.len() as u64)
        && file.read_exact(&mut header[..magic.len()]).is_ok()
        && header[..magic.len()] == *magic
}

fn packed_wav(path: &Path) -> bool {
    File::open(path).is_ok_and(|mut file| {
        let mut header = [0; 8];
        file.read_exact(&mut header).is_ok()
            && header == *b"ZWAV\x01\x00\x00\x00"
            && file.metadata().is_ok_and(|info| info.is_file() && info.len() > 8)
    })
}

impl Catalog {
    pub fn load(path: &Path) -> Result<Self, String> {
        if !path.exists() {
            return Ok(Self::default());
        }
        let catalog: Self = serde_json::from_reader(File::open(path).map_err(|e| e.to_string())?)
            .map_err(|e| e.to_string())?;
        for (id, game) in &catalog.games {
            let invalid_id = id.is_empty()
                || !id.bytes().all(|c| c.is_ascii_lowercase() || c.is_ascii_digit() || c == b'-');
            if invalid_id || game.archives.is_empty() || !game.executable.is_absolute()
                || game.room.as_ref().is_some_and(|room| {
                    let valid = |value: &str, size| value.len() == size
                        && value.bytes().all(|byte| byte.is_ascii_digit() || (b'a'..=b'f').contains(&byte));
                    !valid(&room.build, 24) || !valid(&room.generation, 64)
                })
                || game.archives.iter().chain(&game.required_files).chain(&game.music.ogg)
                    .chain(&game.music.loose_wav).chain(game.music.packed_wav.iter())
                    .any(|file| !relative(file))
            {
                return Err(format!("无效的桌面游戏配置 {id}"));
            }
        }
        Ok(catalog)
    }
}

impl Game {
    fn music_kind(&self, directory: &Path) -> &'static str {
        if self.music.packed_wav.as_ref().is_some_and(|file| packed_wav(&directory.join(file)))
            || !self.music.loose_wav.is_empty()
                && self.music.loose_wav.iter().all(|file| has_file(&directory.join(file), b"RIFF"))
        {
            "wav"
        } else if self.music.ogg.is_empty() {
            "none"
        } else if self.music.ogg.iter().all(|file| has_file(&directory.join(file), b"OggS")) {
            "ogg"
        } else {
            "missing"
        }
    }

    fn status(&self, id: &str, directory: PathBuf) -> GameStatus {
        let music = self.music_kind(&directory);
        let mut missing: Vec<_> = self.archives.iter().chain(&self.required_files)
            .filter(|file| !has_file(&directory.join(file), b"")).cloned().collect();
        if music == "missing" {
            missing.extend(self.music.ogg.iter().filter(|file| !has_file(&directory.join(file), b"OggS")).cloned());
        }
        GameStatus {
            id: id.into(), title: self.title.clone(), directory, missing, music,
            can_download: self.package.is_some() || !self.required_files.is_empty(),
            room_generation: self.room.as_ref().map(|room| room.generation.clone()),
            swarm_direct: self.room.is_some() && self.executable.with_file_name("eagler-swarm-turn.exe").is_file(),
            display_mode: self.display_mode,
        }
    }

    fn locate(&self, directory: &Path, candidates: &[PathBuf]) -> Result<Option<PathBuf>, String> {
        let complete = |root: &Path| self.archives.iter().all(|file| has_file(&root.join(file), b""));
        if complete(directory) {
            return Ok(Some(directory.into()));
        }
        let mut matches = candidates.iter().filter(|root| complete(root));
        let first = matches.next();
        if let Some(first) = first {
            if matches.any(|root| !root.starts_with(first)) {
                return Err(format!("找到多份{}。请直接选择其中一个游戏目录。", self.title));
            }
        }
        Ok(first.cloned())
    }

    fn prepare(&self, id: &str, directory: &Path, progress: impl Fn(String)) -> Result<(), String> {
        if !self.executable.is_file() {
            return Err(format!("找不到原生运行包 {}", self.executable.display()));
        }
        for file in &self.required_files {
            let target = directory.join(file);
            if !has_file(&target, b"") {
                let source = self.executable.parent().ok_or("找不到运行包目录")?.join(file);
                if has_file(&source, b"") {
                    progress(file.display().to_string());
                    install(&target, |output| {
                        io::copy(&mut File::open(source).map_err(|e| e.to_string())?, output)
                            .map_err(|e| e.to_string())?;
                        Ok(())
                    })?;
                }
            }
        }
        let status = self.status(id, directory.to_owned());
        if status.missing.is_empty() { return Ok(()); }
        let Some(package_url) = &self.package else { return Ok(()) };
        let client = Client::builder().connect_timeout(Duration::from_secs(10))
            .timeout(Duration::from_secs(180))
            .redirect(Policy::limited(8)).build().map_err(|e| e.to_string())?;
        let package: Package = serde_json::from_reader(
            client.get(package_url.clone()).send().and_then(|r| r.error_for_status())
                .map_err(|e| format!("读取资源清单失败：{e}"))?.take(2 * 1024 * 1024),
        ).map_err(|e| e.to_string())?;
        if package.schema != "eagler-touhou/package/1" || package.game != id {
            return Err("资源清单与所选游戏不匹配。".into());
        }
        for path in status.missing {
            let entry = if self.archives.len() == 1 && self.archives[0] == path {
                package.files.get(&package.requirement.data_file)
            } else if self.music.ogg.contains(&path) {
                package.components.get("ogg").and_then(|component| component.files.as_ref())
                    .into_iter().flatten().filter_map(|key| package.files.get(key))
                    .find(|asset| Path::new(&asset.target).file_name() == path.file_name())
            } else {
                None
            };
            if let Some(asset) = entry {
                progress(path.display().to_string());
                asset.download(&client, package_url, &directory.join(&path))?;
            }
        }
        Ok(())
    }
}

fn install(target: &Path, write: impl FnOnce(&mut File) -> Result<(), String>) -> Result<(), String> {
    let parent = target.parent().ok_or("无法定位目标目录")?;
    fs::create_dir_all(parent).map_err(|e| e.to_string())?;
    let mut temporary = NamedTempFile::new_in(parent).map_err(|e| e.to_string())?;
    write(temporary.as_file_mut())?;
    temporary.as_file().sync_all().map_err(|e| e.to_string())?;
    temporary.persist_noclobber(target).map_err(|e| e.to_string())?;
    Ok(())
}

impl Asset {
    fn download(&self, client: &Client, package: &Url, target: &Path) -> Result<(), String> {
        if self.source.contains(['\\', ':', '%', '?', '#'])
            || self.source.split('/').any(|part| matches!(part, "" | "." | ".."))
            || self.bytes == 0 || self.bytes > 1024 * 1024 * 1024
            || self.sha256.len() != 64 || !self.sha256.bytes().all(|c| c.is_ascii_hexdigit())
        {
            return Err("无效的资源清单条目。".into());
        }
        let url = package.join(&self.source).map_err(|e| e.to_string())?;
        let mut response = client.get(url).send().and_then(|r| r.error_for_status())
            .map_err(|e| format!("下载失败：{e}"))?.take(self.bytes + 1);
        install(target, |output| {
            let mut digest = Sha256::new();
            let mut bytes = 0;
            let mut buffer = vec![0; 64 * 1024];
            loop {
                let count = response.read(&mut buffer).map_err(|e| e.to_string())?;
                if count == 0 { break; }
                bytes += count as u64;
                digest.update(&buffer[..count]);
                output.write_all(&buffer[..count]).map_err(|e| e.to_string())?;
            }
            if bytes != self.bytes || !format!("{:x}", digest.finalize()).eq_ignore_ascii_case(&self.sha256) {
                return Err(format!("{} 校验失败。", target.display()));
            }
            Ok(())
        })
    }
}

impl Library {
    pub fn open(catalog: Catalog, app_data: PathBuf) -> Result<Self, String> {
        let file = app_data.join("directories.json");
        let directories = match File::open(file) {
            Ok(input) => serde_json::from_reader(input).map_err(|e| e.to_string())?,
            Err(error) if error.kind() == io::ErrorKind::NotFound => BTreeMap::new(),
            Err(error) => return Err(error.to_string()),
        };
        Ok(Self { catalog: catalog.into(), app_data, directories })
    }

    fn game(&self, id: &str) -> Result<&Game, String> {
        self.catalog.games.get(id).ok_or_else(|| format!("未安装 {id} 的原生运行包。"))
    }

    fn managed(&self, id: &str) -> PathBuf { self.app_data.join("games").join(id) }

    fn directory(&self, id: &str) -> PathBuf {
        self.directories.get(id).cloned().unwrap_or_else(|| self.managed(id))
    }

    pub(crate) fn status(&self, id: &str) -> Result<GameStatus, String> {
        Ok(self.game(id)?.status(id, self.directory(id)))
    }

    pub fn games(&self) -> Vec<GameStatus> {
        self.catalog.games.iter().map(|(id, game)| game.status(id, self.directory(id))).collect()
    }

    pub fn select(&mut self, directory: PathBuf) -> Result<Vec<GameStatus>, String> {
        let mut candidates = Vec::new();
        let mut pending = vec![(directory.clone(), 0)];
        while let Some((root, depth)) = pending.pop() {
            if depth == 3 { continue; }
            let entries = match fs::read_dir(&root) {
                Ok(entries) => entries,
                Err(_) if depth > 0 => continue,
                Err(error) => return Err(error.to_string()),
            };
            for entry in entries.flatten() {
                if entry.file_type().is_ok_and(|kind| kind.is_dir()) {
                    let path = entry.path();
                    if candidates.len() >= 4096 { return Err("目录太大，请选择更接近游戏的目录。".into()); }
                    pending.push((path.clone(), depth + 1));
                    candidates.push(path);
                }
            }
        }
        candidates.sort_by_key(|path| path.components().count());
        let mut next = self.directories.clone();
        let mut found = false;
        let mut ambiguous = None;
        for (id, game) in &self.catalog.games {
            match game.locate(&directory, &candidates) {
                Ok(Some(path)) => {
                    next.insert(id.clone(), path);
                    found = true;
                }
                Ok(None) => {}
                Err(error) => ambiguous = Some(error),
            }
        }
        if !found {
            return Err(ambiguous.unwrap_or_else(|| "这里没有找到已支持的游戏数据。请选择游戏目录或它的上一级。".into()));
        }
        self.save_directories(next)?;
        Ok(self.games())
    }

    fn save_directories(&mut self, next: BTreeMap<String, PathBuf>) -> Result<(), String> {
        fs::create_dir_all(&self.app_data).map_err(|e| e.to_string())?;
        let mut file = NamedTempFile::new_in(&self.app_data).map_err(|e| e.to_string())?;
        serde_json::to_writer_pretty(file.as_file_mut(), &next).map_err(|e| e.to_string())?;
        file.persist(self.app_data.join("directories.json")).map_err(|e| e.to_string())?;
        self.directories = next;
        Ok(())
    }

    pub fn prepare(&self, id: &str, progress: impl Fn(String)) -> Result<GameStatus, String> {
        let game = self.game(id)?;
        let directory = self.directory(id);
        game.prepare(id, &directory, progress)?;
        Ok(game.status(id, directory))
    }

    pub(crate) fn room_options(&self, id: &str, mut room: serde_json::Value) -> Result<serde_json::Value, String> {
        let game = self.game(id)?;
        let build = game.room.as_ref().ok_or("该原生运行包尚未支持联机。")?;
        let options = room.as_object_mut().ok_or("无效的房间配置。")?;
        let mut url = options.get("netplayUrl").and_then(|value| value.as_str())
            .and_then(|url| Url::parse(url).ok()).ok_or("无效的房间信令地址。")?;
        if !matches!(url.scheme(), "ws" | "wss") || !url.username().is_empty() || url.password().is_some() {
            return Err("无效的房间信令地址。".into());
        }
        let player = options.get("netplayPlayer").and_then(|value| value.as_u64()).ok_or("无效的玩家席位。")?;
        let count = options.get("netplayPlayerCount").and_then(|value| value.as_u64()).ok_or("无效的玩家人数。")?;
        if count < 2 || count > 8 || player >= count { return Err("无效的玩家席位。".into()); }
        let run = url.query_pairs().find(|(key, _)| key == "run")
            .and_then(|(_, value)| value.parse::<u32>().ok()).ok_or("无效的开局编号。")?;
        if !url.query_pairs().any(|(key, value)| key == "runtimeGeneration" && value == build.generation) {
            return Err("原生运行包和房间的版本不一致。".into());
        }
        let query: Vec<_> = url.query_pairs().filter(|(key, _)| !matches!(key.as_ref(), "signal" | "players" | "player" | "spectator"))
            .map(|(key, value)| (key.into_owned(), value.into_owned())).collect();
        let spectator = options.get("netplaySpectator").and_then(|value| value.as_bool()).unwrap_or(false);
        let mut pairs = url.query_pairs_mut();
        pairs.clear().extend_pairs(query).append_pair("players", &count.to_string());
        if spectator {
            let viewer = options.get("netplaySpectatorId").and_then(|value| value.as_str()).ok_or("缺少旁观标识。")?;
            if !(8..=64).contains(&viewer.len()) || !viewer.bytes().all(|byte| byte.is_ascii_alphanumeric() || b"_-".contains(&byte)) {
                return Err("无效的旁观标识。".into());
            }
            pairs.append_pair("spectator", viewer);
        } else {
            pairs.append_pair("signal", "1").append_pair("player", &player.to_string());
        }
        drop(pairs);
        options.insert("signalUrl".into(), url.as_str().into());
        options.insert("run".into(), run.into());
        options.insert("build".into(), build.build.as_str().into());
        Ok(room)
    }

    pub fn launch(&self, id: &str, room: Option<serde_json::Value>, display_mode: Option<DisplayMode>, reuse_overlay: Option<&str>, bridge: &crate::overlay::Overlay, site: &Url, window: tauri::WebviewWindow) -> Result<Launch, String> {
        let game = self.game(id)?;
        let status = game.status(id, self.directory(id));
        if !status.missing.is_empty() {
            return Err("请先补齐游戏资源。".into());
        }
        let mut command = Command::new(&game.executable);
        let native_lobby = id == "th09" && (game.native_overlay || game.executable.with_file_name("th09-lobby.exe").is_file());
        let overlay_enabled = cfg!(windows) && game.room.is_some() &&
            (native_lobby || (id != "th09" && game.executable.with_extension("dll").is_file()));
        command.current_dir(&status.directory).args(&game.arguments).envs(&game.environment)
            .env("EAGLER_DESKTOP_GAME_HOME", &status.directory)
            .stdin(Stdio::null()).stdout(Stdio::null()).stderr(Stdio::null());
        if let Some(mode) = display_mode {
            if !game.display_mode { return Err("该原生运行包不支持切换显示模式。".into()); }
            let mode = match mode {
                DisplayMode::Windowed => "windowed",
                DisplayMode::Borderless => "borderless",
                DisplayMode::Exclusive => "exclusive",
            };
            command.env("EAGLER_DESKTOP_DISPLAY_MODE", mode);
        }
        if let Some(room) = room {
            let room = self.room_options(id, room)?.to_string();
            if let Some(overlay_id) = reuse_overlay {
                if let Some(pid) = bridge.start_room(overlay_id, id, &room)? {
                    return Ok(Launch { pid, overlay: Some(overlay_id.to_owned()) });
                }
            }
            command.env("EAGLER_DESKTOP_ROOM", room);
        } else {
            command.env_remove("EAGLER_DESKTOP_ROOM");
        }
        if !game.library_paths.is_empty() {
            let path = std::env::var_os("PATH").unwrap_or_default();
            command.env("PATH", std::env::join_paths(game.library_paths.iter().cloned()
                .chain(std::env::split_paths(&path))).map_err(|e| e.to_string())?);
        }
        #[cfg(windows)]
        {
            use std::os::windows::process::CommandExt;
            command.creation_flags(0x08000000); // CREATE_NO_WINDOW
        }
        if id == "th09" {
            if let Some(version) = &game.room {
                command.env("EAGLER_TH09_BUILD", &version.build).env("EAGLER_TH09_ROOM_GENERATION", &version.generation);
            }
        }
        let connection = overlay_enabled.then(|| bridge.open(id, site, window)).transpose()?;
        if let Some(connection) = &connection {
            if !native_lobby && !game.arguments.iter().any(|argument| argument == "--overlay") { command.arg("--overlay"); }
            command.stdin(Stdio::piped())
                .env("EAGLER_DESKTOP_OVERLAY", &connection.url)
                .env("EAGLER_DESKTOP_ACCOUNT", "1")
                .env("EAGLER_DESKTOP_OVERLAY_SITE", site.as_str());
        }
        let mut game_log = None;
        if native_lobby {
            let log_path = self.app_data.join("th09-runtime.log");
            if let Ok(log) = OpenOptions::new().create(true).truncate(true).write(true).open(log_path) {
                if let (Ok(stdout), Ok(stderr)) = (log.try_clone(), log.try_clone()) {
                    command.stdout(Stdio::from(stdout)).stderr(Stdio::from(stderr));
                    game_log = Some(log);
                }
            }
        }
        let mut child = command.spawn().map_err(|e| {
            if let Some(connection) = &connection { connection.close(); }
            format!("启动失败：{e}")
        })?;
        let pid = child.id();
        if let Some(connection) = &connection {
            connection.attach_child(pid, child.stdin.take());
        }
        let id = connection.as_ref().map(|connection| connection.id.clone());
        let attached = connection.clone().map(|connection| bridge.attach(connection)).transpose();
        std::thread::spawn(move || {
            let exit = child.wait();
            if let Some(mut log) = game_log {
                match exit {
                    Ok(status) => { let _ = writeln!(log, "th09 process {pid} exited with {status}"); }
                    Err(error) => { let _ = writeln!(log, "Could not read th09 process {pid} exit status: {error}"); }
                }
            }
            if let Some(connection) = connection { connection.close(); }
        });
        attached?;
        Ok(Launch { pid, overlay: id })
    }
}

#[derive(serde::Serialize)]
pub struct Launch {
    pub(crate) pid: u32,
    pub(crate) overlay: Option<String>,
}
