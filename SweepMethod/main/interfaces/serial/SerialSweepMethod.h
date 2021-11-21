#pragma once

#include <utility>

#include "main/interfaces/serial/SerialInstrumental.h"
#include "main/interfaces/AbstractSweepMethod.h"


class SerialSweepMethod final : public SerialInstrumental, public AbstractSweepMethod {
private:
	vec Phi;
	pairs kappa, mu, gamma;

public:
	SerialSweepMethod() : SerialSweepMethod(5) {}

    explicit SerialSweepMethod(size_t N) :
        SerialSweepMethod(N,vec(N - 1, 1),std::make_pair(0., 0.), std::make_pair(1., 1.), std::make_pair(1., 1.)) {}

    SerialSweepMethod(size_t N, vec phi, pairs kappa_, pairs mu_, pairs gamma_) :
                        SerialInstrumental(N),
                        Phi(std::move(phi)),
                        kappa(std::move(kappa_)), mu(std::move(mu_)), gamma(std::move(gamma_)) {}

    SerialSweepMethod(vec a, vec c, vec b, vec phi, pairs kappa_, pairs mu_, pairs gamma_) :
        SerialInstrumental(std::move(a), std::move(c), std::move(b)),
        Phi(std::move(phi)),
        kappa(std::move(kappa_)), mu(std::move(mu_)), gamma(std::move(gamma_)) {}

    /*
     * Sequential implementation of sweep method
     *
     * a   - is the diagonal lying under the main
     * c   - the main diagonal of the matrix A
     * b   - the diagonal lying above the main
     * phi - right side
     * x   - vector of solutions
     * kappa, mu, gamma - coefficients
    */
	vec run() override;

    // Getting protected fields
    std::tuple<vec, vec, size_t, size_t, double, vec, vec, vec, vec, vec, pairs, pairs, pairs> getAllFields();

    // Setting protected fields
    void setAllFields(vec v, vec u, size_t N, size_t node, double h, vec x, vec A, vec C, vec B, vec Phi, pairs kappa, pairs mu, pairs gamma);
};