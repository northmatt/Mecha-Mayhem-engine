#include "BloomEffect.h"

void BloomEffect::Init(unsigned width, unsigned height)
{
	int index = int(_buffers.size());
	if (index == 0) {
		//bright pass/final draw buffer
		_buffers.push_back(new Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA8);
		_buffers[index]->Init(width, height);

		//horizontal blur pass
		++index;
		_buffers.push_back(new Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA8);
		_buffers[index]->Init(width / _scalar, height / _scalar);

		//vertical blur pass
		++index;
		_buffers.push_back(new Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA8);
		_buffers[index]->Init(width / _scalar, height / _scalar);
	}

	//Loads the shaders
	index = int(_shaders.size());
	if (index == 0) {
		_shaders.push_back(GetShader("shaders/Post/passthrough_frag.glsl"));
		_shaders.push_back(GetShader("shaders/Post/bloom_bright_pass.glsl"));
		_shaders.push_back(GetShader("shaders/Post/bloom_horizontal_blur.glsl"));
		_shaders.push_back(GetShader("shaders/Post/bloom_vertical_blur.glsl"));
		_shaders.push_back(GetShader("shaders/Post/bloom_combine_pass.glsl"));
	}
}

void BloomEffect::ApplyEffect(PostEffect* buffer)
{
	//bright pass
	BindShader(1);
	_shaders[1]->SetUniform("u_Threshold", _threshold);

	buffer->BindColorAsTexture(0, 0, 0);

	_buffers[1]->RenderToFSQ();

	buffer->UnbindTexture(0);

	UnbindShader();

	_shaders[2]->SetUniform("u_Spread", _radius);
	_shaders[3]->SetUniform("u_Spread", _radius);

	for (int i(0); i < _blurCount; ++i) {
		//hori
		BindShader(2);

		_buffers[1]->BindColorAsTexture(0, 0);

		_buffers[2]->RenderToFSQ();

		_buffers[1]->UnbindTexture(0);

		UnbindShader();

		//verti
		BindShader(3);

		_buffers[2]->BindColorAsTexture(0, 0);

		_buffers[1]->RenderToFSQ();

		_buffers[2]->UnbindTexture(0);

		UnbindShader();
	}

	BindShader(4);

	buffer->BindColorAsTexture(0, 0, 0);
	_buffers[1]->BindColorAsTexture(0, 1);

	_buffers[0]->RenderToFSQ();

	_buffers[1]->UnbindTexture(0);
	buffer->UnbindTexture(0);

	UnbindShader();
}

void BloomEffect::Reshape(unsigned width, unsigned height)
{
	_buffers[0]->Reshape(width, height);
	_buffers[1]->Reshape(width / _scalar, height / _scalar);
	_buffers[2]->Reshape(width / _scalar, height / _scalar);

	_shaders[2]->SetUniform("u_TexelSize", 1.f / width);
	_shaders[3]->SetUniform("u_TexelSize", 1.f / height);
}

void BloomEffect::SetThreshold(float threshold)
{
	_threshold = threshold;
}

float BloomEffect::GetTreshold() const
{
	return _threshold;
}

void BloomEffect::SetRadius(float radius)
{
	_radius = radius;
}

float BloomEffect::GetRadius() const
{
	return _radius;
}

void BloomEffect::SetBlurCount(int amt)
{
	_blurCount = amt;
}

int BloomEffect::GetBlurCount() const
{
	return _blurCount;
}
