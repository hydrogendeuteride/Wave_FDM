#include "Wave_FDM_Solver.h"

Wave_FDM::Wave_FDM(int Num_Grid, double space_size, double k)
        : Space(Num_Grid, Num_Grid, 3), Num_Grid(Num_Grid), k(k)
{
    Space.setZero();

    h = space_size / static_cast<double>(Num_Grid);

    if (h < k)
        std::cout << "h should be bigger than k\n";
}

Eigen::Tensor<double, 3>& Wave_FDM::Update(double Damping_const)
{
    for (int i = 0; i < Num_Grid; ++i)
    {
        for (int j = 0; j < Num_Grid; ++j)
        {
            Space(i, j, 2) = Space(i, j, 1);
            Space(i, j, 1) = Space(i, j, 0);
        }
    }

    for (int i = 1; i < Num_Grid - 1; ++i)
    {
        for (int j = 1; j < Num_Grid - 1; ++j)
        {
            Space(i, j, 0) = (k / h) * (k / h) *
                             (Space(i - 1, j, 1) + Space(i + 1, j, 1) +
                              Space(i, j - 1, 1) + Space(i, j + 1, 1) -
                              4 * Space(i, j, 1))
                              + 2 * Space(i, j, 1) - Space(i, j, 2);
        }
    }

    Space.chip(0, 2) = Damping_const * Space.chip(0, 2);

    return Space;
}

Eigen::Tensor<double, 3>& Wave_FDM::Update_Fast(double Damping_const)
{
    Space.chip(2, 2) = Space.chip(1, 2);
    Space.chip(1, 2) = Space.chip(0, 2);

    auto tmp = Space.chip(1, 2);

    Space.chip(0, 2).slice(offset, extent) = (k / h) * (k / h) *
            (tmp.slice(offset1, extent1) + tmp.slice(offset2, extent2) +
                    tmp.slice(offset3, extent3) + tmp.slice(offset4, extent4)
                    -4 * tmp.slice(offset, extent)) + 2 * tmp.slice(offset, extent)
                    - Space.chip(2, 2).slice(offset, extent);

    Space.chip(0, 2) = Damping_const * Space.chip(0, 2);

    for(int i = 0; i < Num_Grid; ++i)
    {
        Space(i, 0, 0) = 0;
        Space(i, Num_Grid - 1, 0) = 0;
        Space(0, i, 0) = 0;
        Space(Num_Grid - 1, i, 0) = 0;
    }

    return Space;
}

void Wave_FDM::Random_Source(double Source_Height)
{
    std::random_device rd;

    std::mt19937_64 gen(rd());

    std::uniform_int_distribution<int> x(0, Num_Grid -1);

    Space(x(gen), x(gen), 0) = Source_Height;
}