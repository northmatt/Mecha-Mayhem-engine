#include "Skeletal.h"

const SkeletalAnim::VertexPosNormColJointWeight* SkeletalAnim::VertexPosNormColJointWeight::VPNTJW = nullptr;

const std::vector<BufferAttribute> SkeletalAnim::VertexPosNormColJointWeight::V_DECL = {
	BufferAttribute(0, 3, GL_FLOAT, false, sizeof(VertexPosNormColJointWeight), (size_t)&VPNTJW->Position),
	BufferAttribute(2, 3, GL_FLOAT, false, sizeof(VertexPosNormColJointWeight), (size_t)&VPNTJW->Normal),
	BufferAttribute(1, 3, GL_FLOAT, false, sizeof(VertexPosNormColJointWeight), (size_t)&VPNTJW->Colour),
	BufferAttribute(3, 4, GL_FLOAT, false, sizeof(VertexPosNormColJointWeight), (size_t)&VPNTJW->Joints),
	BufferAttribute(4, 4, GL_FLOAT, false, sizeof(VertexPosNormColJointWeight), (size_t)&VPNTJW->Weights)
};

std::vector<SkeletalAnim::Model> SkeletalAnim::m_models = {};

Shader::sptr SkeletalAnim::m_shader = nullptr;

SkeletalAnim::SkeletalAnim(const std::string& filename, const std::string& initialAnim)
{
	Init(filename, initialAnim);
}

SkeletalAnim::SkeletalAnim()
{
}

void SkeletalAnim::Init()
{
	m_shader = Shader::Create();
	m_shader->LoadShaderPartFromFile("shaders/vert_skeletal.glsl", GL_VERTEX_SHADER);
	m_shader->LoadShaderPartFromFile("shaders/frag_mat.glsl", GL_FRAGMENT_SHADER);
	m_shader->Link();
}

void SkeletalAnim::Unload()
{
	m_shader = nullptr;

	//no pointers, no fuss
	m_models.clear();
}

void SkeletalAnim::Init(const std::string& filename, const std::string& initialAnim)
{
	m_blendTimer = 0;
	m_looping = true;

	//first try to find it:
	for (int i(0); i < m_models.size(); ++i) {
		if (m_models[i].filename == filename) {
			m_index = i;

			auto& data = m_models[i];
			bool valid = false;
			//check if it exists
			for (int x(0); x < data.animations.size(); ++x) {
				if (data.animations[x].animName == initialAnim) {
					valid = true;
					m_currentAnim = x;
					break;
				}
			}

			//if not valid, throw error or smt
			if (!valid)
				throw std::runtime_error("didn't find anim");

			m_pose = data.baseTrans;
			m_root = &m_pose[data.rootIndex];
			CalcPose();
			return;
		}
	}

	//do the loading
	// 
	// **************************************************************
	// * LoadBinaryFromFile should be used with .glb files			*
	// * (the b is short for binary)								*
	// *															*
	// * LoadASCIIFromFile should be used with .gltf files			*
	// **************************************************************

	// Get file extension
	size_t extIndex = filename.find_last_of('.');
	if (extIndex == std::string::npos)
	{
		printf("Error: No file extension found at path %s \n", filename.c_str());
		throw std::runtime_error("wrong file type\n");
	}

	tinygltf::TinyGLTF loader;

	std::string err;
	std::string warn;

	bool ret = false;

	//gets deleted on its own
	tinygltf::Model model;
	// Actually load the data into our model
	std::string extension = filename.substr(extIndex + 1);
	if (extension == "glb")
	{
		ret = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
	}
	else if (extension == "gltf")
	{
		ret = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
	}
	else
	{
		printf("Error: Attempting to load a .%s file as a gltf/glb!\n", extension.c_str());
		throw std::runtime_error("wrong file type\n");
	}

	// Note, string.empty does not edit the value of the string
	// It just returns true if the string contains no value
	// Take this as a lesson to properly name your functions
	// string.IsEmpty() would've been 1000 times better :(

	// Display and warnings or errors that were encountered
	if (!warn.empty())
	{
		printf("TinyGLTF warning: %s\n", warn.c_str());
	}

	if (!err.empty())
	{
		printf("TinyGLTF error: %s\n", err.c_str());
	}

	if (!ret)
	{
		printf("Failed to load .gltf/.glb!\n");
		throw std::runtime_error("smt went wrong with gltf\n");
	}

	// Congratualtions! We now have our model loaded! :)
	// Now here comes the fun part, *reading* it 

	// ******************************************
	// * TODO: Add an error check to make sure:	*
	// * - The model's scene is not empty		*
	// * - The default scene index is in bounds *
	// ******************************************

	m_index = m_models.size();
	m_models.push_back({ filename });

	if (!LoadNodes(model))
		throw std::runtime_error("smt went wrong with nodes\n");

	//delete the model now
	model.~Model();


	//now deal with starting anim or smt
	auto& data = m_models[m_index].animations;
	bool valid = false;
	//check if it exists
	for (int x(0); x < data.size(); ++x) {
		if (data[x].animName == initialAnim) {
			valid = true;
			m_currentAnim = x;
			break;
		}
	}
	//if not valid, throw error or smt
	if (!valid)
		throw std::runtime_error("didn't find anim");

	CalcPose();
}

