#include "stdafx.h"
#include "Context.h"
#include "RoguelikeGameApp.h"
#include "0001SimpleDemo.h"
//=============================================================================
#if defined(_MSC_VER)
#	pragma comment( lib, "3rdparty.lib" )
#endif
//=============================================================================
bool ExitRequested(const Context& context)
{
	extern bool IsExitApp;
	return IsExitApp || context.ExitRequested();
}
//=============================================================================
ContextCreateInfo GetContextCreateInfo()
{
	ContextCreateInfo contextInfo{};

	return contextInfo;
}
//=============================================================================
#if defined(__EMSCRIPTEN__)
RoguelikeGameApp* game{ nullptr };
void runFrame(void* arg)
{
	auto context = GetContext();

	context->BeginFrame();

	if (context->IsResize())
	{
		game->Resize(context->GetWidth(), context->GetHeight());
	}

	const auto deltaTime = context->GetDeltaTime();

	game->Update(deltaTime);
	game->Draw(deltaTime);

	context->BeginImgui();
	game->DrawImGui(deltaTime);
	context->EndImgui();

	context->EndFrame();
}
#endif
//=============================================================================
int main(
	[[maybe_unused]] int   argc,
	[[maybe_unused]] char* argv[])
{
	Context context;

	stratusgfx

	//RoguelikeGameApp game(context);
	_0001SimpleDemo game(context);

	if (context.Init(GetContextCreateInfo())
		&& game.Init())
	{
#if defined(__EMSCRIPTEN__)
		game = &game;
		emscripten_set_main_loop_arg(runFrame, this, 0, 1);
#else
		while (!ExitRequested(context))
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
#endif
	game.Close();
	context.Close();
}
//=============================================================================