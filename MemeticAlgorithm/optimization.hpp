#pragma once
#include <functional>
#include <array>

namespace impl {

	template <typename T, typename F>
	T neumeric_diff(F functor, T x, T eps = T(1e-10)) {
		return (functor(x + eps) - functor(x - eps)) / 2 * eps;
	}

	template <typename T, typename F, size_t N>
	T jacobian_impl(F functor, size_t index, T (&x)[N], T eps = T(1e-10)) {
		T p1[N];
		T p2[N];
		for (size_t i = 0; i < N; i++) {
			p1[i] = p2[i] = x[i];
			if (i == index) {
				p1[i] += eps;
				p2[i] -= eps;
			}
		}

		return (functor(p1) - functor(p2)) / (2 * eps);
	}

	template <typename T, typename F, size_t N>
	auto jacobian(F functor, T (&x)[N], T eps = T(1e-10)) {
		std::array<T, N> ret;
		for (size_t i = 0; i < N; i++) {
			ret[i] = jacobian_impl(functor, i, x, eps);
		}
		return ret;
	}

}

