#pragma once

class Layer
{
public:
	virtual void OnAttach(GLFWwindow* handle) {};
	virtual void OnDetach() {};

	virtual void OnUIRender(int ID) {};
	virtual void OnTick() {};
};