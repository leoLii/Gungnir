#include "driver/bgiBase/tokens.h"

#include "driver/bgiVulkan/shaderGenerator.h"
#include "driver/bgiVulkan/bgi.h"
#include "driver/bgiVulkan/conversions.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

static const char *
_GetPackedTypeDefinitions()
{
    return
        "\n"
        "struct bgi_ivec3 { int    x, y, z; };\n"
        "struct bgi_vec3  { float  x, y, z; };\n"
        "struct bgi_dvec3 { double x, y, z; };\n"
        "struct bgi_mat3  { float  m00, m01, m02,\n"
        "                          m10, m11, m12,\n"
        "                          m20, m21, m22; };\n"
        "struct bgi_dmat3 { double m00, m01, m02,\n"
        "                          m10, m11, m12,\n"
        "                          m20, m21, m22; };\n";
}

template<typename SectionType, typename ...T>
SectionType *
BgiVulkanShaderGenerator::CreateShaderSection(T && ...t)
{
    std::unique_ptr<SectionType> p =
        std::make_unique<SectionType>(std::forward<T>(t)...);
    SectionType * const result = p.get();
    GetShaderSections()->push_back(std::move(p));
    return result;
}

BgiVulkanShaderGenerator::BgiVulkanShaderGenerator(
    Bgi const *bgi,
    const BgiShaderFunctionDesc &descriptor)
  : BgiShaderGenerator(descriptor)
  , _bgi(bgi)
  , _textureBindIndexStart(0)
  , _inLocationIndex(0)
  , _outLocationIndex(0)
{
    // Write out all GL shaders and add to shader sections

    if (descriptor.shaderStage == BgiShaderStageCompute) {
        int workSizeX = descriptor.computeDescriptor.localSize[0];
        int workSizeY = descriptor.computeDescriptor.localSize[1];
        int workSizeZ = descriptor.computeDescriptor.localSize[2];

        if (workSizeX == 0 || workSizeY == 0 || workSizeZ == 0) {
            workSizeX = 1;
            workSizeY = 1;
            workSizeZ = 1;
        }
      
        _shaderLayoutAttributes.push_back(
            std::string("layout(") +
            "local_size_x = " + std::to_string(workSizeX) + ", "
            "local_size_y = " + std::to_string(workSizeY) + ", "
            "local_size_z = " + std::to_string(workSizeZ) + ") in;\n"
        );
    } else if (descriptor.shaderStage == BgiShaderStageTessellationControl) {
        _shaderLayoutAttributes.emplace_back(
            "layout (vertices = " +
                descriptor.tessellationDescriptor.numVertsPerPatchOut +
                ") out;\n");
    } else if (descriptor.shaderStage == BgiShaderStageTessellationEval) {
        if (descriptor.tessellationDescriptor.patchType ==
                BgiShaderFunctionTessellationDesc::PatchType::Triangles) {
            _shaderLayoutAttributes.emplace_back(
                "layout (triangles) in;\n");
        } else if (descriptor.tessellationDescriptor.patchType ==
                BgiShaderFunctionTessellationDesc::PatchType::Quads) {
            _shaderLayoutAttributes.emplace_back(
                "layout (quads) in;\n");
        } else if (descriptor.tessellationDescriptor.patchType ==
                BgiShaderFunctionTessellationDesc::PatchType::Isolines) {
            _shaderLayoutAttributes.emplace_back(
                "layout (isolines) in;\n");
        }
        if (descriptor.tessellationDescriptor.spacing ==
                BgiShaderFunctionTessellationDesc::Spacing::Equal) {
            _shaderLayoutAttributes.emplace_back(
                "layout (equal_spacing) in;\n");
        } else if (descriptor.tessellationDescriptor.spacing ==
                BgiShaderFunctionTessellationDesc::Spacing::FractionalEven) {
            _shaderLayoutAttributes.emplace_back(
                "layout (fractional_even_spacing) in;\n");
        } else if (descriptor.tessellationDescriptor.spacing ==
                BgiShaderFunctionTessellationDesc::Spacing::FractionalOdd) {
            _shaderLayoutAttributes.emplace_back(
                "layout (fractional_odd_spacing) in;\n");
        }
        if (descriptor.tessellationDescriptor.ordering ==
                BgiShaderFunctionTessellationDesc::Ordering::CW) {
            _shaderLayoutAttributes.emplace_back(
                "layout (cw) in;\n");
        } else if (descriptor.tessellationDescriptor.ordering ==
                BgiShaderFunctionTessellationDesc::Ordering::CCW) {
            _shaderLayoutAttributes.emplace_back(
                "layout (ccw) in;\n");
        }
    } else if (descriptor.shaderStage == BgiShaderStageGeometry) {
        if (descriptor.geometryDescriptor.inPrimitiveType ==
            BgiShaderFunctionGeometryDesc::InPrimitiveType::Points) {
            _shaderLayoutAttributes.emplace_back(
                "layout (points) in;\n");
        } else if (descriptor.geometryDescriptor.inPrimitiveType ==
            BgiShaderFunctionGeometryDesc::InPrimitiveType::Lines) {
            _shaderLayoutAttributes.emplace_back(
                "layout (lines) in;\n");
        } else if (descriptor.geometryDescriptor.inPrimitiveType ==
            BgiShaderFunctionGeometryDesc::InPrimitiveType::LinesAdjacency) {
            _shaderLayoutAttributes.emplace_back(
                "layout (lines_adjacency) in;\n");
        } else if (descriptor.geometryDescriptor.inPrimitiveType ==
            BgiShaderFunctionGeometryDesc::InPrimitiveType::Triangles) {
            _shaderLayoutAttributes.emplace_back(
                "layout (triangles) in;\n");
        } else if (descriptor.geometryDescriptor.inPrimitiveType ==
            BgiShaderFunctionGeometryDesc::InPrimitiveType::TrianglesAdjacency){
            _shaderLayoutAttributes.emplace_back(
                "layout (triangles_adjacency) in;\n");
        }

        if (descriptor.geometryDescriptor.outPrimitiveType ==
            BgiShaderFunctionGeometryDesc::OutPrimitiveType::Points) {
            _shaderLayoutAttributes.emplace_back(
                "layout (points, max_vertices = " +
                descriptor.geometryDescriptor.outMaxVertices + ") out;\n");
        } else if (descriptor.geometryDescriptor.outPrimitiveType ==
            BgiShaderFunctionGeometryDesc::OutPrimitiveType::LineStrip) {
            _shaderLayoutAttributes.emplace_back(
                "layout (line_strip, max_vertices = " +
                descriptor.geometryDescriptor.outMaxVertices + ") out;\n");
        } else if (descriptor.geometryDescriptor.outPrimitiveType ==
            BgiShaderFunctionGeometryDesc::OutPrimitiveType::TriangleStrip) {
            _shaderLayoutAttributes.emplace_back(
                "layout (triangle_strip, max_vertices = " +
                descriptor.geometryDescriptor.outMaxVertices + ") out;\n");
        }
    } else if (descriptor.shaderStage == BgiShaderStageFragment) {
        if (descriptor.fragmentDescriptor.earlyFragmentTests) {
            _shaderLayoutAttributes.emplace_back(
                "layout (early_fragment_tests) in;\n");
        }
    }

    // The ordering here is important (buffers before textures), because we
    // need to increment the bind location for resources in the same order
    // as HgiVulkanResourceBindings.
    // In Vulkan buffers and textures cannot have the same binding index.
    _WriteConstantParams(descriptor.constantParams);
    _WriteBuffers(descriptor.buffers);
    _WriteTextures(descriptor.textures);
    _WriteInOuts(descriptor.stageInputs, "in");
    _WriteInOutBlocks(descriptor.stageInputBlocks, "in");
    _WriteInOuts(descriptor.stageOutputs, "out");
    _WriteInOutBlocks(descriptor.stageOutputBlocks, "out");
}

