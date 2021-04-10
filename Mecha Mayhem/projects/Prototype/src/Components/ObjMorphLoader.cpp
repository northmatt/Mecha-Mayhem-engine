#include "ObjMorphLoader.h"
#include "Utilities/stringTrimming.h"

std::vector<ObjMorphLoader::DrawData> ObjMorphLoader::m_transQueue = {};
std::vector<ObjMorphLoader::DrawData> ObjMorphLoader::m_texQueue = {};
std::vector<ObjMorphLoader::DrawData> ObjMorphLoader::m_matQueue = {};
std::vector<ObjMorphLoader::DrawData> ObjMorphLoader::m_defaultQueue = {};
std::vector<ObjMorphLoader::DrawData> ObjMorphLoader::m_texTempQueue = {};
std::vector<ObjMorphLoader::DrawData> ObjMorphLoader::m_matTempQueue = {};
std::vector<ObjMorphLoader::DrawData> ObjMorphLoader::m_defaultTempQueue = {};
std::vector<ObjMorphLoader::Animations> ObjMorphLoader::m_anims = {};
const std::vector<BufferAttribute> ObjMorphLoader::pos1Buff = { BufferAttribute(0, 3, GL_FLOAT, false, NULL, NULL) };
const std::vector<BufferAttribute> ObjMorphLoader::pos2Buff = { BufferAttribute(1, 3, GL_FLOAT, false, NULL, NULL) };
const std::vector<BufferAttribute> ObjMorphLoader::norm1Buff = { BufferAttribute(2, 3, GL_FLOAT, false, NULL, NULL) };
const std::vector<BufferAttribute> ObjMorphLoader::norm2Buff = { BufferAttribute(3, 3, GL_FLOAT, false, NULL, NULL) };
const std::vector<BufferAttribute> ObjMorphLoader::col1Buff = { BufferAttribute(4, 3, GL_FLOAT, false, NULL, NULL) };
const std::vector<BufferAttribute> ObjMorphLoader::col2Buff = { BufferAttribute(5, 3, GL_FLOAT, false, NULL, NULL) };
const std::vector<BufferAttribute> ObjMorphLoader::spec1Buff = { BufferAttribute(6, 3, GL_FLOAT, false, NULL, NULL) };
const std::vector<BufferAttribute> ObjMorphLoader::spec2Buff = { BufferAttribute(7, 3, GL_FLOAT, false, NULL, NULL) };
const std::vector<BufferAttribute> ObjMorphLoader::UVBuff = { BufferAttribute(8, 3, GL_FLOAT, false, NULL, NULL) };
Shader::sptr ObjMorphLoader::m_shader = nullptr;
Shader::sptr ObjMorphLoader::m_matShader = nullptr;
Shader::sptr ObjMorphLoader::m_texShader = nullptr;
Shader::sptr ObjMorphLoader::m_shadowShader = nullptr;

struct Materials
{
	std::string name;
	glm::vec3 colours;
	glm::vec3 specStrength;
	bool isText = false;
};

ObjMorphLoader::ObjMorphLoader(const std::string& baseFileName, bool usingMaterial)
{
	LoadMeshs(baseFileName, usingMaterial);
}

ObjMorphLoader::~ObjMorphLoader()
{
	m_vao = nullptr;
}

