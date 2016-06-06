/*********************************************************************/
/* File:   coefficient.cpp                                           */
/* Author: Joachim Schoeberl                                         */
/* Date:   24. Jan. 2002                                             */
/*********************************************************************/

/* 
   Finite Element Coefficient Function
*/

#include <fem.hpp>
#include <../ngstd/evalfunc.hpp>



namespace ngfem
{
  
  CoefficientFunction :: CoefficientFunction ()
  { ; }

  CoefficientFunction :: ~CoefficientFunction ()
  { ; }


  void CoefficientFunction :: GenerateCode(Code &code, FlatArray<int> inputs, int index) const
  {
    code.header += string("// Missing implementation: ") + typeid(*this).name() + "\n";
  }

  void CoefficientFunction :: PrintReport (ostream & ost) const
  {
    // ost << "CoefficientFunction is " << typeid(*this).name() << endl;
    PrintReportRec (ost, 0);
  }
  
  void CoefficientFunction :: PrintReportRec (ostream & ost, int level) const
  {
    for (int i = 0; i < 2*level; i++)
      ost << ' ';
    ost << "coef " << GetName()
        << (IsComplex() ? " complex" : " real")
        << " dim=" << Dimension()
        << endl;

    Array<CoefficientFunction*> input = InputCoefficientFunctions();
    for (int i = 0; i < input.Size(); i++)
      input[i] -> PrintReportRec (ost, level+1);
  }
  
  string CoefficientFunction :: GetName () const
  {
    return typeid(*this).name();
  }    

  
  void CoefficientFunction :: TraverseTree (const function<void(CoefficientFunction&)> & func)
  {
    func(*this);
  }
  
  void CoefficientFunction :: 
  Evaluate (const BaseMappedIntegrationRule & ir, FlatMatrix<double> values) const
  {
    for (int i = 0; i < ir.Size(); i++)
      Evaluate (ir[i], values.Row(i)); 
  }

  void CoefficientFunction ::   
  Evaluate (const SIMD_BaseMappedIntegrationRule & ir, AFlatMatrix<double> values) const
  {
    throw ExceptionNOSIMD (string("CF :: simd-Evaluate not implemented for class ") + typeid(*this).name());
  }

  
  void CoefficientFunction :: 
  Evaluate (const BaseMappedIntegrationRule & ir, FlatMatrix<Complex> values) const
  {
    for (int i = 0; i < ir.Size(); i++)
      Evaluate (ir[i], values.Row(i)); 
  }

  /*
  void CoefficientFunction ::
  EvaluateSoA (const BaseMappedIntegrationRule & ir, AFlatMatrix<double> values) const
  {
    throw Exception(string ("EvaluateSoA called for ") + typeid(*this).name());
  }
    
  void CoefficientFunction ::
  EvaluateSoA (const BaseMappedIntegrationRule & ir, AFlatMatrix<Complex> values) const
  {
    throw Exception(string ("EvaluateSoAComplex called for ") + typeid(*this).name());
  }
  */

  void CoefficientFunction :: 
  NonZeroPattern (const class ProxyUserData & ud, FlatVector<bool> nonzero) const
  {
    nonzero = true;
  }
  

  ///
  ConstantCoefficientFunction ::   
  ConstantCoefficientFunction (double aval) 
    : val(aval) 
  { ; }

  ConstantCoefficientFunction ::
  ~ConstantCoefficientFunction ()
  { ; }

  void ConstantCoefficientFunction :: PrintReport (ostream & ost) const
  {
    ost << "ConstantCF, val = " << val << endl;
  }

  void ConstantCoefficientFunction :: Evaluate (const BaseMappedIntegrationRule & ir,
                                                FlatMatrix<double> values) const
  {
    values = val;
  }

  void ConstantCoefficientFunction :: GenerateCode(Code &code, FlatArray<int> inputs, int index) const
  {
    string type = "double";
    if(code.is_simd) type = "SIMD<double>";
    if(code.deriv==1) type = "AutoDiff<1,"+type+">";
    if(code.deriv==2) type = "AutoDiffDiff<1,"+type+">";
    code.body += Var(index).Declare(type);
    code.body += Var(index).Assign(Var(val), false);
  }
  
  ///
  ConstantCoefficientFunctionC ::   
  ConstantCoefficientFunctionC (Complex aval) 
    : val(aval) 
  { ; }

  ConstantCoefficientFunctionC ::
  ~ConstantCoefficientFunctionC ()
  { ; }

  void ConstantCoefficientFunctionC :: PrintReport (ostream & ost) const
  {
    ost << "ConstantCFC, val = " << val << endl;
  }

  void ConstantCoefficientFunctionC :: GenerateCode(Code &code, FlatArray<int> inputs, int index) const
  {
    code.body += Var(index).Assign(Var(val));
  }

  
  DomainConstantCoefficientFunction :: 
  DomainConstantCoefficientFunction (const Array<double> & aval)
    : val(aval) { ; }
  /*
    .Size())
    {
    for (int i = 0; i < val.Size(); i++)
    val[i] = aval[i];
    }
  */
 
  double DomainConstantCoefficientFunction :: Evaluate (const BaseMappedIntegrationPoint & ip) const
  {
    int elind = ip.GetTransformation().GetElementIndex();
    
    if (elind < 0 || elind >= val.Size())
      {
	ostringstream ost;
	ost << "DomainConstantCoefficientFunction: Element index "
	    << elind << " out of range 0 - " << val.Size()-1 << endl;
	throw Exception (ost.str());
      }
    
    return val[elind]; 
  }

  void DomainConstantCoefficientFunction :: Evaluate (const BaseMappedIntegrationRule & ir, FlatMatrix<double> values) const
  {
    int elind = ir[0].GetTransformation().GetElementIndex();
    
    if (elind < 0 || elind >= val.Size())
      {
	ostringstream ost;
	ost << "DomainConstantCoefficientFunction: Element index "
	    << elind << " out of range 0 - " << val.Size()-1 << endl;
	throw Exception (ost.str());
      }
    
    values = val[elind];
    if (val[elind] < 1e-50 && val[elind] > 0) cout << "very small" << endl;
  }

  void DomainConstantCoefficientFunction :: Evaluate (const BaseMappedIntegrationRule & ir, FlatMatrix<Complex> values) const
  {
    int elind = ir[0].GetTransformation().GetElementIndex();
    
    if (elind < 0 || elind >= val.Size())
      {
	ostringstream ost;
	ost << "DomainConstantCoefficientFunction: Element index "
	    << elind << " out of range 0 - " << val.Size()-1 << endl;
	throw Exception (ost.str());
      }
    
    values = val[elind]; 
  }

  void DomainConstantCoefficientFunction :: GenerateCode(Code &code, FlatArray<int> inputs, int index) const
    {
      code.header += "double tmp_" + ToString(index) + "["+ToString(val.Size())+"] = {";
      for (auto i : Range(val))
      {
        code.header += ToString(val[i]);
        if(i<val.Size()-1)
          code.header += ", ";
      }
      code.header += "};\n";
      code.header += Var(index).Assign("tmp_"+ToString(index) + "[mir.GetTransformation().GetElementIndex()]");
    }


  DomainConstantCoefficientFunction :: 
  ~DomainConstantCoefficientFunction ()
  { ; }



  DomainVariableCoefficientFunction ::
  DomainVariableCoefficientFunction (const EvalFunction & afun)
    : fun(1)
  {
    fun[0] = make_shared<EvalFunction> (afun);
    numarg = 3;
  }

  DomainVariableCoefficientFunction ::
  DomainVariableCoefficientFunction (const EvalFunction & afun,
				     const Array<shared_ptr<CoefficientFunction>> & adepends_on)
    : fun(1), depends_on(adepends_on)
  {
    fun[0] = make_shared<EvalFunction> (afun);
    numarg = 3;
    for (int i = 0; i < depends_on.Size(); i++)
      numarg += depends_on[i]->Dimension();
  }


  DomainVariableCoefficientFunction ::
  DomainVariableCoefficientFunction (const Array<shared_ptr<EvalFunction>> & afun)
    : fun(afun.Size())
  {
    for (int i = 0; i < fun.Size(); i++)
      if (afun[i])
        // fun[i] = new EvalFunction (*afun[i]);
        fun[i] = afun[i];
      else
        fun[i] = nullptr;
    numarg = 3;
  }

  DomainVariableCoefficientFunction ::
  DomainVariableCoefficientFunction (const Array<shared_ptr<EvalFunction>> & afun,
				     const Array<shared_ptr<CoefficientFunction>> & adepends_on)
    : fun(afun.Size()), depends_on(adepends_on)
  {
    for (int i = 0; i < fun.Size(); i++)
      if (afun[i])
        // fun[i] = new EvalFunction (*afun[i]);
        fun[i] = afun[i];
      else
        fun[i] = nullptr;

    numarg = 3;
    for (int i = 0; i < depends_on.Size(); i++)
      numarg += depends_on[i]->Dimension();
  }


  DomainVariableCoefficientFunction ::
  ~DomainVariableCoefficientFunction ()
  {
    ;
    /*
    for (int i = 0; i < fun.Size(); i++)
      delete fun[i];
    */
  }

  double DomainVariableCoefficientFunction ::
  Evaluate (const BaseMappedIntegrationPoint & ip) const
  {
    Vec<1> result;
    Evaluate (ip, result);
    return result(0);
    /*
      int numarg = max2(3, depends_on.Size());
      VectorMem<10> args(numarg);
      args.Range(0,DIM) = static_cast<const DimMappedIntegrationPoint<DIM>&>(ip).GetPoint();
    
      for (int i = 3; i < depends_on.Size(); i++)
      args(i) = depends_on[i] -> Evaluate (ip);

      int elind = ip.GetTransformation().GetElementIndex();
      if (fun.Size() == 1) elind = 0;
      double val = fun[elind]->Eval (&args(0));
      return val;
    */
  }

  bool DomainVariableCoefficientFunction :: IsComplex() const 
  {
    for (int i = 0; i < fun.Size(); i++)
      if (fun[i]->IsResultComplex()) return true;
    return false;
  }
  
  int DomainVariableCoefficientFunction :: Dimension() const
  { 
    return fun[0]->Dimension(); 
  }


  Complex DomainVariableCoefficientFunction ::
  EvaluateComplex (const BaseMappedIntegrationPoint & ip) const
  {
    Vec<1, Complex> result;
    Evaluate (ip, result);
    return result(0);
    /*
      int elind = ip.GetTransformation().GetElementIndex();
      Vec<DIM, Complex> hp;
      for (int i = 0; i < DIM; i++)
      hp(i) = static_cast<const DimMappedIntegrationPoint<DIM>&>(ip).GetPoint()(i);
      return fun[elind]->Eval (&hp(0));
    */
  }
  
  void DomainVariableCoefficientFunction ::
  Evaluate(const BaseMappedIntegrationPoint & ip,
	   FlatVector<> result) const
  {
    int elind = ip.GetTransformation().GetElementIndex();
    if (fun.Size() == 1) elind = 0;
    
    if (! fun[elind] -> IsComplex ())
      {
	VectorMem<10> args(numarg);
	// args.Range(0,DIM) = static_cast<const DimMappedIntegrationPoint<DIM>&>(ip).GetPoint();
        args.Range(0,ip.Dim()) = ip.GetPoint();
	
	for (int i = 0, an = 3; i < depends_on.Size(); i++)
	  {
	    int dim = depends_on[i]->Dimension();
	    depends_on[i] -> Evaluate (ip, args.Range(an,an+dim));
	    an += dim;
	  }
	fun[elind]->Eval (&args(0), &result(0), result.Size());      
      }
    else
      {
	VectorMem<10, Complex> args(numarg);
	// args.Range(0,DIM) = static_cast<const DimMappedIntegrationPoint<DIM>&>(ip).GetPoint();
        args.Range(0,ip.Dim()) = ip.GetPoint();
	
	for (int i = 0, an = 3; i < depends_on.Size(); i++)
	  {
	    int dim = depends_on[i]->Dimension();
	    depends_on[i] -> Evaluate (ip, args.Range(an,an+dim));
	    an += dim;
	  }
	fun[elind]->Eval (&args(0), &result(0), result.Size());      
      }
  }


  void DomainVariableCoefficientFunction ::
  Evaluate(const BaseMappedIntegrationPoint & ip,
           FlatVector<Complex> result) const
  {
    VectorMem<10,Complex> args(numarg);
    args = -47;
    // args.Range(0,DIM) = static_cast<const DimMappedIntegrationPoint<DIM>&>(ip).GetPoint();
    args.Range(0,ip.Dim()) = ip.GetPoint();
    for (int i = 0, an = 3; i < depends_on.Size(); i++)
      {
        int dim = depends_on[i]->Dimension();
        depends_on[i] -> Evaluate (ip, args.Range(an,an+dim));
        an += dim;
      }
    
    int elind = ip.GetTransformation().GetElementIndex();
    if (fun.Size() == 1) elind = 0;
    fun[elind]->Eval (&args(0), &result(0), result.Size());
  }
  
  
void DomainVariableCoefficientFunction ::
Evaluate (const BaseMappedIntegrationRule & ir, 
	  FlatMatrix<double> values) const
{
  if (ir.Size() == 0) return;
  int elind = ir.GetTransformation().GetElementIndex();
  if (fun.Size() == 1) elind = 0;

  if (! fun[elind] -> IsComplex ())
    {
      ArrayMem<double,2000> mem(ir.Size()*numarg);
      FlatMatrix<> args(ir.Size(), numarg, &mem[0]);
      
      int dim = ir[0].Dim();
      switch (dim)
        {
        case 2:
          for (int i = 0; i < ir.Size(); i++)
            args.Row(i).Range(0,2) = ir[i].GetPoint();
          break;
        case 3:
          for (int i = 0; i < ir.Size(); i++)
            args.Row(i).Range(0,3) = ir[i].GetPoint();
          break;
        default:
          for (int i = 0; i < ir.Size(); i++)
            args.Row(i).Range(0,dim) = ir[i].GetPoint();
        }
      

      /*
	args.Row(i).Range(0,DIM) = 
	  static_cast<const DimMappedIntegrationPoint<DIM> & > (ir[i]).GetPoint();
      */
      for (int i = 0, an = 3; i < depends_on.Size(); i++)
	{
	  int dim = depends_on[i]->Dimension();
	  Matrix<> hmat(ir.Size(), dim);
	  depends_on[i] -> Evaluate (ir, hmat);
	  args.Cols(an,an+dim) = hmat;
	  an += dim;
	}
      for (int i = 0; i < ir.Size(); i++)
	fun[elind]->Eval (&args(i,0), &values(i,0), values.Width());
    }
  else
    {
      Matrix<Complex> args(ir.Size(), numarg);
      for (int i = 0; i < ir.Size(); i++)
	args.Row(i).Range(0,ir[i].Dim()) = ir[i].GetPoint();
      
      for (int i = 0, an = 3; i < depends_on.Size(); i++)
	{
	  int dim = depends_on[i]->Dimension();
	  Matrix<Complex> hmat(ir.Size(), dim);
	  depends_on[i] -> Evaluate (ir, hmat);
	  args.Cols(an,an+dim) = hmat;
	  an += dim;
	}
    
      for (int i = 0; i < ir.Size(); i++)
	fun[elind]->Eval (&args(i,0), &values(i,0), values.Width());
    }
}

  
void DomainVariableCoefficientFunction :: PrintReport (ostream & ost) const
{
  *testout << "DomainVariableCoefficientFunction, functios are: " << endl;
  for (int i = 0; i < fun.Size(); i++)
    fun[i] -> Print(ost);
}

void DomainVariableCoefficientFunction :: GenerateCode(Code &code, FlatArray<int> inputs, int index) const
{
  code.body += "// DomainVariableCoefficientFunction: not implemented";
}