void
BgiVulkanShaderGenerator::_WriteVersion(std::ostream &ss)
{
    const int glslVersion = _bgi->GetCapabilities()->GetShaderVersion();

    ss << "#version " << std::to_string(glslVersion) << "\n";
}

void
BgiVulkanShaderGenerator::_WriteExtensions(std::ostream &ss)
{
    const int glslVersion = _bgi->GetCapabilities()->GetShaderVersion();
    const bool shaderDrawParametersEnabled = _bgi->GetCapabilities()->
        IsSet(BgiDeviceCapabilitiesBitsShaderDrawParameters);
    const bool builtinBarycentricsEnabled = _bgi->GetCapabilities()->
        IsSet(BgiDeviceCapabilitiesBitsBuiltinBarycentrics);

    if (_GetShaderStage() & BgiShaderStageVertex) {
        if (glslVersion < 460 && shaderDrawParametersEnabled) {
            ss << "#extension GL_ARB_shader_draw_parameters : require\n";
        }
        if (shaderDrawParametersEnabled) {
            ss << "int BgiGetBaseVertex() {\n";
            if (glslVersion < 460) { // use ARB extension
                ss << "  return gl_BaseVertexARB;\n";
            } else {
                ss << "  return gl_BaseVertex;\n";
            }
            ss << "}\n";

            ss << "int BgiGetBaseInstance() {\n";
            if (glslVersion < 460) { // use ARB extension
                ss << "  return gl_BaseInstanceARB;\n";
            } else {
                ss << "  return gl_BaseInstance;\n";
            }
            ss << "}\n";
        }
    }

    if (_GetShaderStage() & BgiShaderStageFragment) {
        if (builtinBarycentricsEnabled) {
            ss << "#extension GL_NV_fragment_shader_barycentric: require\n";
        }
    }
}