ObjMorphLoader& ObjMorphLoader::LoadMeshs(const std::string& baseFileName, bool usingMaterial)
{
	m_timer = m_t = m_p0 = 0;
	m_speed = 1.f;
	m_vao = nullptr;
	m_bounce = false;
	m_loop = false;
	m_reversing = false;
	m_enabled = true;

	for (int count(0); count < m_anims.size(); ++count) {
		if (m_anims[count].mat == usingMaterial) if (m_anims[count].fileName == baseFileName) {
			m_index = count;
			auto& data = m_anims[m_index];
			m_p0 = data.start;
			m_loop = data.loop;
			m_reversing = data.reversing;
			m_bounce = data.bounce;
			m_t = data.t;
			m_timer = data.startTime;
			if (m_reversing)
				m_p1 = (m_p0 == 0 ? data.frameIndices.size() - 1 : m_p0 - 1);
			else
				m_p1 = (m_p0 + 1) % data.frameIndices.size();

			m_vao = VertexArrayObject::Create();

			m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].pos, pos1Buff);
			m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].pos, pos2Buff);
			m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].normal, norm1Buff);
			m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].normal, norm2Buff);
			if (m_anims[m_index].mat) {
				m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].colour, col1Buff);
				m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].colour, col2Buff);
				m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].spec, spec1Buff);
				m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].spec, spec2Buff);
			}
			if (data.text)
				m_vao->AddVertexBuffer(data.UVs, UVBuff);


			return *this;
		}
	}

	std::ifstream dataFile;
	dataFile.open("morph/" + baseFileName + "Data.jpegs");

	if (!dataFile) {
		throw std::runtime_error("failed to open file\nno Data file for " + baseFileName);
	}

	size_t ind = m_anims.size();
	size_t maxPos = 0;
	m_anims.push_back({ baseFileName, usingMaterial });

	m_index = ind;
	auto& data = m_anims[m_index];
	{
		std::string line, input;
		int counter = 0;
		while (std::getline(dataFile, line)) {
			std::istringstream ss(line);

			while (ss >> input) {
				if (input == "anim") {
					float endTime;
					int pos;
					ss >> pos >> endTime;
					if (pos > maxPos)
						maxPos = pos;
					data.frameIndices.push_back(pos);
					data.durations.push_back(endTime);
				}
				else if (input == "bounce") {
					data.bounce = 
					m_bounce = true;
				}
				else if (input == "reverse") {
					data.reversing = 
					m_reversing = true;
				}
				else if (input == "loop") {
					data.loop = 
					m_loop = true;
				}
				else if (input == "start") {
					ss >> m_p0;
				}
				else if (input == "t") {
					ss >> m_t;
				}
				else if (input == "time") {
					ss >> m_timer;
				}
			}
		}
	}

	if (m_p0 >= data.durations.size())
		throw std::runtime_error("start index bad, change it for " + baseFileName + "\nstop it");
	else if (m_timer < 0)
		throw std::runtime_error("start time was negative in " + baseFileName + "\ndont do this");
	else {
		data.t = m_t;
		data.startTime = m_timer;
		data.start = m_p0;
		if (m_reversing)
			m_p1 = (m_p0 == 0 ? data.frameIndices.size() - 1 : m_p0 - 1);
		else
			m_p1 = (m_p0 + 1) % data.frameIndices.size();
	}

	std::vector<Materials> materials = {};

	if (usingMaterial) {
		std::ifstream materialFile;
		std::string materialFileName = "morph/" + baseFileName + ".mtl";
		materialFile.open(materialFileName, std::ios::binary);
		if (!materialFile) {
			throw std::runtime_error("Failed to open material file for " +  baseFileName + "\nmaybe it doesn't have one");
		}

		std::string matLine;
		size_t matIndex = 0;
		float tempExponent = 1.f, tempTrans = 1.f, emissive = 0.f;
		while (std::getline(materialFile, matLine))
		{
			stringTrimming::ltrim(matLine);
			if (matLine.substr(0, 6) == "newmtl")
			{
				matIndex = materials.size();
				materials.push_back({ matLine.substr(7), glm::vec3(1.f), glm::vec3(1.f) });
				tempExponent = tempTrans = 1.f;
				emissive = 0.f;
			}
			else if (matLine.substr(0, 6) == "map_Kd")
			{
				std::string textureName = matLine.substr(7);
				bool dupt = false;
				for (int i(0); i < Sprite::m_textures.size(); ++i) {
					if (Sprite::m_textures[i].fileName == textureName) {
						data.texture = i;
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
						data.texture = Sprite::m_textures.size();
						Sprite::m_textures.push_back({ textureName, texture });
					}
					else {
						Texture2D::sptr tex = Texture2D::LoadFromFile("textures/" + textureName);
						if (!tex)
						{
							throw std::runtime_error("Failed to open texture\nError 0: " + textureName);
						}
						data.texture = Sprite::m_textures.size();
						Sprite::m_textures.push_back({ textureName, tex });
					}
				}
				materials[matIndex].isText = true;
				data.text = true;
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
			//else if (matLine.size() > 1 && matLine[0] == 'K')
			else if (matLine.substr(0, 2) == "Kd")
			{
				//only colour data is taken for now, textures later
				//if (matLine[1] == 'd')
				//{
					//diffuse Colour, aka object colour
					std::istringstream ss = std::istringstream(matLine.substr(2));
					glm::vec3 colour;
					ss >> colour.x >> colour.y >> colour.z;

					materials[matIndex].colours = colour;
				//}
				/*else if (matLine[1] == 's')
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
				}*/
			}
			else if (matLine.substr(0, 4) == "emis") {
				std::istringstream ss = std::istringstream(matLine.substr(4));
				ss >> emissive;
			}
			//send the data at the end
			else if (matLine.substr(0, 5) == "illum") {
				materials[matIndex].specStrength = glm::vec3(emissive, tempExponent, tempTrans);
			}
			else if (matLine[0] == '#')
			{
				//it's a comment lol i cant even, tho maybe i can
			}
		}
		materialFile.close();
	}

	bool getUVs = true;
	size_t verts = 0;
	for (int i(0); i <= maxPos; ++i) {
		std::ifstream file;
		file.open("morph/" + baseFileName + std::to_string(i) + ".obj", std::ios::binary);

		if (!file) {
			throw std::runtime_error("Failed to open a frame\ncheck the data file");
		}


		std::vector<glm::vec3> vertex = { glm::vec3() };
		std::vector<glm::vec2> UV = { glm::vec2() };
		std::vector<glm::vec3> normals = { glm::vec3() };

		std::vector<size_t> bufferVertex = {};
		std::vector<size_t> bufferUV = {};
		std::vector<size_t> bufferNormals = {};

		std::vector<size_t> bufferColours = {};	//index of material vector

		// stringStream was learnt throught the NotObjLoader, thank you Shawn once again
		std::string line;
		glm::size_t currentColour = 0;
		//size_t vectorIndex = 0;
		
		bool usingTexture = data.text, drawingText = false;
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
				else if (line[1] == 't' && usingTexture)
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
			else if (line[0] == 'f')
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
						bufferVertex.push_back(index);
						if (usingMaterial)
							bufferColours.push_back(currentColour);
					}
					else if (i % 3 == 1 && usingTexture)
					{
						//it's the UV index, we do care
						if (drawingText)
							bufferUV.push_back(index);
						else
							bufferUV.push_back(0);
					}
					else if (i % 3 == 2)
					{
						//it's the normals index
						bufferNormals.push_back(index);
					}
				}

				while (offset < line.size())
				{

					bufferVertex.push_back(bufferVertex[bufferVertex.size() - 3]);
					bufferVertex.push_back(bufferVertex[bufferVertex.size() - 2]);
					if (usingTexture) {
						bufferUV.push_back(bufferUV[bufferUV.size() - 3]);
						bufferUV.push_back(bufferUV[bufferUV.size() - 2]);
					}
					bufferNormals.push_back(bufferNormals[bufferNormals.size() - 3]);
					bufferNormals.push_back(bufferNormals[bufferNormals.size() - 2]);
					if (usingMaterial) {
						bufferColours.push_back(currentColour);
						bufferColours.push_back(currentColour);
					}

					for (int j(0); j < 3; ++j)
					{
						size_t lengthIndex = 0;
						for (; isalnum(line[offset + lengthIndex]); ++lengthIndex);
						size_t index = stoi(line.substr(offset, offset + lengthIndex));

						offset += 1 + lengthIndex;

						if (j == 0)
						{
							//it's the vertex index
							bufferVertex.push_back(index);
							if (usingMaterial)
								bufferColours.push_back(currentColour);

						}
						else if (j == 1 && usingTexture)
						{
							//it's the UV index, we do care
							if (drawingText)
								bufferUV.push_back(index);
							else
								bufferUV.push_back(0);

						}
						else if (j == 2)
						{
							//it's the normals index
							bufferNormals.push_back(index);

						}
					}

				}
			}
			else if (usingMaterial && line.substr(0, 6) == "usemtl")
			{
				//usemtl
				std::string materialName = line.substr(7);
				for (int j = 0; j < materials.size(); ++j)
				{
					if (materialName == materials[j].name) {
						currentColour = j;
						drawingText = materials[j].isText;
						break;
					}
				}
			}
			else if (line[0] == 'o')
			{
				//it's a new object, ignore
			}
			else if (line[0] == '#')
			{
				//it's a comment lol i cant even
			}
		}

		file.close();

		/*std::vector<glm::vec3> vertexPos = {};
		std::vector<glm::vec3> vertexNorm = {};
		std::vector<glm::vec2> vertexUV = {};
		std::vector<glm::vec3> vertexSpec = {};
		std::vector<glm::vec3> vertexCol = {};

		for (size_t i = 0; i < bufferVertex.size(); ++i) {
			vertexPos.push_back(vertex[bufferVertex[i]]);
			vertexNorm.push_back(normals[bufferNormals[i]]);
			if (usingMaterial) {
				vertexCol.push_back(materials[bufferColours[i]].colours);
				vertexSpec.push_back(materials[bufferColours[i]].specStrength);
			}
			if (usingTexture) {
				vertexUV.push_back(UV[bufferUV[i]]);
			}
		}*/

		std::vector<float> vertexPos = {};
		std::vector<float> vertexNorm = {};
		std::vector<float> vertexUV = {};
		std::vector<float> vertexSpec = {};
		std::vector<float> vertexCol = {};

		for (size_t i = 0; i < bufferVertex.size(); ++i) {
			vertexPos.push_back(vertex[bufferVertex[i]].x);
			vertexPos.push_back(vertex[bufferVertex[i]].y);
			vertexPos.push_back(vertex[bufferVertex[i]].z);
			vertexNorm.push_back(normals[bufferNormals[i]].x);
			vertexNorm.push_back(normals[bufferNormals[i]].y);
			vertexNorm.push_back(normals[bufferNormals[i]].z);
			if (usingMaterial) {
				vertexCol.push_back(materials[bufferColours[i]].colours.x);
				vertexCol.push_back(materials[bufferColours[i]].colours.y);
				vertexCol.push_back(materials[bufferColours[i]].colours.z);
				vertexSpec.push_back(materials[bufferColours[i]].specStrength.x);
				vertexSpec.push_back(materials[bufferColours[i]].specStrength.y);
				vertexSpec.push_back(materials[bufferColours[i]].specStrength.z);
			}
			if (usingTexture) {
				vertexUV.push_back(UV[bufferUV[i]].x);
				vertexUV.push_back(UV[bufferUV[i]].y);
				vertexUV.push_back(0);
			}
		}

		//now to create VBOs...
		data.frames.push_back({ VertexBuffer::Create(), VertexBuffer::Create() });

		data.frames[i].pos->LoadData(vertexPos.data(), vertexPos.size());
		data.frames[i].normal->LoadData(vertexNorm.data(), vertexNorm.size());
		if (getUVs) {
			verts = data.frames[i].pos->GetElementSize();
			if (usingTexture) {
				data.UVs = VertexBuffer::Create();
				data.UVs->LoadData(vertexUV.data(), vertexUV.size());
			}
		}
		if (usingMaterial) {
			data.frames[i].colour = VertexBuffer::Create();
			data.frames[i].colour->LoadData(vertexCol.data(), vertexCol.size());
			data.frames[i].spec = VertexBuffer::Create();
			data.frames[i].spec->LoadData(vertexSpec.data(), vertexSpec.size());
		}

		if (verts != data.frames[i].pos->GetElementSize())
			throw std::runtime_error(baseFileName + " doesn't contain same amount of vertices, fix it!");
	}
	
	m_vao = VertexArrayObject::Create();

	m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].pos, pos1Buff);
	m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].pos, pos2Buff);
	m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].normal, norm1Buff);
	m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].normal, norm2Buff);
	if (data.mat) {
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].colour, col1Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].colour, col2Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].spec, spec1Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].spec, spec2Buff);
	}
	if (data.text)
		m_vao->AddVertexBuffer(data.UVs, UVBuff);

	return *this;
}

