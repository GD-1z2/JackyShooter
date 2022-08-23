#pragma once

#include <jacky-common/defs.hpp>
#include <jcs/model.hpp>
#include <jcs/file_parser.hpp>

class ModelLoader {
public:
    ModelLoader(const fs::path &model_path, Model &model);

    void load();

private:
    Mesh loadMesh();

    void loadMaterialLibrary(const fs::path &lib_path);

    fs::path model_path;
    FileParser file_parser;
    std::vector<Mesh> &meshes;
    std::vector<Material> &materials;
    int total_vertices{}, total_tex_coords{}, total_normals{};
};
