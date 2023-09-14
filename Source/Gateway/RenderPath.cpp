#include "../Gateway/RenderPath.h"
#include "../Gateway/World.h"
#include "../Gateway/FileSystem.h"
#include "../Gateway/UI.h"
#include "../Gateway/Texture.h"
#include "../Gateway/Window.h"

#include <glad/glad.h>
#include <toml.hpp>

#include <iostream>
#include <unordered_set>

#include <tracy/TracyOpenGL.hpp>

namespace Gateway
{

Ref<RenderPath> RenderPath::Create()
{
    return Ref<RenderPath>(new RenderPath());
}

RenderPath::RenderPath()
{
    glGenBuffers(1, &m_cameraUBO);
    glGenBuffers(1, &m_directionalLightsSSBO);
    glGenBuffers(1, &m_pointLightsSSBO);
    glGenBuffers(1, &m_spotLightsSSBO);
    glGenBuffers(1, &m_sceneUBO);
    glGenBuffers(1, &m_lightClustersSSBO);

    {
        glGenFramebuffers(1, &m_fbo.id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo.id);
        {
            glGenTextures(1, &m_fbo.colorAttachment);
            glBindTexture(GL_TEXTURE_2D, m_fbo.colorAttachment);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo.colorAttachment, 0);
        }
        {
            glGenTextures(1, &m_fbo.depthAttachment);
            glBindTexture(GL_TEXTURE_2D, m_fbo.depthAttachment);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_fbo.depthAttachment, 0);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glGenVertexArrays(1, &m_dummyVAO);

    glGenQueries(1, &m_queryID);

    {
        glGenTextures(1, &m_voxel.voxelTexture);
        glBindTexture(GL_TEXTURE_3D, m_voxel.voxelTexture);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, 10, 10, 10, 0, GL_RGB, GL_FLOAT, nullptr);
        glBindTexture(GL_TEXTURE_3D, 0);


        std::string voxelGenerator_VS =
            "void main()\n"
            "{\n"
            "}\n"
            ;
    }
}


RenderPath::~RenderPath()
{
    glDeleteBuffers(1, &m_cameraUBO);
    glDeleteBuffers(1, &m_directionalLightsSSBO);
    glDeleteBuffers(1, &m_pointLightsSSBO);
    glDeleteBuffers(1, &m_spotLightsSSBO);
    glDeleteBuffers(1, &m_sceneUBO);
    glDeleteBuffers(1, &m_lightClustersSSBO);
    
    {
        glDeleteTextures(1, &m_fbo.colorAttachment);
        glDeleteTextures(1, &m_fbo.depthAttachment);
    }
    glDeleteFramebuffers(1, &m_fbo.id);

    glDeleteVertexArrays(1, &m_dummyVAO);

    glDeleteTextures(1, &m_voxel.voxelTexture);
}

