#pragma once

#include "../Gateway/RefCounted.h"
#include "../Gateway/Vec4.h"
#include "../Gateway/Graphics.h"
#include "../Gateway/RenderTarget.h"
#include "../Gateway/Texture.h"
#include "../Gateway/BoundingBox.h"
#include "../Gateway/World.h"

#include <toml.hpp>

#include <list>
#include <string>
#include <vector>
#include <array>

namespace Gateway
{

constexpr size_t MAX_DIRECTIONAL_LIGHTS = 8;
constexpr size_t MAX_POINT_LIGHTS		= 128;
constexpr size_t MAX_SPOT_LIGHTS		= 64;

constexpr size_t NUM_CLUSTER_SLICES		= 24;
constexpr size_t SLICE_CLUSTER_COUNT_X	= 16;
constexpr size_t SLICE_CLUSTER_COUNT_Y	= 16;

constexpr size_t TOTAL_CLUSTER_COUNT	= NUM_CLUSTER_SLICES*SLICE_CLUSTER_COUNT_X*SLICE_CLUSTER_COUNT_Y;

class RenderPath : public RefCounted
{
public:
	static Ref<RenderPath> Create();

public:
	void				Execute(Ref<World> world);

	Ref<Texture>		GetColorTexture();

public:
	RenderPath();
	
	~RenderPath();

private:
	unsigned int		m_cameraUBO = 0;
	unsigned int		m_sceneUBO = 0;

	unsigned int		m_directionalLightsSSBO = 0;
	unsigned int		m_spotLightsSSBO = 0;
	unsigned int		m_pointLightsSSBO = 0;
	unsigned int		m_lightClustersSSBO = 0;

	unsigned int		m_dummyVAO = 0;

	struct
	{
		struct
		{
			unsigned int vertexShader;
			unsigned int fragmentShader;
			unsigned int program;
		}generator;

		unsigned int voxelTexture = 0;
	}m_voxel;

	struct
	{
		unsigned int id = 0;
		unsigned int colorAttachment = 0;
		unsigned int depthAttachment = 0;
	}m_fbo;

	unsigned int m_queryID = 0;

private:
	struct ClusterData
	{
		std::array<unsigned int, MAX_POINT_LIGHTS*4> lightIndexBlock = {};

		unsigned int lightCount = 0;

		unsigned int pad0 = 0, pad1 = 0, pad2 = 0;
	};

	struct ClusterBuffer
	{	
		uVec2                                   clusterPixelSize;
		unsigned int                            numSlices;
		unsigned int                            numClustersPerSlice;

		unsigned int							numClustersPerRow;
		unsigned int							pad0, pad1, pad2;

		std::array<ClusterData, TOTAL_CLUSTER_COUNT> clusterData;
	} m_clusterBuffer;

	std::array<BoundingBox, TOTAL_CLUSTER_COUNT> m_clusterStructure;

	bool m_clusterStructureDirty = true;

private:
	void UpdateClusterStructure(const RenderData::Camera& camera);
};

}