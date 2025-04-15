#pragma once

namespace demo
{
	void Print(const std::string& message);
	void Warning(const std::string& message);
	void Error(const std::string& message);
	void Fatal(const std::string& message);

	void Exit();

	float GetFrameDeltaTime();

	struct ContextCreateInfo final
	{

	};

	class Context final
	{
	public:
		Context(const ContextCreateInfo& createInfo);
		~Context();

		bool ShouldClose() const;
		void StartFrame();
		void EndFrame();
	};
}