void RenderPath::UpdateClusterStructure(const RenderData::Camera& camera)
{
    const unsigned int clusterPixelSizeX = (float)camera.target.size.x/SLICE_CLUSTER_COUNT_X;
    const unsigned int clusterPixelSizeY = (float)camera.target.size.y/SLICE_CLUSTER_COUNT_Y;

    m_clusterBuffer.numSlices = NUM_CLUSTER_SLICES;
    m_clusterBuffer.clusterPixelSize.x = clusterPixelSizeX;
    m_clusterBuffer.clusterPixelSize.y = clusterPixelSizeY;
    m_clusterBuffer.numClustersPerSlice = SLICE_CLUSTER_COUNT_X*SLICE_CLUSTER_COUNT_Y;
    m_clusterBuffer.numClustersPerRow = SLICE_CLUSTER_COUNT_X;

    auto getSliceZ =
        [&] (unsigned int slice) -> float
        {
            return camera.nearPlane*std::powf(camera.farPlane/camera.nearPlane, (float)slice/NUM_CLUSTER_SLICES);
        };

    auto convertScreenToViewSpace =
        [&] (const iVec2& coord, float z) -> Vec4
        {
            float near = camera.nearPlane;
            float far = camera.farPlane;

            float z_clipSpace = z*( -near-far )/( near-far )+( ( 2.0*near*far )/( near-far ) );
            float z_ndcSpace = z_clipSpace/z;

            Vec3 nearTopLeft_ndcSpace = {
                ( (float)coord.x/camera.target.size.x )*2.0-1.0,
                ( (float)coord.y/camera.target.size.y )*2.0-1.0,
                z_ndcSpace
            };

            Vec4 nearTopLeft_clipSpace = {
                nearTopLeft_ndcSpace.x*z,
                nearTopLeft_ndcSpace.y*z,
                z_clipSpace,
                z
            };

            Mat4 invProj = camera.projection.Inverse();

            Vec4 nearTopLeft_viewSpace = invProj*nearTopLeft_clipSpace;

            return nearTopLeft_viewSpace;
        };

    for ( unsigned int slice = 0; slice<NUM_CLUSTER_SLICES; slice++ )
    {
        float sliceZ = getSliceZ(slice);
        float nextSliceZ = getSliceZ(slice+1);

        for ( unsigned int clusterX = 0; clusterX<SLICE_CLUSTER_COUNT_X; clusterX++ )
        {
            for ( unsigned int clusterY = 0; clusterY<SLICE_CLUSTER_COUNT_Y; clusterY++ )
            {
                iVec2 nearBottomLeft_screenSpace = {
                    ( clusterX+0 )*clusterPixelSizeX,
                    ( clusterY+0 )*clusterPixelSizeY
                };

                iVec2 nearTopLeft_screenSpace = {
                    ( clusterX+0 )*clusterPixelSizeX,
                    ( clusterY+1 )*clusterPixelSizeY
                };

                iVec2 nearTopRight_screenSpace = {
                    ( clusterX+1 )*clusterPixelSizeX,
                    ( clusterY+1 )*clusterPixelSizeY
                };

                //iVec2 nearBottmRight_screenSpace = {
                //    ( clusterX+1 )*clusterPixelSizeX,
                //    ( clusterY+0 )*clusterPixelSizeY
                //};

                Vec4 nearBottomLeft_viewSpace = convertScreenToViewSpace(nearBottomLeft_screenSpace, sliceZ);
                Vec4 nearTopLeft_viewSpace = convertScreenToViewSpace(nearTopLeft_screenSpace, sliceZ);
                Vec4 nearTopRight_viewSpace = convertScreenToViewSpace(nearTopRight_screenSpace, sliceZ);

                Vec4 farBottomLeft_viewSpace = convertScreenToViewSpace(nearBottomLeft_screenSpace, nextSliceZ);
                Vec4 farTopLeft_viewSpace = convertScreenToViewSpace(nearTopLeft_screenSpace, nextSliceZ);
                Vec4 farTopRight_viewSpace = convertScreenToViewSpace(nearTopRight_screenSpace, nextSliceZ);

                //Vec4 farBottmRight_viewSpace = convertScreenToViewSpace(nearBottmRight_screenSpace, nextSliceZ);
                //Vec4 nearBottmRight_viewSpace = convertScreenToViewSpace(nearBottmRight_screenSpace, sliceZ);

                std::array<Vec3, 6> clusterPoints =
                {
                    nearTopLeft_viewSpace.XYZ(),
                    farTopLeft_viewSpace.XYZ(),

                    nearBottomLeft_viewSpace.XYZ(),
                    farBottomLeft_viewSpace.XYZ(),

                    nearTopRight_viewSpace.XYZ(),
                    farTopRight_viewSpace.XYZ()
                };

                BoundingBox clusterAABB = BoundingBox::FromPoints(clusterPoints);

                unsigned int clusterIndex = ( slice*SLICE_CLUSTER_COUNT_X*SLICE_CLUSTER_COUNT_Y )+SLICE_CLUSTER_COUNT_X*clusterY+clusterX;

                m_clusterStructure.at(clusterIndex) = clusterAABB;
            }
        }
    }
}

