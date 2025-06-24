#pragma once

#include <memory>
#include <vector>
#include <ranges>
#include <functional>

class Component;

using ComponentsView =
std::ranges::transform_view<std::ranges::ref_view<const std::vector<std::unique_ptr<Component>>>,
    std::function<Component* (const std::unique_ptr<Component>&)>>;