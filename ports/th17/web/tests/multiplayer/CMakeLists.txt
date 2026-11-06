cmake_minimum_required(VERSION 3.20)
project(th17_multiplayer_tests LANGUAGES CXX)
enable_testing()

add_subdirectory(../../../../../common common)
add_executable(th17-multiplayer-session session-test.cpp
    ../../cpp/game/FrameScheduler.cpp
    ../../cpp/multiplayer/FrameState.cpp
    ../../cpp/multiplayer/Session.cpp)
target_include_directories(th17-multiplayer-session PRIVATE ../../cpp/multiplayer)
target_link_libraries(th17-multiplayer-session PRIVATE eagler::session_channel)
add_test(NAME th17-multiplayer-session COMMAND th17-multiplayer-session)


set(game ../../cpp/game)
add_executable(th17-team-spirits team-spirits.cpp
    ../../cpp/multiplayer/AnimationState.cpp
    ../../cpp/multiplayer/PickupState.cpp
    ${game}/AnimalSpirits.cpp
    ${game}/AnmCamera.cpp
    ${game}/AnmCoordinates.cpp
    ${game}/AnmGeometry.cpp
    ${game}/AnmInterpolators.cpp
    ${game}/AnmInterpreter.cpp
    ${game}/AnmManager.cpp
    ${game}/AnmOverlay.cpp
    ${game}/AnmOverlayRenderer.cpp
    ${game}/AnmRegistry.cpp
    ${game}/AnmRenderer.cpp
    ${game}/AnmResource.cpp
    ${game}/AnmStrip.cpp
    ${game}/AnmTrail.cpp
    ${game}/AnmTransforms.cpp
    ${game}/AnmUpdate.cpp
    ${game}/AnmVariables.cpp
    ${game}/AnmVisualState.cpp
    ${game}/DamageSources.cpp
    ${game}/EffectManager.cpp
    ${game}/Hud.cpp
    ${game}/HudDraw.cpp
    ${game}/HudResults.cpp
    ${game}/Interpolation.cpp
    ${game}/ItemCollection.cpp
    ${game}/ItemFrame.cpp
    ${game}/ItemManager.cpp
    ${game}/ItemSpirits.cpp
    ${game}/MotionState.cpp
    ${game}/RoaringSystem.cpp)
target_sources(th17-team-spirits PRIVATE
    ${game}/BulletActivation.cpp ${game}/BulletAppearance.cpp ${game}/BulletArea.cpp
    ${game}/BulletCancellation.cpp ${game}/BulletCollision.cpp ${game}/BulletDraw.cpp
    ${game}/BulletFormation.cpp ${game}/BulletFrame.cpp ${game}/BulletManager.cpp
    ${game}/BulletScene.cpp ${game}/BulletShooter.cpp ${game}/BulletSpawn.cpp
    ${game}/BulletState.cpp ${game}/BulletVisual.cpp
    ${game}/EnemyCallbacks.cpp
    ${game}/CurveCancellation.cpp ${game}/CurveGeometry.cpp ${game}/CurveMotion.cpp
    ${game}/CurvePath.cpp ${game}/CurveProgram.cpp ${game}/LaserCancellation.cpp
    ${game}/LaserContact.cpp ${game}/LaserDynamics.cpp ${game}/LaserManager.cpp
    ${game}/LaserMotion.cpp ${game}/LaserProgram.cpp ${game}/LaserReflection.cpp
    ${game}/LaserScene.cpp ${game}/LaserVisual.cpp ${game}/LineIntersection.cpp
    ${game}/PlayerCollision.cpp ${game}/SegmentIntersection.cpp ${game}/StationaryLaserProgram.cpp)
set_source_files_properties(${game}/LaserScene.cpp PROPERTIES COMPILE_DEFINITIONS TH17_MULTIPLAYER=1)
target_compile_features(th17-team-spirits PRIVATE cxx_std_17)
target_compile_options(th17-team-spirits PRIVATE -ffp-contract=off)
add_test(NAME th17-team-spirits COMMAND th17-team-spirits)
