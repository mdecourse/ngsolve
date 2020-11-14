#include <bla.hpp>


namespace ngbla
{
#include "matkernel.hpp"


  // these kernels should go into generate_mat_kernels ...
  // T * X      T .. triangular, X rowwise
  // L/R, Norm/NonNorm,  Solve + Mult
  //
  // not so nice: T * X  ... X colwise    needed ???
  

  template <size_t H, OPERATION OP, typename TB>
  INLINE void MatKernel2AddAB (size_t hb, size_t wb, double * pa, size_t da, TB * pb, size_t db, double * pc, size_t dc)
  {
    constexpr size_t SW = SIMD<double>::Size();
    constexpr size_t SWdTB = sizeof(SIMD<double>)/sizeof(TB);
    size_t l = 0, lb = 0;
    for ( ; l+3*SW <= wb; l += 3*SW, lb += 3*SWdTB)
      MatKernelMultAB<H,3,OP> (hb, pa, da, pb+lb, db, pc+l, dc);
    for ( ; l+SW <= wb; l += SW, lb += SWdTB)
      MatKernelMultAB<H,1,OP> (hb, pa, da, pb+lb, db, pc+l, dc);
    if (l < wb)
      MatKernelMultABMask<H,OP>(hb, SIMD<mask64>(wb-l), pa, da, pb+lb, db, pc+l, dc);
  }


  
  template <int HC>
  void TriangularMultKernel (size_t wx, double * pl, size_t dl, double * px, size_t dx);


  template <>
  void TriangularMultKernel<0> (size_t wx, double * pl, size_t dl, double * px, size_t dx)
  {
    ;
  }
  
  template <>
  void TriangularMultKernel<1> (size_t wx, double * pl, size_t dl, double * px, size_t dx)
  {
    constexpr size_t SW = SIMD<double>::Size();
    
    SIMD<double> L00(pl[0]);

    size_t i = 0;
    for ( ; i+SW <= wx; i+=SW)
      {
        SIMD<double> x0(px+i);
        SIMD<double> y0 = L00 * x0;
        y0.Store (px+i);
      }

    for ( ; i+1 <= wx; i+=1)
      {
        SIMD<double,1> x0(px+i);
        SIMD<double,1> y0 = L00[0] * x0;
        y0.Store (px+i);
      }
  }

  template <>
  void TriangularMultKernel<2> (size_t wx, double * pl, size_t dl, double * px, size_t dx)
  {
    constexpr size_t SW = SIMD<double>::Size();
    
    SIMD<double> L00(pl[0]);
    SIMD<double> L10(pl[dl]);
    SIMD<double> L11(pl[dl+1]);

    size_t i = 0;
    for ( ; i+SW <= wx; i+=SW)
      {
        SIMD<double> x0(px+i);
        SIMD<double> x1(px+dx+i);
        SIMD<double> y0 = L00 * x0;
        SIMD<double> y1 = L10 * x0 + L11 * x1;
        y0.Store (px+i);
        y1.Store (px+dx+i);
      }

    for ( ; i+1 <= wx; i+=1)
      {
        SIMD<double,1> x0(px+i);
        SIMD<double,1> x1(px+dx+i);
        SIMD<double,1> y0 = L00[0] * x0;
        SIMD<double,1> y1 = L10[0] * x0 + L11[0] * x1;
        y0.Store (px+i);
        y1.Store (px+dx+i);
      }
  }


  

