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
#include "Scene.h"
#include "shapes/Triangle.h"

#include <stdio.h>
#include <string>

class ExampleLayer : public Layer
{
public:
	ExampleLayer()
		: renderer(&scene, &camera)
	{
		camera.position = glm::vec3(0.03f, 0.0f, 1.8f);
		camera.speed = 1.0f;

		{
			// BSDF 0
#define Basic 0
			scene.bsdf.emplace_back(std::make_unique<BasicBsdf>());
		}

		{
			// BSDF 1
#define Test 1
			scene.bsdf.emplace_back(std::make_unique<TestBsdf>());
		}

		{
			// BSDF 2
#define Glass 2
			scene.bsdf.emplace_back(std::make_unique<GlassBsdf>());
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
		}
	}

	virtual void onAttach(GLFWwindow* handle) override
	{
		m_handle = handle;
	}

	virtual void onDetach() override
    {
	}

	virtual void onUIRender(int id) override
	{
		tick.start();
        mainViewport(id);
		editor();
		tick.stop();
	}

	virtual void onTick() override
	{
		return;
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			glm::vec<2, double> pos;

			glfwGetCursorPos(m_handle, &pos.x, &pos.y);

			pos.y -= 30.0f;

			if (ImGui::GetMousePos().x > renderer.getWidth())
				return;

			Ray ray;
			ray.Origin = camera.position;
			ray.Origin.y = -ray.Origin.y;
			int index = (pos.y) * renderer.getWidth() + pos.x;

			if (index > camera.rayDirections.size() - 1)
				return;

			ray.Direction = camera.rayDirections[index];
			ray.Direction.y = -ray.Direction.y;
			//ray.Origin += ray.Direction;
			HitPayload payload = renderer.traceRay(ray);

			//Triangle& tri = scene.triangles.emplace_back();
			//tri.PosA = ray.Origin;
			//tri.PosB = ray.Origin + ray.Direction * 10.0f;
			//tri.PosB.y += 0.1f;
			//tri.PosC = ray.Origin + ray.Direction * 10.0f;
			//tri.PosC.y -= 0.1f;
			//tri.bsdfIndex = Basic;
			//tri.matIndex = scene.mat.size() - 1;

			if (payload.hasHit)
			{
				selected = true;
				sIndex = payload.hitIndex;
				sType = payload.hitType;
			}
			else
			{
				selected = false;
			}
		}
	}