  /*
  template class DomainVariableCoefficientFunction<1>;
template class DomainVariableCoefficientFunction<2>;
template class DomainVariableCoefficientFunction<3>;
  */

PolynomialCoefficientFunction::PolynomialCoefficientFunction(const Array < Array< Array<double>* >* > & polycoeffs_in,
							     const Array < Array<double>* > & polybounds_in)
  : polycoeffs(polycoeffs_in), polybounds(polybounds_in)
{}

PolynomialCoefficientFunction::PolynomialCoefficientFunction(const Array < Array<double>* > & polycoeffs_in)
{
  polycoeffs.SetSize(polycoeffs_in.Size());
  polybounds.SetSize(polycoeffs_in.Size());
    
  for(int i=0; i<polycoeffs_in.Size(); i++)
    {
      polycoeffs[i] = new Array< Array<double>* >(1);
      (*polycoeffs[i])[0] = polycoeffs_in[i];
      polybounds[i] = new Array<double>(0);
    } 
}


PolynomialCoefficientFunction::~PolynomialCoefficientFunction()
{
  for(int i=0; i<polycoeffs.Size(); i++)
    {
      delete polybounds[i];
      for(int j=0; j<polycoeffs[i]->Size(); j++)
	{
	  delete (*polycoeffs[i])[j];
	}
      delete polycoeffs[i];
    }
  polycoeffs.DeleteAll();
  polybounds.DeleteAll();
}
    
  
  
double PolynomialCoefficientFunction::Evaluate (const BaseMappedIntegrationPoint & ip) const
{
  return Evaluate(ip,0);
}



double PolynomialCoefficientFunction::EvalPoly(const double t, const Array<double> & coeffs) const
{
  const int last = coeffs.Size()-1;
    
  double retval = coeffs[last];
  for(int i=last-1; i>=0; i--)
    {
      retval *= t;
      retval += coeffs[i];
    }

  return retval;    
}


double PolynomialCoefficientFunction::EvalPolyDeri(const double t, const Array<double> & coeffs) const
{
  const int last = coeffs.Size()-1;

  double retval = last*coeffs[last];
  for(int i=last-1; i>=1; i--)
    {
      retval *= t;
      retval += i*coeffs[i];
    }  

  return retval;    
}


double PolynomialCoefficientFunction::Evaluate (const BaseMappedIntegrationPoint & ip, const double & t) const
{
  const int elind = ip.GetTransformation().GetElementIndex();
    
  if (elind < 0 || elind >= polycoeffs.Size())
    {
      ostringstream ost;
      ost << "PolynomialCoefficientFunction: Element index "
	  << elind << " out of range 0 - " << polycoeffs.Size()-1 << endl;
      throw Exception (ost.str());
    }
 
  int pos;
  for(pos=0; pos < polybounds[elind]->Size() && t > (*polybounds[elind])[pos]; pos++){}
   
  return EvalPoly(t,*((*(polycoeffs[elind]))[pos]));

    
}


 
double PolynomialCoefficientFunction::EvaluateDeri (const BaseMappedIntegrationPoint & ip, const double & t) const
{
  const int elind = ip.GetTransformation().GetElementIndex();
    
  if (elind < 0 || elind >= polycoeffs.Size())
    {
      ostringstream ost;
      ost << "PolynomialCoefficientFunction: Element index "
	  << elind << " out of range 0 - " << polycoeffs.Size()-1 << endl;
      throw Exception (ost.str());
    }

  int pos;
  for(pos=0; pos < polybounds[elind]->Size() && t > (*polybounds[elind])[pos]; pos++){}

  return EvalPolyDeri(t,*((*(polycoeffs[elind]))[pos]));
}


double PolynomialCoefficientFunction::EvaluateConst () const
{
  return (*(*polycoeffs[0])[0])[0];
}



//////////////////

FileCoefficientFunction :: FileCoefficientFunction ()
{
  writeips = false;
}

  
FileCoefficientFunction :: FileCoefficientFunction (const string & filename)
{
  StartWriteIps(filename);
}

FileCoefficientFunction :: FileCoefficientFunction (const string & aipfilename,
						    const string & ainfofilename,
						    const string & avaluesfilename,
						    const bool loadvalues)
{
  ipfilename = aipfilename;
  infofilename = ainfofilename;
  valuesfilename = avaluesfilename;

  if(loadvalues)
    {
      writeips = false;
      LoadValues();
    }
  else
    StartWriteIps();
}
    

  
void FileCoefficientFunction :: EmptyValues(void)
{
  for(int i=0; i<ValuesAtIps.Size(); i++)
    delete ValuesAtIps[i];

  ValuesAtIps.SetSize(0);
}

void FileCoefficientFunction :: Reset(void)
{
  EmptyValues();
}

FileCoefficientFunction :: ~FileCoefficientFunction()
{
  if(writeips)
    StopWriteIps(); 

  EmptyValues();
}


void FileCoefficientFunction :: LoadValues(const string & filename)
{
  cout << "Loading values for coefficient function ..."; cout.flush();

  if(writeips) cerr << "WARNING: CoefficientFunction still writing points to \"" 
		    << ipfilename << "\"" << endl;

  ifstream infile(filename.c_str());
    
  int numels,numips,numentries,eln,ipn;
  double val;

  infile >> numels;
  infile >> numips;
  infile >> numentries;
    
  EmptyValues();
    
  ValuesAtIps.SetSize(numels);
    
  for(int i=0; i<numels; i++)
    {
      ValuesAtIps[i] = new Array<double>(numips);
      *(ValuesAtIps[i]) = 0.;
    }

  for(int i=0; i<numentries; i++)
    {
      infile >> eln;
      infile >> ipn;
      infile >> val;
      (*(ValuesAtIps[eln]))[ipn] = val;
    }

  infile.close();
  cout << "done\n";
}



double FileCoefficientFunction :: Evaluate (const BaseMappedIntegrationPoint & ip) const
{
  const ElementTransformation & eltrans = ip.GetTransformation();
  const int elnum = eltrans.GetElementNr();
  const int ipnum = ip.GetIPNr();

  if(writeips)
    {
      if(elnum > maxelnum) const_cast<int&> (maxelnum) = elnum;
      if(ipnum > maxipnum) const_cast<int&> (maxipnum) = ipnum;
      const_cast<int&> (totalipnum)++;

      Vec<3> point;
      eltrans.CalcPoint(ip.IP(),point);

      const_cast<ofstream&> (outfile) << elnum << " " << ipnum << " " << point << "\n";
    }

  if(elnum < ValuesAtIps.Size())
    {
      return (*(ValuesAtIps[elnum]))[ipnum];
    }

  return 0.;
}

void FileCoefficientFunction :: StartWriteIps(const string & filename)
{
  writeips = true;
  maxelnum = 0;
  maxipnum = 0;
  totalipnum = 0;

  outfile.open(filename.c_str());
  outfile.precision(12);
    
}

void FileCoefficientFunction :: StopWriteIps(const string & infofilename)
{
  writeips = false;

  outfile.close();

    
  cout << "Stopped writing to " << ipfilename << endl;
  cout << "Writing info file to " << infofilename << endl;

  ofstream info(infofilename.c_str());

  info << "numelts " << maxelnum+1 << endl
       << "maxnumips " << maxipnum+1 << endl
       << "totalipnum " << totalipnum << endl;

  info.close();

}




  
class ScaleCoefficientFunction : public CoefficientFunction
{
  double scal;
  shared_ptr<CoefficientFunction> c1;
public:
  ScaleCoefficientFunction (double ascal, 
                            shared_ptr<CoefficientFunction> ac1)
    : scal(ascal), c1(ac1) { ; }
  
  virtual bool IsComplex() const { return c1->IsComplex(); }
  virtual int Dimension() const { return c1->Dimension(); }
  virtual Array<int> Dimensions() const { return c1->Dimensions(); }
  
  virtual void PrintReport (ostream & ost) const
  {
    ost << scal << "*(";
    c1->PrintReport(ost);
    ost << ")";
  }

  virtual void GenerateCode(Code &code, FlatArray<int> inputs, int index) const
  {
    TraverseDimensions( c1->Dimensions(), [&](int ind, int i, int j) {
        code.body += Var(index,i,j).Assign(Var(scal) * Var(inputs[0],i,j));
    });
  }

  virtual void TraverseTree (const function<void(CoefficientFunction&)> & func)
  {
    c1->TraverseTree (func);
    func(*this);
  }

  virtual Array<CoefficientFunction*> InputCoefficientFunctions() const
  { return Array<CoefficientFunction*>({ c1.get() }); }

  
  virtual double Evaluate (const BaseMappedIntegrationPoint & ip) const 
  {
    return scal * c1->Evaluate(ip);
  }
  virtual Complex EvaluateComplex (const BaseMappedIntegrationPoint & ip) const 
  {
    return scal * c1->EvaluateComplex(ip);
  }
  virtual double EvaluateConst () const
  {
    return scal * c1->EvaluateConst();
  }
  virtual void Evaluate(const BaseMappedIntegrationPoint & ip,
                        FlatVector<> result) const
  {
    c1->Evaluate (ip, result);
    result *= scal;
  }
  
  virtual void Evaluate(const BaseMappedIntegrationPoint & ip,
                        FlatVector<Complex> result) const
  {
    c1->Evaluate (ip, result);
    result *= scal;
  }

  virtual void Evaluate (const BaseMappedIntegrationRule & ir,
                         FlatMatrix<double> values) const
  {
    c1->Evaluate (ir, values);
    values *= scal;
  }

  virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & ir,
                         AFlatMatrix<double> values) const
  {
    c1->Evaluate (ir, values);
    values *= scal;
  }

  virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & ir, FlatArray<AFlatMatrix<double>*> input,
                         AFlatMatrix<double> values) const
  {
    auto in0 = *input[0];
    values = scal * in0;
  }
  
  virtual void Evaluate (const BaseMappedIntegrationRule & ir,
                         FlatMatrix<Complex> values) const
  {
    c1->Evaluate (ir, values);
    values *= scal;
  }
  
  virtual void EvaluateDeriv (const BaseMappedIntegrationRule & ir,
                              FlatMatrix<> result, FlatMatrix<> deriv) const
  {
    c1->EvaluateDeriv (ir, result, deriv);
    result *= scal;
    deriv *= scal;
  }
  
  virtual void EvaluateDDeriv (const BaseMappedIntegrationRule & ir,
                               FlatMatrix<> result, FlatMatrix<> deriv,
                               FlatMatrix<> dderiv) const
  {
    c1->EvaluateDDeriv (ir, result, deriv, dderiv);
    result *= scal;
    deriv *= scal;
    dderiv *= scal;
  }



  virtual void Evaluate (const BaseMappedIntegrationRule & mir,
                         FlatArray<FlatMatrix<>*> input,
                         FlatMatrix<> result) const
  {
    FlatMatrix<> v1 = *input[0];
    result = scal * v1;
  }

  virtual void EvaluateDeriv (const BaseMappedIntegrationRule & mir,
                              FlatArray<FlatMatrix<>*> input,
                              FlatArray<FlatMatrix<>*> dinput,
                              FlatMatrix<> result,
                              FlatMatrix<> deriv) const
  {
    FlatMatrix<> v1 = *input[0];
    FlatMatrix<> dv1 = *dinput[0];

    result = scal * v1;
    deriv = scal * dv1;
  }

  virtual void EvaluateDDeriv (const BaseMappedIntegrationRule & mir,
                               FlatArray<FlatMatrix<>*> input,
                               FlatArray<FlatMatrix<>*> dinput,
                               FlatArray<FlatMatrix<>*> ddinput,
                               FlatMatrix<> result,
                               FlatMatrix<> deriv,
                               FlatMatrix<> dderiv) const
  {
    FlatMatrix<> v1 = *input[0];
    FlatMatrix<> dv1 = *dinput[0];
    FlatMatrix<> ddv1 = *ddinput[0];

    result = scal * v1;
    deriv = scal * dv1;
    dderiv = scal * ddv1;
  }

  virtual void NonZeroPattern (const class ProxyUserData & ud, FlatVector<bool> nonzero) const
  {
    c1->NonZeroPattern (ud, nonzero);
  }  
};


class ScaleCoefficientFunctionC : public CoefficientFunction
{
  Complex scal;
  shared_ptr<CoefficientFunction> c1;
public:
  ScaleCoefficientFunctionC (Complex ascal, 
                            shared_ptr<CoefficientFunction> ac1)
    : scal(ascal), c1(ac1) { ; }
  
  virtual bool IsComplex() const { return true; }
  virtual int Dimension() const { return c1->Dimension(); }

  virtual void TraverseTree (const function<void(CoefficientFunction&)> & func)
  {
    c1->TraverseTree (func);
    func(*this);
  }

  virtual Array<CoefficientFunction*> InputCoefficientFunctions() const
  { return Array<CoefficientFunction*>({ c1.get() }); }
  
  virtual void GenerateCode(Code &code, FlatArray<int> inputs, int index) const
  {
    TraverseDimensions( c1->Dimensions(), [&](int ind, int i, int j) {
        code.body += Var(index,i,j).Assign(Var(scal) * Var(inputs[0],i,j));
    });
  }

  virtual double Evaluate (const BaseMappedIntegrationPoint & ip) const 
  {
    throw Exception ("real Evaluate called for complex ScaleCF");
  }
  virtual Complex EvaluateComplex (const BaseMappedIntegrationPoint & ip) const 
  {
    return scal * c1->EvaluateComplex(ip);    
  }
  virtual void Evaluate(const BaseMappedIntegrationPoint & ip,
                        FlatVector<Complex> result) const
  {
    c1->Evaluate (ip, result);
    result *= scal;
  }
  virtual void Evaluate(const BaseMappedIntegrationRule & ir,
                        FlatMatrix<Complex> result) const
  {
    c1->Evaluate (ir, result);
    result *= scal;
  }
  
  virtual void NonZeroPattern (const class ProxyUserData & ud, FlatVector<bool> nonzero) const
  {
    c1->NonZeroPattern (ud, nonzero);
  }  
    
};


class MultScalVecCoefficientFunction : public CoefficientFunction
{
  shared_ptr<CoefficientFunction> c1;  // scalar
  shared_ptr<CoefficientFunction> c2;  // vector
public:
  MultScalVecCoefficientFunction (shared_ptr<CoefficientFunction> ac1,
                                  shared_ptr<CoefficientFunction> ac2)
    : c1(ac1), c2(ac2) { ; }
  
  virtual bool IsComplex() const { return c1->IsComplex() || c2->IsComplex(); }
  virtual int Dimension() const { return c2->Dimension(); }
  virtual Array<int> Dimensions() const { return c2->Dimensions(); }

  virtual void TraverseTree (const function<void(CoefficientFunction&)> & func)
  {
    c1->TraverseTree (func);
    c2->TraverseTree (func);
    func(*this);
  }

  virtual Array<CoefficientFunction*> InputCoefficientFunctions() const
  { return Array<CoefficientFunction*>({ c1.get(), c2.get() }); }

  virtual void GenerateCode(Code &code, FlatArray<int> inputs, int index) const
  {
    TraverseDimensions( c2->Dimensions(), [&](int ind, int i, int j) {
      code.body += Var(index,i,j).Assign( Var(inputs[0]) * Var(inputs[1],i,j) );
    });
  }
  
  virtual double Evaluate (const BaseMappedIntegrationPoint & ip) const
  {
    throw Exception ("double MultScalVecCF::Evaluate called");
  }

