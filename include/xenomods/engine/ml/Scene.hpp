//
// Created by block on 6/26/2022.
//

#pragma once

#include "WinView.hpp"
#include "xenomods/Utils.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"

namespace ml {

	struct AttrTransform {
		mm::Vec3 pos;
		INSERT_PADDING_BYTES(0x4);
		mm::Vec3 camPos;
		INSERT_PADDING_BYTES(0x4);
		INSERT_PADDING_BYTES(sizeof(mm::Vec3));
		INSERT_PADDING_BYTES(0x4);
		INSERT_PADDING_BYTES(sizeof(mm::Mat44));
		mm::Mat44 viewMatInverse;
		mm::Mat44 viewMat;
		mm::Vec3 eulerAngles;
		INSERT_PADDING_BYTES(0x4);
		mm::Vec3 camRot;
		INSERT_PADDING_BYTES(0x4);
		INSERT_PADDING_BYTES(sizeof(mm::Vec3));
		INSERT_PADDING_BYTES(0x4);
		INSERT_PADDING_BYTES(sizeof(mm::Vec3));
		INSERT_PADDING_BYTES(0x4);
		INSERT_PADDING_BYTES(sizeof(mm::Vec3));
		INSERT_PADDING_BYTES(0x4);
		mm::Vec3 target;
		INSERT_PADDING_BYTES(0x4);
		INSERT_PADDING_BYTES(sizeof(mm::Vec3));
		INSERT_PADDING_BYTES(0x4);
		mm::Mat44 frustum;
		mm::Mat44 projection;
		mm::Pnt<int> camSize;
		float fov;
		float field_492_0x1ec;
		float camAspect;
		float zNear;
		float zFar;
	};

	class Scn;

	class ScnObjCam {
	   public:
		INSERT_PADDING_BYTES(0x8)
		Scn* ScnPtr {};
		INSERT_PADDING_BYTES(0x70)
		AttrTransform* AttrTransformPtr {};

		void setWorldMatrix(const mm::Mat44& matrix);
		void setViewMatrix(const mm::Mat44& matrix);
		void updateFovNearFar();
	};

	enum class SCNCAM : std::uint32_t {};
	enum class SCNPRIO_CB : std::uint32_t {
		debug = 0xe
	};
	enum class SCNCB : std::uint32_t {};

	class IScnRender {
	};

	class ScnSetup {

	};

	class ScnDebug : public ml::IScnRender {

	};

	class Scn {
	   public:
		INSERT_PADDING_BYTES(0x38)
		ml::ScnDebug* scnDebug;
		void* somePointer;

		//static Scn* create(ScnSetup const*);

		// will this work? lol
		ScnObjCam* getCam(int index);

		void addRenderCB(IScnRender* scnRender, SCNPRIO_CB prio, bool param_3, bool param_4);

		void callbackToListener(SCNCB callback);

		void renderStop();

		void setDebDraw(SCNCAM cam, SCNPRIO_CB prio);
		bool enableScnDebug(bool nuts);
	};

	class DrMan {
	   public:
#if XENOMODS_CODENAME(bfsw)
		INSERT_PADDING_BYTES(0x3958);
#else
		INSERT_PADDING_BYTES(0x1D7C);
#endif
		bool hideChara;
		bool hideMap;

		static DrMan* get();
	};

	class ScnRenderDrSysParmAcc {
	   public:
		DrMan* drMan;
		void* drPixlPostParm;
		int unk;

		ScnRenderDrSysParmAcc();

		// there's so, so many of these.
		// i'm only going to add the interesting ones unless the others are needed

		bool isBloomOn() const;
		void setBloom(bool);

		bool isToneMap() const;
		void setToneMap(bool);

		bool isMotBlur() const;
		void setMotBlur(bool);
		void setMotBlurOverride(bool);

		bool isDOF() const;
		void setDOF(bool);
		void setDOFOverride(bool);

		// doesn't seem to do anything?
		bool isDispChara() const;
		void setDispChara(bool);

		// doesn't seem to do anything?
		bool isDispGlobalIBLTex() const;
		void setDispGlobalIBLTex(bool);
#if XENOMODS_CODENAME(bfsw)
		bool isDispMap() const;
		void setDispMap(bool);

		bool isDispMirrorBall() const;
		void setDispMirrorBall(bool);

		bool isDispMpf() const;
		void setDispMpf(bool);
#endif
		bool isDispReduction() const;
		void setDispReduction(bool);

		// doesn't seem to do anything on its own
		void setColorFilterOverride(bool);
		void setColorFilterNum(int);
		void setColorFilterFarNum(int);
		void setColorFilterFrm(float);

		// doesn't seem to do anything?
		bool isDefferedColor() const;
		void setDefferedColor(bool);

		bool isCharDirShadow() const;
		void setCharDirShadow(bool);

		void setClip(bool); // no accompanying, doesn't seem to do anything?

		void setFogSkip(bool); // no accompanying

		void setToonAmbient(bool); // no accompanying

		bool isAA() const;
		void setAA(bool);

		void setAppBlur(bool); // no accompanying
		void setAutoReduction(bool); // no accompanying

		bool isGodRay() const;
		void setGodRay(bool);

		void setShadowStr(float);
		void setSpecularStr(float);
	};

} // namespace ml
