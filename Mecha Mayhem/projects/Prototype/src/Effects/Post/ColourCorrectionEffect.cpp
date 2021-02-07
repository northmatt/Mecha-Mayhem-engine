#include "ColourCorrectionEffect.h"

void ColourCorrection::Init(unsigned width, unsigned height)
{
	int index = int(_buffers.size());
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->Init(width, height);

	//Loads the shaders
	index = int(_shaders.size());
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/Post/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[index]->LoadShaderPartFromFile("shaders/Post/passthrough_frag.glsl", GL_FRAGMENT_SHADER);
	_shaders[index]->Link();
	index = int(_shaders.size());
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/Post/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[index]->LoadShaderPartFromFile("shaders/Post/color_correction_frag.glsl", GL_FRAGMENT_SHADER);
	_shaders[index]->Link();
}

void ColourCorrection::ApplyEffect(PostEffect* buffer)
{
	BindShader(1);

	buffer->BindColorAsTexture(0, 0, 0);
	_cube.bind(30);

	_buffers[0]->RenderToFSQ();

	_cube.unbind(30);
	buffer->UnbindTexture(0);

	UnbindShader();
}

void ColourCorrection::SetCube(LUT3D cube)
{
	_cube = cube;
}

LUT3D ColourCorrection::GetCube() const
{
	return _cube;
}
