#include <jcs/model_loader.hpp>
#include <jcs/texture.hpp>

ModelLoader::ModelLoader(const fs::path &model_path, Model &model)
    : model_path{model_path},
      file_parser{model_path},
      meshes{model.meshes},
      materials{model.materials} {
}

void ModelLoader::load() {
    while (file_parser.next_line()) {
        while (file_parser.expect("o "))
            meshes.push_back(loadMesh());

        if (file_parser.expect("mtllib "))
            loadMaterialLibrary(
                model_path.parent_path() / file_parser.read_string());
    }
}

Mesh ModelLoader::loadMesh() {
    Mesh mesh{&DEFAULT_MAT};

    std::vector<Vertex> vertices;
    std::vector<TexCoord> tex_coords;
    std::vector<Normal> normals;
    std::vector<Face> faces;

    while (file_parser.next_line()) {
        if (file_parser.expect("v ")) {
            vertices.push_back({file_parser.read_float(),
                                file_parser.read_float(),
                                file_parser.read_float()});

        } else if (file_parser.expect("vt ")) {
            tex_coords.push_back({file_parser.read_float(),
                                  file_parser.read_float()});

        } else if (file_parser.expect("vn ")) {
            normals.push_back({file_parser.read_float(),
                               file_parser.read_float(),
                               file_parser.read_float()});

        } else if (file_parser.expect("f ")) {
            faces.push_back({{{file_parser.read_int() - 1 - total_vertices,
                               file_parser.read_int() - 1 - total_tex_coords,
                               file_parser.read_int() - 1 - total_normals},
                              {file_parser.read_int() - 1 - total_vertices,
                               file_parser.read_int() - 1 - total_tex_coords,
                               file_parser.read_int() - 1 - total_normals},
                              {file_parser.read_int() - 1 - total_vertices,
                               file_parser.read_int() - 1 - total_tex_coords,
                               file_parser.read_int() - 1 - total_normals}}});

        } else if (file_parser.expect("usemtl ")) {
            const auto mat_name = file_parser.read_string();
            for (auto &mat: materials) {
                if (mat.name == mat_name) {
                    mesh.material = &mat;
                    break;
                }
            }

        } else if (file_parser.expect("o ")) {
            file_parser.rollback();
            break;
        }
    }

    total_vertices += static_cast<int>(vertices.size());
    total_tex_coords += static_cast<int>(tex_coords.size());
    total_normals += static_cast<int>(normals.size());

    mesh.init(vertices, tex_coords, normals, faces);

    return mesh;
}

void ModelLoader::loadMaterialLibrary(const fs::path &lib_path) {
    Material material{DEFAULT_MAT};

    FileParser lib_parser{lib_path};

    while (lib_parser.next_line()) {
        if (lib_parser.expect("newmtl ")) {
            // If another material was being loaded, save it.
            if (material.name != "DefaultMaterial")
                materials.push_back(material);

            // Reset the material.
            material = DEFAULT_MAT;
            material.name = lib_parser.read_string();

        } else if (lib_parser.expect("Kd ")) {
            if (!material.use_texture) {
                material.diffuse.color[0] = lib_parser.read_float();
                material.diffuse.color[1] = lib_parser.read_float();
                material.diffuse.color[2] = lib_parser.read_float();
            }

        } else if (lib_parser.expect("map_Kd ")) {
            // Texture has priority over diffuse color.
            material.use_texture = true;
            material.diffuse.texture = loadTexture(lib_parser.read_string(),
                                                   false);

        } else if (lib_parser.expect("refl ")) {
            material.use_metalness = true;
            material.metalness_texture = loadTexture(lib_parser.read_string(),
                                                     false);

        } else if (lib_parser.expect("Ns ")) {
            material.specular_exponent = lib_parser.read_float() / 100;
        }
    }

    if (material.name != "DefaultMaterial")
        materials.push_back(material);
}
