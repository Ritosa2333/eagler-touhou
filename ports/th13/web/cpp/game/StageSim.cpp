// stage1 模拟核心：资源装载、逐帧积分与线程驱动。锚点与合同见 docs/plans/event-contract.md。
#include "StageSim.hpp"
#include "SessionState.hpp"

#include "BulletAppearance.hpp"
#include "AnmVm.hpp"
#include "StageMath.hpp"

#include <cmath>
#include <cstdio>
#include <cstring>

namespace th13 {

float StageSim::time_rate() const { return session ? session->rate : standalone_rate_; }
void StageSim::set_time_rate(float rate) {
    standalone_rate_ = rate;
    if (session) session->rate = rate;
    if (time_rate_changed) time_rate_changed(rate);
}

namespace {

// 仓库文件读取（raw dump 目录；oracle 侧同源）。
bool read_file(const std::string& path, std::vector<u8>& out) {
    std::FILE* file = std::fopen(path.c_str(), "rb");
    if (!file) return false;
    std::fseek(file, 0, SEEK_END);
    const long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    out.resize(size_t(size < 0 ? 0 : size));
    const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
    std::fclose(file);
    return got == out.size();
}

// ECLI 递归 + ANIM 名单核验（0x4795b0 注册序的行为子集）。
class StageResourceProvider final : public EclResourceProvider {
public:
    std::string resource_dir;
    StageSim* sim = nullptr;
    std::vector<std::string>* anim_list = nullptr;
    bool read(const std::string& name, std::vector<u8>& data) override {
        return read_file(resource_dir + "/" + name, data);
    }
    bool animation(u32, const std::string& name) override {
        // 依赖清单装载核验：ANM 文件必须在 raw 目录中存在（内容解析不在 stage1 范围）。
        std::vector<u8> probe;
        if (!read_file(resource_dir + "/" + name, probe)) {
            if (sim) sim->error_ = "missing ANM dependency: " + name;
            return false;
        }
        if (anim_list) anim_list->push_back(name);
        return true;
    }
};

}  // namespace

void StageEt::reset() {
    // 0x41b260 case 0x61（0x41d573）：槽全清后置默认 {angle1=0, count=1,
    // ways=1, speed1=2.0}；dword[0x235] 位型 0x00010001 = count/ways 各 1。
    sprite = subtype = 0;
    spawn[0] = spawn[1] = spawn[2] = 0;
    sound = 21;
    transform_sound = 38;
    laser_width = 0;
    for (auto& value : laser_timing) value = 0;
    angle1 = angle2 = 0;
    speed1 = 2.0f;
    speed2 = 0;
    spin = 0;
    count = 1;
    ways = 1;
    aim = 0;
    offset[0] = offset[1] = offset[2] = 0;
    absolute[0] = absolute[1] = absolute[2] = 0;
    // etEx 队列（0x41df90 写入、600 etNew 全清）：0x41d573 槽清含 ex 区。
    for (auto& item : ex) {
        item.op = 0;
        item.flags = 0;
        for (float& value : item.args) value = 0;
        item.integer_arguments = {};
        item.subroutine = nullptr;
    }
    ex_next = 0;
}

bool StageSim::load(const char* resource_dir, const char* event_path, const char* stage) {
    gvars.sim = this;
    commands.sim = this;
    const std::string stage_file = stage ? stage : "st01.ecl";
    std::vector<u8> st01;
    if (!read_file(std::string(resource_dir) + "/" + stage_file, st01)) {
        error_ = "cannot read " + stage_file;
        return false;
    }
    StageResourceProvider provider;
    provider.resource_dir = resource_dir;
    provider.sim = this;
    provider.anim_list = &anim_list;
    if (program.attach(st01.data(), u32(st01.size()), &provider) != 0 || !program.error.empty()) {
        error_ = program.error.empty() ? "st01.ecl attach failed" : program.error;
        return false;
    }
    if (anim_list.size() < 2) {
        error_ = "st01.ecl ANIM list incomplete";
        return false;
    }
    if (!sink.open(event_path)) {
        error_ = "cannot open event output";
        return false;
    }
    return true;
}

bool StageSim::begin(EclResourceProvider& provider, const char* stage_file, i32 difficulty,
                     const char* event_path) {
    gvars.sim = this;
    commands.sim = this;
    interactive = true;
    difficulty_rank = difficulty;
    diff = 0;  // begin 的入参是掩码（1 << DIFF），取尾零还原 DIFF
    while (difficulty > 1) {
        difficulty >>= 1;
        ++diff;
    }
    std::vector<u8> stage;
    if (!provider.read(stage_file, stage)) {
        error_ = std::string("cannot read ") + stage_file;
        return false;
    }
    if (program.attach(stage.data(), u32(stage.size()), &provider) != 0 || !program.error.empty()) {
        error_ = program.error.empty() ? "stage attach failed" : program.error;
        return false;
    }
    if (anim_list.size() < 2) {
        error_ = "stage ANIM list incomplete";
        return false;
    }
    // 回放对拍模式：事件落盘开启（§7）；浏览器侧保持静默。
    if (event_path && !sink.open(event_path)) {
        error_ = "cannot open event output";
        return false;
    }
    replay_events = event_path != nullptr;
    return true;
}

// 0x4234b0/0x422f80 共用的推进：elapsed+1；到终点写终值并失活（duration 清 0），
// 否则按缓动模式线性插值。特殊模式 7/17/8 在 setup 处拒绝（st01 窗口外）。
namespace {
void step_polar_itp(StagePolarItp& itp, float& out_a, float& out_b, float rate) {
    itp.elapsed = float(itp.elapsed + (rate > .99f && rate < 1.01f ? 1.f : rate));
    if (itp.elapsed >= i32(itp.duration)) {
        out_a = itp.to_a;
        out_b = itp.to_b;
        itp.duration = 0;
        return;
    }
    const float t = ease_interp(itp.mode, float(itp.elapsed), itp.duration);
    out_a = itp.from_a + t * (itp.to_a - itp.from_a);
    out_b = itp.from_b + t * (itp.to_b - itp.from_b);
}

void step_pos_itp(StagePosItp& itp, float* out, float rate) {
    itp.elapsed = float(itp.elapsed + (rate > .99f && rate < 1.01f ? 1.f : rate));
    if (itp.elapsed >= i32(itp.duration)) {
        out[0] = itp.target[0];
        out[1] = itp.target[1];
        out[2] = itp.target[2];
        itp.duration = 0;
        return;
    }
    if (!itp.per_axis && (itp.mode == 7 || itp.mode == 17)) {
        for (int i=0;i<3;++i) {
            out[i] = itp.start[i] += itp.mode == 7 ? itp.target[i] : itp.p2[i];
            if (itp.mode == 17) itp.p2[i] += itp.target[i];
        }
        return;
    }
    if (itp.per_axis) {
        // 422f80 separate-axis branch: final multiply/add stays extended,
        // unlike the shared weight vector path below.
        for (int i = 0; i < 3; ++i) {
            const i32 mode = itp.axis_mode[i];
            if (mode == 7) out[i] = itp.start[i] += itp.target[i];
            else if (mode == 17) {
                out[i] = itp.start[i] += itp.p2[i];
                itp.p2[i] += itp.target[i];
            } else if (mode == 8) {
                const double t = float(itp.elapsed) / itp.duration;
                const double u = t - 1;
                out[i] = float(u*t*t*itp.p2[i] + t*(1-t)*(1-t)*itp.p1[i] +
                               (3-2*t)*t*t*itp.target[i] + (2*t+1)*u*u*itp.start[i]);
            } else {
                const float weight = ease_interp(mode, float(itp.elapsed), itp.duration);
                out[i] = float((double(itp.target[i]) - itp.start[i]) * weight + itp.start[i]);
            }
        }
        return;
    }
    if (itp.mode == 8) {
        // 贝塞尔/厄米特（0x4230d4）：权重 f32 落点后逐对累加（次序与原版一致）。
        const float t = float(itp.elapsed) / float(i32(itp.duration));
        const double x = t;
        const float w_start = static_cast<float>((2 * x + 1) * (x - 1) * (x - 1));
        const float w_target = static_cast<float>(x * x * (3 - 2 * x));
        const float w_p1 = static_cast<float>((1 - x) * (1 - x) * x);
        const float w_p2 = static_cast<float>((x - 1) * x * x);
        for (int i = 0; i < 3; ++i) {
            const float a = itp.start[i] * w_start;
            const float b = itp.target[i] * w_target;
            const float c = itp.p1[i] * w_p1;
            const float d = itp.p2[i] * w_p2;
            out[i] = ((a + b) + c) + d;
        }
        return;
    }
    const float t = ease_interp(itp.mode, float(itp.elapsed), itp.duration);
    out[0] = itp.start[0] + t * (itp.target[0] - itp.start[0]);
    out[1] = itp.start[1] + t * (itp.target[1] - itp.start[1]);
    out[2] = itp.start[2] + t * (itp.target[2] - itp.start[2]);
}

// 0x4736a0：mode0 产极坐标位移写入 aux[0..1]；mode2/3 圆周推进角与半径。
void step_mover_delta(StageMover& m, float rate) {
    switch (m.mode & 0xF) {
    case 0:
        polar_xy(m.angle, float(m.speed * rate), m.aux[0], m.aux[1]);
        break;
    case 2:
    case 3:
        m.radius = float(double(m.growth) * rate + double(m.radius));
        m.angle = wrap_angle(float(double(m.speed) * rate + double(m.angle)));
        break;
    case 4:
        m.sine_phase = wrap_angle(float(float(m.growth * rate) + m.sine_phase));
        polar_xy(m.ellipse_angle, float(m.speed * rate), m.aux[0], m.aux[1]);
        m.aux[2] = 0;
        break;
    default:
        break;
    }
}

}  // namespace

// 0x473780: the ECL immediate update and the frame update use the same geometry.
void StageMover::advance(float rate) {
    step_mover_delta(*this, rate);
    accumulate(rate);
}

void StageMover::accumulate(float rate) {
    StageMover& m = *this;
    switch (m.mode & 0xF) {
    case 0:
        m.pos[0] = m.pos[0] + m.aux[0];
        m.pos[1] = m.pos[1] + m.aux[1];
        m.pos[2] = m.pos[2] + m.aux[2];
        break;
    case 2: {
        float ox = 0, oy = 0;
        polar_xy(m.angle, m.radius, ox, oy);
        m.pos[0] = m.aux[0] + ox;
        m.pos[1] = m.aux[1] + oy;
        m.pos[2] = m.aux[2] + 0.0f;
        break;
    }
    case 3: {
        // 438220 -> 472f30 twice -> 474f00 -> 472fc0. The ellipse's
        // world angle is converted to local axes before scaling its X axis.
        double difference = double(m.angle) - double(m.ellipse_angle);
        if (difference > double(3.1415927f))
            difference = double(m.angle) - (double(m.ellipse_angle) + double(6.2831855f));
        else if (-difference > double(3.1415927f))
            difference = double(m.angle) - (double(m.ellipse_angle) - double(6.2831855f));
        const float local_angle = wrap_angle(wrap_angle(float(difference)));
        float x = 0, y = 0;
        polar_xy(local_angle, m.radius, x, y);
        x = float(x * m.ellipse_ratio);
        const float sine = float(std::sin(double(m.ellipse_angle)));
        const float cosine = float(std::cos(double(m.ellipse_angle)));
        const float rx = float(double(cosine) * x - double(sine) * y);
        const float ry = float(double(cosine) * y + double(x) * sine);
        m.pos[0] = m.aux[0] + rx;
        m.pos[1] = m.aux[1] + ry;
        m.pos[2] = m.aux[2] + 0.0f;
        break;
    }

    case 4: {
        // 4738a8: advance the center, then offset across the movement axis.
        // The CRT sine and both atan2 operands are stored as f32 first.
        const float old_x = m.pos[0], old_y = m.pos[1];
        for (int i = 0; i < 3; ++i) m.origin[i] += m.aux[i];
        const float sine = float(std::sin(double(m.sine_phase)));
        const float radius = float(double(sine) * m.radius * rate);
        const float axis = wrap_angle(wrap_angle(float(double(m.ellipse_angle) +
                                                       1.5707963705062866)));
        float x = 0, y = 0;
        polar_xy(axis, radius, x, y);
        m.pos[0] = m.origin[0] + x;
        m.pos[1] = m.origin[1] + y;
        m.pos[2] = m.origin[2] + 0.0f;
        const float dx = m.pos[0] - old_x, dy = m.pos[1] - old_y;
        m.angle = wrap_angle(float(std::atan2(double(dy), double(dx))));
        break;
    }
    default:
        break;
    }
    m.pos[0] = round001(m.pos[0]);
    m.pos[1] = round001(m.pos[1]);
}

void StageSim::integrate(StageEnemy& e) {
    const float rate = time_rate();
    // 0x419450 序：vel 插值 → pos 插值 → movePosTime/极坐标 delta → 累加 → 0x419950。
    // 0x419450:34-44：极坐标插值步进（elapsed 为 float，0x401400 每帧 +1.0）。
    // 圆周模式（mode 2/3）不写回 angle——插值角维（409 setup 的 from=to=当前
    // 角）不得覆写积分累计的 angle，仅 speed 维写回（vm+0xa0）。
    if (e.vel.polar.active()) {
        float a = 0, b = 0;
        step_polar_itp(e.vel.polar, a, b, rate);
        const u32 mode = e.vel.mode & 0xF;
        if (mode != 2 && mode != 3) e.vel.angle = wrap_angle(a);
        e.vel.speed = b;
    }
    if(e.vel.radial.duration) {
        const auto value=e.vel.radial.step(rate);e.vel.radius=value[0];e.vel.growth=value[1];
    }
    if (e.pos.polar.active()) {
        float a = 0, b = 0;
        step_polar_itp(e.pos.polar, a, b, rate);
        const u32 mode = e.pos.mode & 0xF;
        if (mode != 2 && mode != 3) e.pos.angle = wrap_angle(a);
        e.pos.speed = b;
    }
    if(e.pos.radial.duration) {
        const auto value=e.pos.radial.step(rate);e.pos.radius=value[0];e.pos.growth=value[1];
    }
    if (e.vel.postime.active()) {
        float out[3];
        step_pos_itp(e.vel.postime, out, rate);
        // 0x419450:71-79：插值增量写 vel mover 的 aux（vm+0xbc），
        // 由 0x473780 case0 累加（pos += aux），不直接写子位置。
        e.vel.aux[0] = out[0] - e.vel.pos[0];
        e.vel.aux[1] = out[1] - e.vel.pos[1];
        e.vel.aux[2] = out[2] - e.vel.pos[2];
    } else {
        step_mover_delta(e.vel, rate);
    }
    if (e.pos.postime.active()) {
        float out[3];
        step_pos_itp(e.pos.postime, out, rate);
        e.pos.aux[0] = out[0] - e.pos.pos[0];
        e.pos.aux[1] = out[1] - e.pos.pos[1];
        e.pos.aux[2] = out[2] - e.pos.pos[2];
    } else {
        step_mover_delta(e.pos, rate);
    }

    e.vel.accumulate();

    e.pos.accumulate();

    // 0x419950：主位置 += (vel.pos + pos.pos) − 主位置，再量化；活动界限时钳制并回写 vel 子位置。
    for (int i = 0; i < 3; ++i) {
        const float delta = (e.vel.pos[i] + e.pos.pos[i]) - e.position[i];
        e.motion_delta[i] = delta;
        e.position[i] = e.position[i] + delta;
    }
    e.position[0] = round001(e.position[0]);
    e.position[1] = round001(e.position[1]);
    if (e.flags & 0x40000u) {
        const float hw = e.limit_half[0] * 0.5f;
        const float hh = e.limit_half[1] * 0.5f;
        if (e.position[0] < e.limit_center[0] - hw)
            e.position[0] = e.limit_center[0] - hw;
        else if (e.position[0] > e.limit_center[0] + hw)
            e.position[0] = e.limit_center[0] + hw;
        if (e.position[1] < e.limit_center[1] - hh)
            e.position[1] = e.limit_center[1] - hh;
        else if (e.position[1] > e.limit_center[1] + hh)
            e.position[1] = e.limit_center[1] + hh;
        e.vel.pos[0] = e.position[0] - e.pos.pos[0];
        e.vel.pos[1] = e.position[1] - e.pos.pos[1];
        e.vel.pos[2] = e.position[2] - e.pos.pos[2];
    }
}

bool StageSim::in_bounds(StageEnemy& e) {
    // 419450 reads the current slot0 VM before deciding whether the enemy
    // has left the playfield. Keep the native swapped size fields: sprite
    // height*scaleY bounds X, sprite width*scaleX bounds Y. A missing handle
    // leaves the previous values intact (constructor defaults are zero).
    if(enemy_animation) {
        if(const auto* vm=enemy_animation(e,0)) {
            e.anm_size_a[0]=std::abs(vm->visual.sprite_size.y*vm->visual.scale.y);
            e.anm_size_a[1]=std::abs(vm->visual.sprite_size.x*vm->visual.scale.x);
        }
    }
    const float hw=e.anm_size_a[0]*.5f,hh=e.anm_size_a[1]*.5f;
    if (e.position[0] + hw < -192.0f || 192.0f < e.position[0] - hw) {
        if ((e.flags & 0x20000u) && !(e.flags & 0x4u)) return false;
    } else if (e.position[1] + hh < 0.0f || 448.0f < e.position[1] - hh) {
        if ((e.flags & 0x20000u) && !(e.flags & 0x8u)) return false;
    } else {
        e.flags |= 0x20000u;
    }
    return true;
}

StageEnemy* StageSim::find_by_script_id(i32 id) {
    // 0x417f40：id 0 → null；线性查找，未命中返回最后检视者（原版行为），空表 null。
    if (id == 0) return nullptr;
    StageEnemy* last = nullptr;
    for (auto& entry : enemies)
        if (!entry->dead) {
            last = entry.get();
            if (entry->script_id == id) return last;
        }
    return last;
}

StageEnemy* StageSim::boss() const {
    for (i32 slot : boss_slot)
        if (slot >= 0)
            for (auto& entry : enemies)
                if (!entry->dead && entry->script_id == slot) return entry.get();
    return nullptr;
}

StageEnemy* StageSim::spawn_enemy(const char* routine, float x, float y, float life, StageEnemy* parent,
                                  bool inherit_anm, bool mirror, const StageSpawnArguments* arguments) {
    (void)inherit_anm;  // ANM 遗产拷贝（0x41fcc0:73-79 无条件 12 dword）不参与事件，暂略
    auto enemy = std::make_unique<StageEnemy>();
    enemy->serial = next_serial_++;  // mgr+0xBC 序号：root=0，此后逐次 +1
    if (next_serial_ == 0) ++next_serial_;  // 418f40 skips zero after wraparound.
    enemy->script_id = i32(enemy->serial);
    // Written before constructor ECL; nested creations can replace it.
    last_created_enemy_id = enemy->script_id;
    enemy->trace_id = parent ? parent->script_id : 0;
    enemy->routine = routine;
    enemy->life = life;
    enemy->spell_base_life=i32(life); // 4183f0 also writes outer+513c
    // 出生无敌（0x51bc=2，0x4183f0:44）：构造趟（:65→41a8a0）本身消耗 1。
    // 经验依据：th13_01.rpy 11675 帧 RNG 逐帧对齐（交付构建 HUD 与原版
    // 精确一致）要求构造趟即扣 1——把写 2 挪到构造趟后会使击杀延后一拍、
    // 全程轨迹发散（capture 探针 5641 GameOver 实证）。
    enemy->invulnerable = 2;
    enemy->vel.pos[0] = x;
    enemy->vel.pos[1] = y;
    if (arguments) {
        enemy->vel.pos[2] = arguments->z;
        enemy->spawn_score = arguments->score;
        enemy->drop_main = arguments->drop_main;
        std::copy(arguments->integers.begin(), arguments->integers.end(), enemy->i_slots);
        std::copy(arguments->floats.begin(), arguments->floats.end(), enemy->f_slots);
    }
    // 0x4183f0:28：param2[6]<<20 无条件写入 flags bit20（在构造趟 :65 之前，
    // 构造趟内 404/405 族按镜像翻转角度）。
    enemy->flags = (enemy->flags & ~0x100000u) | (u32(mirror) << 20);
    // 4183f0: param2[5] > 999 sets this marker before constructor ECL.
    if (life > 999) enemy->flags |= 0x80000000u;
    enemy->threads = std::make_unique<EclThreads>(program, &gvars, &random);
    enemy->threads->main.commands = &commands;
    const u8 difficulty = parent ? parent->threads->main.difficulty
                                 : u8(interactive ? difficulty_rank : 0x08);  // 契约：Lunatic
    enemy->threads->start(routine, difficulty);
    StageEnemy* raw = enemy.get();
    if (std::strcmp(routine, "Boss") == 0) boss_main_serial = raw->serial;
    // 合同 §3.1：root（parent 为空）不由事件产生；enmCreate 族才发 enemy_spawn。
    // 事件在构造趟之前（0x4183f0 入口即探参观察点）。
    if (parent || arguments)
        sink.enemy_spawn(raw->serial, routine, x, y, enemy->vel.pos[2], life,
                         parent ? parent->serial : 0);
    tick_enemy(*raw);  // 0x4183f0:65 构造内直调 0x41a8a0（首趟，计入当前帧）
    raw->spirit_threshold = 1000;  // 0x4183f0:66，必须晚于构造首趟。
    // 4183f0 chooses these defaults after constructor ECL, including323.
    raw->death_sound = (raw->script_id & 1) + 3;
    if (raw->death_effect == 0) {
        raw->death_effect = 34;raw->death_effect_file = 1;
        if (raw->anm_file == 2) switch(raw->anm_script[0]) {
        case 5:case 25:case 53:case 78:raw->death_effect=29;break;
        case 10:case 56:case 80:raw->death_effect=39;break;
        case 15:case 84:raw->death_effect=44;break;
        case 30:raw->death_effect=51;break;
        case 35:raw->death_effect=50;break;
        case 40:raw->death_effect=49;break;
        default:break;
        }
    }
    raw->tick_gate = true;  // 0x41a8a0:31 干活后置 0x80000；待 MGR_UPDATE 到达时清
    // 0x4183f0:106-124 挂链在构造趟之后——构造趟内 enmCreate 的后代先入链，
    // 宿主随后尾插（vector 序 = 链序）。这决定「创建者是否链尾」，进而决定
    // 新敌机创建帧是否被遍历到达（oracle 探针实证：root 构造趟先建 Maple，
    // 链序 [Maple, root]；root 为尾时其新敌机次帧空转、次次帧才干活）。
    enemies.push_back(std::move(enemy));
    ++spawned_;
    return raw;
}

void StageSim::tick_enemy(StageEnemy& e) {
    if (e.dead || !error_.empty()) return;
    StageEnemy* saved = current;
    current = &e;
    std::copy_n(e.position,3,e.previous_position);
    integrate(e);
    if (in_bounds(e)) {
        // 原版敌机恒有 ECL 线程块（0x4183f0:28 分配）；threads 为空的裸
        // 敌机只存在于几何/碰撞合成夹具，无 ECL 可步进——跳过解释器，
        // 其余 0x41a8a0 子步（碰撞回调/反馈/计时）照常执行。
        const int result = e.threads ? e.threads->step(time_rate()) : 0;
        if (result == -2) {
            const std::string detail = e.threads->error.empty()
                                           ? (error_.empty() ? "unknown ECL error" : error_)
                                           : e.threads->error;
            error_ = "enemy " + std::to_string(e.serial) + " (" + e.routine + ") frame " +
                     std::to_string(frame) + ": " + detail;
        }
        else if (result)
            e.dead = true;  // 主线程结束（0x418e00 销毁支路）
        // 41a8a0 invokes the registered native callback after ECL, before damage.
        if(!e.dead&&error_.empty()&&e.native_function) {
            const i32 function=e.native_function;
            invoke_enemy_function(e,function);
        }
        // 0041a8a0 calls 00419a90 before advancing this enemy's timers.
        // Damage/constructor spawns must finish here before the next enemy's ECL.
        e.flags &= ~0x400000u; // 419a90 clears the per-update damage marker
        if (!e.dead && error_.empty() && enemy_hit) enemy_hit(e);
        // A timeout arms the new routine for the next tick, unlike immediate
        // post-hit continuation inside damage(). Bare geometry fixtures have
        // no interrupt table to examine; 4218f0 would not fire for them.
        if (!e.dead && error_.empty() && e.threads) interrupt_enemy(e);
        if (!e.dead && error_.empty() && enemy_body_collision) enemy_body_collision(e);
        if (!e.dead && error_.empty()) update_enemy_feedback(e);
        // 41a8a0 calls41a3d0 after damage and before slot following/timers.
        if(!e.dead&&error_.empty()&&e.fog) {
            e.fog->update({e.position[0],e.position[1],e.position[2]},session?session->rate:1.f);
            if(enemy_fog&&!enemy_fog(e,false))error_="Enemy fog update failed";
        }
        if (!e.dead && error_.empty()) {
            const float rate = time_rate(); // ECL547 may have changed it this tick.
            if (e.invulnerable > 0) {
                if (e.invulnerable_clock.current != e.invulnerable)
                    e.invulnerable_clock.set(e.invulnerable);
                e.invulnerable_clock.decrement(&rate);
                e.invulnerable = e.invulnerable_clock.current;
            }
            if (e.body_invulnerable > 0) {
                if (e.body_invulnerable_clock.current != e.body_invulnerable)
                    e.body_invulnerable_clock.set(e.body_invulnerable);
                e.body_invulnerable_clock.decrement(&rate);
                e.body_invulnerable = e.body_invulnerable_clock.current;
            }
            // 0041a8a0: ECL and 00419a90 observe the current TIME. Its timer
            // advances only at 0041aab9, after the collision/death gates.
            e.age = float(e.age + (rate > .99f && rate < 1.01f ? 1.f : rate));
        }
    } else {
        e.dead = true;  // 出界自杀（0x419450 返回 -1）
    }
    current = saved;
}

void StageSim::update_enemy_feedback(StageEnemy& enemy) {
    if (!enemy_animation) return;
    AnmVm* vm = enemy_animation(enemy, 0);
    if (!vm || !error_.empty()) return;
    auto& visual = vm->visual;
    if (enemy.hit_feedback_cooldown != 0) {
        visual.flags &= ~0x60000u;
        --enemy.hit_feedback_cooldown;
        return;
    }
    const auto flash = [&](u32 color) {
        visual.secondary_color = color;
        visual.flags = (visual.flags & ~0x40000u) | 0x20000u;
    };
    const bool fourth_frame = i32(enemy.age) % 4 == 0;
    if (enemy.periodic_flash) {
        if (fourth_frame) flash(0xffff00ffu);
        else visual.flags &= ~0x60000u;
    }
    const auto& card = spell.state();
    const bool boss_feedback = (enemy.flags & 0x81000000u) != 0 &&
                              !(card.active() && (card.flags & kSpellSurvival));
    if (enemy.flags & 0x400000u) {
        flash(0xff0000ffu);
        enemy.hit_feedback_cooldown = 4;
        const i32 voice = enemy.hit_sound >= 0 ? enemy.hit_sound :
            boss_feedback && enemy.phase_life_remaining < (card.active() ? 200 : 900) ? 35 : 34;
        if (enemy_hit_sound && !enemy_hit_sound(voice)) error_ = "Enemy hit sound failed";
    } else if (!fourth_frame) {
        visual.flags &= ~0x60000u;
    } else if (boss_feedback && enemy.phase_life_remaining < (card.active() ? 100 : 500)) {
        flash(0xff0000ffu);
    }
}

bool StageSim::interrupt_enemy(StageEnemy& e) {
    // 0x4218f0 first examines ONLY the first live health slot, then the first
    // live slot with a positive timeout. MBoss uses slot1 and overrides its
    // timeout with MBossEscape, while health defeat must still reach MBossDead.
    e.phase_life_remaining = i32(e.life);
    e.phase_life_threshold = 0;
    EclContext::Interrupt* fired = nullptr;
    bool timed_out = false;
    for (auto& slot : e.interrupts) {
        if (slot.life < 0) continue;
        e.phase_life_remaining = i32(e.life) - slot.life;
        e.phase_life_threshold = slot.life;
        if (e.life <= float(slot.life)) fired = &slot;
        break;
    }
    if (!fired) {
        for (auto& slot : e.interrupts) {
            if (slot.life < 0 || slot.timeout <= 0) continue;
            if(e.flags&0x1000000u) {
                const i32 remaining=slot.timeout-i32(e.age);
                const i32 seconds=remaining/60;
                boss_timer_seconds=std::min(99,seconds);
                boss_timer_centiseconds=seconds>99?99:(remaining%60)*100/60;
            }
            if (e.age >= float(slot.timeout)) {
                fired = &slot;
                timed_out = true;
            }
            break;
        }
    }
    if (!fired) return false;
    const std::string label = timed_out ? fired->timeout_label : fired->health_label;
    if (timed_out) spell.phase_timeout(bomb_active);
    e.life = float(fired->life);  // discard overkill at phase boundaries
    fired->life = -1;  // consume before running a routine which may re-arm it
    e.age = 0;
    e.phase_timed_out = timed_out;
    e.phase_life_remaining = 0;
    e.phase_life_threshold = i32(e.life);
    const u8 difficulty = e.threads->main.difficulty;
    // Clears async threads and ECL stack/interpolators, not the enemy-owned
    // interrupt table or ANM handles (0x4176c0 + 0x417740 + 0x4797d0).
    if (!e.threads->restart(label.c_str(), difficulty))
        error_ = "enemy interrupt " + label + ": " + e.threads->error;
    return true;
}

bool StageSim::emit_spell(const SpellEvent& event, const StageEnemy& enemy) {
    if (!event.emitted()) return spell.error.empty();
    if (event.kind == SpellEventKind::End && spell_award)
        spell_award(event.score_award);
    if (spell_event && !spell_event(event, enemy)) {
        error_ = "Spell event consumer failed";
        return false;
    }
    return true;
}

bool StageSim::run(u32 frames) {
    if (!error_.empty()) return false;
    if (!start()) return false;
    for (frame = 1; frame < frames && error_.empty(); ++frame) {
        sink.set_frame(frame);
        if (!step_frame()) break;
    }
    sink.close();  // 落盘（读回/比对前保证缓冲清净）
    return error_.empty() && sink.ok();
}

bool StageSim::start() {
    if (!error_.empty()) return false;
    // 第 0 帧 = stage 装载趟：root（serial 0，承载 main）创建并跑构造内首趟。
    frame = 0;
    sink.set_frame(0);
    spawn_enemy("main", 0.0f, 0.0f, 0.0f, nullptr, false);
    return error_.empty();
}

bool StageSim::step_frame() {
    if (!error_.empty()) return false;
    // 敌链清空（root 结束且无存活者）：无头窗口内不会到达（root 常驻），
    // 交互模式可能到达（合成脚本/全灭）——空转保步进可用。
    if (enemies.empty()) {
        if (!defer_laser_update) advance_lasers();
        if (interactive) advance_bullets();
        return true;
    }
    // 0x418e00:16-30：游标在每个敌机处理前取其实时 next；0x41a8a0 返回
    // -1（死亡）当场摘链——已死敌机不占链位，链尾创建者的新敌机是否被
    // 本帧遍历到达取决于创建时游标位置（vector 插入序 = 链序）。
    u32 i = 0;
    while (true) {
        StageEnemy* entry = enemies[i].get();
        const u32 next = i + 1;
        const bool has_next = next < enemies.size();
        bool remove = entry->dead;
        if (!entry->dead) {
            // 0x41a8a0:30-31：0x80000 位已置则本帧空转；0x418e00:20 返回 0 后清位。
            if (!entry->tick_gate) {
                tick_enemy(*entry);
                entry->tick_gate = true;
            }
            if (entry->dead) remove = true;
            else entry->tick_gate = false;
        }
        if (remove) {
            // 敌机销毁（0x418e00 销毁支路）先经宿主回收其槽位 VM 句柄
            // （enemies_from_ecl 路径的敌机本体 VM 所有权在 registry——
            // 敌机结构消亡后句柄即悬空，此处回收防泄漏）。damage() 致死
            // 的敌机已在其死亡帧同帧回收（0x419a90 当趟销毁语义），此处
            // 只兜出界自杀/525 清场等 dead 置位路径；retire 幂等（句柄
            // 已归零则空转）。
            if (retire_sink) retire_sink(entry->serial);
            enemies.erase(enemies.begin() + i);  // 0x418e00 else：当场摘链（无 RNG）
            if (i >= enemies.size()) break;
            continue;  // 游标走处理前记下的 next（摘除后恰为当前位置）
        }
        if (!error_.empty()) break;
        if (!has_next) break;
        i = next;
    }
    if (interactive) advance_bullets();
    if (!defer_laser_update) advance_lasers();
    return error_.empty();
}

// ---- etEx 行为队列（0x40f030）：消费到首个阻塞项 ----
// 阻塞三条件（0x40f087-0x40f0ad，汇编核对）：
//   op == 0：队列终止（游标不前进，之后每次调用都在该项停住）；
//   entry.flags == 0 且 bb4 除 0x100 外含任意位：串行项等活跃行为清位；
//   (op & bb4) != 0：该行为位已活跃（同类项不叠发）。
// 每项消费后游标 +1（op 0x8000/0x10000/0x2000/0x80000000 的特例在分支内）。
// 未实现的 op 显式报错，不静默跳过。
void StageSim::process_bullet_queue(StageBullet& bullet, bool spawn_time) {
    (void)spawn_time;  // 0x40f030 的两种调用点共用同一消费语义
    while (bullet.ex_cursor < 18) {
        const StageEt::Ex& entry = bullet.ex[bullet.ex_cursor];
        if (entry.op == 0) return;
        if (entry.flags == 0 && (bullet.behavior_flags & 0xfffffeffu) != 0) return;
        if ((entry.op & bullet.behavior_flags) != 0) return;
        const float* args = entry.args;        // +0x00..+0x0C 浮点
        const bool exact_integers = entry.op == 0x1000000 || entry.op == 0x2000 || entry.op == 0x80;
        const i32 i4 = exact_integers ? entry.integer_arguments[0] : i32(entry.args[4]);
        const i32 i5 = exact_integers ? entry.integer_arguments[1] : i32(entry.args[5]);
        switch (entry.op) {
        case 1:  // 减速突进（0x40f0f3：bb4|=1; 0x4016c0(0) 计时清零; f20=0）
            bullet.behavior_flags |= 1;
            bullet.ex1_timer = 0;
            bullet.ex1_time = 0;
            break;
        case 2:  // 透明占位（default 分支 = 仅推进游标）
            break;
        case 4: {  // steer/accel（0x40f11a：step=args[0]、target=args[1]）
            bullet.behavior_flags |= 4u;
            bullet.steer[0] = 0;
            bullet.steer[1] = 0;
            bullet.steer[2] = 0;
            bullet.ex4_timer = 0;
            bullet.ex4_time = 0;
            const float step = args[0];
            bullet.ex4_acceleration = step;
            float target = args[1];
            if (target <= -990.0f) target = bullet.angle;
            else if (target >= 990.0f) {
                // 0x443f60(弹体位置)：出生点→玩家（此处弹体即出生位置）。
                const float dy = player_y - bullet.y, dx = player_x - bullet.x;
                target = (dy == 0.0f && dx == 0.0f)
                             ? 1.5707964f
                             : float(std::atan2(static_cast<double>(dy),
                                                static_cast<double>(dx)));
            }
            bullet.ex4_duration = i4;
            // 0x412510(f60, target, step)：steer = polar(target, step)。
            bullet.steer[0] = std::cos(static_cast<double>(target)) * step;
            bullet.steer[1] = std::sin(static_cast<double>(target)) * step;
            bullet.steer[2] = 0;
            break;
        }
        case 8:  // 螺旋加速（0x40f1da：f94=args[0]、f98=args[1]、fb4=i4）
            bullet.behavior_flags |= 8u;
            bullet.ex8_speed_delta = args[0];
            bullet.ex8_angle_delta = args[1];
            bullet.ex8_duration = i4;
            bullet.ex8_timer = 0;
            bullet.ex8_time = 0;
            break;
        case 0x10: {  // 减停-转向循环（0x40f210：fe0 目标角按 f6 模式解析）
            bullet.behavior_flags |= 0x10;
            const float f0 = args[0];
            switch (i32(entry.args[6])) {  // f6 = +0x18 模式选择
            case 0: {
                float target = f0;
                if (target <= -990.0f || target >= 990.0f) {
                    const float dy = player_y - bullet.y, dx = player_x - bullet.x;
                    target = (dy == 0.0f && dx == 0.0f)
                                 ? 1.5707964f
                                 : float(std::atan2(static_cast<double>(dy),
                                                    static_cast<double>(dx)));
                }
                bullet.ex10_angle = wrap_angle(target);
                break;
            }
            case 1:
            case 4:
                bullet.ex10_angle = f0;
                break;
            case 2: {  // 0x800000 快照位置 → 玩家向（st01 未用 0x800000，快照恒 0）
                const float dy = player_y - bullet.saved_pos[1];
                const float dx = player_x - bullet.saved_pos[0];
                const float base = (dy == 0.0f && dx == 0.0f)
                                       ? 1.5707964f
                                       : float(std::atan2(static_cast<double>(dy),
                                                          static_cast<double>(dx)));
                bullet.ex10_angle =
                    wrap_angle(wrap_angle(base + f0));
                break;
            }
            case 3:
                bullet.ex10_angle = wrap_angle(wrap_angle(bullet.saved_angle + f0));
                break;
            case 5: {
                // 0x472e10 signed_unit：> -990 字面量（>=990 加瞄准角）、
                // <= -990 相对当前角；均 wrap。
                const float unit = random.signed_unit();
                float value;
                if (f0 > -990.0f) {
                    value = f0 < 990.0f ? unit * f0
                                        : unit * f0 +
                                              float(std::atan2(
                                                  static_cast<double>(player_y - bullet.y),
                                                  static_cast<double>(player_x - bullet.x)));
                } else {
                    value = bullet.angle + unit * f0;
                }
                bullet.ex10_angle = wrap_angle(value);
                break;
            }
            case 6:  // 原始 signed_unit×f0，不 wrap（0x40f3f6）
                bullet.ex10_angle = random.signed_unit() * f0;
                break;
            default:
                error_ = "etEx op 0x10 unknown angle mode " + std::to_string(i32(entry.args[6]));
                return;
            }
            // f1 <= -999 → 保持当前速度（0x40f40b 的 fcomi 链）。
            bullet.ex10_speed = args[1] <= -999.0f ? bullet.speed : args[1];
            bullet.ex10_duration = i4;
            bullet.ex10_repeats = i5;
            bullet.ex10_count = 0;
            bullet.ex10_mode = i32(entry.args[6]);
            bullet.ex10_timer = 0;
            bullet.ex10_time = 0;
            break;
        }
        case 0x40: {  // 边界反弹（0x40f41b：1024/102c/1030/1044/1048/1050）
            bullet.behavior_flags |= 0x40;
            bullet.ex40_speed = args[0];
            if ((u32(i5) & 0x20u) == 0) {
                bullet.ex40_w = 384.0f;  // native0x43c00000; height is448
                bullet.ex40_h = 448.0f;
            } else {
                bullet.ex40_w = args[1];
                bullet.ex40_h = args[2];
            }
            bullet.ex40_bounces = 0;
            bullet.ex40_limit = i4;
            bullet.ex40_mode = u32(i5);
            break;
        }
        case 0x80:
            bullet.cancel_protection = i4;
            break;
        case 0x100: {
            // 40f67x -> 410f80: retain offscreen bullets for i4 frames;
            // i5 enables the direction-to-screen check.
            bullet.behavior_flags |= 0x100u;
            bullet.offscreen_timer = i4;
            bullet.offscreen_time = float(i4);
            bullet.offscreen_directional = i5;
            break;
        }
        case 0x200: {  // 变弹型（0x40f6a4：1354=i4、1356=i5&0x7fff、弹表重载）
            // 405010/461cb0 reinitialize the embedded VM, discarding an
            // earlier blend write in this same queue traversal.
            bullet.animation_blend = -1;
            bullet.sprite = u32(u16(i4));
            bullet.subtype = u32(u16(i5) & 0x7fffu);
            if (const BulletAppearance* appearance = bullet_appearance(i32(bullet.sprite))) {
                bullet.hit_size[0] = appearance->scale;
                bullet.hit_size[1] = appearance->scale;
            } else {
                bullet.hit_size[0] = 4.0f;
                bullet.hit_size[1] = 4.0f;
            }
            // (u32)i5 & 0x8000 → 0x4124d0（出场 ANM 重置）：纯表现，无事件。
            break;
        }
        case 0x400:  // 411380: retire with ANM interrupt1, half-speed state3
            if(!cancel_bullet(bullet))return;
            break;
        case 0x2000: {
            // 子弹发射（0x40f79a）：下一项为角度/条数描述符。参数打包见 i4：
            // byte0=子弹 ex 种子游标、byte1=subtype（有符号字节）、byte2=sprite、
            // byte3&0x7f=aim 模式、bit31=发射后立即取消父弹（FUN_00411380）。
            // 游标语义（0040f030:291/305/306-308）：发射前 +1、发射后再 +1；
            // bit31=0 → LAB_0040f489（不再 +1，净 +2）；bit31=1 →
            // FUN_00411380 → LAB_0040f483 尾部再 +1（净 +3）。本函数公共尾块
            // 恒 +1（此处等价 0x40f489 需补、0x40f483 本身那次），故 bit31=1
            // 分支需自补 :305 的一次 +1。
            if (bullet.ex_cursor + 1 >= 18) {
                error_ = "etEx op 0x2000 descriptor outside queue";
                return;
            }
            const StageEt::Ex& descriptor = bullet.ex[bullet.ex_cursor + 1];
            StageEt temp;
            temp.sprite = u32((u32(i4) >> 16) & 0xffu);
            temp.subtype = u32(i32(i8(u32(i4) >> 8))); //40f816 movsx byte
            temp.angle1 = descriptor.args[0];  // 描述符 f0/f1
            temp.angle2 = descriptor.args[1];
            temp.speed1 = args[0];
            temp.speed2 = args[1];
            temp.count = i16(u32(i5) & 0xffffu);
            temp.ways = i16(u32(descriptor.integer_arguments[0]) & 0xffffu);
            temp.aim = i16((u32(i4) >> 24) & 0x7fu);
            for (u32 n = 0; n < 18; ++n) temp.ex[n] = bullet.ex[n];  // 队列整块继承
            const float spawn[3] = {bullet.x, bullet.y, bullet.z};
            bullet.ex_cursor += 1;  // 0040f030:291 FUN_0040ffe0 发射前 +1
            emit_et(temp, bullet.source_enemy, 0, spawn, 0.0f, u32(i4) & 0xffu);
            if (i4 < 0) {           // 0040f030:306：bit31=1 → FUN_00411380
                bullet.ex_cursor += 1;  // 0040f030:305 发射后再 +1；break 后
                if(!cancel_bullet(bullet))return;  // 公共尾块即 LAB_0040f483 尾部，净 +3
            }
            // bit31=0 → LAB_0040f489：:305 的 +1 由公共尾块承担，净 +2
            break;
        }
        case 0x4000:  // 透明占位（0x40f030 大 op 链未命中 → 仅推进）
            break;
        case 0x100000: { // 40fc1b: bullet+40 = embedded ANM+18, blend bits5..8.
            bullet.animation_blend = i4 != 0 ? 1 : 0;
            break;
        }
        case 0x200000: { // 40f030: +11d4 = (target speed - speed) / duration.
            bullet.behavior_flags|=0x200000u;
            bullet.target_speed_time=0;
            bullet.target_speed_fraction=0;
            bullet.target_speed_duration=i4;
            bullet.target_speed_step=float((double(args[0])-double(bullet.speed))/double(float(i4)));
            float angle=bullet.angle;
            if(args[1]>-990.f) {
                angle=args[1];
                if(angle>=990.f) {
                    const float dx=player_x-bullet.x,dy=player_y-bullet.y;
                    angle=dx==0&&dy==0?1.5707964f:float(std::atan2(double(dy),double(dx)));
                }
                angle=wrap_angle(angle);
            }
            polar_xy(angle,bullet.target_speed_step,bullet.target_speed_velocity[0],
                     bullet.target_speed_velocity[1]);
            bullet.target_speed_velocity[2]=0;
            break;
        }
        case 0x1000000: { // 40f030 -> 4183f0: spawn an ECL enemy at this bullet
            if (!entry.subroutine || program.find_index(entry.subroutine) < 0) {
                error_ = "etEx enemy subroutine unavailable";
                return;
            }
            StageSpawnArguments initial;
            initial.z = bullet.z;
            initial.integers = entry.integer_arguments;
            std::copy_n(entry.args, 4, initial.floats.begin());
            // The native zeroed 0x54-byte descriptor has life=10000,
            // trace_id=0, no inherited parent and no mirror flag.
            spawn_enemy(entry.subroutine, bullet.x, bullet.y, 10000, nullptr,
                        false, false, &initial);
            break;
        }
        case 0x8000:  // 0x40f030:267-270：写 0xb98（全语料无读者；0x437d70
            // 构造清零、0x402ff0 为 ANM 域引用）——不影响弹道，仅推进游标。
            bullet.b98_mirror = args[0];
            break;
        case 0x10000: // 0040f030: replace +0xbd0, then LAB_0040f489 (no increment).
            if (i4 < 0) { error_ = "etEx jump before queue"; return; }
            bullet.ex_cursor = i4;
            continue;
        case 0x80000000:  // 帧等待（0x40f030:145-163 内联）
            if (i4 < 1) break;  // 0x40f0f3:146-148：<1 仅推进游标，不置位
            // 0x105c/0x1060 = (float)N 驱动倒计时；0x1058 = (int)N-1 是
            // 写穿镜像（全语料无读者）。到期语义保持交付版（th13_01.rpy
            // 11675 帧 RNG 逐帧对齐实证）；Ghidra 把 0x40db10:162 的浮点
            // 比较渲染成 int<1，按 wait_time<1 提前清位会在变速窗（灵界
            // rate≠1）产生一帧漂移，已被 capture 探针实证为轨迹回归。
            bullet.behavior_flags |= 0x80000000u;
            bullet.wait_time = float(i4);
            bullet.wait_timer = i4 - 1;
            break;
        default: {
            char hex[12];
            std::snprintf(hex, sizeof hex, "%x", entry.op);
            error_ = std::string("unimplemented etEx op 0x") + hex;
            return;
        }
        }
        if (!error_.empty()) return;
        bullet.ex_cursor += 1;
    }
}

// ---- 弹体每帧行为派发（0x40ddb5..0x40dea7 的位测试链）----
void StageSim::step_bullet_behaviors(StageBullet& bullet) {
    const float rate = time_rate();
    const float timer_step = rate > .99f && rate < 1.01f ? 1.f : rate;
    const auto tick = [timer_step](i32& current, float& fractional) {
        if (truncate_int(fractional) != current) fractional = float(current);
        fractional = float(fractional + timer_step);
        current = truncate_int(fractional);
    };
    // op 1（0x410230）：计时 <0x11 期间 vel = polar(角, 速度+5-t*0.3125)。
    if (bullet.behavior_flags & 1u) {
        if (bullet.ex1_timer < 0x11) {
            const float bonus = 5.0f - bullet.ex1_time * 5.0f * 0.0625f;
            bullet.vel[0] = std::cos(static_cast<double>(bullet.angle)) * (bonus + bullet.speed);
            bullet.vel[1] = std::sin(static_cast<double>(bullet.angle)) * (bonus + bullet.speed);
            bullet.vel[2] = 0;
            tick(bullet.ex1_timer,bullet.ex1_time);
        } else {
            bullet.behavior_flags &= ~1u;
        }
    }
    // op 4（0x4102b0）：timer < duration 时 vel += steer；|vel|>1e-4 重算角/速。
    if (bullet.behavior_flags & 4u) {
        if (bullet.ex4_timer < bullet.ex4_duration) {
            tick(bullet.ex4_timer,bullet.ex4_time);
            bullet.speed = float(double(bullet.ex4_acceleration)*rate+bullet.speed);
            bullet.vel[0] += float(bullet.steer[0]*rate);
            bullet.vel[1] += float(bullet.steer[1]*rate);
            bullet.vel[2] += float(bullet.steer[2]*rate);
            const float ax = std::abs(bullet.vel[0]);
            const float ay = std::abs(bullet.vel[1]);
            if (ax > 0.0001f || ay > 0.0001f) {
                // 0x4977aa = atan2(y, x)（x87 双精度）；0x472f30 归一。
                bullet.angle =
                    wrap_angle(wrap_angle(float(std::atan2(static_cast<double>(bullet.vel[1]),
                                                            static_cast<double>(bullet.vel[0])))));
                //4103c9 stores the squared magnitude as f32 before sqrt.
                const float speed_squared = float(double(bullet.vel[0]) * bullet.vel[0] +
                                                  double(bullet.vel[1]) * bullet.vel[1]);
                bullet.speed = float(std::sqrt(double(speed_squared)));
            }
        } else {
            bullet.behavior_flags &= ~4u;
        }
    }
    // 00410400 runs between op4 and op8. Speed remains the accumulated scalar;
    // only direction is recomputed from the resulting velocity (no hypot).
    if(bullet.behavior_flags&0x200000u) {
        if(bullet.target_speed_time<bullet.target_speed_duration) {
            bullet.speed=float(double(bullet.target_speed_step)*rate+bullet.speed);
            for(u32 i=0;i<3;++i)bullet.vel[i]+=float(bullet.target_speed_velocity[i]*rate);
            if(std::abs(bullet.vel[0])>0.0001f||std::abs(bullet.vel[1])>0.0001f)
                bullet.angle=wrap_angle(float(std::atan2(double(bullet.vel[1]),double(bullet.vel[0]))));
            tick(bullet.target_speed_time,bullet.target_speed_fraction);
        } else bullet.behavior_flags&=~0x200000u;
    }
    // op 8（0x410520）：timer < duration 时 angle += delta、speed += delta、
    // vel = polar（0x412510 直写覆盖）。
    if (bullet.behavior_flags & 8u) {
        if (bullet.ex8_timer < bullet.ex8_duration) {
            tick(bullet.ex8_timer,bullet.ex8_time);
            bullet.angle = wrap_angle(wrap_angle(bullet.angle + float(bullet.ex8_angle_delta*rate)));
            bullet.speed = float(double(bullet.ex8_speed_delta)*rate+bullet.speed);
            bullet.vel[0] = std::cos(static_cast<double>(bullet.angle)) * bullet.speed;
            bullet.vel[1] = std::sin(static_cast<double>(bullet.angle)) * bullet.speed;
        } else {
            bullet.behavior_flags &= ~8u;
        }
    }
    // op 0x10（0x4105d0）：减停 → 按模式转向 → 重复 repeats 次。
    if (bullet.behavior_flags & 0x10u) {
        if (bullet.ex10_timer < bullet.ex10_duration) {
            // 减速相：speed × (1 - t/duration)（float 计时）。
            //410725..410738 retains product/division/subtraction in x87
            // until the final speed store (no intermediate f32 product).
            const float decayed = float(double(bullet.speed) -
                double(bullet.ex10_time) * double(bullet.speed) / bullet.ex10_duration);
            bullet.vel[0] = std::cos(static_cast<double>(bullet.angle)) * decayed;
            bullet.vel[1] = std::sin(static_cast<double>(bullet.angle)) * decayed;
            tick(bullet.ex10_timer,bullet.ex10_time);
        } else {
            bullet.ex10_count += 1;
            float target = bullet.ex10_angle;
            switch (bullet.ex10_mode) {
            case 0:
            case 5:
                target = bullet.ex10_angle + bullet.angle;
                break;
            case 1:
            case 6: {
                const float dy = player_y - bullet.y, dx = player_x - bullet.x;
                target = (dy == 0.0f && dx == 0.0f)
                             ? 1.5707964f + bullet.ex10_angle
                             : float(std::atan2(static_cast<double>(dy),
                                                static_cast<double>(dx))) +
                                   bullet.ex10_angle;
                break;
            }
            case 2: {
                const float dy = player_y - bullet.saved_pos[1];
                const float dx = player_x - bullet.saved_pos[0];
                target = (dy == 0.0f && dx == 0.0f)
                             ? 1.5707964f + bullet.ex10_angle
                             : float(std::atan2(static_cast<double>(dy),
                                                static_cast<double>(dx))) +
                                   bullet.ex10_angle;
                break;
            }
            case 3:
                target = bullet.saved_angle + bullet.ex10_angle;
                break;
            case 4:
                target = bullet.ex10_angle;
                break;
            default:
                break;  // 0x410616 default：跳过转向，仅恢复速度
            }
            bullet.angle = wrap_angle(target);
            // 0x4106xx：转向帧把速度域直接写为恢复速度并复位计时；未达重复
            // 上限时落 LAB_0041073b 立即推进一次（计时 = 1）。
            bullet.ex10_timer = 0;
            bullet.ex10_time = 0;
            bullet.speed = bullet.ex10_speed;
            bullet.vel[0] = std::cos(static_cast<double>(bullet.angle)) * bullet.ex10_speed;
            bullet.vel[1] = std::sin(static_cast<double>(bullet.angle)) * bullet.ex10_speed;
            if (bullet.ex10_repeats <= bullet.ex10_count) {
                bullet.behavior_flags &= ~0x10u;
            } else {
                tick(bullet.ex10_timer,bullet.ex10_time);
            }
        }
    }
    // op 0x40（0x410a80）：边界外按 1050 位反弹，计满 limit 清位。
    if (bullet.behavior_flags & 0x40u) {
        const bool global_box = bullet_bounce_size[0] > 0;
        const float box_width = global_box ? bullet_bounce_size[0] : bullet.ex40_w;
        const float box_height = global_box ? bullet_bounce_size[1] : bullet.ex40_h;
        const float width = bullet_bounce_size[0] > 0 ? bullet_bounce_size[0] : bullet.ex40_w;
        const float height = bullet_bounce_size[1] > 0 ? bullet_bounce_size[1] : bullet.ex40_h;
        // 0x412400：x ∈ [-w/2, w/2]、y ∈ [224-h/2, 224+h/2] 之外 = 出界。
        const bool outside = bullet.x <= -box_width * 0.5f ||
                             bullet.x >= box_width * 0.5f ||
                             bullet.y <= 224.0f - box_height * 0.5f ||
                             bullet.y >= 224.0f + box_height * 0.5f;
        if (outside) {
            bool bounced = false;
            const bool reflect = (bullet.ex40_mode & 0x10u) == 0;  // bit10 = 只计数
            if ((bullet.ex40_mode & 1u) != 0 &&
                bullet.y < 224.0f - height * 0.5f) {  // 顶（0x410960）
                bounced = true;
                if (reflect) {
                    bullet.angle = wrap_angle(-bullet.angle);
                    bullet.y = (448.0f - height) - bullet.y;
                }
            }
            if ((bullet.ex40_mode & 2u) != 0 &&
                bullet.y >= 224.0f + height * 0.5f) {  // 底（0x4109f0）
                bounced = true;
                if (reflect) {
                    bullet.angle = wrap_angle(-bullet.angle);
                    bullet.y = (height + 448.0f) - bullet.y;
                }
            }
            if ((bullet.ex40_mode & 8u) != 0 &&
                bullet.x >= width * 0.5f) {  // 右（0x4108b0）
                bounced = true;
                if (reflect) {
                    bullet.angle = wrap_angle(wrap_angle(-bullet.angle - 3.1415927f));
                    bullet.x = width - bullet.x;
                }
            }
            if ((bullet.ex40_mode & 4u) != 0 &&
                bullet.x < -width * 0.5f) {  // 左（0x4107f0）
                bounced = true;
                if (reflect) {
                    bullet.angle = wrap_angle(wrap_angle(-bullet.angle - 3.1415927f));
                    bullet.x = -width - bullet.x;
                }
            }
            if (-990.0f < bullet.ex40_speed) bullet.speed = bullet.ex40_speed;
            bullet.vel[0] = std::cos(static_cast<double>(bullet.angle)) * bullet.speed;
            bullet.vel[1] = std::sin(static_cast<double>(bullet.angle)) * bullet.speed;
            if (bounced) {
                bullet.ex40_bounces += 1;
                if (bullet.ex40_limit <= bullet.ex40_bounces)
                    bullet.behavior_flags &= ~0x40u;
            }
        }
    }
    // op 0x80000000 等待（0x40de74 内联）：float 计时 >0 时倒数，归零清位。
    if (bullet.behavior_flags & 0x100u) {
        // 410f80 decrements first and clears at <=0; it is nonblocking
        // for other ex entries (40f087 masks this bit out).
        bullet.offscreen_time -= timer_step;
        bullet.offscreen_timer = i32(bullet.offscreen_time);
        bool unreachable = false;
        if (bullet.offscreen_directional) {
            if (!bullet.animation_geometry) {
                error_ = "etEx 0x100 requires bullet sprite geometry";
                return;
            }
            const float hw = bullet.sprite_width * .5f;
            const float hh = bullet.sprite_height * .5f;
            const bool outside = !(bullet.x + hw >= -192.f && bullet.x - hw < 192.f &&
                                   bullet.y + hh >= 0.f && bullet.y - hh < 448.f);
            if (outside) {
                float ux, uy;
                polar_xy(bullet.angle, 1.f, ux, uy);
                float negative = -999.f, positive = -999.f;
                for (int n = 0; n < 4; ++n) {
                    float x = ((n & 1) ? 192.f + hw : -192.f - hw) - bullet.x;
                    float y = ((n & 2) ? 448.f + hh : -hh) - bullet.y;
                    const float magnitude = std::sqrt(float(x*x + y*y));
                    if (magnitude != 0.f) { x /= magnitude; y /= magnitude; }
                    const float cross = float(double(y)*ux - double(x)*uy);
                    const float dot = float(double(ux)*x + double(uy)*y);
                    if (cross <= 0.f && dot >= 0.f && negative < dot) negative = dot;
                    if (cross >= 0.f && dot >= 0.f && positive < dot) positive = dot;
                }
                unreachable = negative < -998.f || positive < -998.f;
            }
        }
        if (unreachable || bullet.offscreen_timer <= 0) bullet.behavior_flags &= ~0x100u;
    }
    if (bullet.behavior_flags & 0x80000000u) {
        // 0x40db10:161-165：wait_time < 1 时清 bb4 位并令迭代计数 +1（do-while
        // 同帧重入 40f030）；否则 FUN_004732c0 按 rate 递减。小数速率的完成帧
        // 以 ecl_bullet_rate_test 的原版契约为准（0.5→13、0.25→25，即 <1 清位），
        // 与交付版 int 镜像驱动逐帧同拍。0x1058=(int)N-1 是 40f030:161 的只写
        // 镜像，不驱动到期。
        if (bullet.wait_time < 1.0f) {
            bullet.behavior_flags &= ~0x80000000u;
        } else {
            bullet.wait_time -= timer_step;
        }
        bullet.wait_timer = i32(bullet.wait_time);
    }
}

void StageSim::advance_bullets() {
    const float global_rate = time_rate();
    // 0x40db10 每弹一帧（队列在管理器链序 = 新弹在头的插入序；行为不抽
    // RNG，序不影响同步）：
    //   state 2（种子 op==2 的延迟弹）：pos += vel×0.5（0x40dbf9 分支）；
    //   随后与普通弹同路（0x40dd8d 门 0x4d4 默认 1.0f 恒过）。
    //   state 1：0x40f030 队列消费 → 行为派发 → pos += vel（0x40dec1）。
    //   出界回收：游戏区外扩（保持既有 ±256/-64..512 缓冲带）。
    // 帧首快照弹数：本帧中途 0x2000 新产弹不入当帧更新（原版头插在游标
    // 之后再到达）。逐弹以本地副本推进再写回：队列内 0x2000 的递归
    // push_back 会使 vector 重分配，禁止持有跨调用引用。
    const size_t count = bullets.size();
    //40e840 inserts at the active-list head;40e360 traverses newest first.
    //Child emissions consume game RNG, so processing vector insertion order
    //assigns random angles/speeds to the wrong parents even with equal totals.
    for (size_t next = count; next > 0;) {
        const size_t i = --next;
        StageBullet bullet = bullets[i];
        if (!bullet.alive) continue;
        if (bullet.state == 2) {
            bullet.x += float(bullet.vel[0] * global_rate) * 0.5f;
            bullet.y += float(bullet.vel[1] * global_rate) * 0.5f;
            bullet.z += float(bullet.vel[2] * global_rate) * 0.5f;
            bullet.collision_ready = bullet.state_age > 7;
            if (bullet.animation_ready) bullet.state = 1;
        } else {
            bullet.collision_ready = true;
        }
        if (bullet.state == 1) {
            // 40db10 repeats queue consumption in this update when a
            // behavior returns completion; the next serial command starts
            // before movement, not on the next frame.
            u32 finished;
            do {
                process_bullet_queue(bullet, false);
                if (!error_.empty()) return;
                const u32 previous = bullet.behavior_flags;
                if (!previous) break;
                step_bullet_behaviors(bullet);
                if (!error_.empty()) return;
                if (bullet.cancel_protection != 0) --bullet.cancel_protection;
                finished = previous & ~bullet.behavior_flags;
            } while (finished);
        }
        if (bullet.state != 2 && bullet.state != 4) {
            const float rate = bullet.state == 3 ? .5f : 1.f;
            bullet.x += float(bullet.vel[0] * global_rate) * rate;
            bullet.y += float(bullet.vel[1] * global_rate) * rate;
            bullet.z += float(bullet.vel[2] * global_rate) * rate;
        }
        if (bullet.state == 1) bullet.collision_ready = true;
        if (bullet.state == 3 || bullet.state == 4) bullet.collision_ready = false;
        if (!(bullet.behavior_flags & 0x100u)) {
            if (bullet.animation_geometry && bullet.cull_grace <= 0) {
                // 40db10 -> 40da30 reads the current sprite dimensions
                // before this update's ANM tick. Boundary contact is out.
                const double half_w = double(bullet.sprite_width) * .5;
                const double half_h = double(bullet.sprite_height) * .5;
                if (double(bullet.x) + half_w <= -192 || double(bullet.x) - half_w >= 192 ||
                    double(bullet.y) + half_h <= -64 || double(bullet.y) - half_h >= 448)
                    bullet.alive = false;
            } else if (!bullet_animation &&
                (bullet.x < -256.f || bullet.x > 256.f || bullet.y < -64.f || bullet.y > 512.f)) {
                bullet.alive = false; // animation-free diagnostic boundary
            }
        }
        if (bullet.cull_grace > 0) --bullet.cull_grace;
        if (bullet.alive && bullet_animation && !bullet_animation(bullet, false)) return;
        if (truncate_int(bullet.state_time) != bullet.state_age)
            bullet.state_time = float(bullet.state_age);
        bullet.state_time += global_rate > .99f && global_rate < 1.01f ? 1.f : global_rate;
        bullet.state_age = truncate_int(bullet.state_time);
        bullets[i] = bullet;
    }
    // 尾部压缩：存活弹前移（稳定序，逐帧 O(n)）。
    size_t keep = 0;
    for (size_t i = 0; i < bullets.size(); ++i) {
        if (bullets[i].alive) bullets[keep++] = bullets[i];
    }
    bullets.resize(keep);
}

void StageSim::clear_bullets() {
    for(auto& bullet:bullets)if(!cancel_bullet(bullet))return;
}

bool StageSim::cancel_bullet(StageBullet& bullet) {
        if(!bullet.alive||(bullet.state!=1&&bullet.state!=2))return true;
        bullet.state=3;bullet.state_age=0;bullet.state_time=0;
        bullet.delay=bullet.animation_interrupt=1;bullet.collision_ready=false;
        const i32 script=bullet_cancel_script(i32(bullet.sprite),i32(bullet.subtype));
        if(script>=0&&bullet_cancel_effect){
            //411df0 stores velocity*rate to f32, then multiplies by f64 10.
            const float rate=session?session->rate:1.f;
            const Vec3 displacement{float(double(float(bullet.vel[0]*rate))*10.0),
                float(double(float(bullet.vel[1]*rate))*10.0),
                float(double(float(bullet.vel[2]*rate))*10.0)};
            if(!bullet_cancel_effect(script,{bullet.x,bullet.y,bullet.z},displacement)){
                error_="Bullet cancel animation failed";return false;
            }
        }
    return true;
}

bool StageSim::cancel_respawn_bullets(const Vec3& center,float radius) {
    const float rate=time_rate();
    for(auto& bullet:bullets) {
        if(!bullet.alive || (bullet.state!=1 && bullet.state!=2) || bullet.cancel_protection)continue;
        const double dx=double(bullet.x)-center.x,dy=double(bullet.y)-center.y;
        const float limit=float(double(bullet.hit_size[0])*.5+radius);
        if(dx*dx+dy*dy>double(limit)*limit)continue;
        //411770 creates the independent particle at the old position before
        // the one half-step, and does not attach411df0's motion interpolation.
        const i32 script=bullet_cancel_script(i32(bullet.sprite),i32(bullet.subtype));
        if(script>=0 && stationary_cancel_effect &&
           !stationary_cancel_effect(script,{bullet.x,bullet.y,bullet.z})) {
            error_="Respawn bullet cancel particle failed";return false;
        }
        bullet.animation_interrupt=1;
        // 0x411770 半步（0x4118cf..0x41193c）：三轴同式三级 f32 链——
        // t=f32(vel·rate)（0x4118da fld [0x4c0a28] 后逐轴 fmulp+fstp）→
        // h=f32(t·0.5)（fld qword 0x4ae8b8 后逐轴 fmul+fstp）→ pos=f32(pos+h)。
        // x 轴原 double 单舍入式为移植笔误（rate=1.0 时数值恒等，
        // fp-audit-20261028 §4 D2）；统一后与 y/z 及 0x40db10 case2 半步同构。
        bullet.x+=float(bullet.vel[0]*rate)*.5f;
        bullet.y+=float(bullet.vel[1]*rate)*.5f;
        bullet.z+=float(bullet.vel[2]*rate)*.5f;
        bullet.state=4;bullet.state_age=0;bullet.state_time=0;bullet.collision_ready=false;
    }
    return true;
}

// ---- ECL 掉落表执行（0x417ad0 + 0x417b10）----
// RNG 全归属本函数（= 原版 0x417ad0/0x417b10 体内）：
//   主道具（drop_main != 0）：1 个 kind 号道具，无 RNG；
//   散落：初始 signed_unit×1（表空也抽）；每件 unit×1（散布份额
//   0.5..1）→（kind≥10 的 0x438ec0 体内再抽 signed_unit+unit 各 1，
//   由道具宿主 spawn_value_item 承接）→ signed_unit×1（角步进）。
// 道具实体的生成经 StageItemHost 即时同步调用；宿主为空时仅消耗等量
// RNG（原版道具池满/缺席的同构行为），次序不变。
void StageSim::drop_table(StageEnemy& enemy) {
    if (items && enemy.drop_main != 0)
        items->spawn_item(enemy.position[0], enemy.position[1], enemy.position[2],
                          enemy.drop_main, -1.5707964f, 2.2f);
    float angle = random.signed_unit() * 3.1415927f;
    for (int kind = 0; kind < 14; ++kind) {
        for (int n = 0; n < enemy.drop_extra[kind]; ++n) {
            // 0x424770：offset = (cos(angle)×area_x, sin(angle)×area_y)。
            const float ox = std::cos(static_cast<double>(angle)) * enemy.drop_area[0];
            const float oy = std::sin(static_cast<double>(angle)) * enemy.drop_area[1];
            const float jitter = 0.5f + random.unit() * 0.5f;
            const float x = enemy.position[0] + jitter * ox;
            const float y = enemy.position[1] + jitter * oy;
            if (items) {
                if (kind + 1 >= 10)
                    items->spawn_value_item(kind + 1 - 10, x, y, enemy.position[2], 0.5f);
                else
                    items->spawn_item(x, y, enemy.position[2], kind + 1, -1.5707964f, 2.2f);
            }
            // 417be6 stores RNG*pi to f32, then keeps angle+pi/2 and
            // noise*.25 extended until the single store at417c04.
            const float noise = float(double(random.signed_unit()) * double(3.1415927f));
            angle = wrap_angle(float(double(angle) + double(1.5707964f) + double(noise) * .25));
        }
    }
    for (i32& slot : enemy.drop_extra) slot = 0;  // memset(EDI+4, 0, 0x3c)
    enemy.drop_main = 0;                          // *param_1 = 0（0x417aff）
}

// ---- 615/616 半径消弹（0x4114f0）----
void StageSim::cancel_bullets_radius(const StageEnemy& enemy, float radius, bool spawn_cancel) {
    cancel_bullets_radius({enemy.position[0],enemy.position[1],enemy.position[2]},radius,spawn_cancel);
}

void StageSim::cancel_bullets_radius(const Vec3& center, float radius, bool spawn_cancel,
                                     bool respect_protection) {
    for (auto& bullet : bullets) {
        if (!bullet.alive || (bullet.state != 1 && bullet.state != 2)) continue;
        // 411770 (Bomb) skips protected bullets; ECL's 4114f0 does not.
        if (respect_protection && bullet.cancel_protection != 0) continue;
        const float dx = bullet.x - center.x;
        const float dy = bullet.y - center.y;
        const float limit = float(double(radius) + double(bullet.hit_size[0]) * 0.5);
        // 4117c5/4117d1 store dx/dy, 4117f9 stores the squared sum once;
        // 411801 keeps limit squared extended through the comparison.
        const float distance2 = float(double(dx) * dx + double(dy) * dy);
        if (double(distance2) > double(limit) * limit) continue;
        // 场内判定（0x4114f0 的 ±32 窗）：615 给场内弹产取消道具（kind 9）。
        if (items && spawn_cancel && bullet.x + 32.0f >= -192.0f &&
            bullet.x - 32.0f < 192.0f && bullet.y + 32.0f >= 0.0f &&
            bullet.y - 32.0f < 448.0f)
            items->spawn_item(bullet.x, bullet.y, bullet.z, 9, -1.5707964f, 2.2f);
        bullet.x += bullet.vel[0] * 0.5f;  // 4114f0 moves AFTER item creation
        bullet.y += bullet.vel[1] * 0.5f;
        bullet.z += bullet.vel[2] * 0.5f;
        bullet.alive = false;  // state → 4
    }
    if (items && !spawn_cancel)
        items->on_et_clear(center.x, center.y, center.z);
}

// ---- 发射内核（0x40ffe0 外层 ways/count 循环 + 0x40e840 单弹出生）----
void StageSim::emit_et(const StageEt& et, u32 enemy_serial, u32 slot, const float spawn[3],
                       float aim, u32 ex_seed) {
    const i32 ways = et.ways;
    const i32 count = et.count;
    const i32 mode = et.aim;
    // 0x40e840 角合成全程 x87 80 位（double 近似）；f32 常量提升后参与：
    // 6.2831855f = 6.283185482025146484375、3.1415927f = 3.1415927410125732421875。
    const double kTwoPiF = static_cast<double>(6.2831855f);
    const double kPiF = static_cast<double>(3.1415927f);
    const double aim_d = aim;
    for (i32 way = 0; way < ways; ++way) {
        // 路间速率（0x40e840 开头，80 位中间 → f32 落点 node+0xb88）。
        float speed = et.speed1;
        if (ways >= 2)
            speed = static_cast<float>(et.speed1 - (et.speed1 - et.speed2) *
                                                      static_cast<double>(way) /
                                                      static_cast<double>(ways - 1));
        for (i32 idx = 0; idx < count; ++idx) {
            double angle = 0;
            switch (mode) {
            case 0:
            case 1: {
                if ((count & 1) == 0)
                    angle = et.angle2 * 0.5 + static_cast<double>(idx / 2) * et.angle2;
                else
                    angle = static_cast<double>((idx + 1) / 2) * et.angle2;
                angle = angle + 0.0;
                if (idx & 1) angle = angle * -1.0;
                if (mode == 0) angle = angle + aim_d;
                angle = et.angle1 + angle;
                break;
            }
            case 2:
            case 3:
                // 0x40e950..0x40e988 精确求值序：aim+0 落 f32 → idx*2π/count 相加后
                // 再落 f32 → 与 80 位的 angle2*way+angle1 求和 → 尾部双重 wrap。
                {
                    const float base = mode == 2 ? aim : 0.0f;
                    const double spoke = static_cast<double>(idx) * kTwoPiF /
                                         static_cast<double>(count);
                    const float mid = static_cast<float>(spoke + base);
                    angle = (et.angle2 * static_cast<double>(way) + et.angle1) +
                            static_cast<double>(mid);
                }
                break;
            case 4:
                angle = aim_d;
                angle = et.angle2 * static_cast<double>(way) + et.angle1 +
                        static_cast<double>(idx) * kTwoPiF / static_cast<double>(count) +
                        kPiF / static_cast<double>(count) + angle;
                break;
            case 5:
                angle = et.angle2 * static_cast<double>(way) + et.angle1 +
                        static_cast<double>(idx) * kTwoPiF / static_cast<double>(count) +
                        kPiF / static_cast<double>(count);
                break;
            case 6:
                angle = et.angle2 +
                        random.unit() * static_cast<double>(et.angle1 - et.angle2);
                break;
            case 7:
                speed = static_cast<float>(random.unit() *
                                               static_cast<double>(et.speed1 - et.speed2) +
                                           et.speed2);
                angle = et.angle2 * static_cast<double>(way) + et.angle1 +
                        static_cast<double>(idx) * kTwoPiF / static_cast<double>(count);
                break;
            case 8:
                angle = et.angle2 +
                        random.unit() * static_cast<double>(et.angle1 - et.angle2);
                speed = static_cast<float>(random.unit() *
                                               static_cast<double>(et.speed1 - et.speed2) +
                                           et.speed2);
                break;
            case 9:
            case 10: {
                if ((count & 1) == 0)
                    angle = et.angle2 * 0.5 + static_cast<double>(way / 2) * et.angle2;
                else
                    angle = static_cast<double>((way + 1) / 2) * et.angle2;
                angle = angle + static_cast<double>(idx) * kTwoPiF / static_cast<double>(count);
                if (way & 1) angle = angle * -1.0;
                if (mode == 9) angle = angle + aim_d;
                angle = et.angle1 + angle;
                break;
            }
            default:
                angle = et.angle1;
                break;
            }
            // 0x40e840 尾：raw 合成角先落 f32（local_8），再 0x472e90(acc,0)→
            // 0x472f30 双重 wrap 写 node+0xb8c（stored）。出生速度向量用 raw 角
            // （尾部 0x412510(node+0xb7c, local_8, speed)）；spin 偏移与弹体
            // angle 字段用 wrap 后的 stored（0x40e840 即如此不对称——
            // s5-drift-20261028 §2.1/§2.2 位级实证）。
            const float raw = static_cast<float>(angle);
            const float stored = wrap_angle(wrap_angle(raw));
            float x = spawn[0], y = spawn[1];
            // z：0x40e840 写入 spawn z 后无条件覆写 0x3dcccccd（渲染层常量），
            // 事件口径恒 0.1f；spin（et[9]）只作用于 x/y（0x40e840 尾段）。
            const float z = 0.1f;
            if (et.spin != 0.0f) {
                float ox = 0, oy = 0;
                polar_xy(stored, et.spin, ox, oy);
                x = x + ox;
                y = y + oy;
            }
            // 40e840 checks the final radial spawn after angle/RNG generation,
            // before sprite initialization and queue execution. Boundary survives.
            if(active_bullet_protect_radius2>0) {
                const double dx=double(x)-player_x,dy=double(y)-player_y;
                const float distance2=float(dx*dx+dy*dy); // 40eca0 f32 store before compare
                if(distance2<active_bullet_protect_radius2)continue;
            }
            sink.bullet_spawn(enemy_serial, slot, u32(way), u32(idx), x, y, z, stored, speed,
                              et.sprite, et.subtype);
            if (interactive) {
                // 交互模式同源产弹体实体；无 RNG（事件口径无涉）。
                StageBullet bullet;
                //40e840 only constructs the node. Collision belongs to
                //40db10, so children inserted behind40e360's saved cursor
                //cannot hit/graze until their first actual manager update.
                bullet.collision_ready = false;
                bullet.x = x;
                bullet.y = y;
                bullet.z = z;
                bullet.angle = stored;
                bullet.speed = speed;
                // 速度向量（0x40e840 尾 0x412510(b7c, raw角, speed) 出生写：
                // fsincos(raw)×speed 一次落 f32；|angle|≤π 时 raw==stored，零变化）。
                bullet.vel[0] = std::cos(static_cast<double>(raw)) * speed;
                bullet.vel[1] = std::sin(static_cast<double>(raw)) * speed;
                bullet.vel[2] = 0;
                bullet.sprite = et.sprite;
                bullet.subtype = et.subtype;
                bullet.source_enemy = enemy_serial;
                bullet.transform_sound=et.transform_sound;
                // 0x40e840 allocates a distinct runtime bullet node for every
                // way/index birth.  Keep an explicit identity because the
                // interactive vector is compacted after culling.
                bullet.generation = ++next_bullet_generation_;
                if (bullet.generation == 0) bullet.generation = ++next_bullet_generation_;
                // 0x40e840:237-238：弹体尺寸（两轴同值）= 弹型表 +0x104。
                // 表覆盖 0..37；越界类型原版读表外（无合法用例），此处保留
                // StageBullet 默认定标值，待显式报错任务收敛。
                if (const BulletAppearance* appearance = bullet_appearance(i32(et.sprite))) {
                    bullet.hit_size[0] = appearance->scale;
                    bullet.hit_size[1] = appearance->scale;
                }
                // etEx 队列整块拷贝（0x40e840 尾 0xc6 int）+ 种子游标。
                for (u32 n = 0; n < 18; ++n) bullet.ex[n] = et.ex[n];
                bullet.ex_cursor = i32(ex_seed & 0xffu);
                if (bullet.ex_cursor > 18) bullet.ex_cursor = 18;
                // 种子项 op==2 → 延迟弹（0x40e840 尾分支）：state 2、游标越过
                // 种子项、出生位回退 vel×4（此后首帧 pos += vel×0.5 折返）。
                if (bullet.ex_cursor < 18 && bullet.ex[bullet.ex_cursor].op == 2) {
                    const i32 packed = i32(bullet.ex[bullet.ex_cursor].args[4]);
                    bullet.state = 2;
                    bullet.ex_cursor += 1;
                    bullet.x -= bullet.vel[0] * 4.0f;
                    bullet.y -= bullet.vel[1] * 4.0f;
                    bullet.z -= bullet.vel[2] * 4.0f;
                    if (i16(packed) != 1) bullet.delay = packed + 7;  // 出场 ANM 帧数（表现）
                }
                // 出生时队列消费（0x40e840 尾 FUN_0040f030）——op 0x10 mode 5/6
                // 的 RNG 与 0x2000 子发射都在此按项发生。消费先于入列：本地
                // 实体吸收 0x2000 的递归 push（vector 重分配不悬空引用）；
                // 事件序 = 父弹先于子弹（0x40e840 调用序），列表序 = 子弹在
                // 父弹前（原版头插语义）。
                process_bullet_queue(bullet, true);
                if (!error_.empty()) return;
                if (bullet_animation && !bullet_animation(bullet, true)) return;
                bullets.push_back(bullet);
            }
        }
    }
}

bool StageSim::clear_enemies() {
    for (size_t index = 0; index < enemies.size(); ++index) {
        // 41ac70 saves next before41aad0 and keeps marked entities linked
        // until418e00. Another clear in that interval repeats their effects.
        const bool had_next = index + 1 < enemies.size();
        StageEnemy& entry = *enemies[index];
        const u32 f = entry.flags;
        if ((((f & 0xa0u) == 0) && ((f & 0x1800400u) == 0)) || (f & 0x100u)) {
            entry.drop_main = 0;
            for (auto& count : entry.drop_extra) count = 0;
            entry.drop_area[0] = entry.drop_area[1] = 32.f;
            if (!death_effects_and_drops(entry)) return false;
            entry.flags |= 0x4000000u;
            entry.dead = true;
        }
        if (!had_next) break;
    }
    return error_.empty();
}

bool StageSim::death_effects_and_drops(StageEnemy& enemy) {
    // 41aad0 emits sound and the oriented death animation before drops.
    if(enemy.death_sound>=0&&enemy_sound&&!enemy_sound(enemy.death_sound)) {
        error_="Enemy death sound failed";return false;
    }
    if(enemy.death_effect>=0&&enemy_death_effect) {
        const double dx=double(enemy.position[0])-enemy.previous_position[0];
        const double dy=double(enemy.position[1])-enemy.previous_position[1];
        const float distance=float(dx*dx+dy*dy);
        const float angle=distance>=0.040000003f?float(std::atan2(dy,dx)):-1.5707964f;
        if(!enemy_death_effect(enemy.death_effect_file,enemy.death_effect,
              {enemy.position[0],enemy.position[1],enemy.position[2]},angle)) {
            error_="Enemy death animation failed";return false;
        }
    }
    drop_table(enemy);
    if (!enemy.death_routine.empty() && enemy.threads) {
        // 41aad0 drops first, resets all ECL threads, executes the death
        // routine once with time0, then clears its pointer and retires.
        const std::string routine = enemy.death_routine;
        const u8 difficulty = enemy.threads->main.difficulty;
        StageEnemy* saved = current;
        current = &enemy;
        if (!enemy.threads->restart(routine.c_str(), difficulty) || enemy.threads->step(0) == -2)
            error_ = enemy.threads->error;
        enemy.death_routine.clear();
        current = saved;
    }
    return error_.empty();
}

void StageSim::damage(StageEnemy& enemy, float amount) {
    if (enemy.dead || amount <= 0 || (enemy.flags & 0x31u) ||
        enemy.invulnerable > 0 || (dialog_host && dialog_host->dialog_active())) return;
    if (enemy.spell_active) {
        // 0x4179e0: spells use a 7x integer accumulator, with signed integer
        // division, not a floating 1/7 damage multiplier.
        enemy.spell_life -= i32(amount);
        // param1[4] is inner+3f5c = outer+5148: the current health interrupt
        // threshold written by4218f0, separate from511's max-life+3f50.
        // The distinction changes truncation toward zero on either side.
        enemy.life = float((enemy.spell_life - enemy.phase_life_threshold * 7) / 7 +
                           enemy.phase_life_threshold);
    } else {
        enemy.life -= amount;
    }
    if (interrupt_enemy(enemy)) {
        // Health interruption inside 0x419a90 immediately executes the new
        // phase. It must not destroy the Boss entity or clear bossWait's slot.
        StageEnemy* saved = current;
        current = &enemy;
        const int result = enemy.threads->step(time_rate());
        if (result == -2) error_ = enemy.threads->error;
        else if (result) enemy.dead = true;
        current = saved;
        // 419a90 returns immediately when the interrupt script terminates.
        // MBossDead has already emitted its scripted drops; it must not
        // fall through to death spirits / 41aad0 a second time.
        if (enemy.dead && retire_sink) retire_sink(enemy.serial);
        return;
    }
    if (enemy.dead || (enemy.life <= 0.0f && !(enemy.flags & 0x80u))) {
        // 0x419f3c：计时奖励神灵先于 0x41aad0 的掉落表消费共享 RNG。
        if (items && i32(enemy.age) <= enemy.death_spirit_limit) {
            if (enemy.death_spirit_count == 0) {
                error_ = "Enemy death spirit count divides by zero";
                return;
            }
            const i32 interval = enemy.death_spirit_limit / enemy.death_spirit_count;
            if (interval == 0) {
                error_ = "Enemy death spirit interval divides by zero";
                return;
            }
            const i32 count = (enemy.death_spirit_limit - i32(enemy.age)) / interval +
                              (diff == 2 || diff == 3 ? 2 : 1);
            for (i32 index = 0; index < count; ++index)
                items->spawn_value_item(1, enemy.position[0], enemy.position[1],
                                        enemy.position[2], 0.5f);
        }
        if(!death_effects_and_drops(enemy))return;
        // 与出界自杀同路：下帧遍历摘链。Boss 槽指向一并清除
        // （oracle 侧同式写 mgr+0x5c[slot]=0，§7.2）。
        for (i32& slot : boss_slot)
            if (slot == enemy.script_id) slot = -1;
        enemy.dead = true;
        // 同帧 VM 回收（0x419a90 命中致死 → 同趟销毁的 ANM 面）。原版命中
        // 检测在敌机自身更新趟内（0x41a8a0 → 0x419a90 → 返回 -1 → 0x418e00
        // 销毁支路），VM 在绘制前已释放——无输入窗出界自杀同路同拍
        // （f528 首例，oracle 实测批内 quad 同帧消失）；移植侧自机弹命中在
        // step_frame 走链之后结算（StageBattle::collide_shots），死亡置位
        // 当帧即回收才能与原版「销毁当帧出捕获面」对齐——回放窗 f124
        // op[16] count 6≠4 的实测根因（修复后 f124-134 逐字节对齐）。
        // vector 摘链仍在下帧游标（[-9946] ENM_CNT_REAL 读 vector 大小的
        // 契约语义不变）；retire 幂等（句柄已归零则空转）。
        if (retire_sink) retire_sink(enemy.serial);
        if (replay_events)  // §7.3：kill 事件（回放会话）
            sink.kill(enemy.serial, enemy.position[0], enemy.position[1]);
    }
}

}  // namespace th13
