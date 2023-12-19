#pragma once

class Layer
{
public:
	virtual void onAttach(GLFWwindow* handle) {};
	virtual void onDetach() {};

	virtual void onUIRender(int ID) {};
	virtual void onTick() {};
};