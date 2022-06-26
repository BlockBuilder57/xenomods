//
// Created by block on 6/21/2022.
//

#include "camera_tools.hpp"

#include <bf2mods/mm/math_types.hpp>
#include "bf2mods/stuff/utils/util.hpp"
#include "bf2logger.hpp"
#include "skyline/logger/Logger.hpp"

#include "debug_stuff.hpp" // i'll get it sometime

#include <bf2mods/fw/camera.hpp>

namespace fw {

	//namespace Camera {

		GENERATE_SYM_HOOK(Camera_setLookAt, "_ZN2fw6Camera9setLookAtERKN2mm4Vec3ES4_f", void, fw::Camera* this_pointer, mm::Vec3* vec1, mm::Vec3* vec2, float num) {
			//bf2mods::g_Logger->LogInfo("setLookAt(this: %p, vec1: %s, vec2: %s, num: %f)", this_pointer, bf2mods::Prettyprinter<mm::Vec3>::format(*vec1).c_str(),  bf2mods::Prettyprinter<mm::Vec3>::format(*vec2).c_str(), num);

			// draw a arrow to the hit i guess lol

			auto seconds = nn::os::GetSystemTick()/19200000.;
			auto diff = vec1 - vec2;

			mm::Vec3 wackyvec {vec2->x, vec2->y + static_cast<float>((sin(seconds) * 4.f)), vec2->z};


			//bf2mods::g_Logger->LogInfo( "length: %f", (*vec1 - *vec2).LengthSqu());

			Camera_setLookAtBak(this_pointer, vec1, &wackyvec, num);

			//debug::drawArrow(*vec1, *vec2, mm::Col4{1.f, 0.f, 0.f, 1.f});
		}

		GENERATE_SYM_HOOK(Camera_updateViewMatrix, "_ZN2fw6Camera16updateViewMatrixEv", void, fw::Camera* this_pointer) {
			bf2mods::g_Logger->LogInfo("updateViewMatrix(this: %p) - pos: %s", this_pointer, bf2mods::Prettyprinter<mm::Vec3>::format(this_pointer->position).c_str());

			Camera_updateViewMatrixBak(this_pointer);
		}

	//}

}

namespace bf2mods {

	void SetupCameraTools() {
		skyline::logger::s_Instance->LogFormat("Setting up camera tools...");

		fw::Camera_setLookAtHook();
		fw::Camera_updateViewMatrixHook();
	}

}