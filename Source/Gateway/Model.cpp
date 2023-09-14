#include "../Gateway/Model.h"
#include "../Gateway/FileSystem.h"
#include "../Gateway/Context.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glad/glad.h>
#include <toml.hpp>

#include <functional>
#include <fstream>
#include <filesystem>

namespace Gateway
{

Ref<Model> Model::Create(const std::vector<Mesh>& meshes, unsigned int flags)
{
	return Ref<Model>(new Model(meshes, flags));
}

bool Model::Import(const std::string& filepath, int flags)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		FileSystem::GetFullPath(filepath), 
		aiProcess_Triangulate|aiProcess_SortByPType|aiProcess_MakeLeftHanded|aiProcess_CalcTangentSpace|aiProcess_GenBoundingBoxes
	);
	if ( !scene )
		return false;
		
	std::vector<Mesh> meshes;
	BoundingBox aabb;

	aiMatrix4x4 accumMatrix;
	std::function<void(aiNode*, aiMatrix4x4&)> processNode = [&processNode, &scene, &meshes, &aabb] (aiNode* node, aiMatrix4x4& accumMatrix)
	{
		unsigned int numMeshes = node->mNumMeshes;
		accumMatrix = node->mTransformation*accumMatrix;

		unsigned int baseVertex = 0;
		for ( unsigned int m = 0; m<1/*numMeshes*/; m++ )
		{
			Mesh mesh;
			
			const aiMesh* assimpMesh = scene->mMeshes[node->mMeshes[m]];
			
			Vec3 aabbMax = { assimpMesh->mAABB.mMax.x, assimpMesh->mAABB.mMax.y, assimpMesh->mAABB.mMax.z };
			Vec3 aabbMin = { assimpMesh->mAABB.mMin.x, assimpMesh->mAABB.mMin.y, assimpMesh->mAABB.mMin.z };

			aabb = BoundingBox::FromMaxMin(aabbMax, aabbMin);

			for ( unsigned int v = 0; v<assimpMesh->mNumVertices; v++ )
			{
				aiVector3D assimpVertex = assimpMesh->mVertices[v];
				assimpVertex = accumMatrix*assimpVertex;

				aiVector3D assimpNormal = accumMatrix*assimpMesh->mNormals[v];

				aiVector3D assimpTangent = accumMatrix*assimpMesh->mTangents[v];

				aiVector3D uv = assimpMesh->mTextureCoords[0][v];

				VertexStandard vertex;
				vertex.position = { assimpVertex.x, assimpVertex.y,assimpVertex.z };
				vertex.normal = { assimpNormal.x, assimpNormal.y, assimpNormal.z };
				vertex.tangent = { assimpTangent.x, assimpTangent.y, assimpTangent.z };
				vertex.uv = { uv.x, uv.y };

				mesh.vertexData.push_back(vertex);
			}

			LODIndexData& indexDataLod0 = mesh.indexData.emplace_back();
			for ( unsigned int f = 0; f<assimpMesh->mNumFaces; f++ )
			{
				indexDataLod0.push_back(assimpMesh->mFaces[f].mIndices[0]+baseVertex);
				indexDataLod0.push_back(assimpMesh->mFaces[f].mIndices[1]+baseVertex);
				indexDataLod0.push_back(assimpMesh->mFaces[f].mIndices[2]+baseVertex);
			}

			baseVertex += assimpMesh->mNumVertices; // (?)

			meshes.push_back(mesh);
		}

		for ( int c = 0; c<node->mNumChildren; c++ )
			processNode(node->mChildren[c], accumMatrix);
	};

	processNode(scene->mRootNode, accumMatrix);

	Ref<Model> model = Model::Create(meshes, MODEL_FLAG_DONT_TRANSITION);

	model->m_aabb = aabb;

	Model::Save(model, filepath+".import");

	return true;
}

Ref<Model> Model::Load(const std::string& filepath, int flags)
{
	static std::unordered_map<unsigned int, std::unordered_map<std::string, Ref<Model>>> s_cache;

	unsigned int currentContextID = Context::GetCurrentID();

	if ( s_cache[currentContextID].contains(filepath) )
		return s_cache[currentContextID].at(filepath);

	toml::value metaData = toml::parse(FileSystem::GetFullPath(filepath));
	toml::value modelData = toml::find(metaData, "model");

	toml::value importData = toml::find(modelData, "import");

	std::string	dataPath = toml::find<std::string>(importData, "path");
	
	std::ifstream inFile(FileSystem::GetFullPath(dataPath), std::ios::binary);

	if ( !inFile.is_open() )
		return nullptr;

	BoundingBox aabb;
	{
		Vec3 aabbMax;
		Vec3 aabbMin;

		inFile.read((char*)&aabbMax.x, sizeof(float));
		inFile.read((char*)&aabbMax.y, sizeof(float));
		inFile.read((char*)&aabbMax.z, sizeof(float));

		inFile.read((char*)&aabbMin.x, sizeof(float));
		inFile.read((char*)&aabbMin.y, sizeof(float));
		inFile.read((char*)&aabbMin.z, sizeof(float));

		aabb = BoundingBox::FromMaxMin(aabbMax, aabbMin);
	}

	unsigned int numMeshes;
	inFile.read((char*)&numMeshes, sizeof(unsigned int));

	std::vector<Mesh> meshes;

	for ( unsigned int m = 0; m<numMeshes; m++ )
	{
		unsigned int numVertices;
		unsigned int numIndices;

		inFile.read((char*)&numVertices, sizeof(unsigned int));
		inFile.read((char*)&numIndices, sizeof(unsigned int));
		
		Mesh& mesh = meshes.emplace_back();
		
		LODIndexData& indexDataLod0 = mesh.indexData.emplace_back();

		mesh.vertexData.resize(numVertices);
		indexDataLod0.resize(numIndices);

		inFile.read((char*)mesh.vertexData.data(), numVertices*sizeof(VertexStandard));
		inFile.read((char*)indexDataLod0.data(), numIndices*sizeof(unsigned int));
	}

	Ref<Model> model	=  Model::Create(meshes);

	model->m_filePath	= filepath;
	model->m_aabb		= aabb;

	s_cache[currentContextID][filepath] = model;
	return model;
}

