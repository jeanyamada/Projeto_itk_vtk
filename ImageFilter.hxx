
#ifndef __itkImageFilter_hxx
#define __itkImageFilter_hxx

#include "ImageFilter.h"
#include "itkObjectFactory.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

namespace itk {

template <class TImage>
void ImageFilter<TImage>::GenerateData()
{
    typename TImage::ConstPointer input = this->GetInput();
    typename TImage::Pointer output = this->GetOutput();

    //FILE* arq = fopen("dados.txt", "w");

    itk::Index<3> cornerPixel = input->GetLargestPossibleRegion().GetIndex();


    this->AllocateOutputs();

    /*
    ImageAlgorithm::Copy(input.GetPointer(), output.GetPointer(), output->GetRequestedRegion(),
        output->GetRequestedRegion());
    */

    // std::cout << input->GetPixel(cornerPixel);
    typename TImage::RegionType region = output->GetLargestPossibleRegion();

    typename TImage::SizeType size = region.GetSize();

    float k0 = 3.7, k1 = 0.4, k2 = 1, e = 4;

    double maxValue =0.0;

    for (int i = 0; i < size[2]; i++) {

        float mG = 0.0, dG = 0.0;
        double hist[4096];

        memset(hist, 0, sizeof(hist));

        cornerPixel[2] = i;

        for (int p = 0; p < size[0]; p++)
            for (int o = 0; o < size[1]; o++) {
                itk::Index<3> cornerPixelHist;
                cornerPixelHist[0] = p;
                cornerPixelHist[1] = o;
                cornerPixelHist[2] = i;
                hist[(int)input->GetPixel(cornerPixelHist)]++;
            }

        for (int p = 0; p < 4096; p++)
            mG += (hist[p] / (size[0] * size[1])) * p;

        for (int p = 0; p < size[0]; p++)
            for (int o = 0; o < size[1]; o++) {
                itk::Index<3> cornerPixelHist;
                cornerPixelHist[0] = p;
                cornerPixelHist[1] = o;
                cornerPixelHist[2] = i;
                dG += pow(((float)input->GetPixel(cornerPixelHist) - mG), 2);
            }

        dG /= (float)(size[0] * size[1]);

        dG = sqrt(dG);
/*
        fprintf(arq, "%f;%f;", mG, dG);

        for (int p = 0; p < 1261; p++)
            fprintf(arq, "%f;", hist[p]);

        fprintf(arq, "\n");
*/
        for (int k = 0; k < size[0]; k++) {

            for (int j = 0; j < size[1]; j++) {

                if (k + 3 < (int)size[0] && j + 3 < (int)size[1]) {
                    float mS = 0.0, dS = 0.0;
                    /*
                    double hist_aux[1261];

                    memset(hist_aux, 0, sizeof(hist_aux));
                    */
                    for (int p = 0; p < 3; p++)
                        for (int o = 0; o < 3; o++) {
                            itk::Index<3> cornerPixelHist;
                            cornerPixelHist[0] = p + k;
                            cornerPixelHist[1] = o + j;
                            cornerPixelHist[2] = i;
                            //hist_aux[(int)input->GetPixel(cornerPixelHist)]++;
                            mS+=input->GetPixel(cornerPixelHist);
                        }

                    /*
                    for (int p = 0; p < 1261; p++) {
                        mS += (hist_aux[p] / pow(3, 2)) * p;
                    }
                    */
                    mS/=9;

                    for (int p = 0; p < 3; p++)
                        for (int o = 0; o < 3; o++) {
                            itk::Index<3> cornerPixelHist;
                            cornerPixelHist[0] = p + k;
                            cornerPixelHist[1] = o + j;
                            cornerPixelHist[2] = i;
                            dS += pow(((float)input->GetPixel(cornerPixelHist) - mS), 2);
                        }

                    dS /= (pow(3, 2) - 1);

                    dS = sqrt(dS);

                    itk::Index<3> teste;
                    teste[0] = k;
                    teste[1] = j;
                    teste[2] = i;


                    if(maxValue < input->GetPixel(teste))
                        maxValue = input->GetPixel(teste);

                    if (/*input->GetPixel(teste) >= 1200*/mS >= k0 * mG /*|| ((k1 * dG <= dS) && (dS <= k2 * dG))*/) {
                        itk::Index<3> cornerPixelHist;
                        cornerPixelHist[0] = k + (3 / 2);
                        cornerPixelHist[1] = j + (3 / 2);
                        cornerPixelHist[2] = i;

                        //output->SetPixel(cornerPixelHist, input->GetPixel(cornerPixelHist)*e);
                       // std::cout << std::endl <<cornerPixelHist[0] <<" " << cornerPixelHist[1] <<" " <<cornerPixelHist[2] << std::endl;
                        output->SetPixel(cornerPixelHist,4095);
                    }

                    else {
                        itk::Index<3> cornerPixelHist;
                        cornerPixelHist[0] = k;
                        cornerPixelHist[1] = j;
                        cornerPixelHist[2] = i;
                        //output->SetPixel(cornerPixelHist, input->GetPixel(cornerPixelHist));
                        output->SetPixel(cornerPixelHist, 0);
                    }
                }
            }
        }
    }
    std::cout << std::endl <<maxValue << std::endl;

//    fclose(arq);
}

} // end namespace

#endif
