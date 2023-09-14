[shader]
domain-path = "Shaders/Domains/Surface/PBR.shaderdomain"
code-path   = "Shaders/Code/Surface/Floor.shadercode"
defines     = ["MY_DEFINE_1", "MY_DEFINE_2"]

[[permutations]]
name    = "_DEFAULT_"
defines = ""

[[permutations]]
name    = "_VOXEL_GENERATION_"
defines = ["VOXEL_WRITE"]

