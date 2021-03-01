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
#include "Engine/Shader.h"

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
	static void SetTabbed();

	static void SetBorderless(bool value) { borderless = value; }
	static bool GetBorderless() { return borderless; }

	static int GetHalfWidth() {
		if (fullscreen)
			return monitorVec[2] / 2;
		return _lastHalfWidth;
	}
	static int GetHalfHeight() {
		if (fullscreen)
			return monitorVec[3] / 2;
		return _lastHalfHeight;
	}
	static int GetWidth() {
		if (fullscreen)
			return monitorVec[2];
		return _lastWidth;
	}
	static int GetHeight() {
		if (fullscreen)
			return monitorVec[3];
		return _lastHeight;
	}

	static bool HasFocus() { return focus; }
private:
	static void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height);
	static void GlfwWindowMovedCallback(GLFWwindow* window, int yPos, int xPos);
	static void GlfwWindowFocusCallback(GLFWwindow* window, int result);
	static GLFWwindow* window;
	static float _aspect;
	static float _aspect2;
	static bool focus;
	static bool fullscreen;
	static bool borderless;
	static int _lastHalfHeight;
	static int _lastHalfWidth;
	static int _lastHeight;
	static int _lastWidth;
	static int _xPos;
	static int _yPos;
	static int monitorVec[4];
	static GLFWmonitor* monitor;
};