void
BgiVulkanShaderGenerator::_WriteMacros(std::ostream &ss)
{
    ss << "#define REF(space,type) inout type\n"
          "#define FORWARD_DECL(func_decl) func_decl\n"
          "#define ATOMIC_LOAD(a) (a)\n"
          "#define ATOMIC_STORE(a, v) (a) = (v)\n"
          "#define ATOMIC_ADD(a, v) atomicAdd(a, v)\n"
          "#define ATOMIC_EXCHANGE(a, v) atomicExchange(a, v)\n"
          "#define ATOMIC_COMP_SWAP(a, expected, desired) atomicCompSwap(a, "
          "expected, desired)\n"
          "#define atomic_int int\n"
          "#define atomic_uint uint\n";

    // Advertise to shader code that we support double precision math
    ss << "\n"
        << "#define BGI_HAS_DOUBLE_TYPE 1\n"
        << "\n";
}

void
BgiVulkanShaderGenerator::_WriteConstantParams(
    const BgiShaderFunctionParamDescVector &parameters)
{
    if (parameters.empty()) {
        return;
    }
    CreateShaderSection<BgiVulkanBlockShaderSection>(
        "ParamBuffer",
        parameters);
}

void
BgiVulkanShaderGenerator::_WriteTextures(
    const BgiShaderFunctionTextureDescVector& textures)
{
    for (const BgiShaderFunctionTextureDesc& desc : textures) {
        BgiShaderSectionAttributeVector attrs = {
            BgiShaderSectionAttribute{
                "binding",
                std::to_string(_textureBindIndexStart + desc.bindIndex)}};

        if (desc.writable) {
            attrs.insert(attrs.begin(), BgiShaderSectionAttribute{
                BgiVulkanConversions::GetImageLayoutFormatQualifier(
                    desc.format), 
                ""});
        }

        CreateShaderSection<BgiVulkanTextureShaderSection>(
            desc.nameInShader,
            _textureBindIndexStart + desc.bindIndex,
            desc.dimensions,
            desc.format,
            desc.textureType,
            desc.arraySize,
            desc.writable,
            attrs);
    }
}

void
BgiVulkanShaderGenerator::_WriteBuffers(
    const BgiShaderFunctionBufferDescVector &buffers)
{
    // Extract buffer descriptors and add appropriate buffer sections
    for (size_t i=0; i<buffers.size(); i++) {
        const BgiShaderFunctionBufferDesc &bufferDescription = buffers[i];
        
        const bool isUniformBufferBinding =
            (bufferDescription.binding == BgiBindingTypeUniformValue) ||
            (bufferDescription.binding == BgiBindingTypeUniformArray);

        const std::string arraySize =
            (bufferDescription.arraySize > 0)
                ? std::to_string(bufferDescription.arraySize)
                : std::string();

        const uint32_t bindIndex = bufferDescription.bindIndex;
        
        if (isUniformBufferBinding) {
            const BgiShaderSectionAttributeVector attrs = {
                BgiShaderSectionAttribute{"std140", ""},
                BgiShaderSectionAttribute{"binding", 
                    std::to_string(bindIndex)}};

            CreateShaderSection<BgiVulkanBufferShaderSection>(
                bufferDescription.nameInShader,
                bindIndex,
                bufferDescription.type,
                bufferDescription.binding,
                arraySize,
                false,
                attrs);
        } else {
            const BgiShaderSectionAttributeVector attrs = {
                BgiShaderSectionAttribute{"std430", ""},
                BgiShaderSectionAttribute{"binding", 
                    std::to_string(bindIndex)}};

            CreateShaderSection<BgiVulkanBufferShaderSection>(
                bufferDescription.nameInShader,
                bindIndex,
                bufferDescription.type,
                bufferDescription.binding,
                arraySize,
                bufferDescription.writable,
                attrs);
        }
				
        // In Vulkan, buffers and textures cannot have the same binding index.
        // Start textures right after the last buffer. 
        // See HgiVulkanResourceBindings for details.
        _textureBindIndexStart =
            std::max(_textureBindIndexStart, bindIndex + 1);
    }
}