  template <>
  void TriangularMultKernel<3> (size_t wx, double * pl, size_t dl, double * px, size_t dx)
  {
    constexpr size_t SW = SIMD<double>::Size();
    
    SIMD<double> L00(pl[0]);
    SIMD<double> L10(pl[dl]);
    SIMD<double> L11(pl[dl+1]);
    SIMD<double> L20(pl[2*dl]);
    SIMD<double> L21(pl[2*dl+1]);
    SIMD<double> L22(pl[2*dl+2]);

    size_t i = 0;
    for ( ; i+SW <= wx; i+=SW)
      {
        SIMD<double> x0(px+i);
        SIMD<double> x1(px+dx+i);
        SIMD<double> x2(px+2*dx+i);
        SIMD<double> y0 = L00 * x0;
        SIMD<double> y1 = L10 * x0 + L11 * x1;
        SIMD<double> y2 = L20 * x0 + L21 * x1 + L22 * x2;
        y0.Store (px+i);
        y1.Store (px+dx+i);
        y2.Store (px+2*dx+i);
      }

    for ( ; i+1 <= wx; i+=1)
      {
        SIMD<double,1> x0(px+i);
        SIMD<double,1> x1(px+dx+i);
        SIMD<double,1> x2(px+2*dx+i);
        SIMD<double,1> y0 = L00[0] * x0;
        SIMD<double,1> y1 = L10[0] * x0 + L11[0] * x1;
        SIMD<double,1> y2 = L20[0] * x0 + L21[0] * x1 + L22[0] * x2;
        y0.Store (px+i);
        y1.Store (px+dx+i);
        y2.Store (px+2*dx+i);
      }
  }

  
  
  template <>
  void TriangularMultKernel<4> (size_t wx, double * pl, size_t dl, double * px, size_t dx)
  {
    constexpr size_t SW = SIMD<double>::Size();
    
    SIMD<double> L00(pl[0]);
    SIMD<double> L10(pl[dl]);
    SIMD<double> L11(pl[dl+1]);
    SIMD<double> L20(pl[2*dl]);
    SIMD<double> L21(pl[2*dl+1]);
    SIMD<double> L22(pl[2*dl+2]);
    SIMD<double> L30(pl[3*dl]);
    SIMD<double> L31(pl[3*dl+1]);
    SIMD<double> L32(pl[3*dl+2]);
    SIMD<double> L33(pl[3*dl+3]);

    size_t i = 0;
    for ( ; i+SW <= wx; i+=SW)
      {
        SIMD<double> x0(px+i);
        SIMD<double> x1(px+dx+i);
        SIMD<double> x2(px+2*dx+i);
        SIMD<double> x3(px+3*dx+i);
        SIMD<double> y0 = L00 * x0;
        SIMD<double> y1 = L10 * x0 + L11 * x1;
        SIMD<double> y2 = L20 * x0 + L21 * x1 + L22 * x2;
        SIMD<double> y3 = L30 * x0 + L31 * x1 + L32 * x2 + L33 * x3;
        y0.Store (px+i);
        y1.Store (px+dx+i);
        y2.Store (px+2*dx+i);
        y3.Store (px+3*dx+i);
      }

    for ( ; i+1 <= wx; i+=1)
      {
        SIMD<double,1> x0(px+i);
        SIMD<double,1> x1(px+dx+i);
        SIMD<double,1> x2(px+2*dx+i);
        SIMD<double,1> x3(px+3*dx+i);
        SIMD<double,1> y0 = L00[0] * x0;
        SIMD<double,1> y1 = L10[0] * x0 + L11[0] * x1;
        SIMD<double,1> y2 = L20[0] * x0 + L21[0] * x1 + L22[0] * x2;
        SIMD<double,1> y3 = L30[0] * x0 + L31[0] * x1 + L32[0] * x2 + L33[0] * x3;
        y0.Store (px+i);
        y1.Store (px+dx+i);
        y2.Store (px+2*dx+i);
        y3.Store (px+3*dx+i);
      }
  }
  
