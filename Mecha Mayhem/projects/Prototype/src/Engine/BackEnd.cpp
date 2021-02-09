#include "BackEnd.h"
#include "Utilities/Scene.h"

#define LOG_GL_NOTIFICATIONS

/*
	Handles debug messages from OpenGL
	https://www.khronos.org/opengl/wiki/Debug_Output#Message_Components
	@param source    Which part of OpenGL dispatched the message
	@param type      The type of message (ex: error, performance issues, deprecated behavior)
	@param id        The ID of the error or message (to distinguish between different types of errors, like nullref or index out of range)
	@param severity  The severity of the message (from High to Notification)
	@param length    The length of the message
	@param message   The human readable message from OpenGL
	@param userParam The pointer we set with glDebugMessageCallback (should be the game pointer)
*/
void GlDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::string sourceTxt;
	switch (source) {
	case GL_DEBUG_SOURCE_API: sourceTxt = "DEBUG"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceTxt = "WINDOW"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceTxt = "SHADER"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: sourceTxt = "THIRD PARTY"; break;
	case GL_DEBUG_SOURCE_APPLICATION: sourceTxt = "APP"; break;
	case GL_DEBUG_SOURCE_OTHER: default: sourceTxt = "OTHER"; break;
	}
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:          LOG_INFO("[{}] {}", sourceTxt, message); break;
	case GL_DEBUG_SEVERITY_MEDIUM:       LOG_WARN("[{}] {}", sourceTxt, message); break;
	case GL_DEBUG_SEVERITY_HIGH:         LOG_ERROR("[{}] {}", sourceTxt, message); break;
#ifdef LOG_GL_NOTIFICATIONS
	case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_INFO("[{}] {}", sourceTxt, message); break;
#endif
	default: break;
	}
}

void BackEnd::GlfwWindowResizedCallback(GLFWwindow* window, int width, int height) {
	//force window back to right ratio
	if (_lastHeight != height / 2) {
		int wide = height * _aspect;
		glfwSetWindowSize(window, wide, height);
		//glViewport(0, 0, wide, height);
		_lastHeight = height / 2;
		_lastWidth = wide / 2;

		Scene::_activeScene->GetRegistry()->view<Framebuffer>().each([=](Framebuffer& buf) {
			buf.Reshape(wide, height);
		});
		Scene::_activeScene->GetRegistry()->view<PostEffect>().each([=](PostEffect& buf) {
			buf.Reshape(wide, height);
		});
	}
	else {
		int high = width * _aspect2;
		glfwSetWindowSize(window, width, high);
		//glViewport(0, 0, width / 2, high / 2);
		_lastHeight = high / 2;
		_lastWidth = width / 2;

		Scene::_activeScene->GetRegistry()->view<Framebuffer>().each([=](Framebuffer& buf) {
			buf.Reshape(width, high);
		});
		Scene::_activeScene->GetRegistry()->view<PostEffect>().each([=](PostEffect& buf) {
			buf.Reshape(width, high);
		});
	}

}

void BackEnd::GlfwWindowFocusCallback(GLFWwindow* window, int result)
{
	focus = (result == GLFW_TRUE);
	if (focus)
		glfwFocusWindow(window);
}

GLFWwindow *BackEnd::window = nullptr;
Scene* _activeScene = nullptr;
float BackEnd::_aspect = 1;
float BackEnd::_aspect2 = 1;
bool BackEnd::focus = false;
bool BackEnd::fullscreen = false;
int BackEnd::_lastHeight = 1;
int BackEnd::_lastWidth = 1;
int BackEnd::monitorVec[4] = { 0 };
GLFWmonitor* BackEnd::monitor = nullptr;

GLFWwindow* BackEnd::Init(std::string name, int width, int height)
{
	if (glfwInit() == GLFW_FALSE) {
		LOG_ERROR("Failed to initialize GLFW");
		return nullptr;
	}

	//Create a new GLFW window
	window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(window);

	monitor = glfwGetPrimaryMonitor();
	_aspect = float(width) / height;
	_aspect2 = float(height) / width;
	_lastHeight = height / 2;
	_lastWidth = width / 2;

	// Set our window resized callback
	glfwSetWindowSizeCallback(window, GlfwWindowResizedCallback);
	glfwSetWindowFocusCallback(window, GlfwWindowFocusCallback);

	SetTabbed(width, height);

	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		LOG_ERROR("Failed to initialize Glad");
		return nullptr;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(GlDebugMessage, nullptr);

	// GL states
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_FRONT); //GL_BACK, GL_FRONT_AND_BACK

    return window;
}

void BackEnd::InitImGui()
{
	//shamelessly copied from Intermidiate CG OTTER

	// Creates a new ImGUI context
	ImGui::CreateContext();
	// Gets our ImGUI input/output 
	ImGuiIO& io = ImGui::GetIO();
	// Allow docking to our window
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// Allow multiple viewports (so we can drag ImGui off our window)
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	// Allow our viewports to use transparent backbuffers
	io.ConfigFlags |= ImGuiConfigFlags_TransparentBackbuffers;

	// Set up the ImGui implementation for OpenGL
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	// Dark mode FTW
	ImGui::StyleColorsDark();

	// Get our imgui style
	ImGuiStyle& style = ImGui::GetStyle();
	//style.Alpha = 1.0f;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 0.8f;
	}
}

void BackEnd::CloseImGui()
{
	// Cleanup the ImGui implementation
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	// Destroy our ImGui context
	ImGui::DestroyContext();
}

void BackEnd::Unload()
{
	delete window;
	delete monitor;
}

void BackEnd::SetAspect(int width, int height)
{
	_aspect = float(width) / height;
	_aspect2 = float(height) / width;
	GlfwWindowResizedCallback(window, width, height);
}

void BackEnd::SetFullscreen()
{
	fullscreen = true;
	glfwGetMonitorWorkarea(monitor, &monitorVec[0], &monitorVec[1], &monitorVec[2], &monitorVec[3]);

	// We want GL commands to be executed for our window, so we make our window's context the current one
	glfwMakeContextCurrent(window);

	//go fullscreen on selected monitor
	//glfwSetWindowMonitor(window, monitor, 0, 0, monitorVec[2], monitorVec[3], 60);
	glfwSetWindowMonitor(window, nullptr, 32, -1, monitorVec[2], monitorVec[3], 60);

	GlfwWindowFocusCallback(window, true);
}

void BackEnd::SetTabbed(int width, int height)
{
	fullscreen = false;
	glfwSetWindowMonitor(window, nullptr, 50, 50, width, height, 60);
	GlfwWindowFocusCallback(window, true);
}