void RenderPath::Execute(Ref<World> world)
{
    RenderData& extractedData = world->PrepareRenderData();

    extractedData = world->ExtractRenderData();
    
    TracyGpuZone("RenderPath");

    const RenderData::Camera& mainCamera = extractedData.camera;
    
    {
        glBindTexture(GL_TEXTURE_2D, m_fbo.colorAttachment);
        GLint width, height;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

        if ( mainCamera.target.size.x!=width||mainCamera.target.size.y!=height )
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mainCamera.target.size.x, mainCamera.target.size.y, 0, GL_RGBA, GL_FLOAT, nullptr);

            glBindTexture(GL_TEXTURE_2D, m_fbo.depthAttachment);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mainCamera.target.size.x, mainCamera.target.size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLbitfield clearBits = 0;
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo.id);

    if ( mainCamera.clearFlags&CLEAR_COLOR )
    {
        clearBits |= GL_COLOR_BUFFER_BIT;

        glClearColor(mainCamera.clearColor.r, mainCamera.clearColor.g, mainCamera.clearColor.b, mainCamera.clearColor.a);
    }
    if ( mainCamera.clearFlags&CLEAR_DEPTH )
    {
        clearBits |= GL_DEPTH_BUFFER_BIT;

        glClearDepth(mainCamera.clearDepth);
    }
    if ( mainCamera.clearFlags&CLEAR_STENCIL )
    {
        clearBits |= GL_STENCIL_BUFFER_BIT;

        glClearStencil(mainCamera.clearStencil);
    }

    glClear(clearBits);

    {
        struct CameraBuffer
        {
            Mat4 view, projection;
        }buffer;

        buffer.view = mainCamera.view;
        buffer.projection = mainCamera.projection;

        glBindBuffer(GL_UNIFORM_BUFFER, m_cameraUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraBuffer), &buffer, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    {
        struct SceneBuffer
        {
            Vec3 cameraWorldPosition;
            float cameraNear;

            float cameraFar;
            float pad0, pad1, pad2;
        }buffer;

        buffer.cameraWorldPosition = mainCamera.worldPosition;
        buffer.cameraNear = mainCamera.nearPlane;
        buffer.cameraFar = mainCamera.farPlane;
        glBindBuffer(GL_UNIFORM_BUFFER, m_sceneUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneBuffer), &buffer, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    glViewport(
        0, 0,
        mainCamera.target.size.x, mainCamera.target.size.y
    );

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GEQUAL);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_cameraUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_sceneUBO);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightClustersSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_directionalLightsSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_pointLightsSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_spotLightsSSBO);

    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_3D, m_voxel.voxelTexture);

    m_clusterBuffer.clusterData.fill({});

    if ( m_clusterStructureDirty )
    {
        UpdateClusterStructure(mainCamera);

        m_clusterStructureDirty = false;
    }

    unsigned int pointLightIndex = 0;
    for ( const auto& pointLight:extractedData.pointLights )
    {
        if ( pointLightIndex>=MAX_POINT_LIGHTS )
            break;

        Vec4 pointLightPos_worldSpace = {
            pointLight.worldPos.x,
            pointLight.worldPos.y,
            pointLight.worldPos.z,
            1.0
        };

        Vec4 pointLightPos_viewSpace = mainCamera.view*pointLightPos_worldSpace;

        Vec3 center_viewSpace = {
            pointLightPos_viewSpace.x,
            pointLightPos_viewSpace.y,
            pointLightPos_viewSpace.z,
        };
        float radius = pointLight.range;

        for ( unsigned int f = 0; f<TOTAL_CLUSTER_COUNT; f++ )
        {
            ClusterData& cluster = m_clusterBuffer.clusterData.at(f);
            
            if ( m_clusterStructure.at(f).IntersectsSphere(center_viewSpace, radius) )
            {
                unsigned int offset = ( cluster.lightCount%4 )+( ( std::floor(cluster.lightCount/4.0) )*4 );

                unsigned int* pData = (unsigned int*)cluster.lightIndexBlock.data();
                pData += offset;

                *pData = pointLightIndex;

                cluster.lightCount++;
            }            
        }

        pointLightIndex++;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lightClustersSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ClusterBuffer), &m_clusterBuffer, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    {
        struct DirectionalLightsBuffer
        {
            unsigned int count;
            unsigned int pad0, pad1, pad2;

            std::array<RenderData::DirectionalLight, MAX_DIRECTIONAL_LIGHTS> lights;
        }buffer;

        std::copy(
            extractedData.directionalLights.cbegin(),
            extractedData.directionalLights.cbegin()+std::min(MAX_DIRECTIONAL_LIGHTS, extractedData.directionalLights.size()),
            buffer.lights.begin()
        );

        buffer.count = extractedData.directionalLights.size();

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_directionalLightsSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(DirectionalLightsBuffer), &buffer, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    {
        struct PointLightsBuffer
        {
            unsigned int count;
            unsigned int pad0, pad1, pad2;

            std::array<RenderData::PointLight, MAX_POINT_LIGHTS> lights;
        }buffer;

        std::copy(
            extractedData.pointLights.cbegin(),
            extractedData.pointLights.cbegin()+std::min(MAX_POINT_LIGHTS, extractedData.pointLights.size()),
            buffer.lights.begin()
        );

        buffer.count = extractedData.pointLights.size();

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_pointLightsSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(PointLightsBuffer), &buffer, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    {
        struct SpotLightsBuffer
        {
            unsigned int count;
            unsigned int pad0, pad1, pad2;

            std::array<RenderData::SpotLight, MAX_SPOT_LIGHTS> lights;
        }buffer;

        std::copy(
            extractedData.spotLights.cbegin(),
            extractedData.spotLights.cbegin()+std::min(MAX_SPOT_LIGHTS, extractedData.spotLights.size()),
            buffer.lights.begin()
        );

        buffer.count = extractedData.spotLights.size();

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_spotLightsSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(SpotLightsBuffer), &buffer, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    glBeginQuery(GL_TIME_ELAPSED, m_queryID);
    {
        for ( auto& entry:extractedData.batches )
        {
            RenderData::Batch& batch = entry.second;

            RenderData::Mesh::LODRange lodRange = batch.mesh.lodRanges.at(batch.lodIndex);

            if ( batch.transformsDirty )
            {
                glBindBuffer(GL_ARRAY_BUFFER, batch.mesh.instanceBufferObject);
                {
                    glBufferData(GL_ARRAY_BUFFER, batch.numInstances*sizeof(Mat4), batch.transforms.data(), GL_STATIC_DRAW);
                }
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            glBindVertexArray(batch.mesh.vao);

            unsigned int lightProgram = batch.material.subDomainPrograms["light"];

            glUseProgram(lightProgram);

            batch.material.BindDataToProgram(lightProgram);

            glDrawElementsInstanced(GL_TRIANGLES, lodRange.indexCount, GL_UNSIGNED_INT, (GLvoid*)( lodRange.startIndex*sizeof(unsigned int) ), batch.numInstances);
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
    }
    glEndQuery(GL_TIME_ELAPSED);

    GLuint gpuTimeNs;
    glGetQueryObjectuiv(m_queryID, GL_QUERY_RESULT, &gpuTimeNs);

    float gpuTimeMs = gpuTimeNs/1000000.0f;
    //std::cout<<gpuTimeMs<<std::endl;

    for ( auto& entry:extractedData.batches )
    {
        RenderData::Batch& batch = entry.second;

        batch.transformsDirty = false;
        batch.numInstances = 0;
        batch.transforms.clear();
    }

    glDisable(GL_BLEND);

    {
        TracyGpuZone("PostProcess");

        if ( !mainCamera.postProcessChain.empty() )
        {
            glBindFramebuffer(GL_FRAMEBUFFER, mainCamera.target.fbo);

            for ( auto postProcess:mainCamera.postProcessChain )
            {
                unsigned int postProcessProgram = postProcess.material.subDomainPrograms["main-post-process"];

                glUseProgram(postProcessProgram);

                postProcess.material.BindDataToProgram(postProcessProgram);

                glBindVertexArray(m_dummyVAO);

                glDisable(GL_DEPTH_TEST);

                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
        }
        else
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo.id);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mainCamera.target.fbo);
            glBlitFramebuffer(
                0, 0, mainCamera.target.size.x, mainCamera.target.size.y,
                0, 0, mainCamera.target.size.x, mainCamera.target.size.y,
                GL_COLOR_BUFFER_BIT, GL_LINEAR
            );
        }
    }

    {
        TracyGpuZone("DrawUI");

        for ( auto ctx:extractedData.rmlContexts )
        {
            RmlRenderImpl::Get()->SetCurrentContext(ctx);

            ctx->Render();
        }
    }
}

Ref<Texture> RenderPath::GetColorTexture()
{
    return new Texture(m_fbo.colorAttachment);
}

}