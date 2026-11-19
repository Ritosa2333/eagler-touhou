use crate::games::Launch;
use crate::Desktop;
use base64::{engine::general_purpose::STANDARD, Engine};
use serde::Deserialize;
use serde_json::{json, Value};
use reqwest::{blocking::Client, header, redirect::Policy, Url};
use std::{
    io::{Read, Write},
    process::ChildStdin,
    sync::{Arc, Mutex},
    thread,
    time::Duration,
};
use tauri::{Manager, WebviewUrl, WebviewWindowBuilder};
use tiny_http::{Header, Method, Response, Server};
use uuid::Uuid;

#[derive(Default)]
struct Session {
    child: Option<(u32, ChildStdin)>,
    closed: bool,
}

pub struct Overlay {
    library: Desktop,
    active: Mutex<Option<Connection>>,
}

#[derive(Clone)]
pub struct Connection {
    pub id: String,
    pub url: String,
    game: String,
    session: Arc<Mutex<Session>>,
}

#[derive(Deserialize)]
#[serde(tag = "command", content = "args", rename_all = "snake_case")]
enum Request {
    Games,
    PrepareGame { game: String },
    LaunchGame { game: String, room: Value },
    SwarmPing { addresses: Vec<std::net::Ipv4Addr> },
    RoomIdentity { challenge: String },
    OpenAccount {},
    RoomProfiles { players: Vec<PublicProfile> },
}

#[derive(Deserialize)]
struct PublicProfile {
    username: String,
    #[serde(default)]
    avatar: String,
}

fn public_profiles(endpoint: &Url, players: Vec<PublicProfile>) -> Result<Value, String> {
    if players.len() > 2 || players.iter().any(|entry| {
        let name = &entry.username;
        name.is_empty() || name.len() > 32 || name == "." || name == ".." ||
        !name.bytes().all(|ch| ch.is_ascii_lowercase() || ch.is_ascii_digit() || b"._-".contains(&ch)) ||
        (!entry.avatar.is_empty() && !(entry.avatar.len() <= 64 &&
            entry.avatar.bytes().all(|ch| ch.is_ascii_digit() || (b'a'..=b'f').contains(&ch))))
    }) { return Err("无效的玩家资料请求".into()); }
    let mut ratings = endpoint.join("/api/v1/th09/ratings").map_err(|error| error.to_string())?;
    ratings.query_pairs_mut().append_pair("cards",
        &players.iter().map(|entry| entry.username.as_str()).collect::<Vec<_>>().join(","));
    let client = Client::builder().timeout(Duration::from_millis(1400)).redirect(Policy::none())
        .build().map_err(|_| "玩家资料暂不可用")?;
    let versions = client.get(ratings).send().ok().and_then(|response| {
        if !response.status().is_success() { return None; }
        serde_json::from_reader::<_, Value>(response.take(32 * 1024)).ok()
    }).unwrap_or_default();
    let ratings = players.iter().filter_map(|entry| {
        let rating = versions.get("players")?.get(&entry.username)?.get("rating")?.as_i64()?;
        Some((entry.username.clone(), json!(rating)))
    }).collect::<serde_json::Map<String,Value>>();
    let mut avatars = serde_json::Map::new();
    for entry in players {
        if entry.avatar.is_empty() { continue; }
        let Ok(url) = endpoint.join(&format!("/api/v1/profiles/{}/avatar/{}", entry.username, entry.avatar)) else { continue };
        let Ok(response) = client.get(url).send() else { continue };
        if !response.status().is_success() { continue; }
        let kind = response.headers().get(header::CONTENT_TYPE).and_then(|value| value.to_str().ok()).unwrap_or("");
        let kind = match kind { "image/webp" => "webp", "image/jpeg" => "jpeg", _ => continue };
        let mut bytes = Vec::new();
        if response.take(8193).read_to_end(&mut bytes).is_err() || bytes.is_empty() || bytes.len() > 8192 { continue; }
        avatars.insert(entry.avatar, json!({"type": kind, "data": STANDARD.encode(bytes)}));
    }
    Ok(json!({"ratings": ratings, "avatars": avatars}))
}

