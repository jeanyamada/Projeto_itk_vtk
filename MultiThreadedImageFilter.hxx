#ifndef __itkMultiThreadedImageFilter_hxx
#define __itkMultiThreadedImageFilter_hxx

#include "MultiThreadedImageFilter.h"

#include "itkObjectFactory.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

namespace itk
{

template< class TImage>
void MultiThreadedImageFilter< TImage>
::ThreadedGenerateData(const OutputImageRegionType & region, ThreadIdType threadId)
{
  std::cout << "Thread " << threadId << " given region: " << region << std::endl;

  typename TImage::ConstPointer input = this->GetInput();
  typename TImage::Pointer output = this->GetOutput();

  mageType::SizeType radius;
    radius[0] = 1;
    radius[1] = 1;

    itk::NeighborhoodIterator<ImageType> iterator(radius, input,region);


    while(!iterator.IsAtEnd())
      {
      // Set the current pixel to white
      iterator.SetCenterPixel(255);

      for(unsigned int i = 0; i < 9; i++)
        {
        ImageType::IndexType index = iterator.GetIndex(i);
        std::cout << index[0] << " " << index[1] << std::endl;

        bool IsInBounds;
        iterator.GetPixel(i, IsInBounds);
        if(IsInBounds)
          {
          iterator.SetPixel(i,255);
          }
        }
      ++iterator;
      }
}

}// end namespace


#endif