  virtual void Evaluate(const BaseMappedIntegrationPoint & ip,
                        FlatVector<> result) const
  {
    Vec<1> v1;
    c1->Evaluate (ip, v1);
    c2->Evaluate (ip, result);
    result *= v1(0);
  }

  virtual void Evaluate(const BaseMappedIntegrationPoint & ip,
                        FlatVector<Complex> result) const
  {
    Vec<1,Complex> v1;
    c1->Evaluate (ip, v1);
    c2->Evaluate (ip, result);
    result *= v1(0);
  }

  virtual void Evaluate(const BaseMappedIntegrationRule & ir,
                        FlatMatrix<> result) const
  {
    STACK_ARRAY(double, hmem1, ir.Size());
    FlatMatrix<> temp1(ir.Size(), 1, hmem1);
    
    c1->Evaluate(ir, temp1);
    c2->Evaluate(ir, result);
    for (int i = 0; i < ir.Size(); i++)
      result.Row(i) *= temp1(i,0);
  }

  virtual void Evaluate(const BaseMappedIntegrationRule & ir,
                        FlatMatrix<Complex> result) const
  {
    STACK_ARRAY(double, hmem1, 2*ir.Size());
    FlatMatrix<Complex> temp1(ir.Size(), 1, reinterpret_cast<Complex*> (&hmem1[0]));
    
    c1->Evaluate(ir, temp1);
    c2->Evaluate(ir, result);
    for (int i = 0; i < ir.Size(); i++)
      result.Row(i) *= temp1(i,0);
  }


  virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & ir, AFlatMatrix<double> values) const
  {
    STACK_ARRAY(SIMD<double>, hmem1, values.Width());
    AFlatMatrix<double> temp1(1, values.Width(), &hmem1[0].Data());
    
    c1->Evaluate (ir, temp1);
    c2->Evaluate (ir, values);

    for (int j = 0; j < values.Height(); j++)
      for (int i = 0; i < values.VWidth(); i++)
        values.Get(j,i) *= temp1.Get(0,i);
  }

  virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & ir, FlatArray<AFlatMatrix<double>*> input,
                         AFlatMatrix<double> values) const
  {
    auto in0 = *input[0];
    auto in1 = *input[1];

    for (int j = 0; j < values.Height(); j++)
      for (int i = 0; i < values.VWidth(); i++)
        values.Get(j,i) = in0.Get(0,i) * in1.Get(j,i);
  }



  
  virtual void EvaluateDeriv (const BaseMappedIntegrationRule & ir,
                              FlatMatrix<> result, FlatMatrix<> deriv) const
  {
    STACK_ARRAY(double, hmem1, ir.Size());
    FlatMatrix<> temp1(ir.Size(), 1, hmem1);
    STACK_ARRAY(double, hmem2, ir.Size());
    FlatMatrix<> deriv1(ir.Size(), 1, hmem2);
    c1->EvaluateDeriv(ir, temp1, deriv1);
    c2->EvaluateDeriv(ir, result, deriv);
    for (int i = 0; i < ir.Size(); i++)
      {
        deriv.Row(i) *= temp1(i,0);
        deriv.Row(i) += deriv1(i,0) * result.Row(i);
        result.Row(i) *= temp1(i,0);
      }
  }


  virtual void EvaluateDDeriv (const BaseMappedIntegrationRule & ir,
                               FlatMatrix<> result, FlatMatrix<> deriv, FlatMatrix<> dderiv) const
  {
    STACK_ARRAY(double, hmem1, ir.Size());
    FlatMatrix<> temp1(ir.Size(), 1, hmem1);
    STACK_ARRAY(double, hmem2, ir.Size());
    FlatMatrix<> deriv1(ir.Size(), 1, hmem2);
    STACK_ARRAY(double, hmem3, ir.Size());
    FlatMatrix<> dderiv1(ir.Size(), 1, hmem3);

    c1->EvaluateDDeriv(ir, temp1, deriv1, dderiv1);
    c2->EvaluateDDeriv(ir, result, deriv, dderiv);
    for (int i = 0; i < ir.Size(); i++)
      {
        dderiv.Row(i) *= temp1(i,0);
        dderiv.Row(i) += 2*deriv1(i,0) * deriv.Row(i);
        dderiv.Row(i) += dderiv1(i,0) * result.Row(i);
        deriv.Row(i) *= temp1(i,0);
        deriv.Row(i) += deriv1(i,0) * result.Row(i);
        result.Row(i) *= temp1(i,0);
      }
  }


  virtual void Evaluate (const BaseMappedIntegrationRule & ir, FlatArray<FlatMatrix<>*> input,
                         FlatMatrix<double> result) const
  {
    FlatMatrix<> temp1 = *input[0];
    FlatMatrix<> temp2 = *input[1];
    for (int i = 0; i < ir.Size(); i++)
      result.Row(i) = temp1(i,0) * temp2.Row(i);
  }

  virtual void EvaluateDeriv (const BaseMappedIntegrationRule & mir,
                              FlatArray<FlatMatrix<>*> input,
                              FlatArray<FlatMatrix<>*> dinput,
                              FlatMatrix<> result,
                              FlatMatrix<> deriv) const
  {
    FlatMatrix<> v1 = *input[0], v2 = *input[1];
    FlatMatrix<> dv1 = *dinput[0], dv2 = *dinput[1];
    
    for (int k = 0; k < mir.Size(); k++)
      {
        result.Row(k) = v1(k,0)*v2.Row(k);
        deriv.Row(k) = v1(k,0)*dv2.Row(k)+dv1(k,0)*v2.Row(k);
      }
  }
  

  
  
  virtual void NonZeroPattern (const class ProxyUserData & ud, FlatVector<bool> nonzero) const
  {
    Vec<1,bool> v1;
    c1->NonZeroPattern (ud, v1);
    if (v1(0))
      c2->NonZeroPattern (ud, nonzero);
    else
      nonzero = false;
  }
};


class MultVecVecCoefficientFunction : public CoefficientFunction
{
  shared_ptr<CoefficientFunction> c1;
  shared_ptr<CoefficientFunction> c2;
  int dim1;
public:
  MultVecVecCoefficientFunction (shared_ptr<CoefficientFunction> ac1,
                                 shared_ptr<CoefficientFunction> ac2)
    : c1(ac1), c2(ac2)
  {
    dim1 = c1->Dimension();
    if (dim1 != c2->Dimension())
      throw Exception("MultVecVec : dimensions don't fit");
  }
  
  virtual bool IsComplex() const { return c1->IsComplex() || c2->IsComplex(); }
  virtual int Dimension() const { return 1; }
  virtual void GenerateCode(Code &code, FlatArray<int> inputs, int index) const
  {
    CodeExpr result;
    TraverseDimensions( c1->Dimensions(), [&](int ind, int i, int j) {
        int i2, j2;
        GetIndex( c2->Dimensions(), ind, i2, j2 );
        result += Var(inputs[0],i,j) * Var(inputs[1],i2,j2);
    });
    code.body += Var(index).Assign(result.S());
  }

  virtual void TraverseTree (const function<void(CoefficientFunction&)> & func)
  {
    c1->TraverseTree (func);
    c2->TraverseTree (func);
    func(*this);
  }

  virtual Array<CoefficientFunction*> InputCoefficientFunctions() const
  { return Array<CoefficientFunction*>({ c1.get(), c2.get() }); }  
  
  virtual double Evaluate (const BaseMappedIntegrationPoint & ip) const
  {
    Vec<1> res;
    Evaluate (ip, res);
    return res(0);
  }

  virtual void Evaluate(const BaseMappedIntegrationPoint & ip,
                        FlatVector<> result) const
  {
    STACK_ARRAY(double, hmem1, dim1);
    FlatVector<> v1(dim1, hmem1);
    STACK_ARRAY(double, hmem2, dim1);
    FlatVector<> v2(dim1, hmem2);

    c1->Evaluate (ip, v1);
    c2->Evaluate (ip, v2);
    result(0) = InnerProduct (v1, v2);
  }

  virtual void Evaluate(const BaseMappedIntegrationPoint & ip,
                        FlatVector<Complex> result) const
  {
    Vector<Complex> v1(dim1), v2(dim1);
    c1->Evaluate (ip, v1);
    c2->Evaluate (ip, v2);
    result(0) = InnerProduct (v1, v2);
  }

  virtual void Evaluate(const BaseMappedIntegrationRule & ir,
                        FlatMatrix<> result) const
  {
    STACK_ARRAY(double, hmem1, ir.Size()*dim1);
    FlatMatrix<> temp1(ir.Size(), dim1, hmem1);
    STACK_ARRAY(double, hmem2, ir.Size()*dim1);
    FlatMatrix<> temp2(ir.Size(), dim1, hmem2);

    c1->Evaluate(ir, temp1);
    c2->Evaluate(ir, temp2);
    for (int i = 0; i < ir.Size(); i++)
      result(i,0) = InnerProduct(temp1.Row(i), temp2.Row(i));
  }

  virtual void Evaluate (const BaseMappedIntegrationRule & ir, FlatArray<FlatMatrix<>*> input,
                         FlatMatrix<double> result) const
  {
    FlatMatrix<> temp1 = *input[0];
    FlatMatrix<> temp2 = *input[1];
    for (int i = 0; i < ir.Size(); i++)
      result(i,0) = InnerProduct(temp1.Row(i), temp2.Row(i));
  }

  virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & ir, FlatArray<AFlatMatrix<double>*> input,
                         AFlatMatrix<double> values) const
  {
    auto in0 = *input[0];
    auto in1 = *input[1];
    
    for (int i = 0; i < values.VWidth(); i++)
      {
        SIMD<double> sum = 0.0;
        for (int j = 0; j < dim1; j++)
          sum += in0.Get(j,i) * in1.Get(j,i);
        values.Get(i) = sum.Data();
      }
//     throw Exception (string("hihi\ncf::Evaluate(simd, input->output) not overloaded for ")+typeid(*this).name());
    // Evaluate (ir, values);
  }

  virtual void EvaluateDeriv(const BaseMappedIntegrationRule & mir,
                             FlatMatrix<> result,
                             FlatMatrix<> deriv) const
  {
    Matrix<> v1(mir.Size(), dim1), v2(mir.Size(),dim1);
    Matrix<> dv1(mir.Size(), dim1), dv2(mir.Size(), dim1);
    c1->EvaluateDeriv (mir, v1, dv1);
    c2->EvaluateDeriv (mir, v2, dv2);
    for (int k = 0; k < mir.Size(); k++)
      {
        result(k,0) = InnerProduct (v1.Row(k), v2.Row(k));
        deriv(k,0) = InnerProduct (v1.Row(k), dv2.Row(k))+InnerProduct(v2.Row(k),dv1.Row(k));
      }
  }

  virtual void EvaluateDDeriv(const BaseMappedIntegrationRule & mir,
                              FlatMatrix<> result,
                              FlatMatrix<> deriv,
                              FlatMatrix<> dderiv) const
  {
    Matrix<> v1(mir.Size(), dim1), v2(mir.Size(), dim1);
    Matrix<> dv1(mir.Size(), dim1), dv2(mir.Size(), dim1);
    Matrix<> ddv1(mir.Size(), dim1), ddv2(mir.Size(), dim1);
    c1->EvaluateDDeriv (mir, v1, dv1, ddv1);
    c2->EvaluateDDeriv (mir, v2, dv2, ddv2);

    for (int k = 0; k < mir.Size(); k++)
      {
        result(k,0) = InnerProduct (v1.Row(k), v2.Row(k));
        deriv(k,0) = InnerProduct (v1.Row(k), dv2.Row(k))+InnerProduct(v2.Row(k),dv1.Row(k));
        dderiv(k,0) = InnerProduct (v1.Row(k), ddv2.Row(k))+
          2*InnerProduct(dv1.Row(k),dv2.Row(k))+InnerProduct(ddv1.Row(k),v2.Row(k));
      }

  }


  virtual void EvaluateDeriv (const BaseMappedIntegrationRule & mir,
                              FlatArray<FlatMatrix<>*> input,
                              FlatArray<FlatMatrix<>*> dinput,
                              FlatMatrix<> result,
                              FlatMatrix<> deriv) const
  {
    FlatMatrix<> v1 = *input[0], v2 = *input[1];
    FlatMatrix<> dv1 = *dinput[0], dv2 = *dinput[1];
    
    for (int k = 0; k < mir.Size(); k++)
      {
        result(k,0) = InnerProduct (v1.Row(k), v2.Row(k));
        deriv(k,0) = InnerProduct (v1.Row(k), dv2.Row(k))+InnerProduct(v2.Row(k),dv1.Row(k));
      }
  }

  virtual void EvaluateDDeriv (const BaseMappedIntegrationRule & mir,
                               FlatArray<FlatMatrix<>*> input,
                               FlatArray<FlatMatrix<>*> dinput,
                               FlatArray<FlatMatrix<>*> ddinput,
                               FlatMatrix<> result,
                               FlatMatrix<> deriv,
                               FlatMatrix<> dderiv) const
  {
    FlatMatrix<> v1 = *input[0], v2 = *input[1];
    FlatMatrix<> dv1 = *dinput[0], dv2 = *dinput[1];
    FlatMatrix<> ddv1 = *ddinput[0], ddv2 = *ddinput[1];
    
    for (int k = 0; k < mir.Size(); k++)
      {
        result(k,0) = InnerProduct (v1.Row(k), v2.Row(k));
        deriv(k,0) = InnerProduct (v1.Row(k), dv2.Row(k))+InnerProduct(v2.Row(k),dv1.Row(k));
        dderiv(k,0) = InnerProduct (v1.Row(k), ddv2.Row(k))+
          2*InnerProduct(dv1.Row(k),dv2.Row(k))+InnerProduct(ddv1.Row(k),v2.Row(k));
      }
  }

  virtual bool ElementwiseConstant () const
  { return c1->ElementwiseConstant() && c2->ElementwiseConstant(); }
  
  virtual void NonZeroPattern (const class ProxyUserData & ud, FlatVector<bool> nonzero) const
  {
    Vector<bool> v1(dim1), v2(dim1);
    c1->NonZeroPattern (ud, v1);
    c2->NonZeroPattern (ud, v2);
    bool nz = false;
    for (int i = 0; i < dim1; i++)
      if (v1(i) && v2(i)) nz = true;
    nonzero = nz;
  }

};

template <int DIM>
class T_MultVecVecCoefficientFunction : public CoefficientFunction
{
  shared_ptr<CoefficientFunction> c1;
  shared_ptr<CoefficientFunction> c2;
public:
  T_MultVecVecCoefficientFunction (shared_ptr<CoefficientFunction> ac1,
                                   shared_ptr<CoefficientFunction> ac2)
    : c1(ac1), c2(ac2)
  {
    if (DIM != c1->Dimension() || DIM != c2->Dimension())
      throw Exception("T_MultVecVec : dimensions don't fit");
  }
  
  virtual bool IsComplex() const { return c1->IsComplex() || c2->IsComplex(); }
  virtual int Dimension() const { return 1; }
  virtual void GenerateCode(Code &code, FlatArray<int> inputs, int index) const
  {
    CodeExpr result;
    TraverseDimensions( c1->Dimensions(), [&](int ind, int i, int j) {
        int i2, j2;
        GetIndex( c2->Dimensions(), ind, i2, j2 );
        result += Var(inputs[0],i,j) * Var(inputs[1],i2,j2);
    });
    code.body += Var(index).Assign(result.S());
  }

  virtual void TraverseTree (const function<void(CoefficientFunction&)> & func)
  {
    c1->TraverseTree (func);
    c2->TraverseTree (func);
    func(*this);
  }

  virtual Array<CoefficientFunction*> InputCoefficientFunctions() const
  { return Array<CoefficientFunction*>({ c1.get(), c2.get() }); }  
  
