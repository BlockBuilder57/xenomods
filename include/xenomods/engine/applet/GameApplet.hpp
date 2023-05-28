#pragma once

#if XENOMODS_CODENAME(bfsw)
namespace applet {
	#define DEFINE_APPLET(T)       \
		struct T {                 \
		   public:                 \
			static void execute(); \
		};

	DEFINE_APPLET(GameTitle)
	DEFINE_APPLET(GameTitleMelia)
	DEFINE_APPLET(GameField)
	DEFINE_APPLET(GameMelia)
	DEFINE_APPLET(MessageSample)
	DEFINE_APPLET(SampleApplet)
	DEFINE_APPLET(SevViewer)
	DEFINE_APPLET(TerrainApplet)
	DEFINE_APPLET(HayanoAppletLauncher)
	DEFINE_APPLET(HayanoApplet)
	DEFINE_APPLET(HayanoAppletDevelopRpc)
	DEFINE_APPLET(HayanoAppletLayer)
	DEFINE_APPLET(HayanoAppletLayout)
	DEFINE_APPLET(HayanoAppletSocket)
	DEFINE_APPLET(InabaAnmPac)
	DEFINE_APPLET(InabaColi)
	DEFINE_APPLET(InabaEfbTest)
	DEFINE_APPLET(InabaLayImg)
	DEFINE_APPLET(InabaLayFont)
	DEFINE_APPLET(InabaLayGrp)
	DEFINE_APPLET(InabaMovie)
	DEFINE_APPLET(InabaNavi)
	DEFINE_APPLET(InabaOffScreen)
	DEFINE_APPLET(KawashimaApplet)
	DEFINE_APPLET(OohataTestApplet)
	DEFINE_APPLET(OohataUITest)

	#undef DEFINE_MANAGER
} // namespace applet
#endif