  void TriangularMultLL3 (BareSliceMatrix<double> bL, SliceMatrix<double> X)
  {
    size_t n = X.Height();
    size_t i = n;
    auto L = bL.AddSize(n,n);
    
    constexpr size_t HA = 4;
    
    size_t reminder = n % HA;
    if (reminder > 0)
      {
        Switch<HA> (reminder, [L,X,i] (auto r)
                    {
                      TriangularMultKernel<r.value> (X.Width(), &L(i-r.value,i-r.value), L.Dist(), &X(i-r.value,0), X.Dist());
                      
                      if (i > r)
                        if constexpr (r.value > 0)
                                       MatKernel2AddAB<r.value,ADD> (i-r.value, X.Width(),
                                                                     &L(i-r.value,0), L.Dist(),
                                                                     &X(0,0), X.Dist(),
                                                                     &X(i-r.value,0), X.Dist());
                    });
      }
    
    i -= reminder;
    for ( ; i >= HA; i -= HA)
      {
        /*
        for (size_t j = 1; j < HA; j++)
          {
            X.Row(i-j) *= L(i-j, i-j);
            X.Row(i-j) += Trans(X.Rows(i-HA,i-j)) * L.Row(i-j).Range(i-HA,i-j);            
          }
        X.Row(i-HA) *= L(i-HA,i-HA);
        */
        TriangularMultKernel<4> (X.Width(), &L(i-HA,i-HA), L.Dist(), &X(i-HA,0), X.Dist());
        if (i > HA)
          MatKernel2AddAB<HA,ADD> (i-HA, X.Width(), &L(i-HA,0), L.Dist(), &X(0,0), X.Dist(), &X(i-HA,0), X.Dist());
      }
    
    /*
    for (size_t i = n; i-- > 1 ; )
      {
        X.Row(i) *= L(i,i);
        X.Row(i) += Trans(X.Rows(0,i-1)) * L.Row(i).Range(0,i-1);
      }
    X.Row(0) *= L(0,0);
    */
  }


  void TriangularMultLL2 (BareSliceMatrix<double> T, SliceMatrix<double> X)
  {
    size_t n = X.Height();
    if (n < 128)
      {
        TriangularMultLL3 (T,X);
        return;
      }
    IntRange r1(0,n/2), r2(n/2,n);
    auto T11 = T.Rows(r1).Cols(r1);
    // auto T12 = T.Rows(r1).Cols(r2).AddSize(r1.Size(), r2.Size());
    auto T21 = T.Rows(r2).Cols(r1).AddSize(r2.Size(), r1.Size());
    auto T22 = T.Rows(r2).Cols(r2);
    auto X1 = X.Rows(r1);
    auto X2 = X.Rows(r2);

    TriangularMultLL2 (T22, X2);
    X2 += T21 * X1;
    TriangularMultLL2 (T11, X1);
  }

  
  void TriangularMultLL (BareSliceMatrix<double> L, SliceMatrix<double> X)  
  {
    size_t i = 0;
    constexpr size_t bw = 256;
    for ( ; i+bw <= X.Width(); i += bw)
      TriangularMultLL2 (L, X.Cols(i,i+bw));
    if (i < X.Width())
      TriangularMultLL2 (L, X.Cols(i,X.Width()));      
  }
















  /* ***************************  TriangularMult - Normalized **************************** */



  
  template <int HC>
  void TriangularMultKernelN (size_t wx, double * pl, size_t dl, double * px, size_t dx);


  template <>
  void TriangularMultKernelN<0> (size_t wx, double * pl, size_t dl, double * px, size_t dx)
  {
    ;
  }
  
  template <>
  void TriangularMultKernelN<1> (size_t wx, double * pl, size_t dl, double * px, size_t dx)
  {
    ;
  }

  template <>
  void TriangularMultKernelN<2> (size_t wx, double * pl, size_t dl, double * px, size_t dx)
  {
    constexpr size_t SW = SIMD<double>::Size();
    
    SIMD<double> L10(pl[dl]);

    size_t i = 0;
    for ( ; i+SW <= wx; i+=SW)
      {
        SIMD<double> x0(px+i);
        SIMD<double> x1(px+dx+i);
        SIMD<double> y0 = x0;
        SIMD<double> y1 = L10 * x0 + x1;
        y0.Store (px+i);
        y1.Store (px+dx+i);
      }

    for ( ; i+1 <= wx; i+=1)
      {
        SIMD<double,1> x0(px+i);
        SIMD<double,1> x1(px+dx+i);
        SIMD<double,1> y0 = x0;
        SIMD<double,1> y1 = L10[0] * x0 + x1;
        y0.Store (px+i);
        y1.Store (px+dx+i);
      }
  }


  

