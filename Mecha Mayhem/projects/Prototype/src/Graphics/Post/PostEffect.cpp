#include "PostEffect.h"

void PostEffect::Init(unsigned width, unsigned height) {
	size_t index = 0;
	if (!_shaders.size() > 0) {
		index = _buffers.size();
		_buffers.push_back(new Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA8);
		_buffers[index]->AddDepthTarget();
		_buffers[index]->Init(width, height);
	}

	index = _shaders.size();
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[index]->LoadShaderPartFromFile("shaders/passthrough_frag.glsl", GL_FRAGMENT_SHADER);
	_shaders[index]->Link();
}

void PostEffect::ApplyAffects(PostEffect* previousBuffer) {
	BindShader(0);
	previousBuffer->BindColourAsTexture(0, 0, 0);

	_buffers[0]->RenderToFSQ();

	previousBuffer->UnbindTexture(0);
	UnbindShader();
}

void PostEffect::DrawToScreen() {
	BindShader(0);
	BindColourAsTexture(0, 0, 0);

	_buffers[0]->DrawFullscreenQuad();

	UnbindTexture(0);
	UnbindShader();
}

void PostEffect::Reshape(unsigned width, unsigned height) {
	for (size_t i = 0; i < _buffers.size(); ++i) {
		_buffers[i]->Reshape(width, height);
	}
}

void PostEffect::Clear() {
	for (size_t i = 0; i < _buffers.size(); ++i) {
		_buffers[i]->Clear();
	}
}

void PostEffect::Unload() {
	for (size_t i = 0; i < _buffers.size(); ++i) {
		if (_buffers[i] != nullptr) {
			_buffers[i]->Unload();
			delete _buffers[i];
			_buffers[i] = nullptr;
		}
	}

	_shaders.clear();
}

void PostEffect::BindBuffer(size_t index) {
	_buffers[index]->Bind();
}

void PostEffect::UnbindBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void PostEffect::BindColourAsTexture(size_t index, int colorBuffer, int textureSlot) {
	_buffers[index]->BindColorAsTexture(colorBuffer, textureSlot);
}

void PostEffect::BindDepthAsTexture(size_t index, int textureSlot) {
	_buffers[index]->BindDepthAsTexture(textureSlot);
}

void PostEffect::UnbindTexture(int textureSlot) {
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void PostEffect::BindShader(size_t index) {
	_shaders[index]->Bind();
}

void PostEffect::UnbindShader() {
	glUseProgram(GL_NONE);
}