fn open_account(window: &tauri::WebviewWindow, endpoint: &Url) -> Result<Value, String> {
    let url = endpoint.join("/login.html").map_err(|error| error.to_string())?;
    let handle = window.app_handle();
    let account = if let Some(existing) = handle.get_webview_window("account") {
        existing
    } else {
        WebviewWindowBuilder::new(handle, "account", WebviewUrl::External(url))
            .title("东方联机 | 账号")
            .inner_size(720.0, 680.0)
            .build()
            .map_err(|error| error.to_string())?
    };
    account.show().map_err(|error| error.to_string())?;
    account.set_focus().map_err(|error| error.to_string())?;
    Ok(json!({}))
}

fn room_identity(window: &tauri::WebviewWindow, endpoint: &Url, challenge: &str) -> Result<Value, String> {
    if challenge.is_empty() || challenge.len() > 1024 || challenge.chars().any(char::is_control) {
        return Err("无效的房间身份请求".into());
    }
    let account = window.get_webview_window("account");
    let cookie_window = account.as_ref().unwrap_or(window);
    let cookies = cookie_window.cookies_for_url(endpoint.clone()).map_err(|_| "无法读取启动器会话")?;
    if cookies.is_empty() { return Ok(json!({})); }
    let mut cookie_header = cookies.iter().collect::<Vec<_>>();
    cookie_header.sort_by_key(|cookie| std::cmp::Reverse(cookie.path().map_or(0, str::len)));
    let cookie_header = cookie_header.iter().map(|cookie| cookie.stripped().to_string()).collect::<Vec<_>>().join("; ");
    let unavailable = |_| "房间身份服务暂不可用";
    let client = Client::builder().redirect(Policy::none()).timeout(Duration::from_secs(5))
        .build().map_err(unavailable)?;
    let response = client.post(endpoint.clone())
        .header(header::COOKIE, cookie_header)
        .header(header::ORIGIN, endpoint.origin().ascii_serialization())
        .header(header::CONTENT_TYPE, "application/json")
        .body(json!({"challenge": challenge}).to_string()).send().map_err(unavailable)?;
    if matches!(response.status().as_u16(), 401 | 403 | 404) { return Ok(json!({})); }
    if !response.status().is_success() { return Err("房间身份服务暂不可用".into()); }
    let result: Value = serde_json::from_reader(response.take(8192)).map_err(|_| "房间身份服务返回无效数据")?;
    match result.get("ticket").and_then(Value::as_str) {
        Some(ticket) if !ticket.is_empty() && ticket.len() <= 4096 => Ok(json!({"ticket": ticket})),
        Some(_) => Err("房间身份服务返回无效凭据".into()),
        None => Ok(json!({})),
    }
}

impl Connection {
    pub fn attach_child(&self, pid: u32, stdin: Option<ChildStdin>) {
        if let Ok(mut session) = self.session.lock() {
            session.child = stdin.map(|stdin| (pid, stdin));
        }
    }

    pub fn close(&self) {
        if let Ok(mut session) = self.session.lock() {
            session.closed = true;
            session.child = None;
        }
    }

    fn start_room(&self, room: &str) -> Result<Option<u32>, String> {
        let mut session = self.session.lock().map_err(|error| error.to_string())?;
        let Some((pid, stdin)) = &mut session.child else { return Ok(None) };
        if room.len() >= 4096 {
            return Err("Room configuration exceeds the native handoff limit.".into());
        }
        let mut line = room.as_bytes().to_vec();
        line.push(b'\n');
        match stdin.write_all(&line) {
            Ok(()) => Ok(Some(*pid)),
            Err(error) if error.kind() == std::io::ErrorKind::BrokenPipe => Ok(None),
            Err(error) => Err(error.to_string()),
        }
    }