void
BgiVulkanShaderGenerator::_WriteInOuts(
    const BgiShaderFunctionParamDescVector &parameters,
    const std::string &qualifier) 
{
    // To unify glslfx across different apis, other apis may want these to be 
    // defined, but since they are taken in opengl we ignore them.
    const static std::set<std::string> takenOutParams {
        "gl_Position",
        "gl_FragColor",
        "gl_FragDepth",
        "gl_PointSize",
        "gl_ClipDistance",
        "gl_CullDistance",
    };

    const static std::unordered_map<std::string, std::string> takenInParams {
        { BgiShaderKeywordTokens->hdPosition, "gl_Position"},
        { BgiShaderKeywordTokens->hdPointCoord, "gl_PointCoord"},
        { BgiShaderKeywordTokens->hdClipDistance, "gl_ClipDistance"},
        { BgiShaderKeywordTokens->hdCullDistance, "gl_CullDistance"},
        { BgiShaderKeywordTokens->hdVertexID, "gl_VertexIndex"},
        { BgiShaderKeywordTokens->hdInstanceID, "gl_InstanceIndex"},
        { BgiShaderKeywordTokens->hdPrimitiveID, "gl_PrimitiveID"},
        { BgiShaderKeywordTokens->hdSampleID, "gl_SampleID"},
        { BgiShaderKeywordTokens->hdSamplePosition, "gl_SamplePosition"},
        { BgiShaderKeywordTokens->hdFragCoord, "gl_FragCoord"},
        { BgiShaderKeywordTokens->hdBaseVertex, "gl_BaseVertex"},
        { BgiShaderKeywordTokens->hdBaseInstance, "BgiGetBaseInstance()"},
        { BgiShaderKeywordTokens->hdFrontFacing, "gl_FrontFacing"},
        { BgiShaderKeywordTokens->hdLayer, "gl_Layer"},
        { BgiShaderKeywordTokens->hdViewportIndex, "gl_ViewportIndex"},
        { BgiShaderKeywordTokens->hdGlobalInvocationID, "gl_GlobalInvocationID"},
        { BgiShaderKeywordTokens->hdBaryCoordNoPerspNV, "gl_BaryCoordNoPerspNV"},
    };

    const bool in_qualifier = qualifier == "in";
    const bool out_qualifier = qualifier == "out";
    for (const BgiShaderFunctionParamDesc &param : parameters) {
        // Skip writing out taken parameter names
        const std::string &paramName = param.nameInShader;
        if (out_qualifier &&
                takenOutParams.find(paramName) != takenOutParams.end()) {
            continue;
        }
        if (in_qualifier) {
            const std::string &role = param.role;
            auto const& keyword = takenInParams.find(role);
            if (keyword != takenInParams.end()) {
                if (role == BgiShaderKeywordTokens->hdGlobalInvocationID) {
                    CreateShaderSection<BgiVulkanKeywordShaderSection>(
                        paramName,
                        param.type,
                        keyword->second);
                } else if (role == BgiShaderKeywordTokens->hdVertexID) {
                    CreateShaderSection<BgiVulkanKeywordShaderSection>(
                        paramName,
                        param.type,
                        keyword->second);
                } else if (role == BgiShaderKeywordTokens->hdInstanceID) {
                    CreateShaderSection<BgiVulkanKeywordShaderSection>(
                        paramName,
                        param.type,
                        keyword->second);
                } else if (role == BgiShaderKeywordTokens->hdBaseInstance) {
                    CreateShaderSection<BgiVulkanKeywordShaderSection>(
                        paramName,
                        param.type,
                        keyword->second);
                }
                continue;
            }
        }

        // If a location has been specified then add it to the attributes.
        const int32_t locationIndex =
            param.location >= 0
            ? param.location
            : (in_qualifier ? _inLocationIndex++ : _outLocationIndex++);

        const BgiShaderSectionAttributeVector attrs {
            BgiShaderSectionAttribute{
                "location", std::to_string(locationIndex) }
        };

        CreateShaderSection<BgiVulkanMemberShaderSection>(
            paramName,
            param.type,
            param.interpolation,
            param.sampling,
            param.storage,
            attrs,
            qualifier,
            std::string(),
            param.arraySize);
    }
}

