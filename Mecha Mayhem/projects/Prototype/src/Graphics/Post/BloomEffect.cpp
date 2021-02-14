#include "BloomEffect.h"

void BloomEffect::Init(unsigned width, unsigned height) {
	size_t index = _buffers.size();
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->Init(width, height);
	++index;
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->Init(unsigned(width / _downscale), unsigned(height / _downscale));
	++index;
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->Init(unsigned(width / _downscale), unsigned(height / _downscale));
	++index;
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->Init(width, height);

	//check if the shader is initialized
	//Load in the shader
	index = _shaders.size();
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[index]->LoadShaderPartFromFile("shaders/bloom/PassThrough.frag", GL_FRAGMENT_SHADER);
	_shaders[index]->Link();

	++index;
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[index]->LoadShaderPartFromFile("shaders/bloom/BloomHighPass.frag", GL_FRAGMENT_SHADER);
	_shaders[index]->Link();

	++index;
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[index]->LoadShaderPartFromFile("shaders/bloom/BlurHorizontal.frag", GL_FRAGMENT_SHADER);
	_shaders[index]->Link();

	++index;
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[index]->LoadShaderPartFromFile("shaders/bloom/BlurVertical.frag", GL_FRAGMENT_SHADER);
	_shaders[index]->Link();

	++index;
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[index]->LoadShaderPartFromFile("shaders/bloom/BloomComposite.frag", GL_FRAGMENT_SHADER);
	_shaders[index]->Link();

	//Pixel size
	_pixelSize = glm::vec2(1.f / width, 1.f / height);
}

void BloomEffect::ApplyAffects(PostEffect * buffer) {
	//Draws previous buffer to first render target
	BindShader(0);
	buffer->BindColourAsTexture(0, 0, 0);

	_buffers[0]->RenderToFSQ();

	buffer->UnbindTexture(0);
	UnbindShader();


	//Performs high pass on the first render target
	//Basically culls unwanted info
	BindShader(1);
	BindColourAsTexture(0, 0, 0);
	_shaders[1]->SetUniform("uThreshold", _threshold);

	_buffers[1]->RenderToFSQ();

	UnbindTexture(0);
	UnbindShader();


	//Computes blur, vertical and horizontal
	for (unsigned i = 0; i < _passes; ++i) {
		//Horizontal pass
		BindShader(2);
		BindColourAsTexture(1, 0, 0);
		_shaders[2]->SetUniform("uPixelSize", _pixelSize.x);

		_buffers[2]->RenderToFSQ();

		UnbindTexture(0);
		UnbindShader();

		//Vertical pass
		BindShader(3);
		BindColourAsTexture(2, 0, 0);
		_shaders[3]->SetUniform("uPixelSize", _pixelSize.y);

		_buffers[1]->RenderToFSQ();

		UnbindTexture(0);
		UnbindShader();
	}


	//Composite the scene and the bloom
	BindShader(4);

	buffer->BindColourAsTexture(0, 0, 0);
	BindColourAsTexture(1, 0, 1);

	_buffers[0]->RenderToFSQ();

	UnbindTexture(1);
	UnbindTexture(0);

	UnbindShader();
}

void BloomEffect::Reshape(unsigned width, unsigned height) {
	_buffers[0]->Reshape(width, height);
	_buffers[1]->Reshape(unsigned(width / _downscale), unsigned(height / _downscale));
	_buffers[2]->Reshape(unsigned(width / _downscale), unsigned(height / _downscale));
	_buffers[3]->Reshape(width, height);
}

float BloomEffect::GetDownscale() const {
	return _downscale;
}

float BloomEffect::GetThreshold() const {
	return _threshold;
}

unsigned BloomEffect::GetPasses() const {
	return _passes;
}

void BloomEffect::SetDownscale(float downscale) {
	_downscale = downscale;
	Reshape(_buffers[0]->_width, _buffers[0]->_height);
}

void BloomEffect::SetThreshold(float threshold) {
	_threshold = threshold;
}

void BloomEffect::SetPasses(unsigned passes) {
	_passes = passes;
}