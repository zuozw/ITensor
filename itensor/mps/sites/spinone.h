//
// Distributed under the ITensor Library License, Version 1.2
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_SPINONE_H
#define __ITENSOR_SPINONE_H
#include "itensor/mps/siteset.h"

namespace itensor {

class SpinOne : public SiteSet
    {
    public:

    SpinOne();

    SpinOne(int N, const Args& args = Global::args());

    private:

    virtual int
    getN() const;

    virtual const IQIndex&
    getSi(int i) const;

    virtual IQIndexVal
    getState(int i, const String& state) const;

    virtual IQTensor
    getOp(int i, const String& opname, const Args& opts) const;

    void
    constructSites(const Args& opts);

    void
    doRead(std::istream& s);
     
    void
    doWrite(std::ostream& s) const;

    //Data members -----------------

    int N_;

    std::vector<IQIndex> site_;

    };

inline SpinOne::
SpinOne()
    : N_(-1)
    { }

inline SpinOne::
SpinOne(int N, const Args& opts)
    : N_(N),
      site_(N_+1)
    { 
    constructSites(opts);
    }

inline void SpinOne::
constructSites(const Args& opts)
    {
    for(int j = 1; j <= N_; ++j)
        {
        if((opts.getBool("SHalfEdge",false) && (j==1 || j == N_))
           || (opts.getBool("SHalfLeftEdge",false) && j==1))
            {
            if(opts.getBool("Verbose",false))
                {
                println("Placing a S=1/2 at site ",j);
                }

            site_.at(j) = IQIndex(nameint("S=1/2 site=",j),
                Index(nameint("Up for site",j),1,Site),QN(+1,0),
                Index(nameint("Dn for site",j),1,Site),QN(-1,0));
            }
        else
            {
            site_.at(j) = IQIndex(nameint("S=1 site=",j),
                Index(nameint("Up for site",j),1,Site),QN(+2,0),
                Index(nameint("Z0 for site",j),1,Site),QN( 0,0),
                Index(nameint("Dn for site",j),1,Site),QN(-2,0));
            }
        }
    }

inline void SpinOne::
doRead(std::istream& s)
    {
    s.read((char*) &N_,sizeof(N_));
    site_.resize(N_+1);
    for(int j = 1; j <= N_; ++j) 
        site_.at(j).read(s);
    }

inline void SpinOne::
doWrite(std::ostream& s) const
    {
    s.write((char*) &N_,sizeof(N_));
    for(int j = 1; j <= N_; ++j) 
        site_.at(j).write(s);
    }

inline int SpinOne::
getN() const
    { return N_; }

inline const IQIndex& SpinOne::
getSi(int i) const
    { return site_.at(i); }

inline IQIndexVal SpinOne::
getState(int i, const String& state) const
    {
    int st = -1;
    if(state == "Up" || state == "+") 
        {
        st = 1;
        }
    else
    if(state == "Z0" || state == "0")
        {
        if(getSi(i).m() == 2)
            Error("Z0 not defined for spin 1/2 site");
        st = 2;
        }
    else
    if(state == "Dn" || state == "-")
        {
        st = getSi(i).m();
        }
    else
        {
        Error("State " + state + " not recognized");
        }
    return getSi(i)(st);
    }

inline IQTensor SpinOne::
getOp(int i, const String& opname, const Args& opts) const
    {
    auto s = si(i);
    auto sP = prime(s);

    IQIndexVal Up(s(1)),
               UpP(sP(1)),
               Dn(s(s.m())),
               DnP(sP(s.m())),
               Z0(s(2)),
               Z0P(sP(2));

    IQTensor Op(dag(s),sP);

    if(opname == "Sz")
        {
        if(s.m() == 2)
            {
            Op.set(+0.5,Up,UpP);
            Op.set(-0.5,Dn,DnP);
            }
        else
            {
            Op.set(+1.0,Up,UpP);
            Op.set(-1.0,Dn,DnP);
            }
        }
    else
    if(opname == "Sx")
        {
        if(s.m() == 2)
            {
            Op.set(+0.5,Up,DnP);
            Op.set(+0.5,Dn,UpP);
            }
        else
            {
            Op.set(ISqrt2,Up,Z0P); 
            Op.set(ISqrt2,Z0,UpP);
            Op.set(ISqrt2,Z0,DnP); 
            Op.set(ISqrt2,Dn,Z0P);
            }
        }
    else
    if(opname == "ISy")
        {
        if(s.m() == 2)
            {
            Op.set(-0.5,Up,DnP);
            Op.set(+0.5,Dn,UpP);
            }
        else
            {
            Op.set(+ISqrt2,Up,Z0P); 
            Op.set(-ISqrt2,Z0,UpP);
            Op.set(+ISqrt2,Z0,DnP); 
            Op.set(-ISqrt2,Dn,Z0P);
            }
        }
    else
    if(opname == "Sp" || opname == "S+")
        {
        if(s.m() == 2)
            {
            Op.set(1,Dn,UpP);
            }
        else
            {
            Op.set(Sqrt2,Dn,Z0P);  
            Op.set(Sqrt2,Z0,UpP);
            }
        }
    else
    if(opname == "Sm" || opname == "S-")
        {
        if(s.m() == 2)
            {
            Op.set(1,Up,DnP);
            }
        else
            {
            Op.set(Sqrt2,Up,Z0P);
            Op.set(Sqrt2,Z0,DnP);
            }
        }
    else
    if(opname == "Sz2")
        {
        if(s.m() == 2) Error("Sz^2 only non-trivial for S=1 sites");
        Op.set(1,Up,UpP); 
        Op.set(1,Dn,DnP);
        }
    else
    if(opname == "Sx2")
        {
        if(s.m() == 2) Error("Sx^2 only non-trivial for S=1 sites");
        Op.set(0.5,Up,UpP); 
        Op.set(0.5,Up,DnP);
        Op.set(1.0, Z0,Z0P);
        Op.set(0.5,Dn,DnP); 
        Op.set(0.5,Dn,UpP);
        }
    else
    if(opname == "Sy2")
        {
        if(s.m() == 2) Error("Sy^2 only non-trivial for S=1 sites");
        Op.set(0.5,Up,UpP); 
        Op.set(-0.5,Up,DnP);
        Op.set(1,Z0,Z0P);
        Op.set(+0.5,Dn,DnP); 
        Op.set(-0.5,Dn,UpP);
        }
    else
    if(opname == "projUp")
        {
        Op.set(1,Up,UpP);
        }
    else
    if(opname == "projZ0")
        {
        if(s.m() == 2) Error("Can only form projZ0 for S=1 sites");
        Op.set(1,Z0,Z0P);
        }
    else
    if(opname == "projDn")
        {
        Op.set(1,Dn,DnP);
        }
    else
    if(opname == "XUp")
        {
        //m = +1 state along x axis
        Op = IQTensor(s);
        Op.set(0.5,Up);
        Op.set(ISqrt2,Z0);
        Op.set(0.5,Dn);
        }
    else
    if(opname == "XZ0")
        {
        //m = 0 state along x axis
        Op = IQTensor(s);
        Op.set(ISqrt2,Up);
        Op.set(-ISqrt2,Dn);
        }
    else
    if(opname == "XDn")
        {
        //m = -1 state along x axis
        Op = IQTensor(s);
        Op.set(0.5,Up);
        Op.set(-ISqrt2,Z0);
        Op.set(0.5,Dn);
        }
    else
    if(opname == "S2")
        {
        auto ssp1 = (s.m()==2 ? 0.75 : 2.);
        Op.set(ssp1,Up,UpP); 
        Op.set(ssp1,Dn,DnP);
        if(s.m() > 2)
            Op.set(ssp1,Z0,Z0P);
        }
    else
        {
        Error("Operator " + opname + " name not recognized");
        }

    return Op;
    }

}; //namespace itensor

#endif