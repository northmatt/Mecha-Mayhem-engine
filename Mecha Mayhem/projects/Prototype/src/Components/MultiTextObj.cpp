#include "MultiTextObj.h"
#include "Utilities/stringTrimming.h"

std::vector<MultiTextObj::Models> MultiTextObj::m_models = {};

struct Materials
{
	std::string name;
	glm::vec3 colours;
	glm::vec3 specStrength;
	bool isText = false;
};

struct BufferData
{
	std::vector<size_t> vertex = {};
	std::vector<size_t> UV = {};
	std::vector<size_t> normals = {};
	std::vector<size_t> colours = {};
};

MultiTextObj::MultiTextObj(const std::string& fileName)
{
	LoadMesh(fileName);
}

MultiTextObj::~MultiTextObj()
{
}

void MultiTextObj::Unload()
{
	for (int i(0); i < m_models.size(); ++i) {
		for (int j(0); j < m_models[i].vao.size(); ++j) {
			m_models[i].vao[j] = nullptr;
		}
		m_models[i].vao.clear();
		m_models[i].texture.clear();
	}
	m_models.clear();
}

MultiTextObj& MultiTextObj::LoadMesh(const std::string& fileName)
{
	for (int count(0); count < m_models.size(); ++count) {
		if (m_models[count].fileName == fileName) {
			m_index = count;

			return *this;
		}
	}

	size_t ind = m_models.size();
	m_models.push_back({ fileName });

	std::ifstream file;
	file.open(fileName, std::ios::binary);

	if (!file)
	{
		throw std::runtime_error("Failed to open file\nError 69: " + fileName);
	}

	std::vector<Materials> materials = {};
	{
		std::ifstream materialFile;
		std::string materialFileName = fileName.substr(0, fileName.size() - 3) + "mtl";
		materialFile.open(materialFileName, std::ios::binary);
		if (!materialFile)
		{
			throw std::runtime_error("Failed to open material file, maybe it doesn't have one\nError 420: " + materialFileName);
		}

		std::string matLine;
		size_t matIndex = 0;
		float tempExponent = 1.f, tempTrans = 1;
		while (std::getline(materialFile, matLine))
		{
			stringTrimming::ltrim(matLine);
			if (matLine.substr(0, 6) == "newmtl")
			{
				if (matIndex > 0) if (materials[matIndex].isText == false)
					throw std::runtime_error("we don't do no texture in multitexture");
				matIndex = materials.size();
				materials.push_back({ matLine.substr(7), glm::vec3(1.f), glm::vec3(1.f) });
				tempExponent = tempTrans = 1;
			}
			else if (matLine.substr(0, 6) == "map_Kd")
			{
				std::string textureName = matLine.substr(7);
				bool dupt = false;
				for (int i(0); i < Sprite::m_textures.size(); ++i) {
					if (Sprite::m_textures[i].fileName == textureName) {
						m_models[ind].texture.push_back(i);
						dupt = true;
						break;
					}
				}
				if (!dupt) {
					if (textureName == ".") {
						Texture2DDescription desc = Texture2DDescription();
						desc.Width = 1;
						desc.Height = 1;
						desc.GenerateMipMaps = false;
						desc.MinificationFilter = MinFilter::Nearest;
						desc.MagnificationFilter = MagFilter::Nearest;
						desc.Format = InternalFormat::RGBA8;
						Texture2D::sptr texture = Texture2D::Create(desc);

						texture->Clear(glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
						//texture->Clear(glm::vec4(0.1f, 0.1f, 0.1f, 0.5f));

						m_models[ind].texture.push_back(Sprite::m_textures.size());
						Sprite::m_textures.push_back({ textureName, texture });
					}
					else {
						Texture2D::sptr tex = Texture2D::LoadFromFile("textures/" + textureName);
						if (!tex)
						{
							throw std::runtime_error("Failed to open texture\nError 0: " + textureName);
						}
						m_models[ind].texture.push_back(Sprite::m_textures.size());
						Sprite::m_textures.push_back({ textureName, tex });
					}
				}
				materials[matIndex].isText = true;
			}
			else if (matLine.substr(0, 2) == "Ns")
			{
				std::istringstream ss = std::istringstream(matLine.substr(2));
				ss >> tempExponent;
			}
			else if (matLine.substr(0, 5) == "trans")
			{
				std::istringstream ss = std::istringstream(matLine.substr(5));
				ss >> tempTrans;
			}
			else if (matLine.size() > 1 && matLine[0] == 'K')
			{
				//only colour data is taken for now, textures later
				if (matLine[1] == 'd')
				{
					//diffuse Colour, aka object colour
					std::istringstream ss = std::istringstream(matLine.substr(2));
					glm::vec3 colour;
					ss >> colour.x >> colour.y >> colour.z;

					materials[matIndex].colours = colour;
				}
				else if (matLine[1] == 's')
				{
					//specular Colour
					//diffuse Colour, aka object colour
					std::istringstream ss = std::istringstream(matLine.substr(2));
					glm::vec3 colour;
					ss >> colour.x >> colour.y >> colour.z;

					materials[matIndex].specStrength = glm::vec3(glm::length(colour), tempExponent, tempTrans);
				}
				else if (matLine[1] == 'a')
				{
					//ambient Colour, ignored
				}
			}
			else if (matLine[0] == '#')
			{
				//it's a comment lol i cant even, tho maybe i can
			}
		}
		materialFile.close();
	}

	std::vector<glm::vec3> vertex = { glm::vec3(0.f) };
	std::vector<glm::vec2> UV = { glm::vec2(0.f) };
	std::vector<glm::vec3> normals = { glm::vec3(0.f) };
	
	std::vector<BufferData> buff = {};
	buff.resize(materials.size());
	//index of material vector

	// stringStream was learnt throught the NotObjLoader, thank you Shawn once again
	std::string line;
	glm::size_t currentColour = 0;
	//size_t vectorIndex = 0;

	bool noDraw = false;
	while (std::getline(file, line))
	{
		stringTrimming::trim(line);

		if (line[0] == 'v')
		{
			if (line[1] == ' ')
			{
				//Vertices			v xvalue yvalue zvalue
				std::istringstream ss = std::istringstream(line.substr(1));
				glm::vec3 pos;
				ss >> pos.x >> pos.y >> pos.z;

				vertex.push_back(pos);
			}
			else if (line[1] == 't')
			{
				//UVs, we do care rn			vt xvalue yvalue
				std::istringstream ss = std::istringstream(line.substr(2));
				glm::vec2 uv;
				ss >> uv.x >> uv.y;

				UV.push_back(uv);
			}
			else if (line[1] == 'n')
			{
				//Normals
				std::istringstream ss = std::istringstream(line.substr(2));
				//is Kinda cringe
				glm::vec3 n;
				ss >> n.x >> n.y >> n.z;

				normals.push_back(n);
			}

		}
		else if (line[0] == 'f' && !noDraw)
		{
			//faces
			//indices, seperated by /
			size_t offset = 2;

			//cause 3 vertices times 3 values
			for (int i = 0; i < 9; ++i)
			{
				size_t lengthIndex = 0;
				for (; isalnum(line[offset + lengthIndex]); ++lengthIndex);
				size_t index = stoi(line.substr(offset, offset + lengthIndex));

				offset += 1 + lengthIndex;

				if (i % 3 == 0)
				{
					//it's the vertex index
					buff[currentColour].vertex.push_back(index);
					buff[currentColour].colours.push_back(currentColour);
				}
				else if (i % 3 == 1)
				{
					//it's the UV index, we do care
					buff[currentColour].UV.push_back(index);
				}
				else if (i % 3 == 2)
				{
					//it's the normals index
					buff[currentColour].normals.push_back(index);
				}
			}

			while (offset < line.size())
			{

				buff[currentColour].vertex.push_back(buff[currentColour].vertex[buff[currentColour].vertex.size() - 3]);
				buff[currentColour].vertex.push_back(buff[currentColour].vertex[buff[currentColour].vertex.size() - 2]);
				buff[currentColour].UV.push_back(buff[currentColour].UV[buff[currentColour].UV.size() - 3]);
				buff[currentColour].UV.push_back(buff[currentColour].UV[buff[currentColour].UV.size() - 2]);
				buff[currentColour].normals.push_back(buff[currentColour].normals[buff[currentColour].normals.size() - 3]);
				buff[currentColour].normals.push_back(buff[currentColour].normals[buff[currentColour].normals.size() - 2]);
				buff[currentColour].colours.push_back(currentColour);
				buff[currentColour].colours.push_back(currentColour);

				for (int j(0); j < 3; ++j)
				{
					size_t lengthIndex = 0;
					for (; isalnum(line[offset + lengthIndex]); ++lengthIndex);
					size_t index = stoi(line.substr(offset, offset + lengthIndex));

					offset += 1 + lengthIndex;

					if (j == 0)
					{
						//it's the vertex index
						buff[currentColour].vertex.push_back(index);
						buff[currentColour].colours.push_back(currentColour);

					}
					else if (j == 1)
					{
						//it's the UV index, we do care
						buff[currentColour].UV.push_back(index);

					}
					else if (j == 2)
					{
						//it's the normals index
						buff[currentColour].normals.push_back(index);

					}
				}

			}
		}
		else if (line.substr(0, 6) == "usemtl")
		{
			//usemtl
			std::string materialName = line.substr(7);
			for (int j = 0; j < materials.size(); ++j)
			{
				if (materialName == materials[j].name) {
					currentColour = j;
					break;
				}
			}
			noDraw = (materialName == "tools/toolsnodraw");
		}
		else if (line[0] == '#')
		{
			//it's a comment lol i cant even
		}
	}

	file.close();
	
	for (int vaoCount = 0; vaoCount < buff.size(); ++vaoCount) {
		BufferData& buffer = buff[vaoCount];

		std::vector<float> position_data = {};
		std::vector<float> colour_data = {};
		std::vector<float> spec_data = {};
		std::vector<float> UV_data = {};
		std::vector<float> normal_data = {};

		for (size_t i = 0; i < buffer.vertex.size(); ++i) {
			position_data.push_back(vertex[buffer.vertex[i]].x);
			position_data.push_back(vertex[buffer.vertex[i]].y);
			position_data.push_back(vertex[buffer.vertex[i]].z);
			colour_data.push_back(materials[buffer.colours[i]].colours.x);
			colour_data.push_back(materials[buffer.colours[i]].colours.y);
			colour_data.push_back(materials[buffer.colours[i]].colours.z);
			spec_data.push_back(materials[buffer.colours[i]].specStrength.x);
			spec_data.push_back(materials[buffer.colours[i]].specStrength.y);
			spec_data.push_back(materials[buffer.colours[i]].specStrength.z);
			UV_data.push_back(UV[buffer.UV[i]].x);
			UV_data.push_back(UV[buffer.UV[i]].y);
			UV_data.push_back(0.f);
			normal_data.push_back(normals[buffer.normals[i]].x);
			normal_data.push_back(normals[buffer.normals[i]].y);
			normal_data.push_back(normals[buffer.normals[i]].z);
		}

		VertexBuffer::sptr position_vbo = VertexBuffer::Create();
		position_vbo->LoadData(position_data.data(), position_data.size());
		VertexBuffer::sptr normal_vbo = VertexBuffer::Create();
		normal_vbo->LoadData(normal_data.data(), normal_data.size());
		VertexBuffer::sptr colour_vbo = VertexBuffer::Create();
		colour_vbo->LoadData(colour_data.data(), colour_data.size());
		VertexBuffer::sptr spec_vbo = VertexBuffer::Create();
		spec_vbo->LoadData(spec_data.data(), spec_data.size());
		VertexBuffer::sptr UV_vbo = VertexBuffer::Create();
		UV_vbo->LoadData(UV_data.data(), UV_data.size());

		VertexArrayObject::sptr newVao = VertexArrayObject::Create();

		newVao->AddVertexBuffer(position_vbo, ObjLoader::m_posAttrib, true);
		newVao->AddVertexBuffer(normal_vbo, ObjLoader::m_normAttrib, true);
		newVao->AddVertexBuffer(colour_vbo, ObjLoader::m_colAttrib, true);
		newVao->AddVertexBuffer(spec_vbo, ObjLoader::m_specAttrib, true);
		newVao->AddVertexBuffer(UV_vbo, ObjLoader::m_uvAttrib, true);

		m_models[ind].vao.push_back(newVao);
	}
	m_index = ind;

	return *this;
}

void MultiTextObj::Draw(const glm::mat4& model, const glm::mat4& view, const Camera& camera,
	const glm::vec3& colour, const std::array<glm::vec3, MAX_LIGHTS>& lightPos, const std::array<glm::vec3, MAX_LIGHTS>& lightColour, const int& lightCount,
	float specularStrength, float shininess,
	float ambientLightStrength, const glm::vec3& ambientColour, float ambientStrength)
{
	glm::mat4 VP = camera.GetProjection() * view;

	ObjLoader::m_texShader->Bind();
	ObjLoader::m_texShader->SetUniform("camPos", camera.GetPosition());

	ObjLoader::m_texShader->SetUniform("lightsPos", *lightPos.data(), MAX_LIGHTS);
	ObjLoader::m_texShader->SetUniform("lightsColour", *lightColour.data(), MAX_LIGHTS);
	ObjLoader::m_texShader->SetUniform("lightCount", lightCount);

	ObjLoader::m_texShader->SetUniform("ambientLightStrength", ambientLightStrength);
	ObjLoader::m_texShader->SetUniform("ambientColour", ambientColour);
	ObjLoader::m_texShader->SetUniform("ambientStrength", ambientStrength);

	ObjLoader::m_texShader->SetUniform("addColour", glm::vec3(0.f));

	for (int i(0); i < m_models[m_index].vao.size(); ++i) {
		ObjLoader::m_texShader->SetUniformMatrix("MVP", VP * model);
		ObjLoader::m_texShader->SetUniformMatrix("transform", model);

		Sprite::m_textures[m_models[m_index].texture[i]].texture->Bind(0);

		m_models[m_index].vao[i]->Render();
	}
	Shader::UnBind();

}

void MultiTextObj::DrawShadow(const glm::mat4& model)
{
	ObjLoader::m_shadowShader->Bind();
	//the light matrix has already been sent

	for (int i(0); i < m_models[m_index].vao.size(); ++i) {
		ObjLoader::m_shadowShader->SetUniformMatrix("model", model);

		m_models[m_index].vao[i]->Render();
	}
	Shader::UnBind();
}