  virtual double Evaluate (const BaseMappedIntegrationPoint & ip) const
  {
    Vec<1> res;
    Evaluate (ip, res);
    return res(0);
  }

  virtual void Evaluate(const BaseMappedIntegrationPoint & ip,
                        FlatVector<> result) const
  {
    Vec<DIM> v1, v2;
    c1->Evaluate (ip, v1);
    c2->Evaluate (ip, v2);
    result(0) = InnerProduct (v1, v2);
  }

  virtual void Evaluate(const BaseMappedIntegrationPoint & ip,
                        FlatVector<Complex> result) const
  {
    Vec<DIM,Complex> v1, v2;
    c1->Evaluate (ip, v1);
    c2->Evaluate (ip, v2);
    result(0) = InnerProduct (v1, v2);
  }

  virtual void Evaluate(const BaseMappedIntegrationRule & ir,
                        FlatMatrix<> result) const
  {
    STACK_ARRAY(double, hmem1, ir.Size()*DIM);
    FlatMatrixFixWidth<DIM> temp1(ir.Size(), hmem1);
    STACK_ARRAY(double, hmem2, ir.Size()*DIM);
    FlatMatrixFixWidth<DIM> temp2(ir.Size(), hmem2);

    c1->Evaluate(ir, temp1);
    c2->Evaluate(ir, temp2);
    for (int i = 0; i < ir.Size(); i++)
      result(i,0) = InnerProduct(temp1.Row(i), temp2.Row(i));
  }

  virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & ir, AFlatMatrix<double> values) const
  {
    STACK_ARRAY(SIMD<double>, hmem1, DIM*values.Width());
    STACK_ARRAY(SIMD<double>, hmem2, DIM*values.Width());
    AFlatMatrix<double> temp1(DIM, values.Width(), &hmem1[0].Data());
    AFlatMatrix<double> temp2(DIM, values.Width(), &hmem2[0].Data());
    
    c1->Evaluate (ir, temp1);
    c2->Evaluate (ir, temp2);

    for (int i = 0; i < values.VWidth(); i++)
      {
        SIMD<double> sum = 0.0;
        for (int j = 0; j < DIM; j++)
          sum += temp1.Get(j,i) * temp2.Get(j,i);
        values.Get(i) = sum.Data();
      }
  }

  virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & ir, FlatArray<AFlatMatrix<double>*> input,
                         AFlatMatrix<double> values) const
  {
    auto in0 = *input[0];
    auto in1 = *input[1];
    
    for (int i = 0; i < values.VWidth(); i++)
      {
        SIMD<double> sum = 0.0;
        for (int j = 0; j < DIM; j++)
          sum += in0.Get(j,i) * in1.Get(j,i);
        values.Get(i) = sum.Data();
      }
  }

  
  virtual void Evaluate(const BaseMappedIntegrationRule & ir,
                        FlatMatrix<Complex> result) const
  {
    STACK_ARRAY(double, hmem1, 2*ir.Size()*DIM);
    FlatMatrix<Complex> temp1(ir.Size(), DIM, (Complex*)hmem1);
    STACK_ARRAY(double, hmem2, 2*ir.Size()*DIM);
    FlatMatrix<Complex> temp2(ir.Size(), DIM, (Complex*)hmem2);

    c1->Evaluate(ir, temp1);
    c2->Evaluate(ir, temp2);
    for (int i = 0; i < ir.Size(); i++)
      result(i,0) = InnerProduct(temp1.Row(i), temp2.Row(i));
  }



  virtual void Evaluate (const BaseMappedIntegrationRule & ir, FlatArray<FlatMatrix<>*> input,
                         FlatMatrix<double> result) const
  {
    FlatMatrix<> temp1 = *input[0];
    FlatMatrix<> temp2 = *input[1];
    for (int i = 0; i < ir.Size(); i++)
      result(i,0) = InnerProduct(temp1.Row(i), temp2.Row(i));
  }

  

  virtual void EvaluateDeriv(const BaseMappedIntegrationRule & mir,
                             FlatMatrix<> result,
                             FlatMatrix<> deriv) const
  {
    Matrix<> v1(mir.Size(), DIM), v2(mir.Size(),DIM);
    Matrix<> dv1(mir.Size(), DIM), dv2(mir.Size(), DIM);
    c1->EvaluateDeriv (mir, v1, dv1);
    c2->EvaluateDeriv (mir, v2, dv2);
    for (int k = 0; k < mir.Size(); k++)
      {
        result(k,0) = InnerProduct (v1.Row(k), v2.Row(k));
        deriv(k,0) = InnerProduct (v1.Row(k), dv2.Row(k))+InnerProduct(v2.Row(k),dv1.Row(k));
      }
  }

  virtual void EvaluateDDeriv(const BaseMappedIntegrationRule & mir,
                              FlatMatrix<> result,
                              FlatMatrix<> deriv,
                              FlatMatrix<> dderiv) const
  {
    Matrix<> v1(mir.Size(), DIM), v2(mir.Size(), DIM);
    Matrix<> dv1(mir.Size(), DIM), dv2(mir.Size(), DIM);
    Matrix<> ddv1(mir.Size(), DIM), ddv2(mir.Size(), DIM);
    c1->EvaluateDDeriv (mir, v1, dv1, ddv1);
    c2->EvaluateDDeriv (mir, v2, dv2, ddv2);

    for (int k = 0; k < mir.Size(); k++)
      {
        result(k,0) = InnerProduct (v1.Row(k), v2.Row(k));
        deriv(k,0) = InnerProduct (v1.Row(k), dv2.Row(k))+InnerProduct(v2.Row(k),dv1.Row(k));
        dderiv(k,0) = InnerProduct (v1.Row(k), ddv2.Row(k))+
          2*InnerProduct(dv1.Row(k),dv2.Row(k))+InnerProduct(ddv1.Row(k),v2.Row(k));
      }

  }



  virtual void EvaluateDeriv (const BaseMappedIntegrationRule & mir,
                              FlatArray<FlatMatrix<>*> input,
                              FlatArray<FlatMatrix<>*> dinput,
                              FlatMatrix<> result,
                              FlatMatrix<> deriv) const
  {
    FlatMatrix<> v1 = *input[0], v2 = *input[1];
    FlatMatrix<> dv1 = *dinput[0], dv2 = *dinput[1];
    
    for (int k = 0; k < mir.Size(); k++)
      {
        result(k,0) = InnerProduct (v1.Row(k), v2.Row(k));
        deriv(k,0) = InnerProduct (v1.Row(k), dv2.Row(k))+InnerProduct(v2.Row(k),dv1.Row(k));
      }
  }

  
  virtual void EvaluateDDeriv (const BaseMappedIntegrationRule & mir,
                               FlatArray<FlatMatrix<>*> input,
                               FlatArray<FlatMatrix<>*> dinput,
                               FlatArray<FlatMatrix<>*> ddinput,
                               FlatMatrix<> result,
                               FlatMatrix<> deriv,
                               FlatMatrix<> dderiv) const
  {
    FlatMatrix<> v1 = *input[0], v2 = *input[1];
    FlatMatrix<> dv1 = *dinput[0], dv2 = *dinput[1];
    FlatMatrix<> ddv1 = *ddinput[0], ddv2 = *ddinput[1];
    
    for (int k = 0; k < mir.Size(); k++)
      {
        result(k,0) = InnerProduct (v1.Row(k), v2.Row(k));
        deriv(k,0) = InnerProduct (v1.Row(k), dv2.Row(k))+InnerProduct(v2.Row(k),dv1.Row(k));
        dderiv(k,0) = InnerProduct (v1.Row(k), ddv2.Row(k))+
          2*InnerProduct(dv1.Row(k),dv2.Row(k))+InnerProduct(ddv1.Row(k),v2.Row(k));
      }
  }

  virtual bool ElementwiseConstant () const
  { return c1->ElementwiseConstant() && c2->ElementwiseConstant(); }
  
  virtual void NonZeroPattern (const class ProxyUserData & ud, FlatVector<bool> nonzero) const
  {
    Vector<bool> v1(DIM), v2(DIM);
    c1->NonZeroPattern (ud, v1);
    c2->NonZeroPattern (ud, v2);
    bool nz = false;
    for (int i = 0; i < DIM; i++)
      if (v1(i) && v2(i)) nz = true;
    nonzero = nz;
  }

};






template <typename TIN>
class NormCoefficientFunction : public CoefficientFunction
{
  shared_ptr<CoefficientFunction> c1;
  int dim1;
public:
  NormCoefficientFunction (shared_ptr<CoefficientFunction> ac1)
    : c1(ac1)
  {
    dim1 = c1->Dimension();
  }
  
  virtual bool IsComplex() const { return false; }
  virtual int Dimension() const { return 1; }

  virtual void TraverseTree (const function<void(CoefficientFunction&)> & func)
  {
    c1->TraverseTree (func);
    func(*this);
  }

  virtual Array<CoefficientFunction*> InputCoefficientFunctions() const
  { return Array<CoefficientFunction*>({ c1.get() }); }  
  
  virtual double Evaluate (const BaseMappedIntegrationPoint & ip) const
  {
    Vec<1> res;
    Evaluate (ip, res);
    return res(0);
  }

  virtual void Evaluate(const BaseMappedIntegrationPoint & ip,
                        FlatVector<> result) const
  {
    VectorMem<10,TIN> v1(dim1);
    c1->Evaluate (ip, v1);
    result(0) = L2Norm(v1);
  }

  virtual void Evaluate(const BaseMappedIntegrationPoint & ip,
                        FlatVector<Complex> result) const
  {
    Vec<1> res;
    Evaluate (ip, res);
    result(0) = res(0);
  }


  /*  
  virtual void Evaluate(const BaseMappedIntegrationRule & ir,
                        FlatMatrix<> result) const
  {
#ifdef VLA
    double hmem1[ir.Size()*dim1];
    FlatMatrix<> temp1(ir.Size(), dim1, hmem1);
    double hmem2[ir.Size()*dim1];
    FlatMatrix<> temp2(ir.Size(), dim1, hmem2);
#else
    Matrix<> temp1(ir.Size(), dim1);
    Matrix<> temp2(ir.Size(), dim1);
#endif
    c1->Evaluate(ir, temp1);
    c2->Evaluate(ir, temp2);
    for (int i = 0; i < ir.Size(); i++)
      result(i,0) = InnerProduct(temp1.Row(i), temp2.Row(i));
  }

  virtual void Evaluate (const BaseMappedIntegrationRule & ir, FlatArray<FlatMatrix<>*> input,
                         FlatMatrix<double> result) const
  {
    FlatMatrix<> temp1 = *input[0];
    FlatMatrix<> temp2 = *input[1];
    for (int i = 0; i < ir.Size(); i++)
      result(i,0) = InnerProduct(temp1.Row(i), temp2.Row(i));
  }

  

  virtual void EvaluateDeriv(const BaseMappedIntegrationRule & mir,
                             FlatMatrix<> result,
                             FlatMatrix<> deriv) const
  {
    Matrix<> v1(mir.Size(), dim1), v2(mir.Size(),dim1);
    Matrix<> dv1(mir.Size(), dim1), dv2(mir.Size(), dim1);
    c1->EvaluateDeriv (mir, v1, dv1);
    c2->EvaluateDeriv (mir, v2, dv2);
    for (int k = 0; k < mir.Size(); k++)
      {
        result(k,0) = InnerProduct (v1.Row(k), v2.Row(k));
        deriv(k,0) = InnerProduct (v1.Row(k), dv2.Row(k))+InnerProduct(v2.Row(k),dv1.Row(k));
      }
  }

  virtual void EvaluateDDeriv(const BaseMappedIntegrationRule & mir,
                              FlatMatrix<> result,
                              FlatMatrix<> deriv,
                              FlatMatrix<> dderiv) const
  {
    Matrix<> v1(mir.Size(), dim1), v2(mir.Size(), dim1);
    Matrix<> dv1(mir.Size(), dim1), dv2(mir.Size(), dim1);
    Matrix<> ddv1(mir.Size(), dim1), ddv2(mir.Size(), dim1);
    c1->EvaluateDDeriv (mir, v1, dv1, ddv1);
    c2->EvaluateDDeriv (mir, v2, dv2, ddv2);

    for (int k = 0; k < mir.Size(); k++)
      {
        result(k,0) = InnerProduct (v1.Row(k), v2.Row(k));
        deriv(k,0) = InnerProduct (v1.Row(k), dv2.Row(k))+InnerProduct(v2.Row(k),dv1.Row(k));
        dderiv(k,0) = InnerProduct (v1.Row(k), ddv2.Row(k))+
          2*InnerProduct(dv1.Row(k),dv2.Row(k))+InnerProduct(ddv1.Row(k),v2.Row(k));
      }

  }


  virtual void EvaluateDeriv (const BaseMappedIntegrationRule & mir,
                              FlatArray<FlatMatrix<>*> input,
                              FlatArray<FlatMatrix<>*> dinput,
                              FlatMatrix<> result,
                              FlatMatrix<> deriv) const
  {
    FlatMatrix<> v1 = *input[0], v2 = *input[1];
    FlatMatrix<> dv1 = *dinput[0], dv2 = *dinput[1];
    
    for (int k = 0; k < mir.Size(); k++)
      {
        result(k,0) = InnerProduct (v1.Row(k), v2.Row(k));
        deriv(k,0) = InnerProduct (v1.Row(k), dv2.Row(k))+InnerProduct(v2.Row(k),dv1.Row(k));
      }
  }

  virtual void EvaluateDDeriv (const BaseMappedIntegrationRule & mir,
                               FlatArray<FlatMatrix<>*> input,
                               FlatArray<FlatMatrix<>*> dinput,
                               FlatArray<FlatMatrix<>*> ddinput,
                               FlatMatrix<> result,
                               FlatMatrix<> deriv,
                               FlatMatrix<> dderiv) const
  {
    FlatMatrix<> v1 = *input[0], v2 = *input[1];
    FlatMatrix<> dv1 = *dinput[0], dv2 = *dinput[1];
    FlatMatrix<> ddv1 = *ddinput[0], ddv2 = *ddinput[1];
    
    for (int k = 0; k < mir.Size(); k++)
      {
        result(k,0) = InnerProduct (v1.Row(k), v2.Row(k));
        deriv(k,0) = InnerProduct (v1.Row(k), dv2.Row(k))+InnerProduct(v2.Row(k),dv1.Row(k));
        dderiv(k,0) = InnerProduct (v1.Row(k), ddv2.Row(k))+
          2*InnerProduct(dv1.Row(k),dv2.Row(k))+InnerProduct(ddv1.Row(k),v2.Row(k));
      }
  }
  */

  
  virtual bool ElementwiseConstant () const
  { return c1->ElementwiseConstant(); }
  
  virtual void NonZeroPattern (const class ProxyUserData & ud, FlatVector<bool> nonzero) const
  {
    Vector<bool> v1(dim1);
    c1->NonZeroPattern (ud, v1);
    bool nz = false;
    for (int i = 0; i < dim1; i++)
      if (v1(i)) nz = true;
    nonzero = nz;
  }

};



  

class MultMatMatCoefficientFunction : public CoefficientFunction
{
  shared_ptr<CoefficientFunction> c1;
  shared_ptr<CoefficientFunction> c2;
  Array<int> dims;
  int inner_dim;
public:
  MultMatMatCoefficientFunction (shared_ptr<CoefficientFunction> ac1,
                                 shared_ptr<CoefficientFunction> ac2)
    : c1(ac1), c2(ac2)
  {
    auto dims_c1 = c1 -> Dimensions();
    auto dims_c2 = c2 -> Dimensions();
    if (dims_c1.Size() != 2 || dims_c2.Size() != 2)
      throw Exception("Mult of non-matrices called");
    if (dims_c1[1] != dims_c2[0])
      throw Exception(string("Matrix dimensions don't fit: m1 is ") +
                      ToString(dims_c1[0]) + " x " + ToString(dims_c1[1]) +
                      ", m2 is " + ToString(dims_c2[0]) + " x " + ToString(dims_c2[1]) );
    dims = { dims_c1[0], dims_c2[1] };
    inner_dim = dims_c1[1];
  }
  