void ObjMorphLoader::BlendTo(const std::string& baseFileName, float delay, int frame)
{
	auto& data = m_anims[m_index];
	if (data.fileName == baseFileName || m_blend) {
		return;
	}

	bool dupt = false;
	for (int i(0); i < m_anims.size(); ++i) {
		if (m_anims[i].fileName == baseFileName && m_anims[i].mat == data.mat) {
			if (m_anims[i].frames[0].pos->GetElementCount() == data.frames[0].pos->GetElementCount() &&
				m_anims[i].frameIndices.size() > frame) {
				dupt = true;
				break;
			}
		}
	}
	if (!dupt)
		return;

	m_blend = true;
	m_transitionSpeed = delay;
	m_indexHold = m_index;
	m_p0Hold = frame;
	float timerHold = m_timer;
	size_t p0hold = m_p0, p1hold = m_p1;
	LoadMeshs(baseFileName, data.mat);
	m_timer = timerHold;
	m_p0 = p0hold;
	m_p1 = p1hold;

	m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].pos, pos1Buff);
	m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].pos, pos2Buff);
	m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].normal, norm1Buff);
	m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].normal, norm2Buff);
	if (data.mat) {
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].colour, col1Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].colour, col2Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].spec, spec1Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].spec, spec2Buff);
	}
}

