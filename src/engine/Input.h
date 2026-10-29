#pragma once

#include "core/Types.h"

namespace Voidfall {

struct InputState {
    Vec2 moveDir{0.0f, 0.0f};
    Vec2 aimDir{1.0f, 0.0f};
    Vec2 mouseWorldPos{0.0f, 0.0f};

    bool firePrimary{false};
    bool firePrimaryHeld{false};
    bool fireOrdnance{false};
    bool fireOrdnanceHeld{false};

    bool dash{false};
    bool heatPurge{false};
    bool pause{false};
    bool interact{false};

    bool profileAssault{false};
    bool profileBulwark{false};
    bool profileInterceptor{false};
    bool profileBalanced{false};

    // UI Navigation
    bool menuUp{false};
    bool menuDown{false};
    bool menuLeft{false};
    bool menuRight{false};
    bool menuConfirm{false};
    bool menuBack{false};
};

class Input {
public:
    static void Update(const Vec2& cameraOffset, float cameraZoom);
    static const InputState& Get() { return s_state; }

private:
    static InputState s_state;
};

} // namespace Voidfall
