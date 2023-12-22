#include "Editor.h"

void Editor::BindScene(Scene* scene)
{
	m_activatedScene = scene;
}

void Editor::BindRenderer(Renderer* renderer)
{
	m_activatedRenderer = renderer;
}

void Editor::ShowWindow()
{
    if (!m_editorOpen)
        return;

    ImGui::Begin("Editor", &m_editorOpen);

    if (ImGui::CollapsingHeader("Statistics"))
    {
        ImGui::Indent();
        ImGui::Text("last render: %fms", m_renderTimer.Ms());
        ImGui::Text("last frame: %fms", m_frameTimer.Ms());
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Switches"))
    {
        ImGui::Indent();
        ImGui::Checkbox("Accumulate", m_activatedRenderer->GetAccumulate());
        if (!(*m_activatedRenderer->GetAccumulate()))
            m_activatedRenderer->ResetFrameIndex();
        ImGui::Checkbox("Sky light emission", m_activatedRenderer->GetSkyLightSwitch());
        ImGui::Checkbox("User Dist Blur", m_activatedRenderer->GetDistBlurSwitch());
        if (*m_activatedRenderer->GetDistBlurSwitch())
            ImGui::DragFloat("Dist Blur", m_activatedRenderer->GetDistBlur(), 0.01f, 0.0f, FLT_MAX);
        ImGui::Unindent();
    }

    if (ImGui::Button("Reset"))
        m_activatedRenderer->ResetFrameIndex();

    
    for (int i = 0; i < m_activatedScene->objects.size(); i++)
    {
        ImGui::PushID(i);
        HitType& type = m_activatedScene->objects[i]->m_type;
        if (type == HitType::TRIANGLE)
        {
            ImGui::SeparatorText("Triangle");
        }
        else if (type == HitType::SPHERE)
        {
            ImGui::SeparatorText("Sphere");
        }
        else if (type == HitType::PLANE)
        {
            ImGui::SeparatorText("Plane");
        }
        ImGui::PopID();
    }

    ImGui::End();
}