void ObjMorphLoader::Init()
{
	m_matShader = Shader::Create();
	m_matShader->LoadShaderPartFromFile("shaders/vert_mat_morph.glsl", GL_VERTEX_SHADER);
	m_matShader->LoadShaderPartFromFile("shaders/frag_mat.glsl", GL_FRAGMENT_SHADER);
	m_matShader->Link();

	m_texShader = Shader::Create();
	m_texShader->LoadShaderPartFromFile("shaders/vert_tex_morph.glsl", GL_VERTEX_SHADER);
	m_texShader->LoadShaderPartFromFile("shaders/frag_tex.glsl", GL_FRAGMENT_SHADER);
	m_texShader->Link();

	m_texShader->SetUniform("s_texture", 0);

	m_shader = Shader::Create();
	m_shader->LoadShaderPartFromFile("shaders/vert_none_morph.glsl", GL_VERTEX_SHADER);
	m_shader->LoadShaderPartFromFile("shaders/frag_none.glsl", GL_FRAGMENT_SHADER);
	m_shader->Link();

	//shadows
	m_shadowShader = Shader::Create();
	m_shadowShader->LoadShaderPartFromFile("shaders/vert_depth_morph.glsl", GL_VERTEX_SHADER);
	m_shadowShader->LoadShaderPartFromFile("shaders/frag_depth.glsl", GL_FRAGMENT_SHADER);
	m_shadowShader->Link();

	m_texQueue.clear();
	m_matQueue.clear();
	m_defaultQueue.clear();
	m_texTempQueue.clear();
	m_matTempQueue.clear();
	m_defaultTempQueue.clear();
}

