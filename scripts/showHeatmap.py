#!/usr/bin/python

import getopt
import numpy
import matplotlib.pylab as plt
import struct
import sys

def print_usage():
    print ''
    print 'Usage:'
    print ''
    print '  showHeatmap.py [Options] <inputfile>'
    print ''
    print 'Options:'
    print ''
    print '  --help, -h              Print this lines.'
    print '  --image, -i <int>       Number of image to visualize (default = 0).'
    print ''

if __name__ == "__main__":

    try:
        opts, args = getopt.getopt(sys.argv[1:],"hi:",["help", "image="])
    except getopt.GetoptError:
        print_usage()
        sys.exit(1)

    imageNumber = 0
    channelNumber = 0

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            print_usage()
            sys.exit()
        elif opt in ("-i", "--image"):
            imageNumber = int(arg)

    if len(args) != 1:
        print_usage()
        print 'ERROR: Input file is missing.'
        sys.exit(1)

    inputfile = args[0]

    print 'Input file is ', inputfile
    print 'Image number is ', imageNumber

    inFile = open(inputfile, 'rb')
    numberOfImages, SOM_width, SOM_height, SOM_depth = struct.unpack('i' * 4, inFile.read(4*4))

    print 'Number of images = ', numberOfImages 
    print 'SOM_width = ', SOM_width
    print 'SOM_height = ', SOM_height
    print 'SOM_depth = ', SOM_depth

    if imageNumber >= numberOfImages:
        print 'Image number too large.'
        sys.exit(1)

    size = SOM_width * SOM_height * SOM_depth
    image_width = SOM_width
    image_height = SOM_depth * SOM_height
    inFile.seek(imageNumber * size * 4, 1)
    array = numpy.array(struct.unpack('f' * size, inFile.read(size * 4)))
    data = numpy.ndarray([SOM_width, SOM_height, SOM_depth], 'float', array)
    data = numpy.swapaxes(data, 0, 2)
    data = numpy.reshape(data, (image_height, image_width))

    fig = plt.figure()
    ax = fig.add_subplot(1,1,1)
    ax.set_aspect('equal')
    plt.imshow(data, interpolation='nearest', cmap=plt.cm.jet)
    plt.colorbar()
    plt.show()

    print 'All done.'
    sys.exit()