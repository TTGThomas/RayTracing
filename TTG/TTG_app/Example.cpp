#pragma once

#include <glm/gtc/type_ptr.hpp>

#include "Application.h"
#include "EntryPoint.h"

#include "ImGuiOpenGL.h"
#include "Graphics/Texture.h"
#include "Utils/Timer.h"
#include "Objects/Camera.h"

#include "Materials/Bsdf.h"
#include "Materials/BasicBsdf.h"
#include "Materials/TestBsdf.h"
#include "Materials/GlassBsdf.h"

#include "Renderer.h"
#include "Hittables/Scene.h"
#include "shapes/Triangle.h"

#include <stdio.h>
#include <string>

class ExampleLayer : public Layer
{
public:
    ExampleLayer()
    : m_renderer(&m_scene, &m_camera)
    {
        m_camera.m_position = glm::vec3(0.03f, 0.0f, 1.8f);
        m_camera.m_speed = 1.0f;
        
        {
            // BSDF 0
#define Basic 0
            m_scene.bsdf.emplace_back(std::make_unique<BasicBsdf>());
        }
        
        {
            // BSDF 1
#define Test 1
			m_scene.bsdf.emplace_back(std::make_unique<TestBsdf>());
        }
        
        {
            // BSDF 2
#define Glass 2
			m_scene.bsdf.emplace_back(std::make_unique<GlassBsdf>());
        }
        
        switch (0)
        {
            case 0:
                cornellBox();
                break;
            case 1:
                test();
                break;
            case 2:
                glass();
                break;
            case 3:
                Balls();
                break;
        }
    }
    
    virtual void OnAttach(GLFWwindow* handle) override
    {
        m_handle = handle;
    }
    
    virtual void OnDetach() override
    {
    }
    
    virtual void OnUIRender(int id) override
    {
        m_tick.Start();
        mainViewport(id);
        editor();
        m_tick.Stop();
    }
    
    virtual void OnTick() override
    {
    }
    
private:
    void mainViewport(int id)
    {
        if (!m_viewPortOpen)
            return;
        
        if (docking)
        {
            if (id == 0)
            {
                ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
            }
        }
        
        ImGui::Begin("main viewport", &m_viewPortOpen, ImGuiWindowFlags_MenuBar);
        
        windowSetup();
        
		m_config = { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 60.0f, 0.1f };
        if (m_camera.Update(m_config))
			m_renderer.ResetFrameIndex();
        render();
        
        ImGui::End();
    }
    
    void editor()
    {
        if (!m_editorOpen)
            return;
        
        ImGui::Begin("Editor", &m_editorOpen);
        
		for (int i = 0; i < m_scene.objects.size(); i++)
		{
			ImGui::PushID(i);
			HitType& type = m_scene.objects[i].m_type;
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
			ImGui::PopID;
		}
        
        ImGui::SeparatorText("editors");
        ImGui::Text("this is the Editor");
        ImGui::Text("last render: %fms", m_t.Ms());
        ImGui::Text("last tick: %fms", m_tick.Ms());
        if (ImGui::Button("reset"))
            m_renderer.ResetFrameIndex();
        ImGui::Checkbox("accumulate", m_renderer.GetAccumulate());
        if (!(*m_renderer.GetAccumulate()))
            m_renderer.ResetFrameIndex();
        
        ImGui::Checkbox("slow random", m_renderer.GetSlowRandom());
        ImGui::Checkbox("sky light emission", m_renderer.GetSkyLightSwitch());
        ImGui::Checkbox("User Dist Blur", m_renderer.GetDistBlurSwitch());
        if (*m_renderer.GetDistBlurSwitch())
            ImGui::DragFloat("Dist Blur", m_renderer.GetDistBlur(), 0.01f, 0.0f, FLT_MAX);
        
        ImGui::End();
    }

	void render()
	{
		m_t.Start();

		if (m_renderer.OnResize())
		{
			m_camera.ReCalculateRayDirections(m_config);
		}
		m_renderer.Render(m_config);

		m_t.Stop();

		unsigned char* imageData = m_renderer.GetImageData();
		TTG::Texture& finalImage = m_renderer.GetFinalImage();
		if (imageData)
		{
			finalImage.LoadTextureFromData(imageData, 4, m_renderer.GetWidth(), m_renderer.GetHeight());
			ImGui::Image(finalImage.GetDescriptorSet(), { (float)m_renderer.GetWidth(), (float)m_renderer.GetHeight()});
		}
	}