void
BgiVulkanShaderGenerator::_WriteInOutBlocks(
    const BgiShaderFunctionParamBlockDescVector &parameterBlocks,
    const std::string &qualifier)
{
    const bool in_qualifier = qualifier == "in";
    const bool out_qualifier = qualifier == "out";

    for (const BgiShaderFunctionParamBlockDesc &p : parameterBlocks) {
        const uint32_t locationIndex = in_qualifier ? 
            _inLocationIndex : _outLocationIndex;

        BgiVulkanShaderSectionPtrVector members;
        for(const BgiShaderFunctionParamBlockDesc::Member &member : p.members) {

            BgiVulkanMemberShaderSection *memberSection =
                CreateShaderSection<BgiVulkanMemberShaderSection>(
                    member.name,
                    member.type,
                    BgiInterpolationDefault,
                    BgiSamplingDefault,
                    BgiStorageDefault,
                    BgiShaderSectionAttributeVector(),
                    qualifier,
                    std::string(),
                    std::string(),
                    p.instanceName);
            members.push_back(memberSection);

            if (in_qualifier) {
                _inLocationIndex++;
            } else if (out_qualifier) {
                _outLocationIndex++;
            }
        }

        const BgiShaderSectionAttributeVector attrs {
            BgiShaderSectionAttribute{
                "location", std::to_string(locationIndex) }
        };

        CreateShaderSection<BgiVulkanInterstageBlockShaderSection>(
            p.blockName,
            p.instanceName,
            attrs,
            qualifier,
            p.arraySize,
            members);
    }
}

void
BgiVulkanShaderGenerator::_Execute(std::ostream &ss)
{
    // Version number must be first line in glsl shader
    _WriteVersion(ss);

    _WriteExtensions(ss);

    _WriteMacros(ss);

    ss << _GetPackedTypeDefinitions();
    
    ss << _GetShaderCodeDeclarations();
    
    for (const std::string &attr : _shaderLayoutAttributes) {
        ss << attr;
    }

    BgiVulkanShaderSectionUniquePtrVector* shaderSections = GetShaderSections();
    //For all shader sections, visit the areas defined for all
    //shader apis. We assume all shader apis have a global space
    //section, capabilities to define macros in global space,
    //and abilities to declare some members or functions there
    
    ss << "\n// //////// Global Includes ////////\n";
    for (const std::unique_ptr<BgiVulkanShaderSection>
            &shaderSection : *shaderSections) {
        shaderSection->VisitGlobalIncludes(ss);
    }

    ss << "\n// //////// Global Macros ////////\n";
    for (const std::unique_ptr<BgiVulkanShaderSection>
            &shaderSection : *shaderSections) {
        shaderSection->VisitGlobalMacros(ss);
    }

    ss << "\n// //////// Global Structs ////////\n";
    for (const std::unique_ptr<BgiVulkanShaderSection>
            &shaderSection : *shaderSections) {
        shaderSection->VisitGlobalStructs(ss);
    }

    ss << "\n// //////// Global Member Declarations ////////\n";
    for (const std::unique_ptr<BgiVulkanShaderSection>
            &shaderSection : *shaderSections) {
        shaderSection->VisitGlobalMemberDeclarations(ss);
    }

    ss << "\n// //////// Global Function Definitions ////////\n";
    for (const std::unique_ptr<BgiVulkanShaderSection>
            &shaderSection : *shaderSections) {
        shaderSection->VisitGlobalFunctionDefinitions(ss);
    }

    ss << "\n";

    // write all the original shader
    ss << _GetShaderCode();
}

BgiVulkanShaderSectionUniquePtrVector*
BgiVulkanShaderGenerator::GetShaderSections()
{
    return &_shaderSections;
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
