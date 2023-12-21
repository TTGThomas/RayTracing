#pragma once

#include "ImGuiOpenGL.h"
#include "Layer.h"

#include <vector>
#include <memory>

namespace TTG
{
	class Application
	{
	public:
		void Init();
		void Cleanup();

		void Run();

		template<typename T>
		void PushLayer()
		{
			static_assert(std::is_base_of<Layer, T>::value, "Layer pushed in is not a Layer");
			m_layers.emplace_back(std::make_shared<T>());
		}

		bool IsProgramRunning();

		void Stop()
		{
			glfwSetWindowShouldClose(m_window, true);
		}

		GLFWwindow* GetInstance() { return m_window; };
		ImGuiIO* GetIO() { return m_io; };
	private:
		GLFWwindow* m_window;
		ImGuiIO* m_io;

		std::vector<std::shared_ptr<Layer>> m_layers;
	};
}

TTG::Application* createApplication();