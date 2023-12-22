#pragma once

#include "ImGuiOpenGL.h"
#include "Utils/Timer.h"

#include "Renderer.h"
#include "Scene.h"

#include <string>

class Editor
{
public:
	void BindRenderer(Renderer* renderer);
	void BindScene(Scene* scene);

	void ShowWindow();

	void StartRenderTimer() { m_renderTimer.Start(); }
	void StopRenderTimer() { m_renderTimer.Stop(); }

	void StartFrameTimer() { m_frameTimer.Start(); }
	void StopFrameTimer() { m_frameTimer.Stop(); }

	float GetFrameS() { return m_frameTimer.Seconds(); }
	float GetRenderS() { return m_renderTimer.Seconds(); }
private:
	TTG::Timer m_renderTimer;
	TTG::Timer m_frameTimer;

	Scene* m_activatedScene = nullptr;
	Renderer* m_activatedRenderer = nullptr;

	bool m_editorOpen = true;
};

