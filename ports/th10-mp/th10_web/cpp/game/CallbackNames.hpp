#pragma once
// Semantic callback names. Executable addresses are retained solely for the
// non-native original-oracle adapter; shipping jobs bind C++ functions once.
namespace th10::callback_id {
#ifdef TH_NATIVE_PLATFORM
enum : unsigned {None=0,
    SessionUpdate,
    SessionDraw,
    PlayerUpdate,
    PlayerDraw,
    BulletsUpdate,
    BulletsDraw,
    LasersUpdate,
    LasersDraw,
    ItemsUpdate,
    ItemsDraw,
    BombUpdate,
    BombDraw,
    EnemiesUpdate,
    EnemiesDraw,
    SpellUpdate,
    SpellBackground,
    SpellForeground,
    ReplayUpdate,
    ReplayFrame,
    ReplayDraw,
    ResultsUpdate,
    ResultsDraw,
    PopupsUpdate,
    PopupsDraw,
    HintsUpdate,
    HintsDraw,
    EffectsUpdate,
    EffectsDraw,
    AnimationsWorld,
    AnimationsUI,
    DistortionUpdate,
    DistortionDraw,
    ApplicationUpdate,
    ApplicationBeginDraw,
    ApplicationDrawBarrier,
    ApplicationFinishDraw,
    FrameStatisticsDraw,
    StageUpdate,
    StageBackground,
    StageForeground,
    CommonUpdate,
    CommonDraw,
    CommonEarlyDraw,
    EndingUpdate,
    EndingDraw,
    EndingLoad,
    HudUpdate,
    HudDraw,
    StartupUpdate,
    StartupDraw,
    StartupLoad,
    TitleUpdate,
    TitleDraw,
    TitleLoad,
    ScreenFadeUpdate,
    ScreenShakeUpdate,
    ScreenFlashUpdate,
    ScreenCircleUpdate,
    ScreenArcadeUpdate,
    ScreenViewShakeUpdate,
    ScreenFadeDraw,
    ScreenFlashDraw,
    ScreenCircleDraw,
    ScreenArcadeFadeDraw,
    ScreenArcadeFlashDraw,
    ScreenEffectDelete,
    HomingShotInitialize,
    HomingShotUpdate,
    LaserShotUpdate,
    PlayerOptionInitialize,
    PlayerOptionUpdate,
    AnimationLayer0,
    AnimationLayer1,
    AnimationLayer2,
    AnimationLayer3,
    AnimationLayer4,
    AnimationLayer5,
    AnimationLayer6,
    AnimationLayer7,
    AnimationLayer8,
    AnimationLayer9,
    AnimationLayer10,
    AnimationLayer11,
    AnimationLayer12,
    AnimationLayer13,
    AnimationLayer14,
    AnimationLayer15,
    AnimationLayer16,
    AnimationLayer19,
    Count
};
#else
inline constexpr unsigned SessionUpdate=0x4187c0;
inline constexpr unsigned SessionDraw=0x4187d0;
inline constexpr unsigned PlayerUpdate=0x426500;
inline constexpr unsigned PlayerDraw=0x426510;
inline constexpr unsigned BulletsUpdate=0x406770;
inline constexpr unsigned BulletsDraw=0x4067a0;
inline constexpr unsigned LasersUpdate=0x41c480;
inline constexpr unsigned LasersDraw=0x41c4e0;
inline constexpr unsigned ItemsUpdate=0x41ba00;
inline constexpr unsigned ItemsDraw=0x41ba30;
inline constexpr unsigned BombUpdate=0x405840;
inline constexpr unsigned BombDraw=0x405850;
inline constexpr unsigned EnemiesUpdate=0x40d810;
inline constexpr unsigned EnemiesDraw=0x40d820;
inline constexpr unsigned SpellUpdate=0x409220;
inline constexpr unsigned SpellBackground=0x409230;
inline constexpr unsigned SpellForeground=0x409270;
inline constexpr unsigned ReplayUpdate=0x42a3d0;
inline constexpr unsigned ReplayFrame=0x42a3e0;
inline constexpr unsigned ReplayDraw=0x42a430;
inline constexpr unsigned ResultsUpdate=0x422a90;
inline constexpr unsigned ResultsDraw=0x422aa0;
inline constexpr unsigned PopupsUpdate=0x42b9a0;
inline constexpr unsigned PopupsDraw=0x42b9b0;
inline constexpr unsigned HintsUpdate=0x4198a0;
inline constexpr unsigned HintsDraw=0x4198b0;
inline constexpr unsigned EffectsUpdate=0x40b050;
inline constexpr unsigned EffectsDraw=0x40b060;
inline constexpr unsigned AnimationsWorld=0x4485d0;
inline constexpr unsigned AnimationsUI=0x4485e0;
inline constexpr unsigned DistortionUpdate=0x445620;
inline constexpr unsigned DistortionDraw=0x445880;
inline constexpr unsigned ApplicationUpdate=0x41ff80;
inline constexpr unsigned ApplicationBeginDraw=0x420000;
inline constexpr unsigned ApplicationDrawBarrier=0x4200c0;
inline constexpr unsigned ApplicationFinishDraw=0x4200d0;
inline constexpr unsigned FrameStatisticsDraw=0x413690;
inline constexpr unsigned StageUpdate=0x403050;
inline constexpr unsigned StageBackground=0x403060;
inline constexpr unsigned StageForeground=0x403070;
inline constexpr unsigned CommonUpdate=0x4014f0;
inline constexpr unsigned CommonDraw=0x401510;
inline constexpr unsigned CommonEarlyDraw=0x401520;
inline constexpr unsigned EndingUpdate=0x40ba70;
inline constexpr unsigned EndingDraw=0x40ba80;
inline constexpr unsigned EndingLoad=0x40c3a0;
inline constexpr unsigned HudUpdate=0x415ae0;
inline constexpr unsigned HudDraw=0x415af0;
inline constexpr unsigned StartupUpdate=0x41feb0;
inline constexpr unsigned StartupDraw=0x41fef0;
inline constexpr unsigned StartupLoad=0x41f990;
inline constexpr unsigned TitleUpdate=0x42d2e0;
inline constexpr unsigned TitleDraw=0x42d2f0;
inline constexpr unsigned TitleLoad=0x42c9f0;
inline constexpr unsigned ScreenFadeUpdate=0x43bd40;
inline constexpr unsigned ScreenShakeUpdate=0x43c550;
inline constexpr unsigned ScreenFlashUpdate=0x43c230;
inline constexpr unsigned ScreenCircleUpdate=0x43c460;
inline constexpr unsigned ScreenArcadeUpdate=0x43c310;
inline constexpr unsigned ScreenViewShakeUpdate=0x43c710;
inline constexpr unsigned ScreenFadeDraw=0x43c1a0;
inline constexpr unsigned ScreenFlashDraw=0x43c2c0;
inline constexpr unsigned ScreenCircleDraw=0x43c500;
inline constexpr unsigned ScreenArcadeFadeDraw=0x43c3c0;
inline constexpr unsigned ScreenArcadeFlashDraw=0x43c410;
inline constexpr unsigned ScreenEffectDelete=0x43c870;
inline constexpr unsigned HomingShotInitialize=0x428ad0;
inline constexpr unsigned HomingShotUpdate=0x428b10;
inline constexpr unsigned LaserShotUpdate=0x428c20;
inline constexpr unsigned PlayerOptionInitialize=0x427950;
inline constexpr unsigned PlayerOptionUpdate=0x427ad0;
inline constexpr unsigned AnimationLayer0=0x4485f0;
inline constexpr unsigned AnimationLayer1=0x448600;
inline constexpr unsigned AnimationLayer2=0x448610;
inline constexpr unsigned AnimationLayer3=0x448620;
inline constexpr unsigned AnimationLayer4=0x4486a0;
inline constexpr unsigned AnimationLayer5=0x4486b0;
inline constexpr unsigned AnimationLayer6=0x4486c0;
inline constexpr unsigned AnimationLayer7=0x4486d0;
inline constexpr unsigned AnimationLayer8=0x4486e0;
inline constexpr unsigned AnimationLayer9=0x4486f0;
inline constexpr unsigned AnimationLayer10=0x448700;
inline constexpr unsigned AnimationLayer11=0x448710;
inline constexpr unsigned AnimationLayer12=0x448720;
inline constexpr unsigned AnimationLayer13=0x448730;
inline constexpr unsigned AnimationLayer14=0x448740;
inline constexpr unsigned AnimationLayer15=0x448770;
inline constexpr unsigned AnimationLayer16=0x4487c0;
inline constexpr unsigned AnimationLayer19=0x448810;
#endif
}
