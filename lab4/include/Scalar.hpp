#pragma once

#include <type_traits>

template<class T>
concept Scalar = std::is_scalar_v<T>;
