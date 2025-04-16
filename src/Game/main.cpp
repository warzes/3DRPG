#include "stdafx.h"
#include "Context.h"
#include "RoguelikeGameApp.h"
//=============================================================================
#if defined(_MSC_VER)
#	pragma comment( lib, "3rdparty.lib" )
#endif
//=============================================================================
bool ShouldCloseApp(const Context& context)
{
	extern bool IsExitApp;
	return IsExitApp || context.ShouldClose();
}
//=============================================================================
ContextCreateInfo GetContextCreateInfo()
{
	ContextCreateInfo contextInfo{};

	return contextInfo;
}
//=============================================================================
int main(
	[[maybe_unused]] int   argc,
	[[maybe_unused]] char* argv[])
{
	Context context;

	RoguelikeGameApp game(context);

	if (context.Init(GetContextCreateInfo())
		&& game.Init())
	{
		while (!ShouldCloseApp(context))
		{
			context.BeginFrame();

			if (context.IsResize())
			{
				game.Resize(context.GetWidth(), context.GetHeight());
			}

			const auto deltaTime = context.GetDeltaTime();

			game.Update(deltaTime);
			// Update и FixedUpdate
			// это не работает
			//float accumulator = 0.0f;
			//float fixedDeltaTime = 1.0f / 60.0f;
			//while (accumulator < deltaTime)
			//{
			//	FixedUpdate(fixedDeltaTime);
			//	accumulator += fixedDeltaTime;
			//}
			game.Draw(deltaTime);

			context.BeginImgui();
			game.DrawImGui(deltaTime);
			context.EndImgui();

			context.EndFrame();
		}
	}
	game.Close();
	context.Close();
}
//=============================================================================