#ifndef VTKINTERACTORSTYLEDICOM_H
#define VTKINTERACTORSTYLEDICOM_H

#include <vtkObjectFactory.h>
#include "vtkInteractorStyleImage.h"
#include "vtkImageViewer2.h"
#include <vtkRenderWindowInteractor.h>
#include <vtkTextMapper.h>

// helper class to format slice status message
class StatusMessage {
public:
	static std::string Format(int slice, int maxSlice) {
		std::stringstream tmp;
		tmp << "Slice:  " << slice + 1 << "/" << maxSlice + 1;
		return tmp.str();
	}
};

class vtkInteractorStyleDicom : public vtkInteractorStyleImage
{
public:
   static vtkInteractorStyleDicom* New();
   vtkTypeMacro(vtkInteractorStyleDicom, vtkInteractorStyleImage);
 
protected:
   vtkImageViewer2* _ImageViewer;
   vtkTextMapper* _StatusMapper;
   int _Slice;
   int _MinSlice;
   int _MaxSlice;
 
public:
   void SetImageViewer(vtkImageViewer2* imageViewer) {
	  _ImageViewer = imageViewer;
      _MinSlice = imageViewer->GetSliceMin();
      _MaxSlice = imageViewer->GetSliceMax();
      _Slice = _MinSlice;
      cout << "Slicer: Min = " << _MinSlice << ", Max = " << _MaxSlice << std::endl;
   }

   void UpdateSliceInfo() {

	   std::cout << "update slice info with viewer " << _ImageViewer <<std::endl;
	   _MinSlice = _ImageViewer->GetSliceMin();
	   _MaxSlice = _ImageViewer->GetSliceMax();
	   _Slice = _MinSlice;
	   cout << "Slicer: Min = " << _MinSlice << ", Max = " << _MaxSlice << std::endl;
   }
 
   void SetStatusMapper(vtkTextMapper* statusMapper) {
      _StatusMapper = statusMapper;
   }
 
 
protected:

   void MoveSliceForward() {
	   std::cout << _MinSlice << "/" << _Slice << "/" << _MaxSlice << std::endl;
      if(_Slice < _MaxSlice) {
         _Slice += 1;
         cout << "MoveSliceForward::Slice = " << _Slice << std::endl;
         _ImageViewer->SetSlice(_Slice);
         std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
         _StatusMapper->SetInput(msg.c_str());
         _ImageViewer->Render();
      }
   }
 
   void MoveSliceBackward() {
	   std::cout << _MinSlice << "/" << _Slice << "/" << _MaxSlice << std::endl;

      if(_Slice > _MinSlice) {
         _Slice -= 1;
         cout << "MoveSliceBackward::Slice = " << _Slice << std::endl;
         _ImageViewer->SetSlice(_Slice);
         std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
         _StatusMapper->SetInput(msg.c_str());
         _ImageViewer->Render();
      }
   }
 
 
   virtual void OnKeyDown() {
      std::string key = this->GetInteractor()->GetKeySym();
      if(key.compare("Up") == 0) {
         //cout << "Up arrow key was pressed." << endl;
         MoveSliceForward();
      }
      else if(key.compare("Down") == 0) {
         //cout << "Down arrow key was pressed." << endl;
         MoveSliceBackward();
      }
      // forward event
      vtkInteractorStyleImage::OnKeyDown();
   }
 
 
   virtual void OnMouseWheelForward() {
      //std::cout << "Scrolled mouse wheel forward." << std::endl;
      MoveSliceForward();
      // don't forward events, otherwise the image will be zoomed 
      // in case another interactorstyle is used (e.g. trackballstyle, ...)
      // vtkInteractorStyleImage::OnMouseWheelForward();
   }
 
 
   virtual void OnMouseWheelBackward() {
      //std::cout << "Scrolled mouse wheel backward." << std::endl;
      if(_Slice > _MinSlice) {
         MoveSliceBackward();
      }
      // don't forward events, otherwise the image will be zoomed 
      // in case another interactorstyle is used (e.g. trackballstyle, ...)
      // vtkInteractorStyleImage::OnMouseWheelBackward();
   }
};
 
vtkStandardNewMacro(vtkInteractorStyleDicom);

#endif