void SkeletalAnim::BlendTo(const std::string& animName, bool looping, float time, float duration)
{
	auto& data = m_models[m_index].animations;

	//ignore if same as current
	if (data[m_currentAnim].animName == animName)	return;

	bool valid = false;
	//check if it exists
	for (int i(0); i < data.size(); ++i) {
		if (data[i].animName == animName) {
			valid = true;
			m_currentAnim = i;
			break;
		}
	}

	//if not valid, stop
	if (!valid)		return;

	//update some info
	m_timer = time;
	m_blendTimer = duration;
	m_looping = looping;

	//store the current data into a temp one
	m_tempPose = m_pose;

	//calculate the pose
	CalcPose();
}

void SkeletalAnim::Update(float dt)
{

	auto& data = m_models[m_index].animations[m_currentAnim];

	if (m_blendTimer > 0) {
		//do the blending
		m_blendTimer -= dt;
		if (m_blendTimer <= 0) {
			m_blendTimer = 0;
		}
		else {
			//calcuate blended pose
			CalcPose();
			return;
		}
	}

	//deal with the timer
	if (m_looping) {
		m_timer += dt;
		while (m_timer >= data.duration)
			m_timer -= data.duration;
	}
	else if (m_timer < data.duration) {
		m_timer += dt;

		if (m_timer >= data.duration)
			m_timer = data.duration;
	}
	//means not looping and timer finished, so no need to update
	else return;


}

void SkeletalAnim::CalcPose()
{
	//blend
	if (m_blendTimer > 0) {


		return;
	}


}

bool SkeletalAnim::LoadNodes(tinygltf::Model& const model)
{
	for (int i = 0; i < model.nodes.size(); i++)
	{
		tinygltf::Node& node = model.nodes[i];

		// Valid mesh found in node
		if (node.mesh > -1)
		{
			// assume you manage duplicate meshes
			if (m_models[m_index].mesh != nullptr)
				throw std::runtime_error("too many meshes\n");

			//if (!m_loadedMeshes.count(node.mesh))
			//{
				// Load mesh
				LoadMesh(model, node.mesh, node.skin);
			//}
		}

		// Valid skin was found in the node
		if (node.skin > -1)
		{
			LoadSkin(model, node.skin);
		}
	}

	return true;
}

// Used to retrieve joint data
// GLTF stores joint data as unsigned shorts
typedef glm::vec<4, unsigned short, glm::highp> UShortVec4;

