#ifndef WAVE_FDM_WAVE_FDM_SOLVER_H
#define WAVE_FDM_WAVE_FDM_SOLVER_H

#include <eigen3/Eigen/Dense>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>
#include <iostream>
#include <random>

class Wave_FDM
{
public:
    Wave_FDM(int Num_Grid, double space_size, double k);

    Eigen::Tensor<double, 3>& Update(double Damping_const);

    Eigen::Tensor<double, 3>& Update_Fast(double Damping_const);

    void Random_Source(double Source_Height);

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    Eigen::Tensor<double, 3> Space;

private:
    int Num_Grid;

    double h, k;
    //  h = simulation size / Num_Grid
    //  k = time differential
    //  h > k
    //  https://en.wikipedia.org/wiki/Courant%E2%80%93Friedrichs%E2%80%93Lewy_condition

    Eigen::array<Eigen::DenseIndex, 2> offset1 = {0, 1};
    Eigen::array<Eigen::DenseIndex, 2> extent1 = {Num_Grid - 2, Num_Grid - 2};

    Eigen::array<Eigen::DenseIndex, 2> offset2 = {2, 1};
    Eigen::array<Eigen::DenseIndex, 2> extent2 = {Num_Grid - 2, Num_Grid - 2};

    Eigen::array<Eigen::DenseIndex, 2> offset3 = {1, 0};
    Eigen::array<Eigen::DenseIndex, 2> extent3 = {Num_Grid - 2, Num_Grid - 2};

    Eigen::array<Eigen::DenseIndex, 2> offset4 = {1, 2};
    Eigen::array<Eigen::DenseIndex, 2> extent4 = {Num_Grid - 2, Num_Grid - 2};

    Eigen::array<Eigen::DenseIndex, 2> offset = {1, 1};
    Eigen::array<Eigen::DenseIndex, 2> extent = {Num_Grid - 2, Num_Grid - 2};
};

#endif //WAVE_FDM_WAVE_FDM_SOLVER_H
