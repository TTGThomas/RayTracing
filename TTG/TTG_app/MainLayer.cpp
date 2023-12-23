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
#include "Editor.h"

#include "Scene.h"

#include "shapes/Triangle.h"
#include "shapes/Sphere.h"
#include "shapes/Plane.h"

#include <stdio.h>
#include <string>

class ExampleLayer : public Layer
{
public:
    ExampleLayer()
    : m_renderer(&m_scene, &m_camera)
    {
		m_editor.BindScene(&m_scene);
		m_editor.BindRenderer(&m_renderer);

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
		m_editor.StopFrameTimer();
		m_editor.StartFrameTimer();
        mainViewport(id);
		m_editor.ShowWindow();
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
        
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("tools"))
			{
				if (ImGui::MenuItem("dockSpace"))
					docking = !docking;
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
        
		m_config = { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 60.0f, m_editor.GetFrameS()};
        if (m_camera.Update(m_config))
			m_renderer.ResetFrameIndex();
        render();
        
        ImGui::End();
    }

	void render()
	{
		if (m_renderer.OnResize())
			m_camera.ResizeBuffer(m_config);
		m_editor.StartRenderTimer();
		m_renderer.Render(m_config);
		m_editor.StopRenderTimer();

		unsigned char* imageData = m_renderer.GetImageData();
		TTG::Texture& finalImage = m_renderer.GetFinalImage();
		if (imageData)
		{
			finalImage.LoadTextureFromData(imageData, 4, m_renderer.GetWidth(), m_renderer.GetHeight());
			ImGui::Image(finalImage.GetDescriptorSet(), { (float)m_renderer.GetWidth(), (float)m_renderer.GetHeight()});
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
			tri.SetPointA(glm::vec3(-0.5f, 0.9f, -0.75f));
			tri.SetPointB(glm::vec3(-0.5f, 0.9f, -0.25f));
			tri.SetPointC(glm::vec3(0.5f, 0.9f, -0.25f));
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(3);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(glm::vec3(-0.5f, 0.9f, -0.75f));
			tri.SetPointB(glm::vec3(0.5f, 0.9f, -0.75f));
			tri.SetPointC(glm::vec3(0.5f, 0.9f, -0.25f));
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(3);
			tri.AddToScene(&m_scene);
		}


		// bottom square
		{
			tri.SetPointA(glm::vec3(-1.0f, -1.0f, -2.0f));
			tri.SetPointB(glm::vec3(-1.0f, -1.0f, 0.0f));
			tri.SetPointC(glm::vec3(1.0f, -1.0f, 0.0f));
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(glm::vec3(-1.0f, -1.0f, -2.0f));
			tri.SetPointB(glm::vec3(1.0f, -1.0f, -2.0f));
			tri.SetPointC(glm::vec3(1.0f, -1.0f, 0.0f));
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		// top square
		{
			tri.SetPointA(glm::vec3(-1.0f, 1.0f, -2.0f));
			tri.SetPointB(glm::vec3(-1.0f, 1.0f, 0.0f));
			tri.SetPointC(glm::vec3(1.0f, 1.0f, 0.0f));
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(glm::vec3(-1.0f, 1.0f, -2.0f));
			tri.SetPointB(glm::vec3(1.0f, 1.0f, -2.0f));
			tri.SetPointC(glm::vec3(1.0f, 1.0f, 0.0f));
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		// left square
		{
			tri.SetPointA(glm::vec3(-1.0f, 1.0f, 0.0f));
			tri.SetPointB(glm::vec3(-1.0f, -1.0f, 0.0f));
			tri.SetPointC(glm::vec3(-1.0f, -1.0f, -2.0f));
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(1);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(glm::vec3(-1.0f, 1.0f, 0.0f));
			tri.SetPointB(glm::vec3(-1.0f, 1.0f, -2.0f));
			tri.SetPointC(glm::vec3(-1.0f, -1.0f, -2.0f));
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(1);
			tri.AddToScene(&m_scene);
		}


		// right square
		{
			tri.SetPointA(glm::vec3(1.0f, 1.0f, 0.0f));;
			tri.SetPointB(glm::vec3(1.0f, -1.0f, 0.0f));
			tri.SetPointC(glm::vec3(1.0f, -1.0f, -2.0f));
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(2);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(glm::vec3(1.0f, 1.0f, 0.0f));
			tri.SetPointB(glm::vec3(1.0f, 1.0f, -2.0f));
			tri.SetPointC(glm::vec3(1.0f, -1.0f, -2.0f));
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(2);
			tri.AddToScene(&m_scene);
		}


		// forward square
		{
			tri.SetPointA(glm::vec3(-1.0f, 1.0f, -2.0f));
			tri.SetPointB(glm::vec3(-1.0f, -1.0f, -2.0f));
			tri.SetPointC(glm::vec3(1.0f, -1.0f, -2.0f));
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(glm::vec3(-1.0f, 1.0f, -2.0f));
			tri.SetPointB(glm::vec3(1.0f, 1.0f, -2.0f));
			tri.SetPointC(glm::vec3(1.0f, -1.0f, -2.0f));
			tri.FlipSequence();
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

		Shapes::Sphere sphere;
		sphere.SetBsdfIndex(Basic);

		sphere.SetPosition(glm::vec3(0.4f, -0.6f, -0.8f));
		sphere.SetRadius(0.4f);
		sphere.SetMaterialIndex(4);
		sphere.AddToScene(&m_scene);

		sphere.SetPosition(glm::vec3(-0.4f, -0.6f, -1.0f));
		sphere.SetRadius(0.4f);
		sphere.SetMaterialIndex(5);
		sphere.AddToScene(&m_scene);
	}

	void glass()
	{
		{
			Material& mat0 = m_scene.mat.emplace_back();
			mat0.m_albedo = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
		}
		Shapes::Triangle tri;
		tri.SetPointA(glm::vec3(0.0f, 6.0f, -2.0f));
		tri.SetPointB(glm::vec3(-3.0f, 0.0f, -2.0f));
		tri.SetPointC(glm::vec3(3.0f, 0.0f, -2.0f));
		//tri.FlipSequence();
		tri.ComputeNormal();
		tri.SetMaterialIndex(0);
		tri.SetBsdfIndex(Glass);
		tri.AddToScene(&m_scene);

		tri.SetPointA(glm::vec3(0.0f, 6.0f, -2.01f));
		tri.SetPointB(glm::vec3(-3.0f, 0.0f, -2.01f));
		tri.SetPointC(glm::vec3(3.0f, 0.0f, -2.01f));
		tri.SetMaterialIndex(0);
		tri.SetBsdfIndex(Glass);
		//tri.FlipSequence();
		tri.ComputeNormal();
		tri.AddToScene(&m_scene);

		{
			Material& mat1 = m_scene.mat.emplace_back();
			mat1.m_albedo = glm::vec4(1.0f, 0.0f, 1.0f, 1.0);
		}

		Shapes::Sphere sphere;

		sphere.SetBsdfIndex(Glass);
		sphere.SetPosition(glm::vec3(2.0f, 2.0f, -3.5f));
		sphere.SetRadius(1.0f);
		sphere.SetMaterialIndex(0);
		sphere.AddToScene(&m_scene);

		sphere.SetBsdfIndex(Basic);
		sphere.SetPosition(glm::vec3(0.0f, 2.0f, -3.5f));
		sphere.SetRadius(1.0f);
		sphere.SetMaterialIndex(1);
		sphere.AddToScene(&m_scene);

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

		Shapes::Sphere sphere;
		sphere.SetMaterialIndex(0);
		sphere.SetBsdfIndex(Basic);
		sphere.SetRadius(1.0f);

        for (float x = -5.0f; x <= 5.0f; x += 2.0f)
        {
            for (float z = -15.0f; z <= -5.0f; z += 2.0f)
			{
				sphere.SetPosition(glm::vec3(x, -3.0f, z));
            }
        }

		Shapes::Plane plane;

		plane.SetPosition(glm::vec3(0.0f, -4.0f, 0.0f));
		plane.SetNormal(glm::vec3(0.0f, 1.0f, 0.0f));
		plane.SetBsdfIndex(Basic);
		plane.SetMaterialIndex(1);
		plane.AddToScene(&m_scene);
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
			tri.SetPointA(glm::vec3(-1.0f, -1.0f, -2.0f));
			tri.SetPointB(pos);
			tri.SetPointC(glm::vec3(1.0f, -1.0f, 0.0f));
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(glm::vec3(-1.0f, -1.0f, -2.0f));
			tri.SetPointB(glm::vec3(1.0f, -1.0f, -2.0f));
			tri.SetPointC(glm::vec3(1.0f, -1.0f, 0.0f));
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		// top square
		{
			tri.SetPointA(glm::vec3(-1.0f, 1.0f, -2.0f));
			tri.SetPointB(glm::vec3(-1.0f, 1.0f, 0.0f));
			tri.SetPointC(glm::vec3(1.0f, 1.0f, 0.0f));
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(glm::vec3(-1.0f, 1.0f, -2.0f));
			tri.SetPointB(glm::vec3(1.0f, 1.0f, -2.0f));
			tri.SetPointC(glm::vec3(1.0f, 1.0f, 0.0f));
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		// left square
		{
			tri.SetPointA(glm::vec3(-1.0f, 1.0f, 0.0f));
			tri.SetPointB(glm::vec3(-1.0f, -1.0f, 0.0f));
			tri.SetPointC(glm::vec3(-1.0f, -1.0f, -2.0f));
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(1);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(glm::vec3(-1.0f, 1.0f, 0.0f));
			tri.SetPointB(glm::vec3(-1.0f, 1.0f, -2.0f));
			tri.SetPointC(glm::vec3(-1.0f, -1.0f, -2.0f));
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(1);
			tri.AddToScene(&m_scene);
		}


		// right square
		{
			tri.SetPointA(glm::vec3(1.0f, 1.0f, 0.0f));
			tri.SetPointB(glm::vec3(1.0f, -1.0f, 0.0f));
			tri.SetPointC(glm::vec3(1.0f, -1.0f, -2.0f));
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(2);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(glm::vec3(1.0f, 1.0f, 0.0f));
			tri.SetPointB(glm::vec3(1.0f, 1.0f, -2.0f));
			tri.SetPointC(glm::vec3(1.0f, -1.0f, -2.0f));
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(2);
			tri.AddToScene(&m_scene);
		}


		// forward square
		{
			tri.SetPointA(glm::vec3(-1.0f, 1.0f, -2.0f));
			tri.SetPointB(glm::vec3(-1.0f, -1.0f, -2.0f));
			tri.SetPointC(glm::vec3(1.0f, -1.0f, -2.0f));
			//tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}

		{
			tri.SetPointA(glm::vec3(-1.0f, 1.0f, -2.0f));
			tri.SetPointB(glm::vec3(1.0f, 1.0f, -2.0f));
			tri.SetPointC(glm::vec3(1.0f, -1.0f, -2.0f));
			tri.FlipSequence();
			tri.ComputeNormal();
			tri.SetBsdfIndex(Basic);
			tri.SetMaterialIndex(0);
			tri.AddToScene(&m_scene);
		}
	}
private:
	TTG::Camera m_camera;
	TTG::Config m_config{};

	bool m_viewPortOpen = true;

	static bool docking;
	Renderer m_renderer;
	Scene m_scene;
	Editor m_editor;

	GLFWwindow* m_handle = nullptr;
};

bool ExampleLayer::docking = true;

TTG::Application* createApplication()
{
	TTG::Application* app = new TTG::Application;
	app->PushLayer<ExampleLayer>();
	return app;
}