bool SkeletalAnim::LoadMesh(tinygltf::Model& const model, int const meshIndex, int const associatedSkin)
{
	tinygltf::Mesh& mesh = model.meshes[meshIndex];

	//we have to store some things outdise
	std::vector<uint32_t> indices = {};

	// Combine all the data into vertices for the VAO
	std::vector<VertexPosNormColJointWeight> vertices = {};

	//each primitive is a different material, so we do this
	for (int i(0); i < mesh.primitives.size(); ++i) {
		tinygltf::Primitive& primitive = mesh.primitives[i];

		//colour is stored in a vector, so we gotta transform it
		std::vector<double> temp = model.materials[primitive.material].pbrMetallicRoughness.baseColorFactor;
		glm::vec3 colour = glm::vec3(temp[0], temp[1], temp[2]);

		std::vector<glm::vec3> positions =
			RetrieveModelData<glm::vec3>(model, primitive.attributes["POSITION"], TINYGLTF_TYPE_VEC3);
		std::vector<glm::vec3> normals =
			RetrieveModelData<glm::vec3>(model, primitive.attributes["NORMAL"], TINYGLTF_TYPE_VEC3);

		std::vector<UShortVec4> jointsRaw =
			RetrieveModelData<UShortVec4>(model, primitive.attributes["JOINTS_0"], TINYGLTF_TYPE_VEC4);
		std::vector<glm::vec4> joints;
		// Combine raw joint data into vec4s
		for (int i = 0; i < jointsRaw.size(); i++)
		{
			joints.push_back(glm::vec4(jointsRaw[i].x, jointsRaw[i].y, jointsRaw[i].z, jointsRaw[i].w));
		}
		std::vector<glm::vec4> weights =
			RetrieveModelData<glm::vec4>(model, primitive.attributes["WEIGHTS_0"], TINYGLTF_TYPE_VEC4);

		vertices.resize(vertices.size() + positions.size());
		for (int i = 0; i < vertices.size(); i++)
		{
			vertices[i].Position = positions[i];
			vertices[i].Normal = normals[i];
			vertices[i].Colour = colour;
			vertices[i].Joints = joints[i];
			vertices[i].Weights = weights[i];
		}

		// Get triangle indices
		std::vector<unsigned short> indices_short =
			RetrieveModelData<unsigned short>(model, primitive.indices, TINYGLTF_TYPE_SCALAR);
		unsigned short bump = indices.size();
		indices.reserve(bump + indices_short.size());
		for (unsigned short i(0); i < indices_short.size(); ++i)
			indices.push_back(bump + indices_short[i]);
	}

	// Create a VAO for our mesh
	// Did you get an error on this line?
	// If glcreatebuffers has a read access violation at this line, make sure you init GLFW and GLAD first!
	// (Trust me I lost a lot of time to that lol)
	VertexBuffer::sptr vbo = VertexBuffer::Create();
	vbo->LoadData(vertices.data(), vertices.size());

	IndexBuffer::sptr ebo = IndexBuffer::Create();
	ebo->LoadData(indices.data(), indices.size());

	VertexArrayObject::sptr vao = VertexArrayObject::Create();
	vao->AddVertexBuffer(vbo, VertexPosNormColJointWeight::V_DECL);
	vao->SetIndexBuffer(ebo);


	m_models[m_index].mesh = vao;
	//m_loadedMeshes[meshIndex].m_primitives.push_back(vao);
	//m_loadedMeshes[meshIndex].m_associatedSkin = associatedSkin;

	return true;
}

bool SkeletalAnim::LoadSkin(tinygltf::Model& const model, int const skinIndex)
{
	//we're gonna load everything here
	tinygltf::Skin& skin = model.skins[skinIndex];
	auto& data = m_models[m_index];

	if (skin.joints.size() > 0) {
		data.inverseBind =
			RetrieveModelData<glm::mat4>(model, skin.inverseBindMatrices, TINYGLTF_TYPE_MAT4);

		// Load joint transforms
		for (int i = 0; i < skin.joints.size(); i++)
		{
			int jointIndex = skin.joints[i];
			if (jointIndex >= data.baseTrans.size())
				data.baseTrans.resize(jointIndex);

			data.baseTrans[jointIndex] = CopyNodeLocalTransform(model.nodes[jointIndex]);
		}


		//get root
		data.rootIndex = skin.joints[0];


		// Setup joint transform parent relationships
		for (int i = 0; i < skin.joints.size(); i++)
		{
			int jointIndex = skin.joints[i];

			// Get indices of children
			std::vector<int> children = model.nodes[jointIndex].children;
			for (int c = 0; c < children.size(); c++)
			{
				int childIndex = children[c];

				if (jointIndex == childIndex)
				{
					// The joint has itself listed as a parent! That's a big issue :((((
					abort();
				}

				//make sure it's in bounds
				if (childIndex < data.baseTrans.size())
				{
					data.baseTrans[jointIndex].m_children.push_back(childIndex);
				}
			}
		}

		//copy all these transforms into our transforms
		m_pose = data.baseTrans;
		m_root = &m_pose[data.rootIndex];
	}

	return true;
}

