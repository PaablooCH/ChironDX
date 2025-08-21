#pragma once

#include <functional>
#include <memory>
#include <ranges>
#include <vector>

class Component;

using ComponentsView =
std::ranges::transform_view<std::ranges::ref_view<const std::vector<std::unique_ptr<Component>>>,
    std::function<Component* (const std::unique_ptr<Component>&)>>;