  template <>
  void TriangularMultKernelN<3> (size_t wx, double * pl, size_t dl, double * px, size_t dx)
  {
    constexpr size_t SW = SIMD<double>::Size();
    
    SIMD<double> L10(pl[dl]);
    SIMD<double> L20(pl[2*dl]);
    SIMD<double> L21(pl[2*dl+1]);

    size_t i = 0;
    for ( ; i+SW <= wx; i+=SW)
      {
        SIMD<double> x0(px+i);
        SIMD<double> x1(px+dx+i);
        SIMD<double> x2(px+2*dx+i);
        SIMD<double> y0 = x0;
        SIMD<double> y1 = L10 * x0 + x1;
        SIMD<double> y2 = L20 * x0 + L21 * x1 + x2;
        y0.Store (px+i);
        y1.Store (px+dx+i);
        y2.Store (px+2*dx+i);
      }

    for ( ; i+1 <= wx; i+=1)
      {
        SIMD<double,1> x0(px+i);
        SIMD<double,1> x1(px+dx+i);
        SIMD<double,1> x2(px+2*dx+i);
        SIMD<double,1> y0 = x0;
        SIMD<double,1> y1 = L10[0] * x0 + x1;
        SIMD<double,1> y2 = L20[0] * x0 + L21[0] * x1 + x2;
        y0.Store (px+i);
        y1.Store (px+dx+i);
        y2.Store (px+2*dx+i);
      }
  }

  
  
  template <>
  void TriangularMultKernelN<4> (size_t wx, double * pl, size_t dl, double * px, size_t dx)
  {
    constexpr size_t SW = SIMD<double>::Size();
    
    SIMD<double> L10(pl[dl]);
    SIMD<double> L20(pl[2*dl]);
    SIMD<double> L21(pl[2*dl+1]);
    SIMD<double> L30(pl[3*dl]);
    SIMD<double> L31(pl[3*dl+1]);
    SIMD<double> L32(pl[3*dl+2]);

    size_t i = 0;
    for ( ; i+SW <= wx; i+=SW)
      {
        SIMD<double> x0(px+i);
        SIMD<double> x1(px+dx+i);
        SIMD<double> x2(px+2*dx+i);
        SIMD<double> x3(px+3*dx+i);
        SIMD<double> y0 = x0;
        SIMD<double> y1 = L10 * x0 + x1;
        SIMD<double> y2 = L20 * x0 + L21 * x1 + x2;
        SIMD<double> y3 = L30 * x0 + L31 * x1 + L32 * x2 + x3;
        y0.Store (px+i);
        y1.Store (px+dx+i);
        y2.Store (px+2*dx+i);
        y3.Store (px+3*dx+i);
      }

    for ( ; i+1 <= wx; i+=1)
      {
        SIMD<double,1> x0(px+i);
        SIMD<double,1> x1(px+dx+i);
        SIMD<double,1> x2(px+2*dx+i);
        SIMD<double,1> x3(px+3*dx+i);
        SIMD<double,1> y0 = x0;
        SIMD<double,1> y1 = L10[0] * x0 + x1;
        SIMD<double,1> y2 = L20[0] * x0 + L21[0] * x1 + x2;
        SIMD<double,1> y3 = L30[0] * x0 + L31[0] * x1 + L32[0] * x2 + x3;
        y0.Store (px+i);
        y1.Store (px+dx+i);
        y2.Store (px+2*dx+i);
        y3.Store (px+3*dx+i);
      }
  }
  
  void TriangularMultLL3N (BareSliceMatrix<double> bL, SliceMatrix<double> X)
  {
    size_t n = X.Height();
    size_t i = n;
    auto L = bL.AddSize(n,n);
    
    constexpr size_t HA = 4;
    
    size_t reminder = n % HA;
    if (reminder > 0)
      {
        Switch<HA> (reminder, [L,X,i] (auto r)
                    {
                      TriangularMultKernelN<r.value> (X.Width(), &L(i-r.value,i-r.value), L.Dist(), &X(i-r.value,0), X.Dist());
                      
                      if (i > r)
                        if constexpr (r.value > 0)
                                       MatKernel2AddAB<r.value,ADD> (i-r.value, X.Width(),
                                                                     &L(i-r.value,0), L.Dist(),
                                                                     &X(0,0), X.Dist(),
                                                                     &X(i-r.value,0), X.Dist());
                    });
      }
    
    i -= reminder;
    for ( ; i >= HA; i -= HA)
      {
        TriangularMultKernelN<4> (X.Width(), &L(i-HA,i-HA), L.Dist(), &X(i-HA,0), X.Dist());
        if (i > HA)
          MatKernel2AddAB<HA,ADD> (i-HA, X.Width(), &L(i-HA,0), L.Dist(), &X(0,0), X.Dist(), &X(i-HA,0), X.Dist());
      }
  }

