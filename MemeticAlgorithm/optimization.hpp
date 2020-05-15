#pragma once
#include <functional>
#include <array>
#include <iostream>

namespace impl {

	template <typename T, typename F>
	T neumeric_diff(F functor, T x, T eps = T(1e-10)) {
		return (functor(x + eps) - functor(x - eps)) / 2 * eps;
	}

	template <typename T, typename F, size_t N>
	T jacobian_impl(F functor, size_t index, const T (&x)[N], T eps = T(1e-10)) {
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
	auto jacobian(F functor, const T (&x)[N], T eps = T(1e-10)) {
		std::array<T, N> ret;
		for (size_t i = 0; i < N; i++) {
			ret[i] = jacobian_impl(functor, i, x, eps);
		}
		return ret;
	}
}

template <typename T, size_t N>
class Solver {
public:
	using Functor = std::function<T(T[N])>;
	using Optimizer = std::function<void(Functor,T(&)[N],bool,T)>;

	Solver(Functor costFunctor, bool minimize = true, size_t maxIteration = 100, T costCriteria = T(1e-10))
		: m_costFunctor(costFunctor), m_maxIteration(maxIteration), m_costCriteria(costCriteria), minimize(minimize) {
		for (size_t i = 0; i < N; i++) {
			upperBound[i].first = false;
			lowerBound[i].first = false;
		}
	}

	T optimize(T (&x)[N]) const {
		T last_cost = m_costFunctor(x);
		T new_cost = T(0);

		init();

		for (size_t i = 0; i < m_maxIteration; i++) {
			update(x, last_cost);
			applyBound(x);
			new_cost = m_costFunctor(x);

			if ((minimize && (last_cost - new_cost < m_costCriteria)) ||
				(!minimize && (new_cost - last_cost < m_costCriteria))) {
				break;
			}

			last_cost = new_cost;
		}

		return new_cost;
	}

	void setUpperBound(size_t index, T value) {
		assert(index < N);
		upperBound[index] = std::make_pair(true, value);
	}

	void setLowerBound(size_t index, T value) {
		assert(index < N);
		lowerBound[index] = std::make_pair(true, value);
	}

	void disableUpperBound(size_t index) {
		assert(index < N);
		upperBound[index].first = false;
	}

	void disableLowerBound(size_t index) {
		assert(index < N);
		lowerBound[index].first = false;
	}

protected:
	bool minimize;
	Functor m_costFunctor;

	virtual void init() const = 0;
	virtual void update(T(&x)[N], T last_cost) const = 0;

	void applyBound(T(&x)[N]) const {
		for (size_t i = 0; i < N; i++) {
			if (upperBound[i].first) {
				x[i] = std::min(upperBound[i].second, x[i]);
			}
			if (lowerBound[i].first) {
				x[i] = std::max(lowerBound[i].second, x[i]);
			}
		}
	}

private:
	T m_costCriteria;
	size_t m_maxIteration;
	std::pair<bool, T> upperBound[N], lowerBound[N];
};

template <typename T, size_t N>
class GradientDescent : public Solver<T, N> {
public:
	using Solver<T, N>::Solver;

protected:
	virtual void init() const override {
		alpha_step = 1e-5;;
	}

	virtual void update(T(&x)[N], T last_cost) const override {
		auto grad = impl::jacobian(this->m_costFunctor, x);
		if (this->minimize) {
			for (size_t i = 0; i < N; i++) {
				grad[i] *= -1;
			}
		}

		const size_t I = 100;

		T last_param[N];

		T a(alpha_step);
		for (size_t i = 0; i < I; i++) {
			for (size_t i = 0; i < N; i++) {
				last_param[i] = x[i];
			}

			for (size_t j = 0; j < N; j++) {
				x[j] += a * grad[j];
			}

			this->applyBound(x);
			T current_cost = this->m_costFunctor(x);

			if ((this->minimize && current_cost >= last_cost) ||
				(!this->minimize && current_cost <= last_cost)) {
				for (size_t i = 0; i < N; i++) {
					x[i] = last_param[i];
				}

				if (alpha_step < 10) {
					alpha_step /= 2;
				} else if (alpha_step * 10 > I) {
					alpha_step *= 2;
				}
				break;
			} else {
				last_cost = current_cost;
			}
		}
	}

private:
	mutable T alpha_step;
};
