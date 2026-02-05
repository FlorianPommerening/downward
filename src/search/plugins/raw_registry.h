#ifndef PLUGINS_RAW_REGISTRY_H
#define PLUGINS_RAW_REGISTRY_H

#include "registry_types.h"
#include "registry.h"

#include <memory>
#include <typeindex>
#include <vector>

namespace plugins {
class RawRegistry {
    std::vector<const CategoryPlugin *> category_plugins;
    std::vector<const SubcategoryPlugin *> subcategory_plugins;
    std::vector<const EnumPlugin *> enum_plugins;
    std::vector<const Plugin *> plugins;
    std::unique_ptr<Registry> registry;

    FeatureTypes collect_types(std::vector<std::string> &errors) const;
    void validate_category_names(std::vector<std::string> &errors) const;
    SubcategoryPlugins collect_subcategory_plugins(
        std::vector<std::string> &errors) const;
    Features collect_features(
        const SubcategoryPlugins &subcategory_plugins,
        std::vector<std::string> &errors) const;
public:
    void insert_category_plugin(const CategoryPlugin &category_plugin);
    void insert_subcategory_plugin(const SubcategoryPlugin &subcategory_plugin);
    void insert_enum_plugin(const EnumPlugin &enum_plugin);
    void insert_plugin(const Plugin &plugin);

    const Registry &construct_registry();

    static RawRegistry *instance() {
        static RawRegistry instance_;
        return &instance_;
    }
};
}

#endif
