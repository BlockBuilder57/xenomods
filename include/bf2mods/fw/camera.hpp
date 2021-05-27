#pragma once

#include "bf2mods/mm/math_types.hpp"
#include "bf2mods/utils.hpp"
#include "document.hpp"
#include "update_info.hpp"

namespace fw {

enum CAMERA_PRIO {};

class Camera {
   public:
    Camera(CAMERA_PRIO);
    ~Camera();

    void setPosRot(mm::Vec3 const& pos, mm::Quat const& quaternion);
    void setLookAt(mm::Vec3 const& camPos, mm::Vec3 const& lookAtPos, float banking);
};

enum class DEBUG_CAMERA { NONE, PAD, MAYA, MAYA_PAD };

class CameraManager {
   public:
    INSERT_PADDING_BYTES(0x698);
    DEBUG_CAMERA debugCameraType;
    INSERT_PADDING_BYTES(0x6B0 - 0x69C);

    void setDebugCamera(fw::DEBUG_CAMERA);

    void update(fw::Document const&, fw::UpdateInfo const&);
};
STATIC_ASSERT_SIZE(CameraManager, 0x6B0);

class DebugCamInfo;

class CtrlMayaCam {
   public:
    bool moveKeyboard(fw::DebugCamInfo&, fw::InputManager const&);
    void moveMouse(fw::DebugCamInfo&, fw::InputManager const&);

    void update(fw::DebugCamInfo&, fw::Document const&, fw::UpdateInfo const&);
};

}  // namespace fw
