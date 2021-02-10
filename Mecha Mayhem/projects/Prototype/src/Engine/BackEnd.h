#pragma once
#include <Logging.h>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <json.hpp>
#include <fstream>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <entt/entt.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include "Engine/VertexArrayObject.h"
#include "Engine/IndexBuffer.h"
#include "Graphics/Post/ColCor.h"

class BackEnd abstract
{
public:
	//returns
	static GLFWwindow* Init(std::string name, int width, int height);
	static void InitImGui();
	static void CloseImGui();
	static void Unload();

	static void SetAspect(int width, int height);
	static float GetAspect() { return _aspect; }

	static bool GetFullscreen() { return fullscreen; }
	static void SetFullscreen();
	static void SetTabbed(int width, int height);

	static int GetHalfWidth() { return _lastWidth; }
	static int GetHalfHeight() { return _lastHeight; }

	static bool HasFocus() { return focus; }
	static GLFWwindow* GetWindow() { return window; }
private:
	static void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height);
	static void GlfwWindowFocusCallback(GLFWwindow* window, int result);
	static GLFWwindow* window;
	static float _aspect;
	static float _aspect2;
	static bool focus;
	static bool fullscreen;
	static int _lastHeight;
	static int _lastWidth;
	static int monitorVec[4];
	static GLFWmonitor* monitor;
};