void ObjMorphLoader::Unload()
{
	m_texQueue.clear();
	m_matQueue.clear();
	m_defaultQueue.clear();
	m_texTempQueue.clear();
	m_matTempQueue.clear();
	m_defaultTempQueue.clear();

	for (int i(0); i < m_anims.size(); ++i) {
		auto& frames = m_anims[i].frames;
		for (int x(0); x < frames.size(); ++x) {
			frames[x].pos = nullptr;
			frames[x].normal = nullptr;
			frames[x].colour = nullptr;
			frames[x].spec = nullptr;
		}
		frames.clear();
		m_anims[i].UVs = nullptr;
	}
	m_anims.clear();

	m_shader = nullptr;
	m_matShader = nullptr;
	m_texShader = nullptr;
	m_shadowShader = nullptr;
}

ObjMorphLoader& ObjMorphLoader::ToggleDirection()
{
	auto& data = m_anims[m_index];
	if (m_reversing = !m_reversing) {
		m_p1 = ((m_p0 = m_p1) == 0 ? data.durations.size() - 1 : m_p1 - 1);
		m_timer = data.durations[m_p0] - m_timer;
	}
	else {
		m_p0 = ((m_p1 = m_p0) == 0 ? data.durations.size() - 1 : m_p0 - 1);
		m_timer = data.durations[m_p1] - m_timer;
	}

	m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].pos, pos1Buff);
	m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].pos, pos2Buff);
	m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].normal, norm1Buff);
	m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].normal, norm2Buff);
	if (data.mat) {
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].colour, col1Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].colour, col2Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].spec, spec1Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].spec, spec2Buff);
	}
	return *this;
}

void ObjMorphLoader::Update(float dt)
{
	if (!m_enabled)	return;

	auto& data = m_anims[m_index];

	if (m_blend) {

		if (m_indexHold != -1) {
			auto& data2 = m_anims[m_indexHold];
			m_timer += m_speed * dt;

			if (m_reversing) {
				if (m_timer >= data2.durations[m_p0]) {
					m_timer -= data2.durations[m_p0];
					m_indexHold = -1;
					size_t p0 = m_p1;
					if ((m_p0 = m_p0Hold) > 0)
						m_p1 = m_p0 - 1;
					else
						m_p1 = data.frames.size() - 1;

					m_vao->AddVertexBuffer(data2.frames[data2.frameIndices[p0]].pos, pos1Buff);
					m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].pos, pos2Buff);
					m_vao->AddVertexBuffer(data2.frames[data2.frameIndices[p0]].normal, norm1Buff);
					m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].normal, norm2Buff);
					if (data.mat) {
						m_vao->AddVertexBuffer(data2.frames[data2.frameIndices[p0]].colour, col1Buff);
						m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].colour, col2Buff);
						m_vao->AddVertexBuffer(data2.frames[data2.frameIndices[p0]].spec, spec1Buff);
						m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].spec, spec2Buff);
					}
				}
				else
					m_t = m_timer / data2.durations[m_p0];
			}
			else {
				if (m_timer >= data2.durations[m_p1]) {
					m_timer -= data2.durations[m_p1];
					m_indexHold = -1;
					size_t p0 = m_p1;
					m_p1 = ((m_p0 = m_p0Hold) + 1) % data.frames.size();

					m_vao->AddVertexBuffer(data2.frames[data2.frameIndices[p0]].pos, pos1Buff);
					m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].pos, pos2Buff);
					m_vao->AddVertexBuffer(data2.frames[data2.frameIndices[p0]].normal, norm1Buff);
					m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].normal, norm2Buff);
					if (data.mat) {
						m_vao->AddVertexBuffer(data2.frames[data2.frameIndices[p0]].colour, col1Buff);
						m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].colour, col2Buff);
						m_vao->AddVertexBuffer(data2.frames[data2.frameIndices[p0]].spec, spec1Buff);
						m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].spec, spec2Buff);
					}
				}
				else
					m_t = m_timer / data2.durations[m_p1];
			}
		}
		if (m_indexHold == -1) {

			m_timer += m_speed * dt;

			if (m_reversing) {
				if (m_timer >= m_transitionSpeed) {
					m_timer -= m_transitionSpeed;
					m_p1 = (m_p0 = m_p1) - 1;
					if (m_p0 == 0) {
						if (m_bounce) {
							m_reversing = false;
							m_p1 = 1;
						}
						else	m_p1 = data.durations.size() - 1;
					}
					m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].pos, pos1Buff);
					m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].pos, pos2Buff);
					m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].normal, norm1Buff);
					m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].normal, norm2Buff);
					if (data.mat) {
						m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].colour, col1Buff);
						m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].colour, col2Buff);
						m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].spec, spec1Buff);
						m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].spec, spec2Buff);
					}
					m_t = m_timer / data.durations[m_p0];

					m_blend = false;
				}
				else
					m_t = m_timer / m_transitionSpeed;
			}
			else {
				if (m_timer >= m_transitionSpeed) {
					m_timer -= m_transitionSpeed;
					m_p1 = (m_p0 = m_p1) + 1;
					if (m_p1 == data.durations.size()) {
						if (m_bounce) {
							m_reversing = true;
							m_p1 = m_p0 - 1;
						}
						else	m_p1 = 0;
					}
					m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].pos, pos1Buff);
					m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].pos, pos2Buff);
					m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].normal, norm1Buff);
					m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].normal, norm2Buff);
					if (data.mat) {
						m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].colour, col1Buff);
						m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].colour, col2Buff);
						m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].spec, spec1Buff);
						m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].spec, spec2Buff);
					}
					m_t = m_timer / data.durations[m_p1];

					m_blend = false;
				}
				else
					m_t = m_timer / m_transitionSpeed;
			}
		}
		return;
	}

	bool change = false;

	if (!m_loop && !m_bounce && (m_reversing ? m_p1 == 0 : m_p1 == data.durations.size() - 1)) {
		if (m_t < 1) {
			change = true;
			m_timer += m_speed * dt;
			if (m_reversing)
				m_t = m_timer / data.durations[m_p0];
			else
				m_t = m_timer / data.durations[m_p1];

			if (m_t >= 1) {
				m_t = 1;
				if (m_reversing)	m_timer = 0;
				else				m_timer = data.durations[data.durations.size() - 1];
			}
		}
	}
	else {
		m_timer += m_speed * dt;

		if (m_reversing) {
			while (m_timer >= data.durations[m_p0]) {
				change = true;
				m_timer -= data.durations[m_p0];
				m_p1 = (m_p0 = m_p1) - 1;
				if (m_p0 == 0) {
					if (m_bounce) {
						m_reversing = false;
						m_p1 = 1;
					}
					else	m_p1 = data.durations.size() - 1;
				}
			}
		}
		else {
			while (m_timer >= data.durations[m_p1]) {
				change = true;
				m_timer -= data.durations[m_p1];
				m_p1 = (m_p0 = m_p1) + 1;
				if (m_p1 == data.durations.size()) {
					if (m_bounce) {
						m_reversing = true;
						m_p1 = m_p0 - 1;
					}
					else	m_p1 = 0;
				}
			}
		}

		if (m_reversing)
			m_t = m_timer / data.durations[m_p0];
		else
			m_t = m_timer / data.durations[m_p1];
	}

	if (change) {
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].pos, pos1Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].pos, pos2Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].normal, norm1Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].normal, norm2Buff);
		if (data.mat) {
			m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].colour, col1Buff);
			m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].colour, col2Buff);
			m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].spec, spec1Buff);
			m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].spec, spec2Buff);
		}
	}
}