  void TriangularMultLL2N (BareSliceMatrix<double> T, SliceMatrix<double> X)
  {
    size_t n = X.Height();
    if (n < 128)
      {
        TriangularMultLL3N (T,X);
        return;
      }
    IntRange r1(0,n/2), r2(n/2,n);
    auto T11 = T.Rows(r1).Cols(r1);
    // auto T12 = T.Rows(r1).Cols(r2).AddSize(r1.Size(), r2.Size());
    auto T21 = T.Rows(r2).Cols(r1).AddSize(r2.Size(), r1.Size());
    auto T22 = T.Rows(r2).Cols(r2);
    auto X1 = X.Rows(r1);
    auto X2 = X.Rows(r2);

    TriangularMultLL2N (T22, X2);
    X2 += T21 * X1;
    TriangularMultLL2N (T11, X1);
  }
  
  void TriangularMultLLN (BareSliceMatrix<double> L, SliceMatrix<double> X)  
  {
    size_t i = 0;
    constexpr size_t bw = 256;
    for ( ; i+bw <= X.Width(); i += bw)
      TriangularMultLL2N (L, X.Cols(i,i+bw));
    if (i < X.Width())
      TriangularMultLL2N (L, X.Cols(i,X.Width()));      
  }












  /* ***************************  TriangularSolve - Normalized **************************** */



  
  template <int HC>
  void TriangularSolveKernelN (size_t wx, double * pl, size_t dl, double * px, size_t dx);


  template <>
  void TriangularSolveKernelN<0> (size_t wx, double * pl, size_t dl, double * px, size_t dx)
  {
    ;
  }
  
  template <>
  void TriangularSolveKernelN<1> (size_t wx, double * pl, size_t dl, double * px, size_t dx)
  {
    ;
  }

  template <>
  void TriangularSolveKernelN<2> (size_t wx, double * pl, size_t dl, double * px, size_t dx)
  {
    constexpr size_t SW = SIMD<double>::Size();
    
    SIMD<double> L10(pl[dl]);

    size_t i = 0;
    for ( ; i+SW <= wx; i+=SW)
      {
        SIMD<double> x0(px+i);
        SIMD<double> x1(px+dx+i);
        x1 -= L10 * x0;
        // x0.Store (px+i);
        x1.Store (px+dx+i);
      }

    for ( ; i+1 <= wx; i+=1)
      {
        SIMD<double,1> x0(px+i);
        SIMD<double,1> x1(px+dx+i);
        x1 -= L10[0] * x0;
        // x0.Store (px+i);
        x1.Store (px+dx+i);
      }
  }

  

  template <>
  void TriangularSolveKernelN<3> (size_t wx, double * pl, size_t dl, double * px, size_t dx)
  {
    constexpr size_t SW = SIMD<double>::Size();
    
    SIMD<double> L10(pl[dl]);
    SIMD<double> L20(pl[2*dl]);
    SIMD<double> L21(pl[2*dl+1]);

    size_t i = 0;
    for ( ; i+SW <= wx; i+=SW)
      {
        SIMD<double> x0(px+i);
        SIMD<double> x1(px+dx+i);
        SIMD<double> x2(px+2*dx+i);
        x1 -= L10 * x0;
        x2 -= L20 * x0 + L21 * x1;
        // x0.Store (px+i);
        x1.Store (px+dx+i);
        x2.Store (px+2*dx+i);
      }

    for ( ; i+1 <= wx; i+=1)
      {
        SIMD<double,1> x0(px+i);
        SIMD<double,1> x1(px+dx+i);
        SIMD<double,1> x2(px+2*dx+i);
        x1 -= L10[0] * x0;
        x2 -= L20[0] * x0 + L21[0] * x1;
        // x0.Store (px+i);
        x1.Store (px+dx+i);
        x2.Store (px+2*dx+i);
      }
  }

  
  
