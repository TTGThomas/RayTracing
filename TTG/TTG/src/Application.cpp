#include "Application.h"

namespace TTG
{
	void Application::Init()
	{
		if (!glfwInit())
			return;

		const char* glslVersion = "#version 330";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		m_window = glfwCreateWindow(800, 600, "TTG", nullptr, nullptr);
		if (!m_window)
			return;

		glfwMakeContextCurrent(m_window);
		glfwSwapInterval(0);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			printf("glad is not loaded!\n");
            assert(false);
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		m_io = &ImGui::GetIO();
		m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		m_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(m_window, true);
		ImGui_ImplOpenGL3_Init(glslVersion);

		m_io->Fonts->AddFontDefault();
	}

	void Application::Cleanup()
	{
		for (int i = 0; i < m_layers.size(); i++)
		{
			m_layers[i]->OnDetach();
		}

		m_layers.clear();

		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void Application::Run()
	{
		for (std::shared_ptr<Layer> layer : m_layers)
		{
			layer->OnAttach(m_window);
		}

		while (!glfwWindowShouldClose(m_window))
		{
			// new frame
			glfwPollEvents();

			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			// run clients tick
			for (int i = 0; i < m_layers.size(); i++)
			{
				m_layers[i]->OnTick();
				m_layers[i]->OnUIRender(i);
			}

			// render
			ImGui::Render();
			int width, height;
			glfwGetFramebufferSize(m_window, &width, &height);
			glViewport(0, 0, width, height);

			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}

			glfwSwapBuffers(m_window);
		}
	}

	bool Application::IsProgramRunning()
	{
		return !glfwWindowShouldClose(m_window);
	}
}
