#include "engine/Input.h"
#include "core/Config.h"
#include <raylib.h>
#include <cmath>

namespace Voidfall {

InputState Input::s_state;

void Input::Update(const Vec2& cameraOffset, float cameraZoom) {
    auto& cfg = ConfigManager::Get();

    // Reset single-frame triggers
    s_state = InputState();

    if (cfg.autotestMode) {
        // AI bot simulation for smoke testing
        static float simAngle = 0.0f;
        simAngle += 0.05f;
        s_state.moveDir = Vec2(std::cos(simAngle), std::sin(simAngle * 0.7f)).Normalized();
        s_state.aimDir = Vec2(std::cos(simAngle * 1.3f), std::sin(simAngle * 1.3f)).Normalized();
        s_state.firePrimaryHeld = true;
        s_state.fireOrdnanceHeld = (static_cast<int>(simAngle * 10) % 20 == 0);
        return;
    }

    // Keyboard Movement
    Vec2 rawMove(0.0f, 0.0f);
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) rawMove.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) rawMove.y += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) rawMove.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) rawMove.x += 1.0f;

    // Gamepad Left Stick Movement
    if (IsGamepadAvailable(0)) {
        float stickX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        float stickY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
        if (std::abs(stickX) > 0.2f || std::abs(stickY) > 0.2f) {
            rawMove = Vec2(stickX, stickY);
        }
    }
    s_state.moveDir = rawMove.Normalized();

    // Mouse Aiming & World Position calculation
    Vector2 mPos = GetMousePosition();
    float zoom = cameraZoom > 0.001f ? cameraZoom : 1.0f;
    s_state.mouseWorldPos = Vec2(
        cameraOffset.x + (mPos.x - GetScreenWidth() * 0.5f) / zoom,
        cameraOffset.y + (mPos.y - GetScreenHeight() * 0.5f) / zoom
    );

    // Aim Direction relative to screen center
    Vec2 screenCenter(GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f);
    Vec2 aimVector(mPos.x - screenCenter.x, mPos.y - screenCenter.y);

    // Gamepad Right Stick Aiming override
    if (IsGamepadAvailable(0)) {
        float rStickX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X);
        float rStickY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y);
        if (std::abs(rStickX) > 0.25f || std::abs(rStickY) > 0.25f) {
            aimVector = Vec2(rStickX, rStickY);
        }
    }

    s_state.aimDir = aimVector.LengthSq() > 1.0f ? aimVector.Normalized() : Vec2(1.0f, 0.0f);

    // Weapons & Actions
    s_state.firePrimary = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    s_state.firePrimaryHeld = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    s_state.fireOrdnance = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    s_state.fireOrdnanceHeld = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

    s_state.dash = IsKeyPressed(KEY_SPACE) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1));
    s_state.heatPurge = IsKeyPressed(KEY_F) || IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1));
    s_state.pause = IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT));
    s_state.interact = IsKeyPressed(KEY_E) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN));

    // Power Routing Hotkeys
    s_state.profileAssault = IsKeyPressed(KEY_ONE) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP));
    s_state.profileBulwark = IsKeyPressed(KEY_TWO) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN));
    s_state.profileInterceptor = IsKeyPressed(KEY_THREE) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT));
    s_state.profileBalanced = IsKeyPressed(KEY_FOUR) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT));

    // UI Navigation
    s_state.menuUp = IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP));
    s_state.menuDown = IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN));
    s_state.menuLeft = IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT));
    s_state.menuRight = IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT));
    s_state.menuConfirm = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN));
    s_state.menuBack = IsKeyPressed(KEY_ESCAPE) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT));
}

} // namespace Voidfall