    fn execute(&self, library: &Desktop, request: Request, window: &tauri::WebviewWindow, account: &Url) -> Result<Value, String> {
        match request {
            Request::Games => {
                let library = library.lock().map_err(|error| error.to_string())?;
                serde_json::to_value([library.status(&self.game)?]).map_err(|error| error.to_string())
            }
            Request::PrepareGame { game } if game == self.game => {
                let library = library.lock().map_err(|error| error.to_string())?;
                serde_json::to_value(library.status(&game)?).map_err(|error| error.to_string())
            }
            Request::LaunchGame { game, room } if game == self.game => {
                let options = library.lock().map_err(|error| error.to_string())?.room_options(&game, room)?;
                let pid = self.start_room(&options.to_string())?.ok_or("The game process has exited.")?;
                serde_json::to_value(Launch { pid, overlay: Some(self.id.clone()) }).map_err(|error| error.to_string())
            }
            Request::SwarmPing { addresses } => {
                let values = tauri::async_runtime::block_on(crate::swarm_ping(addresses))?;
                serde_json::to_value(values).map_err(|error| error.to_string())
            }
            Request::RoomIdentity { challenge } if self.game == "th09" => room_identity(window, account, &challenge),
            Request::OpenAccount {} if self.game == "th09" => open_account(window, account),
            Request::RoomProfiles { players } if self.game == "th09" => public_profiles(account, players),
            _ => Err("This operation does not belong to the running game.".into()),
        }
    }
}

impl Overlay {
    pub fn new(library: Desktop) -> Self {
        Self { library, active: Mutex::new(None) }
    }

    pub fn open(&self, game: &str, site: &Url, window: tauri::WebviewWindow) -> Result<Connection, String> {
        let server = Server::http("127.0.0.1:0").map_err(|error| error.to_string())?;
        let path = format!("/{}", Uuid::new_v4());
        let connection = Connection {
            id: Uuid::new_v4().to_string(),
            url: format!("http://{}{}", server.server_addr(), path),
            game: game.to_owned(),
            session: Arc::new(Mutex::new(Session::default())),
        };
        let worker = connection.clone();
        let library = self.library.clone();
        let account = site.join("/api/v1/me/room-nickname").map_err(|error| error.to_string())?;
        thread::spawn(move || {
            while !worker.session.lock().map(|session| session.closed).unwrap_or(true) {
                let Ok(Some(mut request)) = server.recv_timeout(Duration::from_millis(250)) else { continue };
                let trusted = request.headers().iter().any(|header|
                    header.field.equiv("Origin") && header.value.as_str() == "eagler-overlay://panel");
                if !trusted || request.url() != path || request.method() != &Method::Post {
                    let _ = request.respond(Response::empty(404));
                    continue;
                }
                if !matches!(request.body_length(), Some(length) if length <= 16 * 1024) {
                    let _ = request.respond(Response::empty(413));
                    continue;
                }
                let result = serde_json::from_reader(request.as_reader().take(16 * 1024))
                    .map_err(|error| error.to_string()).and_then(|input| worker.execute(&library, input, &window, &account));
                let (status, body) = match result {
                    Ok(value) => (200, value),
                    Err(error) => (400, json!({"error": error})),
                };
                let mut response = Response::from_string(body.to_string()).with_status_code(status);
                for (name, value) in [("Cache-Control", "no-store"), ("Content-Type", "application/json")] {
                    if let Ok(header) = Header::from_bytes(name, value) { response.add_header(header); }
                }
                let _ = request.respond(response);
            }
        });
        Ok(connection)
    }

    pub fn attach(&self, connection: Connection) -> Result<(), String> {
        let mut slot = self.active.lock().map_err(|error| error.to_string())?;
        if let Some(old) = slot.replace(connection) { old.close(); }
        Ok(())
    }

    pub fn start_room(&self, id: &str, game: &str, room: &str) -> Result<Option<u32>, String> {
        let slot = self.active.lock().map_err(|error| error.to_string())?;
        match slot.as_ref().filter(|connection| connection.id == id && connection.game == game) {
            Some(connection) => connection.start_room(room),
            None => Ok(None),
        }
    }
}