  virtual bool IsComplex() const { return c1->IsComplex() || c2->IsComplex(); }
  virtual int Dimension() const { return dims[0]*dims[1]; }
  virtual Array<int> Dimensions() const { return Array<int> (dims); } 

  virtual void TraverseTree (const function<void(CoefficientFunction&)> & func)
  {
    c1->TraverseTree (func);
    c2->TraverseTree (func);
    func(*this);
  }

  virtual void GenerateCode(Code &code, FlatArray<int> inputs, int index) const {
      for (int i : Range(dims[0]))
        for (int j : Range(dims[1])) {
          CodeExpr s;
          for (int k : Range(inner_dim))
            s += Var(inputs[0], i, k) * Var(inputs[1], k, j);
          code.body += Var(index, i, j).Assign(s);
        }
  }

  virtual Array<CoefficientFunction*> InputCoefficientFunctions() const
  { return Array<CoefficientFunction*>({ c1.get(), c2.get() }); }  


  virtual void NonZeroPattern (const class ProxyUserData & ud, FlatVector<bool> nonzero) const
  {
    Vector<bool> v1(dims[0]*inner_dim), v2(dims[1]*inner_dim);
    c1->NonZeroPattern (ud, v1);
    c2->NonZeroPattern (ud, v2);
    nonzero = false;
    FlatMatrix<bool> m1(dims[0], inner_dim, &v1(0));
    FlatMatrix<bool> m2(inner_dim, dims[1], &v2(0));
    FlatMatrix<bool> m3(dims[0], dims[1], &nonzero(0));
    for (int i = 0; i < dims[0]; i++)
      for (int j = 0; j < dims[1]; j++)
        for (int k = 0; k < inner_dim; k++)
          nonzero(i,j) |= m1(i,k) && m2(k,j);
  }

  
  virtual double Evaluate (const BaseMappedIntegrationPoint & ip) const 
  {
    throw Exception ("MultMatMatCF:: scalar evaluate for matrix called");
  }

  virtual void Evaluate (const BaseMappedIntegrationPoint & ip,
                         FlatVector<> result) const
  {
    Vector<> va(dims[0]*inner_dim);
    Vector<> vb(dims[1]*inner_dim);
    FlatMatrix<> a(dims[0], inner_dim, &va[0]);
    FlatMatrix<> b(inner_dim, dims[1], &vb[0]);
    
    c1->Evaluate (ip, va);
    c2->Evaluate (ip, vb);

    FlatMatrix<> c(dims[0], dims[1], &result(0));
    c = a*b;
  }  

  virtual void Evaluate (const BaseMappedIntegrationPoint & ip,
                         FlatVector<Complex> result) const
  {
    cout << "MultMatMat: complex not implemented" << endl;
  }  

  virtual void Evaluate (const BaseMappedIntegrationRule & mir,
                         FlatMatrix<> result) const
  {
    // Matrix<> va(mir.Size(), dims[0]*inner_dim);
    // Matrix<> vb(mir.Size(), dims[1]*inner_dim);

    STACK_ARRAY(double, mema, mir.Size()*dims[0]*inner_dim);
    STACK_ARRAY(double, memb, mir.Size()*dims[1]*inner_dim);
    FlatMatrix<> va(mir.Size(), dims[0]*inner_dim, mema);
    FlatMatrix<> vb(mir.Size(), dims[1]*inner_dim, memb);

    c1->Evaluate (mir, va);
    c2->Evaluate (mir, vb);

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> a(dims[0], inner_dim, &va(i,0));
        FlatMatrix<> b(inner_dim, dims[1], &vb(i,0));
        FlatMatrix<> c(dims[0], dims[1], &result(i,0));
        c = a*b;
      }
  }

  virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & mir, AFlatMatrix<double> values) const
  {
    STACK_ARRAY(SIMD<double>, hmem1, mir.IR().Size()*dims[0]*inner_dim);
    STACK_ARRAY(SIMD<double>, hmem2, mir.IR().Size()*dims[1]*inner_dim);
    AFlatMatrix<double> va(dims[0]*inner_dim, mir.IR().GetNIP(), &hmem1[0].Data());
    AFlatMatrix<double> vb(dims[1]*inner_dim, mir.IR().GetNIP(), &hmem2[0].Data());
    c1->Evaluate (mir, va);
    c2->Evaluate (mir, vb);
    values = 0.0;

    int d1 = dims[1];
    for (int j = 0; j < dims[0]; j++)
      for (int k = 0; k < dims[1]; k++)
        for (int l = 0; l < inner_dim; l++)
          {
            auto row_a = va.Row(j*inner_dim+l);
            auto row_b = vb.Row(l*d1+k);
            auto row_c = values.Row(j*d1+k);
            for (int i = 0; i < mir.Size(); i++)
              row_c.Get(i) += row_a.Get(i) * row_b.Get(i);
            // row_c = pw_mult (row_a, row_b);
          }
  }

  virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & mir, FlatArray<AFlatMatrix<double>*> input,
                         AFlatMatrix<double> values) const
  {
    auto va = *input[0];
    auto vb = *input[1];
    values = 0.0;
    
    for (int j = 0; j < dims[0]; j++)
      for (int k = 0; k < dims[1]; k++)
        for (int l = 0; l < inner_dim; l++)
          {
            auto row_a = va.Row(j*inner_dim+l);
            auto row_b = vb.Row(l*dims[1]+k);
            auto row_c = values.Row(j*dims[1]+k);
            for (int i = 0; i < mir.Size(); i++)
              row_c.Get(i) += row_a.Get(i) * row_b.Get(i);
          }    
  }
  
  
  virtual void EvaluateDeriv(const BaseMappedIntegrationRule & mir,
                             FlatMatrix<> result,
                             FlatMatrix<> deriv) const
  {
    Matrix<> va(mir.Size(), dims[0]*inner_dim);
    Matrix<> vb(mir.Size(), dims[1]*inner_dim);
    Matrix<> vda(mir.Size(), dims[0]*inner_dim);
    Matrix<> vdb(mir.Size(), dims[1]*inner_dim);
    c1->EvaluateDeriv (mir, va, vda);
    c2->EvaluateDeriv (mir, vb, vdb);

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> a(dims[0], inner_dim, &va(i,0));
        FlatMatrix<> b(inner_dim, dims[1], &vb(i,0));
        FlatMatrix<> da(dims[0], inner_dim, &vda(i,0));
        FlatMatrix<> db(inner_dim, dims[1], &vdb(i,0));
        FlatMatrix<> c(dims[0], dims[1], &result(i,0));
        FlatMatrix<> dc(dims[0], dims[1], &deriv(i,0));
        c = a*b;
        dc = a*db+da*b;
      }
  }
  
  virtual void EvaluateDDeriv(const BaseMappedIntegrationRule & mir,
                              FlatMatrix<> result,
                              FlatMatrix<> deriv,
                              FlatMatrix<> dderiv) const
  {
    Matrix<> va(mir.Size(), dims[0]*inner_dim);
    Matrix<> vb(mir.Size(), dims[1]*inner_dim);
    Matrix<> vda(mir.Size(), dims[0]*inner_dim);
    Matrix<> vdb(mir.Size(), dims[1]*inner_dim);
    Matrix<> vdda(mir.Size(), dims[0]*inner_dim);
    Matrix<> vddb(mir.Size(), dims[1]*inner_dim);
    c1->EvaluateDDeriv (mir, va, vda, vdda);
    c2->EvaluateDDeriv (mir, vb, vdb, vddb);

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> a(dims[0], inner_dim, &va(i,0));
        FlatMatrix<> b(inner_dim, dims[1], &vb(i,0));
        FlatMatrix<> da(dims[0], inner_dim, &vda(i,0));
        FlatMatrix<> db(inner_dim, dims[1], &vdb(i,0));
        FlatMatrix<> dda(dims[0], inner_dim, &vdda(i,0));
        FlatMatrix<> ddb(inner_dim, dims[1], &vddb(i,0));
        FlatMatrix<> c(dims[0], dims[1], &result(i,0));
        FlatMatrix<> dc(dims[0], dims[1], &deriv(i,0));
        FlatMatrix<> ddc(dims[0], dims[1], &dderiv(i,0));
        c = a*b;
        dc = a*db+da*b;
        ddc = a*ddb+2*da*db+dda*b;
      }
  }


  



  virtual void Evaluate(const BaseMappedIntegrationRule & mir,
                        FlatArray<FlatMatrix<>*> input,
                        FlatMatrix<> result) const
  {
    FlatMatrix<> va = *input[0], vb = *input[1];

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> a(dims[0], inner_dim, &va(i,0));
        FlatMatrix<> b(inner_dim, dims[1], &vb(i,0));
        FlatMatrix<> c(dims[0], dims[1], &result(i,0));
        c = a*b;
      }
  }



  virtual void EvaluateDeriv(const BaseMappedIntegrationRule & mir,
                             FlatArray<FlatMatrix<>*> input,
                             FlatArray<FlatMatrix<>*> dinput,
                             FlatMatrix<> result,
                             FlatMatrix<> deriv) const
  {
    FlatMatrix<> va = *input[0], vb = *input[1];
    FlatMatrix<> vda = *dinput[0], vdb = *dinput[1];

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> a(dims[0], inner_dim, &va(i,0));
        FlatMatrix<> b(inner_dim, dims[1], &vb(i,0));
        FlatMatrix<> da(dims[0], inner_dim, &vda(i,0));
        FlatMatrix<> db(inner_dim, dims[1], &vdb(i,0));
        FlatMatrix<> c(dims[0], dims[1], &result(i,0));
        FlatMatrix<> dc(dims[0], dims[1], &deriv(i,0));
        c = a*b;
        dc = a*db+da*b;
      }
  }


  
  virtual void EvaluateDDeriv(const BaseMappedIntegrationRule & mir,
                              FlatArray<FlatMatrix<>*> input,
                              FlatArray<FlatMatrix<>*> dinput,
                              FlatArray<FlatMatrix<>*> ddinput,
                              FlatMatrix<> result,
                              FlatMatrix<> deriv,
                              FlatMatrix<> dderiv) const
  {
    FlatMatrix<> va = *input[0], vb = *input[1];
    FlatMatrix<> vda = *dinput[0], vdb = *dinput[1];
    FlatMatrix<> vdda = *ddinput[0], vddb = *ddinput[1];

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> a(dims[0], inner_dim, &va(i,0));
        FlatMatrix<> b(inner_dim, dims[1], &vb(i,0));
        FlatMatrix<> da(dims[0], inner_dim, &vda(i,0));
        FlatMatrix<> db(inner_dim, dims[1], &vdb(i,0));
        FlatMatrix<> dda(dims[0], inner_dim, &vdda(i,0));
        FlatMatrix<> ddb(inner_dim, dims[1], &vddb(i,0));
        FlatMatrix<> c(dims[0], dims[1], &result(i,0));
        FlatMatrix<> dc(dims[0], dims[1], &deriv(i,0));
        FlatMatrix<> ddc(dims[0], dims[1], &dderiv(i,0));
        c = a*b;
        dc = a*db+da*b;
        ddc = a*ddb+2*da*db+dda*b;
      }
  }


  
};







class MultMatVecCoefficientFunction : public CoefficientFunction
{
  shared_ptr<CoefficientFunction> c1;
  shared_ptr<CoefficientFunction> c2;
  Array<int> dims;
  int inner_dim;
public:
  MultMatVecCoefficientFunction (shared_ptr<CoefficientFunction> ac1,
                                 shared_ptr<CoefficientFunction> ac2)
    : c1(ac1), c2(ac2)
  {
    auto dims_c1 = c1 -> Dimensions();
    auto dims_c2 = c2 -> Dimensions();
    if (dims_c1.Size() != 2 || dims_c2.Size() != 1)
      throw Exception("Not a mat-vec multiplication");
    if (dims_c1[1] != dims_c2[0])
      throw Exception(string ("Matrix dimensions don't fit: mat is ") +
                      ToString(dims_c1[0]) + " x " + ToString(dims_c1[1]) + ", vec is " + ToString(dims_c2[0]));
    dims = Array<int> ({ dims_c1[0] });
    inner_dim = dims_c1[1];
  }
  
  virtual bool IsComplex() const { return c1->IsComplex() || c2->IsComplex(); }
  virtual int Dimension() const { return dims[0]; }
  virtual Array<int> Dimensions() const { return Array<int> (dims); } 

  virtual void TraverseTree (const function<void(CoefficientFunction&)> & func)
  {
    c1->TraverseTree (func);
    c2->TraverseTree (func);
    func(*this);
  }

  virtual Array<CoefficientFunction*> InputCoefficientFunctions() const
  { return Array<CoefficientFunction*>({ c1.get(), c2.get() }); }

  virtual void GenerateCode(Code &code, FlatArray<int> inputs, int index) const {
      auto dims = c1->Dimensions();
      for (int i : Range(dims[0])) {
        CodeExpr s;
        for (int j : Range(dims[1]))
            s += Var(inputs[0], i, j) * Var(inputs[1], j);
	code.body += Var(index, i).Assign(s);
      }
  }

  virtual void NonZeroPattern (const class ProxyUserData & ud, FlatVector<bool> nonzero) const
  {
    Vector<bool> v1(dims[0]*inner_dim), v2(inner_dim);
    c1->NonZeroPattern (ud, v1);
    c2->NonZeroPattern (ud, v2);
    nonzero = false;
    FlatMatrix<bool> m1(dims[0], inner_dim, &v1(0));
    for (int i = 0; i < dims[0]; i++)
      for (int j = 0; j < inner_dim; j++)
        nonzero(i) |= m1(i,j) && v2(j);
  }

  virtual double Evaluate (const BaseMappedIntegrationPoint & ip) const 
  {
    throw Exception ("MultMatVecCF:: scalar evaluate for matrix called");
  }

  virtual void Evaluate (const BaseMappedIntegrationPoint & ip,
                         FlatVector<> result) const
  {
    VectorMem<20> va(dims[0]*inner_dim);
    VectorMem<20> vb(inner_dim);
    FlatMatrix<> a(dims[0], inner_dim, &va[0]);

    c1->Evaluate (ip, va);
    c2->Evaluate (ip, vb);

    result = a * vb;
  }  

  virtual void Evaluate (const BaseMappedIntegrationPoint & ip,
                         FlatVector<Complex> result) const
  {
    cout << "MultMatMat: complex not implemented" << endl;
  }  

