/**
 * @file   SelfOrganizingMapTest/Trainer.cpp
 * @date   Sep 17, 2018
 * @author Bernd Doser, HITS gGmbH
 */

#include <cmath>
#include <limits>
#include <omp.h>

#include "SelfOrganizingMapLib/CartesianLayout.h"
#include "SelfOrganizingMapLib/Data.h"
#include "SelfOrganizingMapLib/DataIO.h"
#include "SelfOrganizingMapLib/SOM.h"
#include "SelfOrganizingMapLib/SOMIO.h"
#include "SelfOrganizingMapLib/Trainer.h"
#include "UtilitiesLib/DistributionFunctor.h"

#include "gtest/gtest.h"

using namespace pink;

TEST(SelfOrganizingMapTest, trainer_num_rot)
{
    typedef SOM<CartesianLayout<2>, CartesianLayout<2>, float> MySOM;
    typedef Trainer<CartesianLayout<2>, CartesianLayout<2>, float, false> MyTrainer;

    uint32_t som_dim = 2;
    uint32_t neuron_dim = 2;
    MySOM som({som_dim, som_dim}, {neuron_dim, neuron_dim}, 0.0);

    auto&& f = GaussianFunctor(1.1, 0.2);

    EXPECT_THROW(MyTrainer(som, f, 0,  0, false, 1.0, Interpolation::BILINEAR), pink::exception);
    EXPECT_THROW(MyTrainer(som, f, 0,  3, false, 1.0, Interpolation::BILINEAR), pink::exception);
    EXPECT_THROW(MyTrainer(som, f, 0, 90, false, 1.0, Interpolation::BILINEAR), pink::exception);

    EXPECT_NO_THROW(MyTrainer(som, f, 0,   1, false, 1.0, Interpolation::BILINEAR));
    EXPECT_NO_THROW(MyTrainer(som, f, 0,   4, false, 1.0, Interpolation::BILINEAR));
    EXPECT_NO_THROW(MyTrainer(som, f, 0, 720, false, 1.0, Interpolation::BILINEAR));
}

TEST(SelfOrganizingMapTest, trainer_cartesian_2d_int)
{
    typedef Data<CartesianLayout<2>, int> DataType;
    typedef SOM<CartesianLayout<2>, CartesianLayout<2>, int> SOMType;
    typedef Trainer<CartesianLayout<2>, CartesianLayout<2>, int, false> MyTrainer;

    uint32_t som_dim = 2;
    uint32_t image_dim = 2;
    uint32_t neuron_dim = 2;

    DataType data({image_dim, image_dim}, {1, 2, 3, std::numeric_limits<int>::max()});
    SOMType som({som_dim, som_dim}, {neuron_dim, neuron_dim}, std::vector<int>(16, 0));

    auto&& f = GaussianFunctor(1.1, 0.2);

    MyTrainer trainer(som, f, 0, 1, false, 0.0, Interpolation::BILINEAR);
    trainer(data);

    EXPECT_EQ(155767632, (som.get_neuron({0, 0}) [{1, 1}] ));
}
