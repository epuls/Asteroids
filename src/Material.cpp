#include <Rendering/Material.hpp>
#include <Assets/AssetManager.hpp>

void Material::SetTexture(std::shared_ptr<AssetHandle> textureAsset) {
    tex = textureAsset;
    Shader->texture0 = tex->GetManager().GetTexture(*tex);
}
