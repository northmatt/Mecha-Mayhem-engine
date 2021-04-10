#include "DepthOfField.h"

void DepthOfFieldEffect::Init(unsigned width, unsigned height)
{
	int index = int(_buffers.size());
	if (index == 0) {
		//draw buffer
		_buffers.push_back(new Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA8);
		_buffers[index]->Init(width, height);

		//blur buffer
		index = int(_buffers.size());
		_buffers.push_back(new Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA8);
		_buffers[index]->Init(width * 0.5f, height * 0.5f);

		index = int(_buffers.size());
		_buffers.push_back(new Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA8);
		_buffers[index]->Init(width * 0.5f, height * 0.5f);
	}

	//Loads the shaders
	index = int(_shaders.size());
	if (index == 0) {
		_shaders.push_back(GetShader("shaders/Post/passthrough_frag.glsl"));
		_shaders.push_back(GetShader("shaders/Post/dof_extract.glsl"));
		_shaders.push_back(GetShader("shaders/Post/dof_deletion.glsl"));
		_shaders.push_back(GetShader("shaders/Post/dof_horizontal_blur.glsl"));
		_shaders.push_back(GetShader("shaders/Post/dof_vertical_blur.glsl"));
		_shaders.push_back(GetShader("shaders/Post/dof_combine.glsl"));
	}

	_shaders[3]->SetUniform("u_TexelSize", 2.f / width);
	_shaders[4]->SetUniform("u_TexelSize", 2.f / height);
}

void DepthOfFieldEffect::ApplyEffect(PostEffect* buffer)
{
	if (_depth >= 1 || _drawBuffer == nullptr) {
		//do a passthrough
		BindShader(0);

		buffer->BindColorAsTexture(0, 0, 0);

		_buffers[0]->RenderToFSQ();

		buffer->UnbindTexture(0);

		UnbindShader();

		return;
	}

	//extract near
	BindShader(1);
	_shaders[1]->SetUniform("depthLimit", _depth);

	buffer->BindColorAsTexture(0, 0, 0);
	_drawBuffer->BindDepthAsTexture(1);

	_buffers[0]->RenderToFSQ();

	_drawBuffer->UnbindTexture(1);
	buffer->UnbindTexture(0);

	UnbindShader();

	//draw to smaller buffer while deleting the near
	BindShader(2);

	buffer->BindColorAsTexture(0, 0, 0);
	_buffers[0]->BindColorAsTexture(0, 1);

	_buffers[1]->RenderToFSQ();

	_buffers[0]->UnbindTexture(1);
	buffer->UnbindTexture(0);

	UnbindShader();

	//blur
	for (int i(0); i < _blurPasses; ++i) {
		//hori
		BindShader(3);

		_buffers[1]->BindColorAsTexture(0, 0);

		_buffers[2]->RenderToFSQ();

		_buffers[1]->UnbindTexture(0);

		UnbindShader();

		//verti
		BindShader(4);

		_buffers[2]->BindColorAsTexture(0, 0);

		_buffers[1]->RenderToFSQ();

		_buffers[2]->UnbindTexture(0);

		UnbindShader();
	}

	//combine
	BindShader(5);

	_buffers[0]->BindColorAsTexture(0, 0);
	_buffers[1]->BindColorAsTexture(0, 1);

	_buffers[0]->RenderToFSQ();

	_buffers[1]->UnbindTexture(1);
	_buffers[0]->UnbindTexture(0);

	UnbindShader();
}

void DepthOfFieldEffect::Reshape(unsigned width, unsigned height)
{
	_buffers[0]->Reshape(width, height);
	_buffers[1]->Reshape(width * 0.5f, height * 0.5f);
	_buffers[2]->Reshape(width * 0.5f, height * 0.5f);

	_shaders[3]->SetUniform("u_TexelSize", 2.f / width);
	_shaders[4]->SetUniform("u_TexelSize", 2.f / height);
}

void DepthOfFieldEffect::SetDepthLimit(float depth)
{
	_depth = depth;
}

float DepthOfFieldEffect::GetDepthLimit() const
{
	return _depth;
}

void DepthOfFieldEffect::SetBlurPasses(int passes)
{
	_blurPasses = passes;
}

int DepthOfFieldEffect::GetBlurPasses() const
{
	return _blurPasses;
}