  template <>
  void TriangularSolveKernelN<4> (size_t wx, double * pl, size_t dl, double * px, size_t dx)
  {
    constexpr size_t SW = SIMD<double>::Size();
    
    SIMD<double> L10(pl[dl]);
    SIMD<double> L20(pl[2*dl]);
    SIMD<double> L21(pl[2*dl+1]);
    SIMD<double> L30(pl[3*dl]);
    SIMD<double> L31(pl[3*dl+1]);
    SIMD<double> L32(pl[3*dl+2]);

    size_t i = 0;
    for ( ; i+SW <= wx; i+=SW)
      {
        SIMD<double> x0(px+i);
        SIMD<double> x1(px+dx+i);
        SIMD<double> x2(px+2*dx+i);
        SIMD<double> x3(px+3*dx+i);
        x1 -= L10 * x0;
        x2 -= L20 * x0 + L21 * x1;
        x3 -= L30 * x0 + L31 * x1 + L32 * x2;
        // x0.Store (px+i);
        x1.Store (px+dx+i);
        x2.Store (px+2*dx+i);
        x3.Store (px+3*dx+i);
      }

    for ( ; i+1 <= wx; i+=1)
      {
        SIMD<double,1> x0(px+i);
        SIMD<double,1> x1(px+dx+i);
        SIMD<double,1> x2(px+2*dx+i);
        SIMD<double,1> x3(px+3*dx+i);
        x1 -= L10[0] * x0;
        x2 -= L20[0] * x0 + L21[0] * x1;
        x3 -= L30[0] * x0 + L31[0] * x1 + L32[0] * x2;
        // x0.Store (px+i);
        x1.Store (px+dx+i);
        x2.Store (px+2*dx+i);
        x3.Store (px+3*dx+i);
      }
  }
  
  void TriangularSolveLL3N (BareSliceMatrix<double> bL, SliceMatrix<double> X)
  {
    size_t n = X.Height();
    auto L = bL.AddSize(n,n);
    
    constexpr size_t HA = 4;

    size_t i = 0;
    for ( ; i+HA <= n; i += HA)
      {
        if (i > 0)
          MatKernel2AddAB<HA,SUB> (i, X.Width(), &L(i,0), L.Dist(), &X(0,0), X.Dist(), &X(i,0), X.Dist());
        
        TriangularSolveKernelN<4> (X.Width(), &L(i,i), L.Dist(), &X(i,0), X.Dist());
      }

    
    size_t reminder = n % HA;
    if (reminder > 0)
      {
        Switch<HA> (reminder, [L,X,i] (auto r)
                    {
                      if (i > 0)
                        if constexpr (r.value > 0)
                                       MatKernel2AddAB<r.value,SUB> (i, X.Width(),
                                                                     &L(i,0), L.Dist(),
                                                                     &X(0,0), X.Dist(),
                                                                     &X(i,0), X.Dist());
                      
                      TriangularSolveKernelN<r.value> (X.Width(), &L(i,i), L.Dist(), &X(i,0), X.Dist());
                    });
      }
  }

  void TriangularSolveLL2N (BareSliceMatrix<double> T, SliceMatrix<double> X)
  {
    size_t n = X.Height();
    if (n < 128)
      {
        TriangularSolveLL3N (T,X);
        return;
      }
    IntRange r1(0,n/2), r2(n/2,n);
    auto T11 = T.Rows(r1).Cols(r1);
    // auto T12 = T.Rows(r1).Cols(r2).AddSize(r1.Size(), r2.Size());
    auto T21 = T.Rows(r2).Cols(r1).AddSize(r2.Size(), r1.Size());
    auto T22 = T.Rows(r2).Cols(r2);
    auto X1 = X.Rows(r1);
    auto X2 = X.Rows(r2);

    TriangularSolveLL2N (T11, X1);
    X2 -= T21 * X1;
    TriangularSolveLL2N (T22, X2);
  }
  
  void TriangularSolveLLN (BareSliceMatrix<double> L, SliceMatrix<double> X)  
  {
    size_t i = 0;
    constexpr size_t bw = 256;
    for ( ; i+bw <= X.Width(); i += bw)
      TriangularSolveLL2N (L, X.Cols(i,i+bw));
    if (i < X.Width())
      TriangularSolveLL2N (L, X.Cols(i,X.Width()));      
  }

  
}