bool Model::Save(Ref<Model> model, const std::string& filepath)
{
	std::ofstream outFile(FileSystem::GetFullPath(filepath), std::ios::binary);

	if ( !outFile.is_open() )
		return false;

	{
		Vec3 aabbMax = model->m_aabb.GetMax();
		Vec3 aabbMin = model->m_aabb.GetMin();

		outFile.write((const char*)&aabbMax.x, sizeof(float));
		outFile.write((const char*)&aabbMax.y, sizeof(float));
		outFile.write((const char*)&aabbMax.z, sizeof(float));

		outFile.write((const char*)&aabbMin.x, sizeof(float));
		outFile.write((const char*)&aabbMin.y, sizeof(float));
		outFile.write((const char*)&aabbMin.z, sizeof(float));
	}

	unsigned int numMeshes = model->m_meshes.size();
	outFile.write((const char*)&numMeshes, sizeof(unsigned int));

	for ( const auto& mesh:model->m_meshes )
	{
		unsigned int numVertices = mesh.vertexData.size();
		unsigned int numIndices = mesh.indexData[0].size();

		outFile.write((const char*)&numVertices, sizeof(unsigned int));
		outFile.write((const char*)&numIndices, sizeof(unsigned int));

		outFile.write((const char*)mesh.vertexData.data(), mesh.vertexData.size()*sizeof(VertexStandard));
		outFile.write((const char*)mesh.indexData[0].data(), mesh.indexData[0].size()*sizeof(unsigned int));
	}

	outFile.close();
	return true;
}

Model::Model(const std::vector<Mesh>& meshes, unsigned int flags)
{
	m_flags		= flags;
	m_meshes	= meshes;

	if ( m_flags&MODEL_FLAG_DONT_TRANSITION )
		return;

	for ( const auto& mesh:m_meshes )
	{
		RenderData::Mesh& meshRenderData = m_renderMeshes.emplace_back();

		glGenBuffers(1, &meshRenderData.vbo);
		glGenBuffers(1, &meshRenderData.ebo);
		glGenBuffers(1, &meshRenderData.instanceBufferObject);
		glGenVertexArrays(1, &meshRenderData.vao);

		size_t vboByteSize = 0;
		size_t eboByteSize = 0;

		vboByteSize = mesh.vertexData.size()*sizeof(VertexStandard);

		for ( auto iData:mesh.indexData )
			eboByteSize += iData.size()*sizeof(unsigned int);

		glBindVertexArray(meshRenderData.vao);

		glBindBuffer(GL_ARRAY_BUFFER, meshRenderData.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshRenderData.ebo);

		glBufferData(GL_ARRAY_BUFFER, vboByteSize, mesh.vertexData.data(), flags&MODEL_FLAG_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboByteSize, NULL, flags&MODEL_FLAG_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		size_t vboByteOffset = 0;
		size_t eboByteOffset = 0;
		for ( auto iData:mesh.indexData )
		{
			const unsigned int ibByteSize = iData.size()*sizeof(unsigned int);

			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, eboByteOffset, ibByteSize, iData.data());

			eboByteOffset += ibByteSize;
		}

		unsigned int baseIndex = 0;
		for ( int lodIdx = 0; lodIdx<mesh.indexData.size(); lodIdx++ )
		{
			const LODIndexData& indexData = mesh.indexData.at(lodIdx);

			RenderData::Mesh::LODRange& lodRange = meshRenderData.lodRanges.emplace_back();

			lodRange.startIndex = baseIndex;
			lodRange.indexCount = indexData.size();

			baseIndex += indexData.size();
		}

		unsigned int stride = sizeof(VertexStandard);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)( sizeof(float)*3 ));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)( sizeof(float)*6 ));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)( sizeof(float)*9 ));

		glBindBuffer(GL_ARRAY_BUFFER, meshRenderData.instanceBufferObject);
		
		stride = sizeof(Mat4);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glVertexAttribDivisor(4, 1);

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, (void*)( 1*sizeof(Vec4) ));
		glVertexAttribDivisor(5, 1);

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, stride, (void*)( 2*sizeof(Vec4) ));
		glVertexAttribDivisor(6, 1);

		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, stride, (void*)( 3*sizeof(Vec4) ));
		glVertexAttribDivisor(7, 1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

Model::~Model()
{
	if ( m_flags&MODEL_FLAG_DONT_TRANSITION )
		return;

	for ( auto& mesh:m_renderMeshes )
	{
		glDeleteBuffers(1, &mesh.vbo);
		glDeleteBuffers(1, &mesh.ebo);
		glDeleteBuffers(1, &mesh.instanceBufferObject);
		glDeleteVertexArrays(1, &mesh.vao);
	}
}

void Model::ExtractRenderMesh(RenderData::Mesh& mesh, unsigned int meshIndex)
{
	mesh = m_renderMeshes.at(meshIndex);
}


}