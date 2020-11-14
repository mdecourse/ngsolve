#pragma once

/*********************************************************************/
/* File:   vtkoutput.hpp                                             */
/* Author: Christoph Lehrenfeld                                      */
/* Date:   1. June 2014                                              */
/*********************************************************************/

namespace ngcomp
{ 

  class ValueField : public Array<double>
  {
    int dim = 1;
    string name = "none";
  public:
    ValueField(){;};
    ValueField(int adim, string aname);
    void SetDimension(int adim){ dim = adim; }
    int Dimension(){ return dim;}
    void SetName(string aname){ name = aname; }
    string Name(){ return name;}
  };
  
/* ---------------------------------------- 
   numproc
   ---------------------------------------- */

  class BaseVTKOutput
  {
  public:
    virtual ~BaseVTKOutput() { ; }
    virtual void Do (LocalHeap & lh, VorB vb = VOL, const BitArray * drawelems = 0) = 0;
    string lastoutputname = "";
  };
  
  template <int D> 
  class VTKOutput : public BaseVTKOutput
  {
  protected:

    shared_ptr<MeshAccess> ma = nullptr;
    Array<shared_ptr<CoefficientFunction>> coefs;
    Array<string> fieldnames;
    string filename;
    int subdivision;
    int only_element = -1;

    Array<shared_ptr<ValueField>> value_field;
    Array<Vec<D>> points;
    Array<INT<ELEMENT_MAXPOINTS+1>> cells;

    int output_cnt = 0;
    
    shared_ptr<ofstream> fileout;
    
  public:

    VTKOutput (const Array<shared_ptr<CoefficientFunction>> &,
               const Flags &,shared_ptr<MeshAccess>);

    VTKOutput (shared_ptr<MeshAccess>, const Array<shared_ptr<CoefficientFunction>> &,
               const Array<string> &, string, int, int);
    virtual ~VTKOutput() { ; }
    
    void ResetArrays();
    
    void FillReferenceTrig(Array<IntegrationPoint> & ref_coords,Array<INT<ELEMENT_MAXPOINTS+1>> & ref_elems);    
    void FillReferenceQuad(Array<IntegrationPoint> & ref_coords,Array<INT<ELEMENT_MAXPOINTS+1>> & ref_elems);    
    void FillReferenceTet(Array<IntegrationPoint> & ref_coords,Array<INT<ELEMENT_MAXPOINTS+1>> & ref_elems);    
    void FillReferenceHex(Array<IntegrationPoint> & ref_coords,Array<INT<ELEMENT_MAXPOINTS+1>> & ref_elems); 
    void FillReferencePrism(Array<IntegrationPoint> & ref_coords,Array<INT<ELEMENT_MAXPOINTS+1>> & ref_elems);    
    // void FillReferenceData3D(Array<IntegrationPoint> & ref_coords, Array<INT<D+1>> & ref_tets);
    void PrintPoints();
    void PrintCells();
    void PrintCellTypes(VorB vb, const BitArray * drawelems=nullptr);
    void PrintFieldData();    

    virtual void Do (LocalHeap & lh, VorB vb = VOL, const BitArray * drawelems = 0);
  };


  class NumProcVTKOutput : public NumProc
  {
  protected:
    shared_ptr<BaseVTKOutput> vtkout = nullptr;
  public:
    NumProcVTKOutput (shared_ptr<PDE> apde, const Flags & flags);
    virtual ~NumProcVTKOutput() { }

    virtual string GetClassName () const
    {
      return "NumProcVTKOutput";
    }
    
    virtual void Do (LocalHeap & lh);
  };
  
}


