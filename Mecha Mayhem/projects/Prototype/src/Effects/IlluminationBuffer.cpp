#include "IlluminationBuffer.h"

void IlluminationBuffer::Init(unsigned width, unsigned height)
{
	sphereThingy = ObjLoader("models/Light_Mesh_Coverage.obj");
	cubeThingy = ObjLoader("models/Light_Mesh_Coverage_C.obj");
	coneThingy = ObjLoader("models/Light_Mesh_Coverage_Co.obj");
	//composite buffer
	int index = int(_buffers.size());
	if (index == 0) {
		_buffers.push_back(new Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA8);
		_buffers[index]->AddDepthTarget();
		_buffers[index]->Init(width, height);

		//light accum buffer (stored in a RGBA16F for floating point numbers)
		index = int(_buffers.size());
		_buffers.push_back(new Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA16F);
		_buffers[index]->AddDepthTarget();
		_buffers[index]->Init(width, height);

		//allocates sun buffer
		_sunBuffer.AllocateMemory(sizeof(DirectionalLight));

		//if sun enabled, send data
		if (_sunEnabled) {
			_sunBuffer.SendData(reinterpret_cast<void*>(&_sun), sizeof(DirectionalLight));
		}
	}

	//Loads the shaders
	index = int(_shaders.size());
	if (index == 0) {
		_shaders.push_back(GetShader("shaders/Post/passthrough_frag.glsl"));
		_shaders.push_back(GetShader("shaders/Post/gBuffer_directional_frag.glsl"));
		_shaders.push_back(GetShader("shaders/Post/gBuffer_ambient_frag.glsl"));
		_shaders.push_back(GetShader("shaders/Post/unlit.glsl"));
	}
}

void IlluminationBuffer::ApplyEffect(GBuffer* gBuffer)
{
	if (_sunEnabled) {
		//bind directional light shader
		_shaders[Lights::DIRECTIONAL]->Bind();

		//the shadow texture is sent elsewhere
		_shaders[Lights::DIRECTIONAL]->SetUniformMatrix("u_LightSpaceMatrix", _lightSpaceViewProj);

		_shaders[Lights::DIRECTIONAL]->SetUniform("u_camPos", *_camPos.data(), _camCount);
		_shaders[Lights::DIRECTIONAL]->SetUniform("camCount", _camCount);

		_sunBuffer.Bind(0);

		gBuffer->BindLighting();
		
		_buffers[1]->RenderToFSQ();

		gBuffer->UnbindLighting();

		_sunBuffer.Unbind(0);

		_shaders[Lights::DIRECTIONAL]->UnBind();
	}

	//insert all other lighting here
	/*
	if (_lights.size()) {
		//bind point light shader
		_shaders[Lights::POINT]->Bind();
		_shaders[Lights::POINT]->SetUniform("u_camPos", *_camPos.data(), 4);
		_shaders[Lights::POINT]->SetUniform("camCount", _camCount);
		for (int i(0); i < _lights.size(); ++i) {
			_lights[i].buffer.SendData(reinterpret_cast<void*>(&_lights[i].point), sizeof(PointLight));

			_lights[i].buffer.Bind(0);

			gBuffer->BindLighting();
			//bind the light accum buffer to keep adding to it
			_buffers[1]->BindColorAsTexture(0, 4);
			_buffers[1]->RenderToFSQ();
			_buffers[1]->UnbindTexture(4);

			gBuffer->UnbindLighting();

			_lights[i].buffer.Unbind(0);


		}
		_shaders[Lights::POINT]->UnBind();
	}
	*/


	_shaders[Lights::AMBIENT]->Bind();
	//for rim lighting
	_shaders[Lights::AMBIENT]->SetUniform("u_camPos", *_camPos.data(), _camCount);
	_shaders[Lights::AMBIENT]->SetUniform("camCount", _camCount);

	_sunBuffer.Bind(0);

	gBuffer->BindLighting();
	_buffers[1]->BindColorAsTexture(0, 5);

	_buffers[0]->RenderToFSQ();

	_buffers[1]->UnbindTexture(5);
	gBuffer->UnbindLighting();

	_sunBuffer.Unbind(0);

	_shaders[Lights::AMBIENT]->UnBind();

	if (!_drawMeshes)
		return;

	//draw the volumes here if we get them working lol
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->GetGBuffer().GetFBO());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _buffers[0]->GetFBO());
	glBlitFramebuffer(0, 0, gBuffer->GetSize().x, gBuffer->GetSize().y, 0, 0, gBuffer->GetSize().x, gBuffer->GetSize().y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	_buffers[0]->Bind();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	_shaders[Lights::UNLIT]->Bind();

	glm::mat4 model = glm::mat4(1.0f);
    for (size_t i = 0; i < lCnt; ++i) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, lPos[i]);
		float radius = (std::sqrtf(-4 * (-(256.0 / 5.0) * glm::length(lCol[i])))) / 2;
        model = glm::scale(model, glm::vec3(radius));
		_shaders[Lights::UNLIT]->SetUniformMatrix("MVP", _camVP * model);
		_shaders[Lights::UNLIT]->SetUniform("colour", lCol[i]);

		switch (thingNum)
		{
		case 0:
			sphereThingy.GetVAO()->Render();
			break;
		case 1:
			cubeThingy.GetVAO()->Render();
			break;
		case 2:
			coneThingy.GetVAO()->Render();
			break;
		default:
			break;
		}
    }

	_shaders[Lights::UNLIT]->UnBind();
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	_buffers[0]->Unbind();
}

void IlluminationBuffer::DrawIllumBuffer()
{
	_shaders[0]->Bind();

	_buffers[1]->BindColorAsTexture(0, 0);

	Framebuffer::DrawFullscreenQuad();

	_buffers[1]->UnbindTexture(0);

	_shaders[0]->UnBind();
}


void IlluminationBuffer::SetLightSpaceViewProj(glm::mat4 lightSpaceViewProj)
{
	_lightSpaceViewProj = lightSpaceViewProj;
}

void IlluminationBuffer::SetCamPos(glm::vec3 camPos, int camNum)
{
	if (camNum > 3 || camNum < 0)	return;

	_camPos[camNum] = camPos;
}

void IlluminationBuffer::SetCamVP(glm::mat4 VP) {
	_camVP = VP;
}

void IlluminationBuffer::SetCamCount(int camNum)
{
	_camCount = camNum;
}

DirectionalLight& IlluminationBuffer::GetSunRef()
{
	return _sun;
}

void IlluminationBuffer::SetSun(DirectionalLight newSun)
{
	_sun = newSun;

	//send the directional light data and bind it
	_sunBuffer.SendData(reinterpret_cast<void*>(&_sun), sizeof(DirectionalLight));
}

void IlluminationBuffer::SetSun(glm::vec4 lightDir, glm::vec4 lightCol)
{
	_sun._lightDirection = lightDir;
	_sun._lightCol = lightCol;

	//send the directional light data and bind it
	_sunBuffer.SendData(reinterpret_cast<void*>(&_sun), sizeof(DirectionalLight));
}

void IlluminationBuffer::EnableSun(bool enabled)
{
	_sunEnabled = enabled;
}

bool IlluminationBuffer::GetSunEnabled() const
{
	return _sunEnabled;
}