private:
	void mainViewport(int id)
	{
		if (!viewPortOpen)
			return;

		if (docking)
		{
			if (id == 0)
			{
				ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
			}
		}

		ImGui::Begin("main viewport", &viewPortOpen, ImGuiWindowFlags_MenuBar);

		windowSetup();

		config = { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 60.0f, 0.1f };
		if (camera.update(config))
			renderer.resetFrameIndex();
		render();

		ImGui::End();
	}

	void editor()
	{
		if (!editorOpen)
			return;

		ImGui::Begin("Editor", &editorOpen);

		if (!selected)
		{
			ImGui::SeparatorText("spheres");
			for (int i = 0; i < scene.spheres.size(); i++)
			{
				ImGui::PushID(i);

				Sphere& sphere = scene.spheres[i];
				ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
				ImGui::DragFloat("Radius", &sphere.radius, 0.1f);
				ImGui::DragInt("material index", &sphere.matIndex, 0.1f, 0, scene.mat.size() - 1);
				ImGui::Checkbox("Visible", &sphere.visible);
				ImGui::DragInt("bsdf index", &sphere.bsdfIndex, 0.1f, 0, scene.bsdf.size() - 1);
				if (ImGui::CollapsingHeader("advance"))
				{
					scene.bsdf[sphere.bsdfIndex]->advanceSetting();
				}
				ImGui::Separator();

				ImGui::PopID();
			}

			ImGui::SeparatorText("planes");
			for (int i = 0; i < scene.planes.size(); i++)
			{
				ImGui::PushID(i + scene.spheres.size());

				Plane& plane = scene.planes[i];
				ImGui::DragFloat3("Position", glm::value_ptr(plane.Position), 0.1f);
				ImGui::DragFloat3("Normal", glm::value_ptr(plane.normal), 0.1f);
				ImGui::DragInt("material index", &plane.matIndex, 0.1f, 0, scene.mat.size() - 1);
				ImGui::Checkbox("Visible", &plane.visible);
				ImGui::Separator();

				ImGui::PopID();
			}

			ImGui::SeparatorText("triangles");
			for (int i = 0; i < scene.triangles.size(); i++)
			{
				ImGui::PushID(i + scene.spheres.size() + scene.planes.size());

				Triangle& tri = scene.triangles[i];
				ImGui::DragFloat3("PositionA", glm::value_ptr(tri.PosA), 0.1f);
				ImGui::DragFloat3("PositionB", glm::value_ptr(tri.PosB), 0.1f);
				ImGui::DragFloat3("PositionC", glm::value_ptr(tri.PosC), 0.1f);

				ImGui::DragFloat3("NormalA", glm::value_ptr(tri.na), 0.1f);
				ImGui::DragFloat3("NormalB", glm::value_ptr(tri.nb), 0.1f);
				ImGui::DragFloat3("NormalC", glm::value_ptr(tri.nc), 0.1f);

				ImGui::DragInt("material index", &tri.matIndex, 0.1f, 0, scene.mat.size() - 1);

				if (ImGui::Button("compute normal"))
				{
					Shapes::Triangle t;
					t.init(tri);
					t.computeNormal();
					scene.triangles[i] = t.getData();
				}

				if (ImGui::Button("flip normal"))
				{
					Shapes::Triangle t;
					t.init(tri);
					t.flipNormal();
					scene.triangles[i] = t.getData();
				}

				if (ImGui::Button("flip Sequence"))
				{
					Shapes::Triangle t;
					t.init(tri);
					t.flipSequence();
					scene.triangles[i] = t.getData();
				}

				ImGui::Checkbox("Visible", &tri.visible);

				ImGui::Separator();

				ImGui::PopID();
			}

			ImGui::SeparatorText("materials");
			for (int i = 0; i < scene.mat.size(); i++)
			{
				ImGui::PushID(i);

				Material& mat = scene.mat[i];
				ImGui::Text("material %i", i);
				ImGui::ColorEdit3("color", glm::value_ptr(mat.albedo));
				ImGui::DragFloat("roughness", &mat.roughness, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("metalic", &mat.metalic, 0.01f, 0.0f, 1.0f);
				ImGui::ColorEdit3("emission color", glm::value_ptr(mat.emissionColor));
				ImGui::DragFloat("emission power", &mat.emissionPower, 0.01f, 0.0f, FLT_MAX);
				ImGui::Separator();

				ImGui::PopID();
			}
		}
		else
		{
			ShapeBase* entity = nullptr;

			if (sType == HitType::SPHERE)
			{
				ImGui::SeparatorText("Sphere");

				Sphere& sphere = scene.spheres[sIndex];
				entity = &sphere;

				ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
				ImGui::DragFloat("Radius", &sphere.radius, 0.1f);
				ImGui::DragInt("material index", &sphere.matIndex, 0.f, 0, scene.mat.size() - 1);
				ImGui::Checkbox("Visible", &sphere.visible);
			}
			else if (sType == HitType::PLANE)
			{
				ImGui::SeparatorText("Plane");

				Plane& plane = scene.planes[sIndex];
				entity = &plane;

				ImGui::DragFloat3("Position", glm::value_ptr(plane.Position), 0.1f);
				ImGui::DragFloat3("Normal", glm::value_ptr(plane.normal), 0.1f);
				ImGui::DragInt("material index", &plane.matIndex, 0.f, 0, scene.mat.size() - 1);
				ImGui::Checkbox("Visible", &plane.visible);
			}
			else if (sType == HitType::TRIANGLE)
			{
				ImGui::SeparatorText("Triangle");

				Triangle& tri = scene.triangles[sIndex];
				entity = &tri;

				ImGui::DragFloat3("PositionA", glm::value_ptr(tri.PosA), 0.1f);
				ImGui::DragFloat3("PositionB", glm::value_ptr(tri.PosB), 0.1f);
				ImGui::DragFloat3("PositionC", glm::value_ptr(tri.PosC), 0.1f);

				ImGui::DragFloat3("NormalA", glm::value_ptr(tri.na), 0.1f);
				ImGui::DragFloat3("NormalB", glm::value_ptr(tri.nb), 0.1f);
				ImGui::DragFloat3("NormalC", glm::value_ptr(tri.nc), 0.1f);

				ImGui::DragInt("material index", &tri.matIndex, 0.f, 0, scene.mat.size() - 1);

				if (ImGui::Button("compute normal"))
				{
					Shapes::Triangle t;
					t.init(tri);
					t.computeNormal();
					tri = t.getData();
				}

				if (ImGui::Button("flip normal"))
				{
					Shapes::Triangle t;
					t.init(tri);
					t.flipNormal();
					tri = t.getData();
				}

				if (ImGui::Button("flip Sequence"))
				{
					Shapes::Triangle t;
					t.init(tri);
					t.flipSequence();
					tri = t.getData();
				}

				ImGui::Checkbox("Visible", &tri.visible);
			}

			ImGui::SeparatorText("Binded Material");

			if (entity != nullptr)
			{
				Material& mat = scene.mat[entity->matIndex];
				ImGui::ColorEdit3("color", glm::value_ptr(mat.albedo));
				ImGui::DragFloat("roughness", &mat.roughness, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("metalic", &mat.metalic, 0.01f, 0.0f, 1.0f);
				ImGui::ColorEdit3("emission color", glm::value_ptr(mat.emissionColor));
				ImGui::DragFloat("emission power", &mat.emissionPower, 0.01f, 0.0f, FLT_MAX);
				ImGui::Separator();
			}
		}
		
		ImGui::SeparatorText("editors");
		ImGui::Text("this is the Editor");
		ImGui::Text("last render: %fms", t.ms());
		ImGui::Text("last tick: %fms", tick.ms());
		if (ImGui::Button("reset"))
			renderer.resetFrameIndex();
		ImGui::Checkbox("accumulate", renderer.getAccumulate());
		if (!(*renderer.getAccumulate()))
			renderer.resetFrameIndex();

		ImGui::Checkbox("slow random", renderer.getSlowRandom());
		ImGui::Checkbox("sky light emission", renderer.getSkyLightSwitch());

		ImGui::End();
	}

	void render()
	{
		t.start();

		if (renderer.onResize())
		{
			camera.reCalculateRayDirections(config);
		}
		renderer.render(config);

		t.stop();

		unsigned char* imageData = renderer.getImageData();
		TTG::Texture& finalImage = renderer.getFinalImage();
		if (imageData)
		{
			finalImage.loadTextureFromData(imageData, 4, renderer.getWidth(), renderer.getHeight());
			ImGui::Image(finalImage.getDescriptorSet(), { (float)renderer.getWidth(), (float)renderer.getHeight()});
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
			Material& mat0 = scene.mat.emplace_back();
			mat0.albedo = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
		}

		{
			Material& mat1 = scene.mat.emplace_back();
			mat1.albedo = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
			mat1.emissionColor = mat1.albedo;
			mat1.emissionPower = 20.0f;
		}

		{
			Material& mat2 = scene.mat.emplace_back();
			mat2.albedo = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
			mat2.roughness = 1.0f;
		}
		*renderer.getSkyLightSwitch() = true;

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
			Material& mat0 = scene.mat.emplace_back();
			mat0.albedo = glm::vec4(1.0f);
		}

		{
			// red
			Material& mat1 = scene.mat.emplace_back();
			mat1.albedo = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}

		{
			// green
			Material& mat2 = scene.mat.emplace_back();
			mat2.albedo = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		}

		{
			// light
			Material& mat3 = scene.mat.emplace_back();
			mat3.emissionColor = glm::vec4(1.0f);
			mat3.albedo = glm::vec4(1.0f);
			mat3.emissionPower = 10.0f;
		}

		//scene.spheres.emplace_back();

		Shapes::Triangle tri;

		// light
		{
			tri.setPointA(-0.5f, 0.9f, -0.75f);
			tri.setPointB(-0.5f, 0.9f, -0.25f);
			tri.setPointC(0.5f, 0.9f, -0.25f);
			//tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(3);
			tri.addToScene(&scene);
		}

		{
			tri.setPointA(-0.5f, 0.9f, -0.75f);
			tri.setPointB(0.5f, 0.9f, -0.75f);
			tri.setPointC(0.5f, 0.9f, -0.25f);
			tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(3);
			tri.addToScene(&scene);
		}


		// bottom square
		{
			tri.setPointA(-1.0f, -1.0f, -2.0f);
			tri.setPointB(-1.0f, -1.0f, 0.0f);
			tri.setPointC(1.0f, -1.0f, 0.0f);
			tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(0);
			tri.addToScene(&scene);
		}

		{
			tri.setPointA(-1.0f, -1.0f, -2.0f);
			tri.setPointB(1.0f, -1.0f, -2.0f);
			tri.setPointC(1.0f, -1.0f, 0.0f);
			//tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(0);
			tri.addToScene(&scene);
		}

		// top square
		{
			tri.setPointA(-1.0f, 1.0f, -2.0f);
			tri.setPointB(-1.0f, 1.0f, 0.0f);
			tri.setPointC(1.0f, 1.0f, 0.0f);
			//tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(0);
			tri.addToScene(&scene);
		}

		{
			tri.setPointA(-1.0f, 1.0f, -2.0f);
			tri.setPointB(1.0f, 1.0f, -2.0f);
			tri.setPointC(1.0f, 1.0f, 0.0f);
			tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(0);
			tri.addToScene(&scene);
		}

		// left square
		{
			tri.setPointA(-1.0f, 1.0f, 0.0f);
			tri.setPointB(-1.0f, -1.0f, 0.0f);
			tri.setPointC(-1.0f, -1.0f, -2.0f);
			tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(1);
			tri.addToScene(&scene);
		}

		{
			tri.setPointA(-1.0f, 1.0f, 0.0f);
			tri.setPointB(-1.0f, 1.0f, -2.0f);
			tri.setPointC(-1.0f, -1.0f, -2.0f);
			//tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(1);
			tri.addToScene(&scene);
		}


		// right square
		{
			tri.setPointA(1.0f, 1.0f, 0.0f);
			tri.setPointB(1.0f, -1.0f, 0.0f);
			tri.setPointC(1.0f, -1.0f, -2.0f);
			//tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(2);
			tri.addToScene(&scene);
		}

		{
			tri.setPointA(1.0f, 1.0f, 0.0f);
			tri.setPointB(1.0f, 1.0f, -2.0f);
			tri.setPointC(1.0f, -1.0f, -2.0f);
			tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(2);
			tri.addToScene(&scene);
		}


		// forward square
		{
			tri.setPointA(-1.0f, 1.0f, -2.0f);
			tri.setPointB(-1.0f, -1.0f, -2.0f);
			tri.setPointC(1.0f, -1.0f, -2.0f);
			tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(0);
			tri.addToScene(&scene);
		}

		{
			tri.setPointA(-1.0f, 1.0f, -2.0f);
			tri.setPointB(1.0f, 1.0f, -2.0f);
			tri.setPointC(1.0f, -1.0f, -2.0f);
			//tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(0);
			tri.addToScene(&scene);
		}

		// objects

#define index 1
#if index
		{
			Material& mat4 = scene.mat.emplace_back();
			mat4.albedo = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
            mat4.emissionColor = mat4.albedo;
            mat4.emissionPower = 2.0f;
		}
        {
            Material& mat5 = scene.mat.emplace_back();
            mat5.albedo = glm::vec4(1.0f);
            mat5.roughness = 0.0f;
        }

		Sphere& sphere0 = scene.spheres.emplace_back();
		sphere0.bsdfIndex = Basic;
		sphere0.matIndex = 4;
		sphere0.Position = glm::vec3(0.4f, -0.6f, -0.8f);
		sphere0.radius = 0.4f;

		

		Sphere& sphere1 = scene.spheres.emplace_back();
		sphere1.bsdfIndex = Basic;
		sphere1.matIndex = 5;
		sphere1.Position = glm::vec3(-0.4f, -0.6f, -1.0f);
		sphere1.radius = 0.4f;
#else
		{
			Material& mat4 = scene.mat.emplace_back();
			mat4.albedo = glm::vec4(1.0f);
		}

		{
			// bottom square
			{
				tri.setPointA(-1.0f, -1.0f, -2.0f);
				tri.setPointB(-1.0f, -1.0f, 0.0f);
				tri.setPointC(1.0f, -1.0f, 0.0f);
				tri.flipSequence();
				tri.computeNormal();
				tri.setBsdfIndex(Basic);
				tri.setMaterialIndex(4);
				tri.addToScene(&scene);
			}

			{
				tri.setPointA(-1.0f, -1.0f, -2.0f);
				tri.setPointB(1.0f, -1.0f, -2.0f);
				tri.setPointC(1.0f, -1.0f, 0.0f);
				//tri.flipSequence();
				tri.computeNormal();
				tri.setBsdfIndex(Basic);
				tri.setMaterialIndex(4);
				tri.addToScene(&scene);
			}

			// top square
			{
				tri.setPointA(-1.0f, 1.0f, -2.0f);
				tri.setPointB(-1.0f, 1.0f, 0.0f);
				tri.setPointC(1.0f, 1.0f, 0.0f);
				//tri.flipSequence();
				tri.computeNormal();
				tri.setBsdfIndex(Basic);
				tri.setMaterialIndex(4);
				tri.addToScene(&scene);
			}

			{
				tri.setPointA(-1.0f, 1.0f, -2.0f);
				tri.setPointB(1.0f, 1.0f, -2.0f);
				tri.setPointC(1.0f, 1.0f, 0.0f);
				tri.flipSequence();
				tri.computeNormal();
				tri.setBsdfIndex(Basic);
				tri.setMaterialIndex(4);
				tri.addToScene(&scene);
			}

			// left square
			{
				tri.setPointA(-1.0f, 1.0f, 0.0f);
				tri.setPointB(-1.0f, -1.0f, 0.0f);
				tri.setPointC(-1.0f, -1.0f, -2.0f);
				tri.flipSequence();
				tri.computeNormal();
				tri.setBsdfIndex(Basic);
				tri.setMaterialIndex(4);
				tri.addToScene(&scene);
			}

			{
				tri.setPointA(-1.0f, 1.0f, 0.0f);
				tri.setPointB(-1.0f, 1.0f, -2.0f);
				tri.setPointC(-1.0f, -1.0f, -2.0f);
				//tri.flipSequence();
				tri.computeNormal();
				tri.setBsdfIndex(Basic);
				tri.setMaterialIndex(4);
				tri.addToScene(&scene);
			}


			// right square
			{
				tri.setPointA(1.0f, 1.0f, 0.0f);
				tri.setPointB(1.0f, -1.0f, 0.0f);
				tri.setPointC(1.0f, -1.0f, -2.0f);
				//tri.flipSequence();
				tri.computeNormal();
				tri.setBsdfIndex(Basic);
				tri.setMaterialIndex(4);
				tri.addToScene(&scene);
			}

			{
				tri.setPointA(1.0f, 1.0f, 0.0f);
				tri.setPointB(1.0f, 1.0f, -2.0f);
				tri.setPointC(1.0f, -1.0f, -2.0f);
				tri.flipSequence();
				tri.computeNormal();
				tri.setBsdfIndex(Basic);
				tri.setMaterialIndex(4);
				tri.addToScene(&scene);
			}


			// forward square
			{
				tri.setPointA(-1.0f, 1.0f, -2.0f);
				tri.setPointB(-1.0f, -1.0f, -2.0f);
				tri.setPointC(1.0f, -1.0f, -2.0f);
				tri.flipSequence();
				tri.computeNormal();
				tri.setBsdfIndex(Basic);
				tri.setMaterialIndex(4);
				tri.addToScene(&scene);
			}

			{
				tri.setPointA(-1.0f, 1.0f, -2.0f);
				tri.setPointB(1.0f, 1.0f, -2.0f);
				tri.setPointC(1.0f, -1.0f, -2.0f);
				//tri.flipSequence();
				tri.computeNormal();
				tri.setBsdfIndex(Basic);
				tri.setMaterialIndex(4);
				tri.addToScene(&scene);
			}
		}
#endif
	}

	void glass()
	{
		{
			Material& mat0 = scene.mat.emplace_back();
			mat0.albedo = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
		}
		Shapes::Triangle tri;
		tri.setPointA(0.0f, 6.0f, -2.0f);
		tri.setPointB(-3.0f, 0.0f, -2.0f);
		tri.setPointC(3.0f, 0.0f, -2.0f);
		tri.flipSequence();
		tri.computeNormal();
		tri.setMaterialIndex(0);
		tri.setBsdfIndex(Glass);
		tri.addToScene(&scene);

		tri.setPointA(0.0f, 6.0f, -2.01f);
		tri.setPointB(-3.0f, 0.0f, -2.01f);
		tri.setPointC(3.0f, 0.0f, -2.01f);
		tri.flipSequence();
		tri.computeNormal();
		tri.addToScene(&scene);

		Sphere& sphere = scene.spheres.emplace_back();
		sphere.bsdfIndex = Glass;
		sphere.matIndex = 0;
		sphere.Position = glm::vec3(2.0f, 2.0f, -3.5f);

		{
			Material& mat1 = scene.mat.emplace_back();
			mat1.albedo = glm::vec4(1.0f, 0.0f, 1.0f, 1.0);
		}

		Sphere& sphere0 = scene.spheres.emplace_back();
		sphere0.Position = glm::vec3(0.0f, 2.0f, -3.5f);
		sphere0.radius = 1.0f;
		sphere0.bsdfIndex = Basic;
		sphere0.matIndex = 1;

		*renderer.getSkyLightSwitch() = true;
	}

	void addSquare(glm::vec3 pos, glm::vec3 scale, int mat, bool flipFace)
	{
		glm::vec3 radius = scale / 2.0f;

		Shapes::Triangle tri;
		tri.setPointA(pos + glm::vec3(-radius.x, radius.y, 0.0f));
		tri.setPointB(pos + glm::vec3(-radius.x, -radius.y, 0.0f));
		tri.setPointC(pos + glm::vec3(radius.x, -radius.y, 0.0f));
		if (!flipFace) tri.flipSequence();
		tri.computeNormal();
		tri.setMaterialIndex(mat);
		tri.addToScene(&scene);

		tri.setPointA(pos + glm::vec3(-radius.x, radius.y, 0.0f));
		tri.setPointB(pos + glm::vec3(radius.x, radius.y, 0.0f));
		tri.setPointC(pos + glm::vec3(radius.x, -radius.y, 0.0f));
		if (flipFace) tri.flipSequence();
		tri.computeNormal();
		tri.setMaterialIndex(mat);
		tri.addToScene(&scene);
	}

	void addCube(glm::vec3 pos, glm::vec3 scale)
	{
		Shapes::Triangle tri;
		// bottom square
		{
			tri.setPointA(-1.0f, -1.0f, -2.0f);
			tri.setPointB(pos);
			tri.setPointC(1.0f, -1.0f, 0.0f);
			tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(0);
			tri.addToScene(&scene);
		}

		{
			tri.setPointA(-1.0f, -1.0f, -2.0f);
			tri.setPointB(1.0f, -1.0f, -2.0f);
			tri.setPointC(1.0f, -1.0f, 0.0f);
			//tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(0);
			tri.addToScene(&scene);
		}

		// top square
		{
			tri.setPointA(-1.0f, 1.0f, -2.0f);
			tri.setPointB(-1.0f, 1.0f, 0.0f);
			tri.setPointC(1.0f, 1.0f, 0.0f);
			//tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(0);
			tri.addToScene(&scene);
		}

		{
			tri.setPointA(-1.0f, 1.0f, -2.0f);
			tri.setPointB(1.0f, 1.0f, -2.0f);
			tri.setPointC(1.0f, 1.0f, 0.0f);
			tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(0);
			tri.addToScene(&scene);
		}

		// left square
		{
			tri.setPointA(-1.0f, 1.0f, 0.0f);
			tri.setPointB(-1.0f, -1.0f, 0.0f);
			tri.setPointC(-1.0f, -1.0f, -2.0f);
			tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(1);
			tri.addToScene(&scene);
		}

		{
			tri.setPointA(-1.0f, 1.0f, 0.0f);
			tri.setPointB(-1.0f, 1.0f, -2.0f);
			tri.setPointC(-1.0f, -1.0f, -2.0f);
			//tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(1);
			tri.addToScene(&scene);
		}


		// right square
		{
			tri.setPointA(1.0f, 1.0f, 0.0f);
			tri.setPointB(1.0f, -1.0f, 0.0f);
			tri.setPointC(1.0f, -1.0f, -2.0f);
			//tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(2);
			tri.addToScene(&scene);
		}

		{
			tri.setPointA(1.0f, 1.0f, 0.0f);
			tri.setPointB(1.0f, 1.0f, -2.0f);
			tri.setPointC(1.0f, -1.0f, -2.0f);
			tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(2);
			tri.addToScene(&scene);
		}


		// forward square
		{
			tri.setPointA(-1.0f, 1.0f, -2.0f);
			tri.setPointB(-1.0f, -1.0f, -2.0f);
			tri.setPointC(1.0f, -1.0f, -2.0f);
			tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(0);
			tri.addToScene(&scene);
		}

		{
			tri.setPointA(-1.0f, 1.0f, -2.0f);
			tri.setPointB(1.0f, 1.0f, -2.0f);
			tri.setPointC(1.0f, -1.0f, -2.0f);
			//tri.flipSequence();
			tri.computeNormal();
			tri.setBsdfIndex(Basic);
			tri.setMaterialIndex(0);
			tri.addToScene(&scene);
		}
	}
private:
	TTG::Timer t;
	TTG::Timer tick;
	TTG::Camera camera;
	TTG::Config config;

	bool viewPortOpen = true;
	bool editorOpen = true;

	bool selected = false;
	HitType sType = HitType::UNKNOWN;
	int sIndex = -1;

	static bool docking;
	Renderer renderer;
	Scene scene;

	GLFWwindow* m_handle;
};

bool ExampleLayer::docking = true;

TTG::Application* createApplication()
{
	TTG::Application* app = new TTG::Application;
	app->pushLayer<ExampleLayer>();
	return app;
}