void ObjMorphLoader::BeginDraw(unsigned amt, unsigned transAmt)
{
	m_texQueue.resize(0);
	m_texQueue.reserve(amt);
	m_matQueue.resize(0);
	m_matQueue.reserve(amt);
	m_defaultQueue.resize(0);
	m_defaultQueue.reserve(amt);
	m_transQueue.resize(0);
	m_transQueue.reserve(transAmt);
}

void ObjMorphLoader::BeginTempDraw()
{
	m_texTempQueue.resize(0);
	m_matTempQueue.resize(0);
	m_defaultTempQueue.resize(0);
}

void ObjMorphLoader::DrawTrans(const glm::mat4& model, const glm::vec3& colour)
{
	if (!m_enabled)	return;

	m_transQueue.push_back({ m_t, m_vao, model, colour + m_colour });
}

void ObjMorphLoader::Draw(const glm::mat4& model, const glm::vec3& colour)
{
	if (!m_enabled)	return;

	if (m_anims[m_index].text)
		m_texQueue.push_back({ m_t, m_vao, model, colour + m_colour, m_receiveShadows, m_rimLighting, m_anims[m_index].texture });
	else if (m_anims[m_index].mat)
		m_matQueue.push_back({ m_t, m_vao, model, colour + m_colour, m_receiveShadows, m_rimLighting });
	else
		m_defaultQueue.push_back({ m_t, m_vao, model, colour + m_colour, m_receiveShadows, m_rimLighting });
}

void ObjMorphLoader::DrawTemp(const glm::mat4& model, const glm::vec3& colour)
{
	if (!m_enabled)	return;

	if (m_anims[m_index].text)
		m_texTempQueue.push_back({ m_t, m_vao, model, colour + m_colour, m_receiveShadows, m_rimLighting, m_anims[m_index].texture });
	else if (m_anims[m_index].mat)
		m_matTempQueue.push_back({ m_t, m_vao, model, colour + m_colour, m_receiveShadows, m_rimLighting });
	else
		m_defaultTempQueue.push_back({ m_t, m_vao, model, colour + m_colour, m_receiveShadows, m_rimLighting });
}

