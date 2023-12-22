#include "Editor.h"

#include <glm/gtc/type_ptr.hpp>

#include "shapes/Triangle.h"

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

    if (ImGui::CollapsingHeader("Objects"))
    {
        ImGui::Indent();
        for (int i = 0; i < m_activatedScene->objects.size(); i++)
        {
            ImGui::PushID(i);
            HitType& type = m_activatedScene->objects[i]->m_type;
            if (type == HitType::TRIANGLE)
            {
                if (ImGui::CollapsingHeader("Triangle"))
                {
                    ImGui::Indent();
                    ::Triangle* tri = dynamic_cast<::Triangle*>(m_activatedScene->objects[i].get());
                    
                    ImGui::DragFloat3("PointA", &tri->m_posA.x, 0.1f);
                    ImGui::DragFloat3("PointB", &tri->m_posB.x, 0.1f);
                    ImGui::DragFloat3("PointC", &tri->m_posC.x, 0.1f);

                    if (ImGui::CollapsingHeader("Normals"))
                    {
                        ImGui::Indent();

                        Shapes::Triangle shape;
                        shape.Init(*tri);

                        if (ImGui::Button("Compute Normal")) shape.ComputeNormal();
                        if (ImGui::Button("Flip Normal")) shape.FlipNormal();
                        if (ImGui::Button("Flip Sequence")) shape.FlipSequence();

                        *tri = shape.GetData();
                        ImGui::Unindent();
                    }

                    ImGui::DragInt("Material Index", &tri->m_matIndex, 0.1f, 0, (int)m_activatedScene->mat.size() - 1);
                    ImGui::DragInt("Bsdf Index", &tri->m_bsdfIndex, 0.1f, 0, (int)m_activatedScene->bsdf.size() - 1);
                    ImGui::Checkbox("Visible", &tri->m_visible);
                    ImGui::Unindent();
                }
            }
            else if (type == HitType::SPHERE)
            {
                if (ImGui::CollapsingHeader("Sphere"))
                {
                    ImGui::Indent();
                    ::Sphere* sphere = dynamic_cast<::Sphere*>(m_activatedScene->objects[i].get());

                    ImGui::DragFloat3("Position", &sphere->m_position.x, 0.1f);
                    ImGui::DragFloat("Radius", &sphere->m_radius, 0.05f);

                    ImGui::DragInt("Material Index", &sphere->m_matIndex, 0.1f, 0, (int)m_activatedScene->mat.size() - 1);
                    ImGui::DragInt("Bsdf Index", &sphere->m_bsdfIndex, 0.1f, 0, (int)m_activatedScene->bsdf.size() - 1);
                    ImGui::Checkbox("Visible", &sphere->m_visible);
                    ImGui::Unindent();
                }
            }
            else if (type == HitType::PLANE)
            {
                if (ImGui::CollapsingHeader("Plane"))
                {
                    ImGui::Indent();
                    ::Plane* plane = dynamic_cast<::Plane*>(m_activatedScene->objects[i].get());
                    
                    ImGui::DragFloat3("Position", &plane->m_position.x, 0.1f);
                    ImGui::DragFloat3("Normal", &plane->m_normal.x, 0.1f);

                    ImGui::DragInt("Material Index", &plane->m_matIndex, 0.1f, 0, (int)m_activatedScene->mat.size() - 1);
                    ImGui::DragInt("Bsdf Index", &plane->m_bsdfIndex, 0.1f, 0, (int)m_activatedScene->bsdf.size() - 1);
                    ImGui::Checkbox("Visible", &plane->m_visible);
                    ImGui::Unindent();
                }
            }
            ImGui::PopID();
        }
        ImGui::Unindent();
    }

    ImGui::End();
}
