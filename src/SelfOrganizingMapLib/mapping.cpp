/**
 * @file   SelfOrganizingMapLib/mapping.cpp
 * @date   Nov 28, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#include "ImageProcessingLib/ImageIterator.h"
#include "SelfOrganizingMap.h"
#include "SOM.h"
#include "UtilitiesLib/Error.h"
#include <chrono>
#include <iomanip>
#include <iostream>

using namespace std;
using namespace PINK;
using namespace chrono;

void SOM::mapping()
{
    cout << "  Starting C version of mapping.\n" << endl;

    // Open result file
    std::ofstream resultFile(inputData_.resultFilename);
    if (!resultFile) fatalError("Error opening " + inputData_.resultFilename);
    resultFile.write((char*)&inputData_.numberOfImages, sizeof(int));
    resultFile.write((char*)&inputData_.som_width, sizeof(int));
    resultFile.write((char*)&inputData_.som_height, sizeof(int));
    resultFile.write((char*)&inputData_.som_depth, sizeof(int));

    // Memory allocation
    int rotatedImagesSize = inputData_.numberOfChannels * inputData_.numberOfRotations * inputData_.neuron_size;
    if (inputData_.useFlip) rotatedImagesSize *= 2;
    if (inputData_.verbose) cout << "  Size of rotated images = " << rotatedImagesSize * sizeof(float) << " bytes" << endl;
    vector<float> rotatedImages(rotatedImagesSize);

    if (inputData_.verbose) cout << "  Size of euclidean distance matrix = " << inputData_.som_size * sizeof(float) << " bytes" << endl;
    vector<float> euclideanDistanceMatrix(inputData_.som_size);

    if (inputData_.verbose) cout << "  Size of best rotation matrix = " << inputData_.som_size * sizeof(int) << " bytes\n" << endl;
    vector<int> bestRotationMatrix(inputData_.som_size);

    float progress = 0.0;
    float progressStep = 1.0 / inputData_.numberOfImages;
    float nextProgressPrint = inputData_.progressFactor;
    int progressPrecision = rint(log10(1.0 / inputData_.progressFactor)) - 2;
    if (progressPrecision < 0) progressPrecision = 0;

    // Start timer
    auto startTime = steady_clock::now();
    int updateCount = 0;

    for (ImageIterator<float> iterImage(inputData_.imagesFilename), iterEnd; iterImage != iterEnd; ++iterImage, ++updateCount)
    {
        if ((inputData_.progressFactor < 1.0 and progress > nextProgressPrint) or
            (inputData_.progressFactor >= 1.0 and updateCount != 0 and !(updateCount % static_cast<int>(inputData_.progressFactor))))
        {
            cout << "  Progress: " << setw(12) << updateCount << " updates, "
                 << fixed << setprecision(progressPrecision) << setw(3) << progress*100 << " % ("
                 << duration_cast<seconds>(steady_clock::now() - startTime).count() << " s)" << endl;

            nextProgressPrint += inputData_.progressFactor;
            startTime = steady_clock::now();
        }
        progress += progressStep;

        generateRotatedImages(&rotatedImages[0], iterImage->getPointerOfFirstPixel(), inputData_.numberOfRotations,
            inputData_.image_dim, inputData_.neuron_dim, inputData_.useFlip, inputData_.interpolation,
            inputData_.numberOfChannels);

        generateEuclideanDistanceMatrix(&euclideanDistanceMatrix[0], &bestRotationMatrix[0],
            inputData_.som_size, &som_[0], inputData_.numberOfChannels * inputData_.neuron_size,
            inputData_.numberOfRotationsAndFlip, &rotatedImages[0]);

        resultFile.write((char*)&euclideanDistanceMatrix[0], inputData_.som_size * sizeof(float));
    }

    cout << "  Progress: " << setw(12) << updateCount << " updates, 100 % ("
         << duration_cast<seconds>(steady_clock::now() - startTime).count() << " s)" << endl;
}