  virtual void Evaluate (const BaseMappedIntegrationRule & mir,
                         FlatMatrix<> result) const
  {
    Matrix<> va(mir.Size(), dims[0]*inner_dim);
    Matrix<> vb(mir.Size(), inner_dim);
    c1->Evaluate (mir, va);
    c2->Evaluate (mir, vb);

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> a(dims[0], inner_dim, &va(i,0));
        result.Row(i) = a * vb.Row(i);
      }
  }

  virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & ir, AFlatMatrix<double> values) const
  {
    STACK_ARRAY(SIMD<double>, hmem1, (ir.IR().GetNIP()+8)*dims[0]*inner_dim);
    STACK_ARRAY(SIMD<double>, hmem2, (ir.IR().GetNIP()+8)*inner_dim);
    AFlatMatrix<double> temp1(dims[0]*inner_dim, ir.IR().GetNIP(), &hmem1[0].Data());
    AFlatMatrix<double> temp2(inner_dim, ir.IR().GetNIP(), &hmem2[0].Data());
    c1->Evaluate (ir, temp1);
    c2->Evaluate (ir, temp2);
    values = 0.0;
    for (int i = 0; i < dims[0]; i++)
      for (int j = 0; j < inner_dim; j++)
        for (int k = 0; k < ir.Size(); k++)
          values.Get(i,k) += temp1.Get(i*inner_dim+j, k) * temp2.Get(j,k);
  }
  
  virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & mir, FlatArray<AFlatMatrix<double>*> input,
                         AFlatMatrix<double> values) const
  {
    auto in0 = *input[0];
    auto in1 = *input[1];
    values = 0.0;
    auto _inner_dim = inner_dim;
    if (_inner_dim <= 0) return;
    int ii = 0;
    for (auto i : Range(dims[0]))
      for (auto j : Range(_inner_dim))
        // values.Row(i) += pw_mult (in0.Row(i*_inner_dim+j), in1.Row(j));
        values.Row(i) += pw_mult (in0.Row(ii++), in1.Row(j));
  }

  
  virtual void EvaluateDeriv(const BaseMappedIntegrationRule & mir,
                             FlatMatrix<> result,
                             FlatMatrix<> deriv) const
  {
    Matrix<> va(mir.Size(), dims[0]*inner_dim);
    Matrix<> vb(mir.Size(), inner_dim);
    Matrix<> vda(mir.Size(), dims[0]*inner_dim);
    Matrix<> vdb(mir.Size(), inner_dim);
    c1->EvaluateDeriv (mir, va, vda);
    c2->EvaluateDeriv (mir, vb, vdb);

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> a(dims[0], inner_dim, &va(i,0));
        FlatMatrix<> da(dims[0], inner_dim, &vda(i,0));

        result.Row(i) = a*vb.Row(i);
        deriv.Row(i) = a*vdb.Row(i) + da*vb.Row(i);
      }
  }
  
  virtual void EvaluateDDeriv(const BaseMappedIntegrationRule & mir,
                              FlatMatrix<> result,
                              FlatMatrix<> deriv,
                              FlatMatrix<> dderiv) const
  {
    Matrix<> va(mir.Size(), dims[0]*inner_dim);
    Matrix<> vb(mir.Size(), inner_dim);
    Matrix<> vda(mir.Size(), dims[0]*inner_dim);
    Matrix<> vdb(mir.Size(), inner_dim);
    Matrix<> vdda(mir.Size(), dims[0]*inner_dim);
    Matrix<> vddb(mir.Size(), inner_dim);
    c1->EvaluateDDeriv (mir, va, vda, vdda);
    c2->EvaluateDDeriv (mir, vb, vdb, vddb);

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> a(dims[0], inner_dim, &va(i,0));
        FlatMatrix<> da(dims[0], inner_dim, &vda(i,0));
        FlatMatrix<> dda(dims[0], inner_dim, &vdda(i,0));

        result.Row(i) = a*vb.Row(i);
        deriv.Row(i) = a*vdb.Row(i) + da*vb.Row(i);
        dderiv.Row(i) = a*vddb.Row(i) + 2*da*vdb.Row(i) + dda*vb.Row(i);
      }
  }


  



  virtual void Evaluate(const BaseMappedIntegrationRule & mir,
                        FlatArray<FlatMatrix<>*> input,
                        FlatMatrix<> result) const
  {
    FlatMatrix<> va = *input[0], vb = *input[1];
    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> a(dims[0], inner_dim, &va(i,0));
        result.Row(i) = a * vb.Row(i);
      }
  }

  virtual void EvaluateDeriv(const BaseMappedIntegrationRule & mir,
                             FlatArray<FlatMatrix<>*> input,
                             FlatArray<FlatMatrix<>*> dinput,
                             FlatMatrix<> result,
                             FlatMatrix<> deriv) const
  {
    FlatMatrix<> va = *input[0], vb = *input[1];
    FlatMatrix<> vda = *dinput[0], vdb = *dinput[1];

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> a(dims[0], inner_dim, &va(i,0));
        FlatMatrix<> da(dims[0], inner_dim, &vda(i,0));

        // FlatMatrix<> c(dims[0], dims[1], &result(i,0));
        // FlatMatrix<> dc(dims[0], dims[1], &deriv(i,0));
        // c = a*b;
        // dc = a*db+da*b;
        result.Row(i) = a * vb.Row(i);
        deriv.Row(i) = da * vb.Row(i) + a*vdb.Row(i);        
      }
  }


  
  virtual void EvaluateDDeriv(const BaseMappedIntegrationRule & mir,
                              FlatArray<FlatMatrix<>*> input,
                              FlatArray<FlatMatrix<>*> dinput,
                              FlatArray<FlatMatrix<>*> ddinput,
                              FlatMatrix<> result,
                              FlatMatrix<> deriv,
                              FlatMatrix<> dderiv) const
  {
    FlatMatrix<> va = *input[0], vb = *input[1];
    FlatMatrix<> vda = *dinput[0], vdb = *dinput[1];
    FlatMatrix<> vdda = *ddinput[0], vddb = *ddinput[1];

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> a(dims[0], inner_dim, &va(i,0));
        FlatMatrix<> da(dims[0], inner_dim, &vda(i,0));
        FlatMatrix<> dda(dims[0], inner_dim, &vdda(i,0));

        result.Row(i) = a*vb.Row(i);
        deriv.Row(i) = a*vdb.Row(i) + da*vb.Row(i);
        dderiv.Row(i) = a*vddb.Row(i) + 2*da*vdb.Row(i) + dda*vb.Row(i);
      }
  }


  
};



  
class TransposeCoefficientFunction : public CoefficientFunction
{
  shared_ptr<CoefficientFunction> c1;
  Array<int> dims;
public:
  TransposeCoefficientFunction (shared_ptr<CoefficientFunction> ac1)
    : c1(ac1)
  {
    auto dims_c1 = c1 -> Dimensions();
    if (dims_c1.Size() != 2)
      throw Exception("Transpose of non-matrix called");
    dims = { dims_c1[1], dims_c1[0] };
  }
  
  virtual bool IsComplex() const { return c1->IsComplex(); }
  virtual int Dimension() const { return c1->Dimension(); }
  virtual Array<int> Dimensions() const { return Array<int> (dims); } 

  virtual void TraverseTree (const function<void(CoefficientFunction&)> & func)
  {
    c1->TraverseTree (func);
    func(*this);
  }

  virtual void GenerateCode(Code &code, FlatArray<int> inputs, int index) const {
      for (int i : Range(dims[0]))
        for (int j : Range(dims[1]))
          code.body += Var(index,i,j).Assign( Var(inputs[0],j,i) );
  }

  virtual Array<CoefficientFunction*> InputCoefficientFunctions() const
  { return Array<CoefficientFunction*>({ c1.get() } ); }  

  virtual void NonZeroPattern (const class ProxyUserData & ud, FlatVector<bool> nonzero) const
  {
    Vector<bool> v1(dims[0]*dims[1]);
    c1->NonZeroPattern (ud, v1);
    FlatMatrix<bool> m1(dims[1], dims[0], &v1(0));
    FlatMatrix<bool> m2(dims[0], dims[1], &nonzero(0));
    m2 = Trans(m1);
  }

  
  virtual double Evaluate (const BaseMappedIntegrationPoint & ip) const 
  {
    throw Exception ("TransposeCF:: scalar evaluate for matrix called");
  }

  virtual void Evaluate (const BaseMappedIntegrationPoint & ip,
                         FlatVector<> result) const
  {
    VectorMem<20> input(result.Size());
    c1->Evaluate (ip, input);    
    FlatMatrix<> reshape1(dims[1], dims[0], &input(0));  // source matrix format
    FlatMatrix<> reshape2(dims[0], dims[1], &result(0));  // range matrix format
    reshape2 = Trans(reshape1);
    
    /*
    c1->Evaluate (ip, result);
    static Timer t("Transpose - evaluate");
    RegionTimer reg(t);
    FlatMatrix<> reshape(dims[1], dims[0], &result(0));  // source matrix format
    Matrix<> tmp = Trans(reshape);
    FlatMatrix<> reshape2(dims[0], dims[1], &result(0));  // range matrix format
    reshape2 = tmp;
    */
  }  

  virtual void Evaluate (const BaseMappedIntegrationPoint & ip,
                         FlatVector<Complex> result) const
  {
    cout << "Transpose: complex not implemented" << endl;
  }  

