#include "stdafx.h"
#include "CoreApp.h"
#include "Context.h"
#include "Test01.h"
//=============================================================================
#if defined(_MSC_VER)
#	pragma comment( lib, "3rdparty.lib" )
#endif
//=============================================================================
bool ShouldCloseApp(const Context& context)
{
	return app::isExit || context.ShouldClose();
}
//=============================================================================
int main(
	[[maybe_unused]] int   argc,
	[[maybe_unused]] char* argv[])
{
	Context context;

	HybridRenderingEngine
	nuEngine
		dw - sample - framework
		limitless - engine
		https ://zhuanlan.zhihu.com/p/357265599
https://www.youtube.com/@pascl62/videos

	Test01 game;

	if (context.Init(1600, 900, "Game") 
		&& game.Init())
	{
		while (!ShouldCloseApp(context))
		{
			context.BeginFrame();

			if (context.IsResize())
			{
				glViewport(0, 0, context.GetWidth(), context.GetHeight());
			}

			// Update и FixedUpdate
			// это не работает
			//float accumulator = 0.0f;
			//float fixedDeltaTime = 1.0f / 60.0f;
			//while (accumulator < deltaTime)
			//{
			//	FixedUpdate(fixedDeltaTime);
			//	accumulator += fixedDeltaTime;
			//}
			game.Frame(context.GetDeltaTime());

			context.BeginImgui();
			game.DrawImGui(context.GetDeltaTime());
			context.EndImgui();

			context.EndFrame();
		}
	}
	game.Close();
	context.Close();
}
//=============================================================================