/*void ObjMorphLoader::PerformDraw(const glm::mat4& view, const Camera& camera, const glm::vec3& colour,
	const std::array<glm::vec3, MAX_LIGHTS>& lightPos, const std::array<glm::vec3, MAX_LIGHTS>& lightColour, const int& lightCount, float specularStrength, float shininess,
	float ambientLightStrength, const glm::vec3& ambientColour, float ambientStrength)*/
void ObjMorphLoader::PerformDraw(const glm::mat4& view, const Camera& camera, const glm::vec3& colour, float emissiveness, float shininess)
{
	glm::mat4 VP = camera.GetProjection() * view;

	if (m_defaultQueue.size() || m_defaultTempQueue.size()) {
		//global stuff
		m_shader->Bind();
		m_shader->SetUniform("colour", colour);
		m_shader->SetUniform("emissiveness", emissiveness);
		m_shader->SetUniform("shininess", shininess);

		/*m_shader->SetUniform("camPos", camera.GetPosition());

		m_shader->SetUniform("lightsPos", *lightPos.data(), MAX_LIGHTS);
		m_shader->SetUniform("lightsColour", *lightColour.data(), MAX_LIGHTS);
		m_shader->SetUniform("lightCount", lightCount);

		m_shader->SetUniform("ambientLightStrength", ambientLightStrength);
		m_shader->SetUniform("ambientColour", ambientColour);
		m_shader->SetUniform("ambientStrength", ambientStrength);*/

		for (int i(0); i < m_defaultTempQueue.size(); ++i) {
			m_shader->SetUniformMatrix("MVP", VP * m_defaultTempQueue[i].model);
			m_shader->SetUniformMatrix("transform", m_defaultTempQueue[i].model);
			m_shader->SetUniform("t", m_defaultTempQueue[i].t);
			m_shader->SetUniform("addColour", m_defaultTempQueue[i].colour);
			m_shader->SetUniform("receiveShadows", m_defaultTempQueue[i].shaded);
			m_shader->SetUniform("rimLighting", m_defaultTempQueue[i].rimLit);

			m_defaultTempQueue[i].vao->Render();
		}

		for (int i(0); i < m_defaultQueue.size(); ++i) {
			m_shader->SetUniformMatrix("MVP", VP * m_defaultQueue[i].model);
			m_shader->SetUniformMatrix("transform", m_defaultQueue[i].model);
			m_shader->SetUniform("t", m_defaultQueue[i].t);
			m_shader->SetUniform("addColour", m_defaultQueue[i].colour);
			m_shader->SetUniform("receiveShadows", m_defaultQueue[i].shaded);
			m_shader->SetUniform("rimLighting", m_defaultQueue[i].rimLit);

			m_defaultQueue[i].vao->Render();
		}

		Shader::UnBind();
	}

	if (m_texQueue.size() || m_texTempQueue.size()) {
		//global stuff
		m_texShader->Bind();
		/*m_texShader->SetUniform("camPos", camera.GetPosition());

		m_texShader->SetUniform("lightsPos", *lightPos.data(), MAX_LIGHTS);
		m_texShader->SetUniform("lightsColour", *lightColour.data(), MAX_LIGHTS);
		m_texShader->SetUniform("lightCount", lightCount);

		m_texShader->SetUniform("ambientLightStrength", ambientLightStrength);
		m_texShader->SetUniform("ambientColour", ambientColour);
		m_texShader->SetUniform("ambientStrength", ambientStrength);*/

		for (int i(0); i < m_texTempQueue.size(); ++i) {
			m_texShader->SetUniformMatrix("MVP", VP * m_texTempQueue[i].model);
			m_texShader->SetUniformMatrix("transform", m_texTempQueue[i].model);
			m_texShader->SetUniform("t", m_texTempQueue[i].t);
			m_texShader->SetUniform("addColour", m_texTempQueue[i].colour);
			m_texShader->SetUniform("receiveShadows", m_texTempQueue[i].shaded);
			m_texShader->SetUniform("rimLighting", m_texTempQueue[i].rimLit);

			Sprite::m_textures[m_texTempQueue[i].texture].texture->Bind(0);

			m_texTempQueue[i].vao->Render();
		}

		for (int i(0); i < m_texQueue.size(); ++i) {
			m_texShader->SetUniformMatrix("MVP", VP * m_texQueue[i].model);
			m_texShader->SetUniformMatrix("transform", m_texQueue[i].model);
			m_texShader->SetUniform("t", m_texQueue[i].t);
			m_texShader->SetUniform("addColour", m_texQueue[i].colour);
			m_texShader->SetUniform("receiveShadows", m_texQueue[i].shaded);
			m_texShader->SetUniform("rimLighting", m_texQueue[i].rimLit);

			Sprite::m_textures[m_texQueue[i].texture].texture->Bind(0);

			m_texQueue[i].vao->Render();
		}

		Shader::UnBind();
	}

	if (m_matQueue.size() || m_matTempQueue.size()) {
		//global stuff
		m_matShader->Bind();
		/*m_matShader->SetUniform("camPos", camera.GetPosition());

		m_matShader->SetUniform("lightsPos", *lightPos.data(), MAX_LIGHTS);
		m_matShader->SetUniform("lightsColour", *lightColour.data(), MAX_LIGHTS);
		m_matShader->SetUniform("lightCount", lightCount);

		m_matShader->SetUniform("ambientLightStrength", ambientLightStrength);
		m_matShader->SetUniform("ambientColour", ambientColour);
		m_matShader->SetUniform("ambientStrength", ambientStrength);*/

		for (int i(0); i < m_matTempQueue.size(); ++i) {
			m_matShader->SetUniformMatrix("MVP", VP * m_matTempQueue[i].model);
			m_matShader->SetUniformMatrix("transform", m_matTempQueue[i].model);
			m_matShader->SetUniform("t", m_matTempQueue[i].t);
			m_matShader->SetUniform("addColour", m_matTempQueue[i].colour);
			m_matShader->SetUniform("receiveShadows", m_matTempQueue[i].shaded);
			m_matShader->SetUniform("rimLighting", m_matTempQueue[i].rimLit);

			m_matTempQueue[i].vao->Render();
		}

		for (int i(0); i < m_matQueue.size(); ++i) {
			m_matShader->SetUniformMatrix("MVP", VP * m_matQueue[i].model);
			m_matShader->SetUniformMatrix("transform", m_matQueue[i].model);
			m_matShader->SetUniform("t", m_matQueue[i].t);
			m_matShader->SetUniform("addColour", m_matQueue[i].colour);
			m_matShader->SetUniform("receiveShadows", m_matQueue[i].shaded);
			m_matShader->SetUniform("rimLighting", m_matQueue[i].rimLit);

			m_matQueue[i].vao->Render();
		}

		Shader::UnBind();
	}
}

