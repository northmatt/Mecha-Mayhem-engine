#include "ColCor.h"

void ColCor::Init(unsigned width, unsigned height, LUT3D cube) { 
	size_t index = _buffers.size();
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->Init(width, height);

	index = _shaders.size();
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[index]->LoadShaderPartFromFile("shaders/color_correction_frag.glsl", GL_FRAGMENT_SHADER);
	_shaders[index]->Link();

	_cube = cube;

	PostEffect::Init(width, height);
}

void ColCor::ApplyAffects(PostEffect* buffer) {
	BindShader(0);
	buffer->BindColourAsTexture(0, 0, 0);
	_cube.bind(30);

	_buffers[0]->RenderToFSQ();

	_cube.unbind(30);
	buffer->UnbindTexture(0);
	UnbindShader();
}