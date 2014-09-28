/* 
 * File:   InverseWishartDistribution.cpp
 * Author: nl
 * 
 * Created on 15 juillet 2014, 10:08
 */

#include "InverseWishartDistribution.h"

#include "RandomNumberFactory.h"
#include "RandomNumberGenerator.h"
#include "RbConstants.h"
#include "RbStatisticsHelper.h"
#include "DistributionNormal.h"
#include "DistributionInverseWishart.h"

using namespace RevBayesCore;

InverseWishartDistribution::InverseWishartDistribution(const TypedDagNode<MatrixRealSymmetric> *insigma0, const TypedDagNode<int>* indf)  :
TypedDistribution<RevBayesCore::MatrixRealSymmetric>(new MatrixRealSymmetric(insigma0->getValue().getDim())),
sigma0(insigma0),
kappaVector(NULL),
kappa(NULL),
df(indf),
dim(0)  {
    
    redrawValue();
}

InverseWishartDistribution::InverseWishartDistribution(const TypedDagNode<RbVector<double> > *inkappaVector, const TypedDagNode<int>* indf)  :
TypedDistribution<RevBayesCore::MatrixRealSymmetric>(new MatrixRealSymmetric( inkappaVector->getValue().size()) ),
    sigma0(NULL),
    kappaVector(inkappaVector),
    kappa(NULL),
    df(indf),
    dim(0)    {
    
        redrawValue();
}

InverseWishartDistribution::InverseWishartDistribution(const TypedDagNode<int>* indim, const TypedDagNode<double> *inkappa, const TypedDagNode<int>* indf)  :
TypedDistribution<RevBayesCore::MatrixRealSymmetric>(new MatrixRealSymmetric( size_t(indim->getValue()) )),
    sigma0(NULL),
    kappaVector(NULL),
    kappa(inkappa),
    df(indf),
    dim(indim)    {
    
        redrawValue();
}

InverseWishartDistribution::InverseWishartDistribution(const InverseWishartDistribution& from) :
    TypedDistribution<RevBayesCore::MatrixRealSymmetric>(new MatrixRealSymmetric(from.getValue().getDim())),
    sigma0(from.sigma0),
    kappaVector(from.kappaVector),
    kappa(from.kappa),
    df(from.df),
    dim(from.dim)   {

        redrawValue();
}

InverseWishartDistribution* InverseWishartDistribution::clone(void) const   {

    return new InverseWishartDistribution(*this);
}



/** Get the parameters of the distribution */
std::set<const DagNode*> InverseWishartDistribution::getParameters( void ) const
{
    std::set<const DagNode*> parameters;
    
    parameters.insert( sigma0 );
    parameters.insert( kappaVector );
    parameters.insert( kappa );
    parameters.insert( dim );
    parameters.insert( df );
    
    parameters.erase( NULL );
    return parameters;
}

void InverseWishartDistribution::swapParameter(const DagNode *oldP, const DagNode *newP) {
    if (oldP == sigma0) {
        sigma0 = static_cast<const TypedDagNode<MatrixRealSymmetric>* >( newP );
    }
    if (oldP == kappaVector)  {
        kappaVector = static_cast<const TypedDagNode<RbVector<double> >* >(newP);
    }
    if (oldP == kappa)  {
        kappa = static_cast<const TypedDagNode<double>* >(newP);
    }
    if (oldP == dim)    {
        dim = static_cast<const TypedDagNode<int>* >(newP);
    }
    if (oldP == df)    {
        df = static_cast<const TypedDagNode<int>* >(newP);
    }
}


double InverseWishartDistribution::computeLnProbability(void)  {
    
    double ret = 0;
    
    if (sigma0) {    
        ret = RbStatistics::InverseWishart::lnPdf(sigma0->getValue(),df->getValue(),getValue());
    }
    else if (kappaVector)    {
        ret = RbStatistics::InverseWishart::lnPdf(kappaVector->getValue(),df->getValue(),getValue());        
    }
    else if (kappa)  {
        ret = RbStatistics::InverseWishart::lnPdf(kappa->getValue(),df->getValue(),getValue());        
    }
    else    {
        std::cerr << "error in inverse wishart: no parameter\n";
        throw(0);
    }
    return ret;
}

void InverseWishartDistribution::redrawValue(void)  {

    RandomNumberGenerator* rng = GLOBAL_RNG;

    if (sigma0) {
        setValue( RbStatistics::InverseWishart::rv(sigma0->getValue(),df->getValue(), *rng) );
    }
    else if (kappaVector)    {
        setValue( RbStatistics::InverseWishart::rv(kappaVector->getValue(),df->getValue(), *rng) );
    }
    else if (kappa)   {
        setValue( RbStatistics::InverseWishart::rv(kappa->getValue(),getValue().getDim(),df->getValue(), *rng) );
    }
    else    {
        std::cerr << "error in inverse wishart: no parameter\n";
        throw(0);
    }
    /*
    for (size_t i=0; i<getValue().getDim(); i++)   {
        for (size_t j=0; j<getValue().getDim(); j++)   {
            getValue()[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }
    */
    getValue().update();


}
