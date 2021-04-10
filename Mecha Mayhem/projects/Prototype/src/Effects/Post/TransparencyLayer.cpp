#include "TransparencyLayer.h"

void TransparencyLayer::Init(unsigned width, unsigned height)
{
	//a buffer to draw on buffer
	int index = int(_buffers.size());
	if (index == 0) {
		_buffers.push_back(new Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA8);
		_buffers[index]->Init(width, height);

		//only reason a second buffer exists: we need the depth buffer we draw to
		index = int(_buffers.size());
		_buffers.push_back(new Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA8);
		_buffers[index]->AddDepthTarget();
		_buffers[index]->Init(width, height);
	}

	//Loads the shaders
	index = int(_shaders.size());
	if (index == 0) {
		_shaders.push_back(GetShader("shaders/Post/passthrough_frag.glsl"));
		_shaders.push_back(GetShader("shaders/Post/transparency_frag.glsl"));
	}
}

void TransparencyLayer::ApplyEffect(PostEffect* buffer, GBuffer* gBuffer)
{
	//use gBuffer to compare depth when combining
	BindShader(1);

	buffer->BindColorAsTexture(0, 0, 0);
	BindColorAsTexture(1, 0, 1);
	gBuffer->GetGBuffer().BindDepthAsTexture(2);
	BindDepthAsTexture(1, 3);

	_buffers[0]->RenderToFSQ();

	UnbindTexture(3);
	UnbindTexture(2);
	UnbindTexture(1);
	UnbindTexture(0);

	UnbindShader();
}