  virtual void Evaluate (const BaseMappedIntegrationRule & mir,
                         FlatMatrix<> result) const
  {
    c1->Evaluate (mir, result);
    STACK_ARRAY(double, hmem, dims[0]*dims[1]);
    FlatMatrix<> tmp (dims[0], dims[1], hmem);

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> reshape(dims[1], dims[0], &result(i,0));  // source matrix format
        tmp = Trans(reshape);
        FlatMatrix<> reshape2(dims[0], dims[1], &result(i,0));  // range matrix format
        reshape2 = tmp;
      }
  }  

  virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & mir,
                         AFlatMatrix<double> result) const
  {
    c1->Evaluate (mir, result);
    STACK_ARRAY(SIMD<double>, hmem, dims[0]*dims[1]);
    AFlatMatrix<double> tmp (dims[0], dims[1]*SIMD<double>::Size(), &hmem[0].Data());

    for (int i = 0; i < mir.Size(); i++)
      {
        for (int j = 0; j < dims[0]; j++)
          for (int k = 0; k < dims[1]; k++)
            tmp.Get(j,k) = result.Get(k*dims[0]+j, i);
        for (int j = 0; j < dims[0]; j++)
          for (int k = 0; k < dims[1]; k++)
            result.Get(j*dims[1]+k, i) = tmp.Get(j,k);
      }
  }  
  virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & mir, FlatArray<AFlatMatrix<double>*> input,
                         AFlatMatrix<double> values) const
  {
    auto in0 = *input[0];
    for (int i = 0; i < mir.Size(); i++)
      {
        for (int j = 0; j < dims[0]; j++)
          for (int k = 0; k < dims[1]; k++)
            values.Get(j*dims[1]+k, i) = in0.Get(k*dims[0]+j, i);
      }
  }

  
  virtual void EvaluateDeriv(const BaseMappedIntegrationRule & mir,
                             FlatMatrix<> result,
                             FlatMatrix<> deriv) const
  {
    c1->EvaluateDeriv (mir, result, deriv);
    Matrix<> tmp (dims[0], dims[1]);

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> reshape(dims[1], dims[0], &result(i,0));  // source matrix format
        tmp = Trans(reshape);
        FlatMatrix<> reshape2(dims[0], dims[1], &result(i,0));  // range matrix format
        reshape2 = tmp;
      }
    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> reshape(dims[1], dims[0], &deriv(i,0));  // source matrix format
        tmp = Trans(reshape);
        FlatMatrix<> reshape2(dims[0], dims[1], &deriv(i,0));  // range matrix format
        reshape2 = tmp;
      }
  }
  
  virtual void EvaluateDDeriv(const BaseMappedIntegrationRule & mir,
                              FlatMatrix<> result,
                              FlatMatrix<> deriv,
                              FlatMatrix<> dderiv) const
  {
    c1->EvaluateDDeriv (mir, result, deriv, dderiv);
    Matrix<> tmp (dims[0], dims[1]);

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> reshape(dims[1], dims[0], &result(i,0));  // source matrix format
        tmp = Trans(reshape);
        FlatMatrix<> reshape2(dims[0], dims[1], &result(i,0));  // range matrix format
        reshape2 = tmp;
      }
    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> reshape(dims[1], dims[0], &deriv(i,0));  // source matrix format
        tmp = Trans(reshape);
        FlatMatrix<> reshape2(dims[0], dims[1], &deriv(i,0));  // range matrix format
        reshape2 = tmp;
      }
    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> reshape(dims[1], dims[0], &dderiv(i,0));  // source matrix format
        tmp = Trans(reshape);
        FlatMatrix<> reshape2(dims[0], dims[1], &dderiv(i,0));  // range matrix format
        reshape2 = tmp;
      }
  }


  virtual void Evaluate (const BaseMappedIntegrationRule & mir,
                         FlatArray<FlatMatrix<>*> input,
                         FlatMatrix<> result) const
  {
    FlatMatrix<> v1 = *input[0];
    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> reshape(dims[1], dims[0], &v1(i,0));  // source matrix format
        FlatMatrix<> reshape2(dims[0], dims[1], &result(i,0));  // range matrix format
        reshape2 = Trans (reshape);
      }
  }  
  
  virtual void EvaluateDeriv (const BaseMappedIntegrationRule & mir,
                              FlatArray<FlatMatrix<>*> input,
                              FlatArray<FlatMatrix<>*> dinput,
                              FlatMatrix<> result,
                              FlatMatrix<> deriv) const
  {
    FlatMatrix<> v1 = *input[0];
    FlatMatrix<> dv1 = *dinput[0];

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> reshape(dims[1], dims[0], &v1(i,0));  // source matrix format
        FlatMatrix<> reshape2(dims[0], dims[1], &result(i,0));  // range matrix format
        reshape2 = Trans (reshape);
      }
    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> reshape(dims[1], dims[0], &dv1(i,0));  // source matrix format
        FlatMatrix<> reshape2(dims[0], dims[1], &deriv(i,0));  // range matrix format
        reshape2 = Trans (reshape);
      }
  }  


  
  virtual void EvaluateDDeriv (const BaseMappedIntegrationRule & mir,
                               FlatArray<FlatMatrix<>*> input,
                               FlatArray<FlatMatrix<>*> dinput,
                               FlatArray<FlatMatrix<>*> ddinput,
                               FlatMatrix<> result,
                               FlatMatrix<> deriv,
                               FlatMatrix<> dderiv) const
  {
    FlatMatrix<> v1 = *input[0];
    FlatMatrix<> dv1 = *dinput[0];
    FlatMatrix<> ddv1 = *ddinput[0];

    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> reshape(dims[1], dims[0], &v1(i,0));  // source matrix format
        FlatMatrix<> reshape2(dims[0], dims[1], &result(i,0));  // range matrix format
        reshape2 = Trans (reshape);
      }
    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> reshape(dims[1], dims[0], &dv1(i,0));  // source matrix format
        FlatMatrix<> reshape2(dims[0], dims[1], &deriv(i,0));  // range matrix format
        reshape2 = Trans (reshape);
      }
    for (int i = 0; i < mir.Size(); i++)
      {
        FlatMatrix<> reshape(dims[1], dims[0], &ddv1(i,0));  // source matrix format
        FlatMatrix<> reshape2(dims[0], dims[1], &dderiv(i,0));  // range matrix format
        reshape2 = Trans (reshape);
      }
    
  }

  
  };  




  

  
  // ///////////////////////////// operators  /////////////////////////

  struct GenericPlus {
    template <typename T> T operator() (T x, T y) const { return x+y; }
  };
  struct GenericMinus {
    template <typename T> T operator() (T x, T y) const { return x-y; }
  };
  struct GenericMult {
    template <typename T> T operator() (T x, T y) const { return x*y; }
  };
  struct GenericDiv {
    template <typename T> T operator() (T x, T y) const { return x/y; }
  };
  GenericPlus gen_plus;
  GenericMinus gen_minus;
  GenericMult gen_mult;
  GenericDiv gen_div;
  
  shared_ptr<CoefficientFunction> operator+ (shared_ptr<CoefficientFunction> c1, shared_ptr<CoefficientFunction> c2)
  {
    return BinaryOpCF (c1, c2, 
                       gen_plus, // [](double a, double b) { return a+b; },
                       [](Complex a, Complex b) { return a+b; },
                       [](double a, double b, double & dda, double & ddb) { dda = 1; ddb = 1; },
                       [](double a, double b, double & ddada, double & ddadb, double & ddbdb) 
                       { ddada = 0; ddadb = 0; ddbdb = 0; },
                       [](bool a, bool b) { return a||b; }, '+'
                       );
  }
  
  shared_ptr<CoefficientFunction> operator- (shared_ptr<CoefficientFunction> c1, shared_ptr<CoefficientFunction> c2)
  {
    return BinaryOpCF (c1, c2, 
                       gen_minus, // [](double a, double b) { return a-b; },
                       [](Complex a, Complex b) { return a-b; },
                       [](double a, double b, double & dda, double & ddb) { dda = 1; ddb = -1; },
                       [](double a, double b, double & ddada, double & ddadb, double & ddbdb) 
                       { ddada = 0; ddadb = 0; ddbdb = 0; },
                       [](bool a, bool b) { return a||b; }, '-'
                       );
  }
  shared_ptr<CoefficientFunction> operator* (shared_ptr<CoefficientFunction> c1, shared_ptr<CoefficientFunction> c2)
  {
    if (c1->Dimensions().Size() == 2 && c2->Dimensions().Size() == 2)
      return make_shared<MultMatMatCoefficientFunction> (c1, c2);
    if (c1->Dimensions().Size() == 2 && c2->Dimensions().Size() == 1)
      return make_shared<MultMatVecCoefficientFunction> (c1, c2);
    if (c1->Dimension() > 1 && c2->Dimension() > 1)
      {
        switch (c1->Dimension())
          {
          case 2:
            return make_shared<T_MultVecVecCoefficientFunction<2>> (c1, c2);
          case 3:
            return make_shared<T_MultVecVecCoefficientFunction<3>> (c1, c2);
          case 4:
            return make_shared<T_MultVecVecCoefficientFunction<4>> (c1, c2);
          case 5:
            return make_shared<T_MultVecVecCoefficientFunction<5>> (c1, c2);
          default:
            return make_shared<MultVecVecCoefficientFunction> (c1, c2);
          }
      }
    if (c1->Dimension() == 1 && c2->Dimension() > 1)
      return make_shared<MultScalVecCoefficientFunction> (c1, c2);
    if (c1->Dimension() > 1 && c2->Dimension() == 1)
      return make_shared<MultScalVecCoefficientFunction> (c2, c1);
    
    return BinaryOpCF (c1, c2, 
                       gen_mult, // [](double a, double b) { return a*b; },
                       [](Complex a, Complex b) { return a*b; },
                       [](double a, double b, double & dda, double & ddb) { dda = b; ddb = a; },
                       [](double a, double b, double & ddada, double & ddadb, double & ddbdb) 
                       { ddada = 0; ddadb = 1; ddbdb = 0; },
                       [](bool a, bool b) { return a&&b; }, '*'
                       );
  }

  shared_ptr<CoefficientFunction> operator* (double v1, shared_ptr<CoefficientFunction> c2)
  {
    return make_shared<ScaleCoefficientFunction> (v1, c2); 
  }
  
  shared_ptr<CoefficientFunction> operator* (Complex v1, shared_ptr<CoefficientFunction> c2)
  {
    return make_shared<ScaleCoefficientFunctionC> (v1, c2); 
  }

  shared_ptr<CoefficientFunction> InnerProduct (shared_ptr<CoefficientFunction> c1,
                                                shared_ptr<CoefficientFunction> c2)
  {
    switch (c1->Dimension())
      {
      case 2:
        return make_shared<T_MultVecVecCoefficientFunction<2>> (c1, c2);
      case 3:
        return make_shared<T_MultVecVecCoefficientFunction<3>> (c1, c2);
      case 4:
        return make_shared<T_MultVecVecCoefficientFunction<4>> (c1, c2);
      case 5:
        return make_shared<T_MultVecVecCoefficientFunction<5>> (c1, c2);
      case 6:
        return make_shared<T_MultVecVecCoefficientFunction<6>> (c1, c2);
      case 8:
        return make_shared<T_MultVecVecCoefficientFunction<8>> (c1, c2);
      case 9:
        return make_shared<T_MultVecVecCoefficientFunction<9>> (c1, c2);
      default:
        return make_shared<MultVecVecCoefficientFunction> (c1, c2);
      }
    
    // return make_shared<MultVecVecCoefficientFunction> (c1, c2);
  }

  shared_ptr<CoefficientFunction> TransposeCF (shared_ptr<CoefficientFunction> coef)
  {
    return make_shared<TransposeCoefficientFunction> (coef);
  }

  shared_ptr<CoefficientFunction> NormCF (shared_ptr<CoefficientFunction> coef)
  {
    if (coef->IsComplex())
      return make_shared<NormCoefficientFunction<Complex>> (coef);
    else
      return make_shared<NormCoefficientFunction<double>> (coef);
  }
  
  
  shared_ptr<CoefficientFunction> operator/ (shared_ptr<CoefficientFunction> c1, shared_ptr<CoefficientFunction> c2)
  {
    return BinaryOpCF (c1, c2,
                       gen_div, // [](double a, double b) { return a/b; },
                       [](Complex a, Complex b) { return a/b; },
                       [](double a, double b, double & dda, double & ddb) { dda = 1.0/b; ddb = -a/(b*b); },
                       [](double a, double b, double & ddada, double & ddadb, double & ddbdb) 
                       { ddada = 0; ddadb = -1.0/(b*b); ddbdb = 2*a/(b*b*b); },
                       [](bool a, bool b) { return a; }, '/'
                       );
  }


  // ///////////////////////////// IfPos   ////////////////////////////////  

  
  class IfPosCoefficientFunction : public CoefficientFunction
  {
    shared_ptr<CoefficientFunction> cf_if;
    shared_ptr<CoefficientFunction> cf_then;
    shared_ptr<CoefficientFunction> cf_else;
  public:
    IfPosCoefficientFunction (shared_ptr<CoefficientFunction> acf_if,
                              shared_ptr<CoefficientFunction> acf_then,
                              shared_ptr<CoefficientFunction> acf_else)
      : cf_if(acf_if), cf_then(acf_then), cf_else(acf_else)
    { ; }

    virtual ~IfPosCoefficientFunction () { ; }
    ///
    virtual double Evaluate (const BaseMappedIntegrationPoint & ip) const
    {
      if (cf_if->Evaluate(ip) > 0)
        return cf_then->Evaluate(ip);
      else
        return cf_else->Evaluate(ip);      
    }
    
    ///
    virtual void Evaluate (const BaseMappedIntegrationRule & ir, FlatMatrix<double> values) const
    {
      STACK_ARRAY(double, hmem1, ir.Size());
      FlatMatrix<> if_values(ir.Size(), 1, hmem1);
      STACK_ARRAY(double, hmem2, ir.Size()*values.Width());
      FlatMatrix<> then_values(ir.Size(), values.Width(), hmem2);
      STACK_ARRAY(double, hmem3, ir.Size()*values.Width());
      FlatMatrix<> else_values(ir.Size(), values.Width(), hmem3);
      
      cf_if->Evaluate (ir, if_values);
      cf_then->Evaluate (ir, then_values);
      cf_else->Evaluate (ir, else_values);
      
      for (int i = 0; i < ir.Size(); i++)
        if (if_values(i) > 0)
          values.Row(i) = then_values.Row(i);
        else
          values.Row(i) = else_values.Row(i);
      
      // for (int i = 0; i < ir.Size(); i++)
      //   values(i) = (if_values(i) > 0) ? then_values(i) : else_values(i);
    }


    virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & ir, AFlatMatrix<double> values) const
    {
      STACK_ARRAY(SIMD<double>, hmem1, ir.Size());
      AFlatMatrix<double> if_values(1, values.Width(), &hmem1[0].Data());
      STACK_ARRAY(SIMD<double>, hmem2, ir.Size()*values.Height());
      AFlatMatrix<double> then_values(values.Height(), values.Width(), &hmem2[0].Data());
      STACK_ARRAY(SIMD<double>, hmem3, ir.Size()*values.Height());
      AFlatMatrix<double> else_values(values.Height(), values.Width(), &hmem3[0].Data());
      
      cf_if->Evaluate (ir, if_values);
      cf_then->Evaluate (ir, then_values);
      cf_else->Evaluate (ir, else_values);

      for (int k = 0; k < values.Height(); k++)
        for (int i = 0; i < values.VWidth(); i++)
          values.Get(k,i) = ngstd::IfPos (if_values.Get(i),
                                          then_values.Get(k,i),
                                          else_values.Get(k,i)).Data();
    }

    virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & ir, FlatArray<AFlatMatrix<double>*> input,
                           AFlatMatrix<double> values) const
    {
      auto if_values = *input[0];
      auto then_values = *input[1];
      auto else_values = *input[2];
      
      for (int k = 0; k < values.Height(); k++)
        for (int i = 0; i < values.VWidth(); i++)
          values.Get(k,i) = ngstd::IfPos (if_values.Get(i),
                                          then_values.Get(k,i),
                                          else_values.Get(k,i)).Data();
    }

    
    virtual void Evaluate (const BaseMappedIntegrationRule & ir, FlatArray<FlatMatrix<>*> input,
                           FlatMatrix<double> values) const
    {
      FlatMatrix<> if_values = *input[0];
      FlatMatrix<> then_values = *input[1];
      FlatMatrix<> else_values = *input[2];
      for (int i = 0; i < if_values.Height(); i++)
        values(i) = (if_values(i) > 0) ? then_values(i) : else_values(i);
    }

    virtual bool IsComplex() const { return cf_then->IsComplex() | cf_else->IsComplex(); }
    virtual int Dimension() const { return cf_then->Dimension(); }

    void GenerateCode(Code &code, FlatArray<int> inputs, int index) const
    {
      auto var_if = Var(inputs[0]);
      TraverseDimensions( cf_then->Dimensions(), [&](int ind, int i, int j) {
          code.body += Var(index,i,j).Declare("decltype("+Var(inputs[1]).S()+")");
      });
      if(code.is_simd) {
        TraverseDimensions( cf_then->Dimensions(), [&](int ind, int i, int j) {
            code.body += Var(index,i,j).Assign("IfPos("+Var(inputs[0]).S()+','+Var(inputs[1],i,j).S()+','+Var(inputs[2],i,j).S()+")", false);
        });
      } else {
        code.body += "if (" + var_if.S() + ">0.0) {\n";
        TraverseDimensions( cf_then->Dimensions(), [&](int ind, int i, int j) {
            code.body += Var(index,i,j).Assign( Var(inputs[1],i,j), false );
        });
        code.body += "} else {\n";
        TraverseDimensions( cf_then->Dimensions(), [&](int ind, int i, int j) {
            code.body += Var(index,i,j).Assign( Var(inputs[2],i,j), false );
        });
        code.body += "}\n";
      }
    }

    virtual Array<int> Dimensions() const
    {
      return cf_then->Dimensions();
    }

    virtual void EvaluateDeriv (const BaseMappedIntegrationRule & ir,
                                FlatMatrix<> values,
                                FlatMatrix<> deriv) const
    {
      STACK_ARRAY(double, hmem1, ir.Size());
      FlatMatrix<> if_values(ir.Size(), 1, hmem1);
      STACK_ARRAY(double, hmem2, ir.Size()*values.Width());
      FlatMatrix<> then_values(ir.Size(), values.Width(), hmem2);
      STACK_ARRAY(double, hmem3, ir.Size()*values.Width());
      FlatMatrix<> else_values(ir.Size(), values.Width(), hmem3);
      STACK_ARRAY(double, hmem4, ir.Size()*values.Width());
      FlatMatrix<> then_deriv(ir.Size(), values.Width(), hmem4);
      STACK_ARRAY(double, hmem5, ir.Size()*values.Width());
      FlatMatrix<> else_deriv(ir.Size(), values.Width(), hmem5);

      
      cf_if->Evaluate (ir, if_values);
      cf_then->EvaluateDeriv (ir, then_values, then_deriv);
      cf_else->EvaluateDeriv (ir, else_values, else_deriv);
      
      for (int i = 0; i < ir.Size(); i++)
        if (if_values(i) > 0)
          {
            values.Row(i) = then_values.Row(i);
            deriv.Row(i) = then_deriv.Row(i);
          }
        else
          {
            values.Row(i) = else_values.Row(i);
            deriv.Row(i) = else_deriv.Row(i);
          }
    }

    /*
    virtual void EvaluateDeriv (const BaseMappedIntegrationRule & ir,
                                FlatMatrix<Complex> result,
                                FlatMatrix<Complex> deriv) const
    {
      Evaluate (ir, result);
      deriv = 0;
    }

    virtual void EvaluateDDeriv (const BaseMappedIntegrationRule & ir,
                                 FlatMatrix<> result,
                                 FlatMatrix<> deriv,
                                 FlatMatrix<> dderiv) const
    {
      EvaluateDeriv (ir, result, deriv);
      dderiv = 0;
    }

    virtual void EvaluateDDeriv (const BaseMappedIntegrationRule & ir,
                                 FlatMatrix<Complex> result,
                                 FlatMatrix<Complex> deriv,
                                 FlatMatrix<Complex> dderiv) const
    {
      EvaluateDeriv (ir, result, deriv);
      dderiv = 0;
    }

    
    virtual void EvaluateDeriv (const BaseMappedIntegrationRule & ir,
                                 FlatArray<FlatMatrix<>*> input,
                                 FlatArray<FlatMatrix<>*> dinput,
                                 FlatMatrix<> result,
                                 FlatMatrix<> deriv) const
    {
      EvaluateDeriv (ir, result, deriv);
    }

    virtual void EvaluateDDeriv (const BaseMappedIntegrationRule & ir,
                                 FlatArray<FlatMatrix<>*> input,
                                 FlatArray<FlatMatrix<>*> dinput,
                                 FlatArray<FlatMatrix<>*> ddinput,
                                 FlatMatrix<> result,
                                 FlatMatrix<> deriv,
                                 FlatMatrix<> dderiv) const
    {
      EvaluateDDeriv (ir, result, deriv, dderiv);
    }
    */

    // virtual bool ElementwiseConstant () const { return false; }
    
    // virtual void NonZeroPattern (const class ProxyUserData & ud, FlatVector<bool> nonzero) const;

    /*
    virtual void PrintReport (ostream & ost) const;
    virtual void PrintReportRec (ostream & ost, int level) const;
    virtual string GetName () const;
    */
    
    virtual void TraverseTree (const function<void(CoefficientFunction&)> & func)
    {
      cf_if->TraverseTree (func);
      cf_then->TraverseTree (func);
      cf_else->TraverseTree (func);
      func(*this);
    }
    
    virtual Array<CoefficientFunction*> InputCoefficientFunctions() const
    {
      return Array<CoefficientFunction*>( { cf_if.get(), cf_then.get(), cf_else.get() } );
    }
  };
  
  extern
  shared_ptr<CoefficientFunction> IfPos (shared_ptr<CoefficientFunction> cf_if,
                                         shared_ptr<CoefficientFunction> cf_then,
                                         shared_ptr<CoefficientFunction> cf_else)
  {
    return make_shared<IfPosCoefficientFunction> (cf_if, cf_then, cf_else);
  }

  void VectorialCoefficientFunction::GenerateCode(Code &code, FlatArray<int> inputs, int index) const
  {
    int input = 0;
    int input_index = 0;
    TraverseDimensions( dims, [&](int ind, int i, int j) {
	auto cfi = ci[input];
        int i1, j1;
        GetIndex( cfi->Dimensions(), input_index, i1, j1 );
        code.body += Var(index,i,j).Assign( Var(inputs[input], i1, j1) );
        input_index++;
        if (input_index == cfi->Dimension() )
        {
            input++;
            input_index = 0;
        }
    });
  }
  
  // ///////////////////////////// Compiled CF /////////////////////////
