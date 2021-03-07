#include "PixelEffect.h"
#include "Engine/BackEnd.h"

void PixelEffect::Init(unsigned width, unsigned height)
{
	int index = int(_buffers.size());
	if (index == 0) {
		_buffers.push_back(new Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA8);
		_buffers[index]->Init(width, height);

		float aspectRatio = float(width) / float(height);

		++index;
		//smaller one
		_buffers.push_back(new Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA8);
		_buffers[index]->Init(_pixelCount * aspectRatio, _pixelCount);
	}

	//Loads the shaders
	index = int(_shaders.size());
	if (index == 0) {
		_shaders.push_back(GetShader("shaders/Post/passthrough_frag.glsl"));
	}
}

void PixelEffect::ApplyEffect(PostEffect* buffer)
{
	BindShader(0);

	buffer->BindColorAsTexture(0, 0, 0);

	_buffers[1]->RenderToFSQ();

	buffer->UnbindTexture(0);

	UnbindShader();


	//second pass to fix size
	BindShader(0);

	_buffers[1]->BindColorAsTexture(0, 0);

	_buffers[0]->RenderToFSQ();

	_buffers[1]->UnbindTexture(0);

	UnbindShader();
}

void PixelEffect::Reshape(unsigned width, unsigned height)
{
	_buffers[0]->Reshape(width, height);
	float aspectRatio = float(width) / float(height);
	_buffers[1]->Reshape(_pixelCount * aspectRatio, _pixelCount);
}

void PixelEffect::SetPixelCount(int amt)
{
	_pixelCount = amt;
	Reshape(BackEnd::GetWidth(), BackEnd::GetHeight());
}

int PixelEffect::GetPixelCount() const
{
	return _pixelCount;
}