void ObjMorphLoader::PerformDrawTrans(const glm::mat4& view, const Camera& camera)
{
	if (m_transQueue.size()) {
		glm::mat4 VP = camera.GetProjection() * view;
		m_matShader->Bind();

		m_matShader->SetUniform("receiveShadows", 0);
		for (int i(0); i < m_transQueue.size(); ++i) {
			m_matShader->SetUniformMatrix("MVP", VP * m_transQueue[i].model);
			m_matShader->SetUniformMatrix("transform", m_transQueue[i].model);
			m_matShader->SetUniform("t", m_transQueue[i].t);
			m_matShader->SetUniform("addColour", m_transQueue[i].colour);

			m_transQueue[i].vao->Render();
		}

		Shader::UnBind();
	}
}

void ObjMorphLoader::PerformDrawShadow(const glm::mat4& lightVPMatrix)
{
	m_shadowShader->Bind();

	for (int i(0); i < m_matQueue.size(); ++i) {
		m_shadowShader->SetUniformMatrix("MVP", lightVPMatrix * m_matQueue[i].model);
		m_shadowShader->SetUniform("t", m_matQueue[i].t);

		m_matQueue[i].vao->Render();
	}
	for (int i(0); i < m_texQueue.size(); ++i) {
		m_shadowShader->SetUniformMatrix("MVP", lightVPMatrix * m_texQueue[i].model);
		m_shadowShader->SetUniform("t", m_texQueue[i].t);

		m_texQueue[i].vao->Render();
	}
	for (int i(0); i < m_defaultQueue.size(); ++i) {
		m_shadowShader->SetUniformMatrix("MVP", lightVPMatrix * m_defaultQueue[i].model);
		m_shadowShader->SetUniform("t", m_defaultQueue[i].t);

		m_defaultQueue[i].vao->Render();
	}
	for (int i(0); i < m_matTempQueue.size(); ++i) {
		m_shadowShader->SetUniformMatrix("MVP", lightVPMatrix * m_matTempQueue[i].model);
		m_shadowShader->SetUniform("t", m_matTempQueue[i].t);

		m_matTempQueue[i].vao->Render();
	}
	for (int i(0); i < m_texTempQueue.size(); ++i) {
		m_shadowShader->SetUniformMatrix("MVP", lightVPMatrix * m_texTempQueue[i].model);
		m_shadowShader->SetUniform("t", m_texTempQueue[i].t);

		m_texTempQueue[i].vao->Render();
	}
	for (int i(0); i < m_defaultTempQueue.size(); ++i) {
		m_shadowShader->SetUniformMatrix("MVP", lightVPMatrix * m_defaultTempQueue[i].model);
		m_shadowShader->SetUniform("t", m_defaultTempQueue[i].t);

		m_defaultTempQueue[i].vao->Render();
	}

	Shader::UnBind();

}