// int myglobalvar;
// int myglobalvar_eval;
  class CompiledCoefficientFunction : public CoefficientFunction
  {
    typedef void (*lib_function)(const ngfem::BaseMappedIntegrationRule &, ngbla::FlatMatrix<double>);
    typedef void (*lib_function_simd)(const ngfem::SIMD_BaseMappedIntegrationRule &, AFlatMatrix<double>);
    typedef void (*lib_function_deriv)(const ngfem::BaseMappedIntegrationRule &, ngbla::FlatMatrix<double>, ngbla::FlatMatrix<double>);
    typedef void (*lib_function_simd_deriv)(const ngfem::SIMD_BaseMappedIntegrationRule &, AFlatMatrix<double>, AFlatMatrix<double>);
    typedef void (*lib_function_dderiv)(const ngfem::BaseMappedIntegrationRule &, ngbla::FlatMatrix<double>, ngbla::FlatMatrix<double>, ngbla::FlatMatrix<double>);
    typedef void (*lib_function_simd_dderiv)(const ngfem::SIMD_BaseMappedIntegrationRule &, AFlatMatrix<double>, AFlatMatrix<double>, AFlatMatrix<double>);
    shared_ptr<CoefficientFunction> cf;
    Array<CoefficientFunction*> steps;
    DynamicTable<int> inputs;
    Array<int> dim;
    Array<bool> is_complex;
    // Array<Timer*> timers;
    Library library;
    lib_function compiled_function = nullptr;
    lib_function_simd compiled_function_simd = nullptr;
    lib_function_deriv compiled_function_deriv = nullptr;
    lib_function_simd_deriv compiled_function_simd_deriv = nullptr;
    lib_function_dderiv compiled_function_dderiv = nullptr;
    lib_function_simd_dderiv compiled_function_simd_dderiv = nullptr;
  public:
    CompiledCoefficientFunction (shared_ptr<CoefficientFunction> acf, bool realcompile)
      : cf(acf) // , compiled_function(nullptr), compiled_function_simd(nullptr)
    {
      cf -> TraverseTree
        ([&] (CoefficientFunction & stepcf)
         {
           if (!steps.Contains(&stepcf))
             {
               steps.Append (&stepcf);
               // timers.Append (new Timer(string("CompiledCF")+typeid(stepcf).name()));
               dim.Append (stepcf.Dimension());
               is_complex.Append (stepcf.IsComplex());
             }
         });

      cout << IM(3) << "Compiled CF:" << endl;
      for (auto cf : steps)
        cout << IM(3) << typeid(*cf).name() << endl;
      
      inputs = DynamicTable<int> (steps.Size());
      
      cf -> TraverseTree
        ([&] (CoefficientFunction & stepcf)
         {
           int mypos = steps.Pos (&stepcf);
           if (!inputs[mypos].Size())
             {
               Array<CoefficientFunction*> in = stepcf.InputCoefficientFunctions();
               for (auto incf : in)
                 inputs.Add (mypos, steps.Pos(incf));
             }
         });

      cout << IM(3) << "inputs = " << endl << inputs << endl;

      if(realcompile)
      {
        stringstream s;
        s << "#include<comp.hpp>" << endl;
        s << "using namespace ngcomp;" << endl;
        s << "extern \"C\" {" << endl;

        string parameters[3] = {"results", "deriv", "dderiv"};

        for (int deriv : Range(3))
        for (auto simd : {false,true}) {
            cout << IM(3) << "Compiled CF:" << endl;
            Code code;
            code.is_simd = simd;
            code.deriv = deriv;
            for (auto i : Range(steps)) {
              cout << IM(3) << "step " << i << ": " << typeid(*steps[i]).name() << endl;
              steps[i]->GenerateCode(code, inputs[i],i);
            }

            // set results
            string res_type = "double";
            if(simd) res_type = "SIMD<" + res_type + ">";
            if(deriv==1) res_type = "AutoDiff<1," + res_type + ">";
            if(deriv==2) res_type = "AutoDiffDiff<1," + res_type + ">";
            int ii = 0;
            TraverseDimensions( cf->Dimensions(), [&](int ind, int i, int j) {
                 code.body += Var(steps.Size(),i,j).Declare(res_type);
                 code.body += Var(steps.Size(),i,j).Assign(Var(steps.Size()-1,i,j),false);
                 string sget = "(i," + ToString(ii) + ") =";
                 if(simd) sget = ".Get(" + ToString(ii) + ",i) =";

                 for (auto ideriv : Range(deriv+1))
                 {
                   code.body += parameters[ideriv] + sget + Var(steps.Size(),i,j).code;
                   if(deriv>=1)
                   {
                     code.body += ".";
                     if(ideriv==2) code.body += "D";
                     if(ideriv>=1) code.body += "DValue(0)";
                     else code.body += "Value()";
                   }
                   if(simd) code.body +=".Data()";
                   code.body += ";\n";
                 }
                 ii++;
            });

            // Function name
            s << "void CompiledEvaluate";
            if(deriv==2) s << "D";
            if(deriv>=1) s << "Deriv";
            if(simd) s << "SIMD";

            // Function parameters
            string param_type = simd ? "AFlatMatrix<double> " : "FlatMatrix<> ";
            s << "( " << (simd?"SIMD_":"") << "BaseMappedIntegrationRule &mir";
            for(auto i : Range(deriv+1))
              s << ", " << param_type << parameters[i];
            s << " ) {" << endl;
            s << code.header << endl;
            s << "for ( auto i : Range(mir)) {" << endl;
            s << "auto & ip = mir[i];" << endl;
            s << code.body << endl;
            s << "}\n}" << endl << endl;

        }
        s << "}" << endl;
        library.Compile( s.str() );
        compiled_function_simd = library.GetFunction<lib_function_simd>("CompiledEvaluateSIMD");
        compiled_function = library.GetFunction<lib_function>("CompiledEvaluate");
        compiled_function_simd_deriv = library.GetFunction<lib_function_simd_deriv>("CompiledEvaluateDerivSIMD");
        compiled_function_deriv = library.GetFunction<lib_function_deriv>("CompiledEvaluateDeriv");
        compiled_function_simd_dderiv = library.GetFunction<lib_function_simd_dderiv>("CompiledEvaluateDDerivSIMD");
        compiled_function_dderiv = library.GetFunction<lib_function_dderiv>("CompiledEvaluateDDeriv");
      }
    }

    virtual void TraverseTree (const function<void(CoefficientFunction&)> & func)
    {
      cf -> TraverseTree (func);
      func(*cf);
    }

    virtual bool IsComplex() const { return cf->IsComplex(); }
    virtual int Dimension() const { return cf->Dimension(); }
    virtual Array<int> Dimensions() const  { return cf->Dimensions(); } 
    
    
    virtual bool ElementwiseConstant () const { return false; }
    virtual void NonZeroPattern (const class ProxyUserData & ud, FlatVector<bool> nonzero) const
    { cf->NonZeroPattern (ud, nonzero); }

    
    virtual double Evaluate (const BaseMappedIntegrationPoint & ip) const
    {
      return cf -> Evaluate(ip);
      // throw Exception ("compiled mip evaluate not implemented");
    }


    virtual void Evaluate (const BaseMappedIntegrationRule & ir, FlatMatrix<double> values) const
    {
      if(compiled_function)
      {
        compiled_function(ir,values);
        return;
      }

      // static Timer t1("CompiledCF::Evaluate 1");
      // static Timer t2("CompiledCF::Evaluate 2");
      // static Timer t3("CompiledCF::Evaluate 3");

      // t1.Start();
      int totdim = 0;
      for (int d : dim) totdim += d;
      ArrayMem<double, 10000> hmem(ir.Size()*totdim);
      int mem_ptr = 0;
      ArrayMem<FlatMatrix<>,100> temp(steps.Size());
      ArrayMem<FlatMatrix<>*, 100> in(steps.Size());

      for (int i = 0; i < steps.Size(); i++)
        {
          temp[i].AssignMemory(ir.Size(), dim[i], &hmem[mem_ptr]);
          mem_ptr += ir.Size()*dim[i];
        }
      // t1.Stop();
      // t2.Start();
      for (int i = 0; i < steps.Size(); i++)
        {
          // myglobalvar ++;
          // timers[i]->Start();
          auto inputi = inputs[i];
          for (int nr : Range(inputi))
            in[nr] = &temp[inputi[nr]];

          steps[i] -> Evaluate (ir, in.Range(0, inputi.Size()), temp[i]);
          // timers[i]->Stop();
        }
      values = temp.Last();
      // t2.Stop();
    }

    virtual void Evaluate (const SIMD_BaseMappedIntegrationRule & ir, AFlatMatrix<double> values) const
    {
      if(compiled_function_simd)
      {
        compiled_function_simd(ir, values);
        return;
      }

      int totdim = 0;
      for (int d : dim) totdim += d;
      STACK_ARRAY(SIMD<double>, hmem, ir.Size()*totdim);      
      int mem_ptr = 0;
      ArrayMem<AFlatMatrix<double>,100> temp(steps.Size());
      ArrayMem<AFlatMatrix<double>*,100> in(steps.Size());

      for (int i = 0; i < steps.Size(); i++)
        {
          new (&temp[i]) AFlatMatrix<double> (dim[i], ir.IR().GetNIP(), &hmem[mem_ptr].Data());
          mem_ptr += ir.Size()*dim[i];
        }

      for (int i = 0; i < steps.Size(); i++)
        {
          // timers[i]->Start();          
          auto inputi = inputs[i];
          for (int nr : Range(inputi))
            in[nr] = &temp[inputi[nr]];

          steps[i] -> Evaluate (ir, in.Range(0, inputi.Size()), temp[i]);
          // timers[i]->Stop();                    
        }
      values = temp.Last();
    }


    
    virtual void EvaluateDeriv (const BaseMappedIntegrationRule & ir,
                                FlatMatrix<double> values, FlatMatrix<double> deriv) const
    {
      if(compiled_function_deriv)
      {
        compiled_function_deriv(ir, values, deriv);
        return;
      }
      /*
      Array<Matrix<>*> temp;
      Array<Matrix<>*> dtemp;
      for (int i = 0; i < steps.Size(); i++)
        {
          // timers[i]->Start();
          
          temp.Append (new Matrix<>(ir.Size(), dim[i]));
          dtemp.Append (new Matrix<>(ir.Size(), dim[i]));
          
          Array<FlatMatrix<>*> in;
          for (int j : inputs[i])
            in.Append (temp[j]);
          Array<FlatMatrix<>*> din;
          for (int j : inputs[i])
            din.Append (dtemp[j]);
          
          steps[i] -> EvaluateDeriv (ir, in, din, *temp[i], *dtemp[i]);
          // timers[i]->Stop();
        }

      values = *temp.Last();
      deriv = *dtemp.Last();
      
      for (int i = 0; i < steps.Size(); i++)
        delete temp[i];
      for (int i = 0; i < steps.Size(); i++)
        delete dtemp[i];
      */

      int totdim = 0;
      for (int d : dim) totdim += d;
      ArrayMem<double, 10000> hmem(ir.Size()*3*totdim);
      int mem_ptr = 0;
      
      ArrayMem<FlatMatrix<>,100> temp(steps.Size());
      ArrayMem<FlatMatrix<>,100> dtemp(steps.Size());
      ArrayMem<FlatMatrix<>*, 20> in, din;

      for (int i = 0; i < steps.Size(); i++)
        {
          // timers[i]->Start();
          temp[i].AssignMemory(ir.Size(), dim[i], &hmem[mem_ptr]);
          mem_ptr += ir.Size()*dim[i];
          dtemp[i].AssignMemory(ir.Size(), dim[i], &hmem[mem_ptr]);          
          mem_ptr += ir.Size()*dim[i];

          in.SetSize(0);
          din.SetSize(0);
          for (int j : inputs[i])
            in.Append (&temp[j]);
          for (int j : inputs[i])
            din.Append (&dtemp[j]);
          steps[i] -> EvaluateDeriv (ir, in, din, temp[i], dtemp[i]);
          // timers[i]->Stop();
        }

      values = temp.Last();
      deriv = dtemp.Last();
    }

    virtual void EvaluateDDeriv (const BaseMappedIntegrationRule & ir,
                                 FlatMatrix<double> values, FlatMatrix<double> deriv,
                                 FlatMatrix<double> dderiv) const
    {
      if(compiled_function_dderiv)
      {
        compiled_function_dderiv(ir, values, deriv, dderiv);
        return;
      }
      int totdim = 0;
      for (int d : dim) totdim += d;
      ArrayMem<double, 10000> hmem(ir.Size()*3*totdim);
      int mem_ptr = 0;
      
      Array<FlatMatrix<>> temp(steps.Size());
      Array<FlatMatrix<>> dtemp(steps.Size());
      Array<FlatMatrix<>> ddtemp(steps.Size());
      ArrayMem<FlatMatrix<>*, 20> in, din, ddin;

      for (int i = 0; i < steps.Size(); i++)
        {
          // timers[i]->Start();          
          temp[i].AssignMemory(ir.Size(), dim[i], &hmem[mem_ptr]);
          mem_ptr += ir.Size()*dim[i];
          dtemp[i].AssignMemory(ir.Size(), dim[i], &hmem[mem_ptr]);          
          mem_ptr += ir.Size()*dim[i];
          ddtemp[i].AssignMemory(ir.Size(), dim[i], &hmem[mem_ptr]);          
          mem_ptr += ir.Size()*dim[i];

          in.SetSize(0);
          din.SetSize(0);
          ddin.SetSize(0);
          for (int j : inputs[i])
            in.Append (&temp[j]);
          for (int j : inputs[i])
            din.Append (&dtemp[j]);
          for (int j : inputs[i])
            ddin.Append (&ddtemp[j]);

          steps[i] -> EvaluateDDeriv (ir, in, din, ddin, temp[i], dtemp[i], ddtemp[i]);
          // timers[i]->Stop();                    
        }

      values = temp.Last();
      deriv = dtemp.Last();
      dderiv = ddtemp.Last();
    }

    
    virtual void EvaluateDeriv (const SIMD_BaseMappedIntegrationRule & ir, 
                                AFlatMatrix<double> values, AFlatMatrix<double> deriv) const
    {
      if(compiled_function_simd_deriv)
      {
        compiled_function_simd_deriv(ir, values, deriv);
        return;
      }
      throw ExceptionNOSIMD ("*************** CompiledCF :: EvaluateDeriv not available without codegeneration");      
    }

    virtual void EvaluateDDeriv (const SIMD_BaseMappedIntegrationRule & ir, 
                                 AFlatMatrix<double> values, AFlatMatrix<double> deriv,
                                 AFlatMatrix<double> dderiv) const
    {
      if(compiled_function_simd_dderiv)
      {
        compiled_function_simd_dderiv(ir, values, deriv, dderiv);
        return;
      }
      throw ExceptionNOSIMD ("*************** CompiledCF :: EvaluateDDeriv coming soon");
    }



    
    virtual void GenerateCode(Code &code, FlatArray<int> inputs, int index) const
    {
      return cf->GenerateCode(code, inputs, index);
    }
  };



  shared_ptr<CoefficientFunction> Compile (shared_ptr<CoefficientFunction> c, bool realcompile)
  {
    return make_shared<CompiledCoefficientFunction> (c, realcompile);
  }
  

  int Library::counter = 0;
 
}