bool SkeletalAnim::LoadAnims(tinygltf::Model& const model)
{
	//so how do we do this?
	for (int i(0); i < model.animations.size(); ++i) {
		tinygltf::Animation& anim = model.animations[i];

		m_models[m_index].animations.push_back({ anim.name });
	}

	return true;
}

template<typename T, typename ComponentType>
std::vector<T> SkeletalAnim::RetrieveDataFromAccessor(tinygltf::Model& const model, tinygltf::Accessor& const accessor) const
{
	// Bufferviews reference a buffer and contain essential data for reading said buffer
	tinygltf::BufferView bufferView = model.bufferViews[accessor.bufferView];

	// Buffers have the actual data!
	tinygltf::Buffer buffer = model.buffers[bufferView.buffer];

	ComponentType* const dataPtr =
		reinterpret_cast<ComponentType* const>
		(buffer.data.data() + accessor.byteOffset + bufferView.byteOffset);

	std::vector<T> dataRetrieved(accessor.count);

	for (int i = 0; i < accessor.count; i++)
	{
		T data;
		// Retrieve the data from the model
		memcpy(&data, dataPtr + i * (sizeof(T) / sizeof(ComponentType)), sizeof(T));

		dataRetrieved[i] = data;
	}

	return dataRetrieved;
}

template<typename T, typename ComponentType>
T SkeletalAnim::RetrieveDataFromAccessor(tinygltf::Model& const model, tinygltf::Accessor& const accessor, unsigned int const dataIndex) const
{
	// Bufferviews reference a buffer and contain essential data for reading said buffer
	tinygltf::BufferView bufferView = model.bufferViews[accessor.bufferView];

	// Buffers have the actual data!
	tinygltf::Buffer buffer = model.buffers[bufferView.buffer];

	ComponentType* const dataPtr =
		reinterpret_cast<ComponentType* const>
		(buffer.data.data() + accessor.byteOffset + bufferView.byteOffset);

	if (accessor.count <= dataIndex)
	{
		printf("Error retrieving data from GLTF model!");
		return T();
	}

	T data;
	memcpy(&data, dataPtr + dataIndex * (sizeof(T) / sizeof(ComponentType)), sizeof(T));
	return data;
}

template<typename T>
std::vector<T> SkeletalAnim::RetrieveModelData(tinygltf::Model& const model, int const accessorIndex, int const accessorType) const
{
	if (accessorIndex > -1)
	{
		// Accessors define how bufferview data is interpreted
		tinygltf::Accessor accessor = model.accessors[accessorIndex];
		assert(accessor.type == accessorType);

		if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
		{
			return RetrieveDataFromAccessor<T, unsigned short>(model, accessor);
		}
		else
		{
			// Assume float if unrecognized component type
			return RetrieveDataFromAccessor<T, float>(model, accessor);
		}
	}

	printf("Error retrieving data from GLTF model!");
	std::vector<T> nullVal(0);
	return nullVal;
}

template<typename T>
T SkeletalAnim::RetrieveModelData(tinygltf::Model& const model, int const accessorIndex, int const accessorType, unsigned int const dataIndex) const
{
	if (accessorIndex > -1)
	{
		// Accessors define how bufferview data is interpreted
		tinygltf::Accessor accessor = model.accessors[accessorIndex];
		assert(accessor.type == accessorType);

		if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
		{
			return RetrieveDataFromAccessor<T, unsigned short>(model, accessor, dataIndex);
		}
		else
		{
			// Assume float if unrecognized component type
			return RetrieveDataFromAccessor<T, float>(model, accessor, dataIndex);
		}
	}

	printf("Error retrieving data from GLTF model!");
	return T();
}

SkeletalAnim::SimpleTransform SkeletalAnim::CopyNodeLocalTransform(tinygltf::Node& const node)
{
	// Default Values
	glm::vec3 position(0, 0, 0);
	glm::quat rotation(1, 0, 0, 0); // Constructor is in format (w, x, y, z), identity quaternion should be (0, 0, 0, 1)

	// Memcpy can't be used here since node.translation etc. are stored as doubles
	if (node.translation.size() > 0)
	{
		for (int i = 0; i < 3; i++)
		{
			position[i] = node.translation[i];
		}
	}

	if (node.rotation.size() > 0)
	{
		for (int i = 0; i < 4; i++)
		{
			rotation[i] = node.rotation[i];
		}
	}

	return SimpleTransform(position, rotation);
}