	void windowSetup()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("tools"))
			{
				if (ImGui::MenuItem("dockSpace"))
				{
					docking = !docking;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	void test()
	{
		{
			Material& mat0 = m_scene.mat.emplace_back();
			mat0.m_albedo = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
		}

		{
			Material& mat1 = m_scene.mat.emplace_back();
			mat1.m_albedo = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
			mat1.m_emissionColor = mat1.m_albedo;
			mat1.m_emissionPower = 20.0f;
		}

		{
			Material& mat2 = m_scene.mat.emplace_back();
			mat2.m_albedo = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
			mat2.m_roughness = 1.0f;
		}
		*m_renderer.GetSkyLightSwitch() = true;

		{
			constexpr float holeSize = 0.1f;
			constexpr float planeSize = 2.0f;

			constexpr float height = (planeSize / 2.0f) - (holeSize / 2.0f);
			constexpr float pY = (holeSize + planeSize) / 4.0f;
			addSquare(glm::vec3(0.0f, pY, -1.0f), glm::vec3(planeSize, height, 0.0f), 0, false);
			addSquare(glm::vec3(0.0f, -pY, -1.0f), glm::vec3(planeSize, height, 0.0f), 0, false);

			addSquare(glm::vec3(0.0f, pY, -1.0f), glm::vec3(planeSize, height, 0.0f), 0, true);
			addSquare(glm::vec3(0.0f, -pY, -1.0f), glm::vec3(planeSize, height, 0.0f), 0, true);

			constexpr float width = (planeSize - holeSize) / 2.0f;
			constexpr float pX = (holeSize + planeSize) / 4.0f;
			addSquare(glm::vec3(pX, 0.0f, -1.0f), glm::vec3(width, holeSize, 0.0f), 0, false);
			addSquare(glm::vec3(-pX, 0.0f, -1.0f), glm::vec3(width, holeSize, 0.0f), 0, false);

			addSquare(glm::vec3(pX, 0.0f, -1.0f), glm::vec3(width, holeSize, 0.0f), 0, true);
			addSquare(glm::vec3(-pX, 0.0f, -1.0f), glm::vec3(width, holeSize, 0.0f), 0, true);
		}

		addSquare(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(2.0f), 2, false);

		addSquare(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.0f), 1, true);
	}

	void cornellBox()
	{
		{
			// white
			Material& mat0 = m_scene.mat.emplace_back();
			mat0.m_albedo = glm::vec4(1.0f);
		}

		{
			// red
			Material& mat1 = m_scene.mat.emplace_back();
			mat1.m_albedo = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}

		{
			// green
			Material& mat2 = m_scene.mat.emplace_back();
			mat2.m_albedo = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		}

		{
			// light
			Material& mat3 = m_scene.mat.emplace_back();
			mat3.m_emissionColor = glm::vec4(1.0f);
			mat3.m_albedo = glm::vec4(1.0f);
			mat3.m_emissionPower = 10.0f;
		}

		//scene.spheres.emplace_back();

		Shapes::Triangle tri;

		// light
		{
			tri.SetPointA(-0.5f, 0.9f, -0.75f);
			tri.SetPointB(-0.5f, 0.9f, -0.25f);
			tri.SetPointC(0.5f, 0.9f, -0.25f);
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(3);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(-0.5f, 0.9f, -0.75f);
			tri.SetPointB(0.5f, 0.9f, -0.75f);
			tri.SetPointC(0.5f, 0.9f, -0.25f);
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(3);
			tri.AddToScene(&m_scene);
		}


		// bottom square
		{
			tri.SetPointA(-1.0f, -1.0f, -2.0f);
			tri.SetPointB(-1.0f, -1.0f, 0.0f);
			tri.SetPointC(1.0f, -1.0f, 0.0f);
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(-1.0f, -1.0f, -2.0f);
			tri.SetPointB(1.0f, -1.0f, -2.0f);
			tri.SetPointC(1.0f, -1.0f, 0.0f);
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		// top square
		{
			tri.SetPointA(-1.0f, 1.0f, -2.0f);
			tri.SetPointB(-1.0f, 1.0f, 0.0f);
			tri.SetPointC(1.0f, 1.0f, 0.0f);
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(-1.0f, 1.0f, -2.0f);
			tri.SetPointB(1.0f, 1.0f, -2.0f);
			tri.SetPointC(1.0f, 1.0f, 0.0f);
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		// left square
		{
			tri.SetPointA(-1.0f, 1.0f, 0.0f);
			tri.SetPointB(-1.0f, -1.0f, 0.0f);
			tri.SetPointC(-1.0f, -1.0f, -2.0f);
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(1);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(-1.0f, 1.0f, 0.0f);
			tri.SetPointB(-1.0f, 1.0f, -2.0f);
			tri.SetPointC(-1.0f, -1.0f, -2.0f);
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(1);
			tri.AddToScene(&m_scene);
		}


		// right square
		{
			tri.SetPointA(1.0f, 1.0f, 0.0f);
			tri.SetPointB(1.0f, -1.0f, 0.0f);
			tri.SetPointC(1.0f, -1.0f, -2.0f);
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(2);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(1.0f, 1.0f, 0.0f);
			tri.SetPointB(1.0f, 1.0f, -2.0f);
			tri.SetPointC(1.0f, -1.0f, -2.0f);
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(2);
			tri.AddToScene(&m_scene);
		}


		// forward square
		{
			tri.SetPointA(-1.0f, 1.0f, -2.0f);
			tri.SetPointB(-1.0f, -1.0f, -2.0f);
			tri.SetPointC(1.0f, -1.0f, -2.0f);
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(-1.0f, 1.0f, -2.0f);
			tri.SetPointB(1.0f, 1.0f, -2.0f);
			tri.SetPointC(1.0f, -1.0f, -2.0f);
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		// objects

		{
			Material& mat4 = m_scene.mat.emplace_back();
			mat4.m_albedo = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
            mat4.m_emissionColor = mat4.m_albedo;
            mat4.m_emissionPower = 2.0f;
		}
        {
            Material& mat5 = m_scene.mat.emplace_back();
            mat5.m_albedo = glm::vec4(1.0f);
            mat5.m_roughness = 0.0f;
        }

		Sphere& sphere0 = m_scene.objects.emplace_back();
		sphere0.m_bsdfIndex = Basic;
		sphere0.m_matIndex = 4;
		sphere0.m_position = glm::vec3(0.4f, -0.6f, -0.8f);
		sphere0.m_radius = 0.4f;

		Sphere& sphere1 = m_scene.objects.emplace_back();
		sphere1.m_bsdfIndex = Basic;
		sphere1.m_matIndex = 5;
		sphere1.m_position = glm::vec3(-0.4f, -0.6f, -1.0f);
		sphere1.m_radius = 0.4f;
	}

	void glass()
	{
		{
			Material& mat0 = m_scene.mat.emplace_back();
			mat0.m_albedo = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
		}
		Shapes::Triangle tri;
		tri.SetPointA(0.0f, 6.0f, -2.0f);
		tri.SetPointB(-3.0f, 0.0f, -2.0f);
		tri.SetPointC(3.0f, 0.0f, -2.0f);
		tri.FlipSequence();
		tri.ComputeNormal();
		tri.SetMaterialIndex(0);
		tri.SetBsdfIndex(Glass);
		tri.AddToScene(&m_scene);

		tri.SetPointA(0.0f, 6.0f, -2.01f);
		tri.SetPointB(-3.0f, 0.0f, -2.01f);
		tri.SetPointC(3.0f, 0.0f, -2.01f);
		tri.FlipSequence();
		tri.ComputeNormal();
		tri.AddToScene(&m_scene);

		Sphere& sphere = m_scene.objects.emplace_back();
		sphere.m_bsdfIndex = Glass;
		sphere.m_matIndex = 0;
		sphere.m_position = glm::vec3(2.0f, 2.0f, -3.5f);

		{
			Material& mat1 = m_scene.mat.emplace_back();
			mat1.m_albedo = glm::vec4(1.0f, 0.0f, 1.0f, 1.0);
		}

		Sphere& sphere0 = m_scene.objects.emplace_back();
		sphere0.m_position = glm::vec3(0.0f, 2.0f, -3.5f);
		sphere0.m_radius = 1.0f;
		sphere0.m_bsdfIndex = Basic;
		sphere0.m_matIndex = 1;

		*m_renderer.GetSkyLightSwitch() = true;
	}
    
    void Balls()
    {
        m_camera.m_position = glm::vec3(0.0f, -10.0f, 1.8f);
        m_camera.m_rotation = glm::vec3(45.0f, -90.0f, 0.0f);
        
        *m_renderer.GetSkyLightSwitch() = true;
        {
            Material& mat0 = m_scene.mat.emplace_back();
            mat0.m_albedo = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
        }
        {
            Material& mat1 = m_scene.mat.emplace_back();
            mat1.m_albedo = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
        }
        
        for (float x = -5.0f; x <= 5.0f; x += 2.0f)
        {
            for (float z = -15.0f; z <= -5.0f; z += 2.0f)
            {
                Sphere& sphere0 = m_scene.objects.emplace_back();
                sphere0.m_position = glm::vec3(x, -3.0f, z);
                sphere0.m_radius = 1.0f;
                sphere0.m_bsdfIndex = Basic;
                sphere0.m_matIndex = 0;
            }
        }
        
        Plane& plane0 = m_scene.objects.emplace_back();
        plane0.m_position = glm::vec3(0.0f, -4.0f, 0.0f);
        plane0.m_normal = glm::vec3(0.0f, 1.0f, 0.0f);
        plane0.m_bsdfIndex = Basic;
        plane0.m_matIndex = 1;
    }

	void addSquare(glm::vec3 pos, glm::vec3 scale, int mat, bool flipFace)
	{
		glm::vec3 radius = scale / 2.0f;

		Shapes::Triangle tri;
		tri.SetPointA(pos + glm::vec3(-radius.x, radius.y, 0.0f));
		tri.SetPointB(pos + glm::vec3(-radius.x, -radius.y, 0.0f));
		tri.SetPointC(pos + glm::vec3(radius.x, -radius.y, 0.0f));
		if (!flipFace) tri.FlipSequence();
		tri.ComputeNormal();
		tri.SetMaterialIndex(mat);
		tri.AddToScene(&m_scene);

		tri.SetPointA(pos + glm::vec3(-radius.x, radius.y, 0.0f));
		tri.SetPointB(pos + glm::vec3(radius.x, radius.y, 0.0f));
		tri.SetPointC(pos + glm::vec3(radius.x, -radius.y, 0.0f));
		if (flipFace) tri.FlipSequence();
		tri.ComputeNormal();
		tri.SetMaterialIndex(mat);
		tri.AddToScene(&m_scene);
	}

	void addCube(glm::vec3 pos, glm::vec3 scale)
	{
		Shapes::Triangle tri;
		// bottom square
		{
			tri.SetPointA(-1.0f, -1.0f, -2.0f);
			tri.SetPointB(pos);
			tri.SetPointC(1.0f, -1.0f, 0.0f);
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(-1.0f, -1.0f, -2.0f);
			tri.SetPointB(1.0f, -1.0f, -2.0f);
			tri.SetPointC(1.0f, -1.0f, 0.0f);
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		// top square
		{
			tri.SetPointA(-1.0f, 1.0f, -2.0f);
			tri.SetPointB(-1.0f, 1.0f, 0.0f);
			tri.SetPointC(1.0f, 1.0f, 0.0f);
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(-1.0f, 1.0f, -2.0f);
			tri.SetPointB(1.0f, 1.0f, -2.0f);
			tri.SetPointC(1.0f, 1.0f, 0.0f);
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		// left square
		{
			tri.SetPointA(-1.0f, 1.0f, 0.0f);
			tri.SetPointB(-1.0f, -1.0f, 0.0f);
			tri.SetPointC(-1.0f, -1.0f, -2.0f);
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(1);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(-1.0f, 1.0f, 0.0f);
			tri.SetPointB(-1.0f, 1.0f, -2.0f);
			tri.SetPointC(-1.0f, -1.0f, -2.0f);
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(1);
			tri.AddToScene(&m_scene);
		}


		// right square
		{
			tri.SetPointA(1.0f, 1.0f, 0.0f);
			tri.SetPointB(1.0f, -1.0f, 0.0f);
			tri.SetPointC(1.0f, -1.0f, -2.0f);
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(2);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(1.0f, 1.0f, 0.0f);
			tri.SetPointB(1.0f, 1.0f, -2.0f);
			tri.SetPointC(1.0f, -1.0f, -2.0f);
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(2);
			tri.AddToScene(&m_scene);
		}


		// forward square
		{
			tri.SetPointA(-1.0f, 1.0f, -2.0f);
			tri.SetPointB(-1.0f, -1.0f, -2.0f);
			tri.SetPointC(1.0f, -1.0f, -2.0f);
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(-1.0f, 1.0f, -2.0f);
			tri.SetPointB(1.0f, 1.0f, -2.0f);
			tri.SetPointC(1.0f, -1.0f, -2.0f);
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}
	}
private:
	TTG::Timer m_t;
	TTG::Timer m_tick;
	TTG::Camera m_camera;
	TTG::Config m_config;

	bool m_viewPortOpen = true;
	bool m_editorOpen = true;

	static bool docking;
	Renderer m_renderer;
	Scene m_scene;

	GLFWwindow* m_handle;
};

bool ExampleLayer::docking = true;

TTG::Application* createApplication()
{
	TTG::Application* app = new TTG::Application;
	app->PushLayer<ExampleLayer>();
	return app;
}
