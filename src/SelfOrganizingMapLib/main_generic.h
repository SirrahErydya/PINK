/**
 * @file   Pink/main_generic.h
 * @brief  Generic main routine of PINK
 * @date   Oct 8, 2018
 * @author Bernd Doser, HITS gGmbH
 */

#include <iostream>

#include "ImageProcessingLib/ImageIterator.h"
#include "SelfOrganizingMapLib/Data.h"
#include "SelfOrganizingMapLib/DataIO.h"
#include "SelfOrganizingMapLib/FileIO.h"
#include "SOM.h"
#include "Trainer_generic.h"
#include "UtilitiesLib/DistributionFunction.h"
#include "UtilitiesLib/DistributionFunctor.h"
#include "UtilitiesLib/InputData.h"
#include "UtilitiesLib/pink_exception.h"

namespace pink {

template <typename SOMLayout, typename DataLayout, typename T, bool UseGPU>
void main_generic(InputData const& input_data)
{
    SOM<SOMLayout, DataLayout, T> som(input_data);
#ifdef __CUDACC__
    som.update_device();
#endif

    auto&& distribution_function = GaussianFunctor(input_data.sigma, input_data.damping);

    if (input_data.executionPath == ExecutionPath::TRAIN)
    {
        Trainer_generic<SOMLayout, DataLayout, T, UseGPU> trainer(
            som
            ,distribution_function
            ,input_data.verbose
            ,input_data.numberOfRotations
            ,input_data.use_flip
            ,input_data.max_update_distance
            ,input_data.interpolation
#ifdef __CUDACC__
            ,input_data.block_size_1
            ,input_data.useMultipleGPUs
#endif
        );

        for (auto&& iter_image_cur = ImageIterator<T>(input_data.imagesFilename), iter_image_end = ImageIterator<T>();
            iter_image_cur != iter_image_end; ++iter_image_cur)
        {
            auto&& beg = iter_image_cur->getPointerOfFirstPixel();
            auto&& end = beg + iter_image_cur->getSize();
            Data<DataLayout, T> data({input_data.image_dim, input_data.image_dim}, std::vector<T>(beg, end));

#ifdef __CUDACC__
            data.update_device();
#endif
            trainer(data);
        }

#ifdef __CUDACC__
        som.update_host();
#endif

        std::cout << "  Write final SOM to " << input_data.resultFilename << " ... " << std::flush;
        write(som, input_data.resultFilename);
        std::cout << "done." << std::endl;

        if (input_data.verbose) {
            std::cout << "\n  Number of updates of each neuron:\n"
                      << trainer.get_update_info()
                      << std::endl;
        }
    }
    else if (input_data.executionPath == ExecutionPath::MAP)
    {
        //Mapper mapper;
    }
    else
    {
        pink::exception("Unknown execution path");
    }
}

} // namespace pink
