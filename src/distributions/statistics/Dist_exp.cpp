/**
 * @file
 * This file contains the implementation of Dist_exp, which is used to hold
 * parameters and functions related to an exponential distribution.
 *
 * @brief Implementation of Dist_exp
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2009-12-14 12:43:32 +0100 (Mån, 14 Dec 2009) $
 * @author The RevBayes core development team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-08-27, version 1.0
 *
 * $Id: MemberObject.h 194 2009-12-14 11:43:32Z ronquist $
 */

#include "ArgumentRule.h"
#include "DAGNode.h"
#include "Dist_exp.h"
#include "Move_mscale.h"
#include "RealPos.h"
#include "RandomNumberGenerator.h"
#include "Real.h"
#include "RbNames.h"
#include "VectorString.h"
#include "Workspace.h"
#include "WrapperRule.h"

#include <cmath>
#include <sstream>


/** Default constructor for parser use */
Dist_exp::Dist_exp(void) : DistributionReal(getMemberRules()) {

}

/** Constructor for internal use */
Dist_exp::Dist_exp(double rate, RandomNumberGenerator* rng) : DistributionReal(getMemberRules()) {

    setValue("rate", new Real(rate));
    setValue("rng",  rng);
}

/**
 * This function calculates the cumulative probability for
 * an exponentially-distributed random variable.
 *
 * @brief Exponential cumulative probability
 *
 * @param q     Quantile
 * @return      Cumulative probability
 *
 */
double Dist_exp::cdf(double q) {

    double lambda = ((Real*) getValue("rate"))->getValue();

    return 1.0 - std::exp( -lambda * q );
}

/** Clone this object */
Dist_exp* Dist_exp::clone(void) const {

    return new Dist_exp(*this);
}

/** Get class vector showing type of object */
const VectorString& Dist_exp::getClass(void) const {

    static VectorString rbClass = VectorString(Dist_exp_name) + DistributionReal::getClass();
    return rbClass;
}

/** Get default move */
Move* Dist_exp::getDefaultMove(StochasticNode* node) {

    return new Move_mscale(node, 2.0*std::log(1.5), 1.0, Workspace::globalWorkspace().get_rng());
}

/** Get min value of distribution */
const Real* Dist_exp::getMin(void) {

    static Real rbZero = Real(0.0);
    return &rbZero;
}

/** Get member variable rules */
const MemberRules& Dist_exp::getMemberRules(void) const {

    static MemberRules memberRules;
    static bool        rulesSet = false;

    if (!rulesSet) {

        memberRules.push_back(new WrapperRule("rate", RealPos_name));

        /* Inherit rng from Distribution, put it at back */
        const MemberRules& inheritedRules = Distribution::getMemberRules();
        memberRules.insert(memberRules.end(), inheritedRules.begin(), inheritedRules.end()); 

        rulesSet = true;
    }

    return memberRules;
}

/** Get random variable type */
const std::string& Dist_exp::getVariableType(void) const {

    return RealPos_name;
}

/**
 * This function calculates the natural log of the likelihood
 * ratio for an exponentially-distributed random variable under
 * two different values of the distribution parameter.
 *
 * @brief Natural log of exponential likelihood ratio
 *
 * @param value     Value of random variable
 * @return          Natural log of the likelihood ratio
 */
double Dist_exp::lnLikelihoodRatio(const RbObject* value) {

    double lambdaNew = ((Real*) (getVariable("rate")->getValue      ()))->getValue();
    double lambdaOld = ((Real*) (getVariable("rate")->getStoredValue()))->getValue();
    double x         = ((Real*) value)->getValue();

    return std::log( lambdaNew ) - std::log( lambdaOld ) + (lambdaOld - lambdaNew) * x;
}

/**
 * This function calculates the natural log of the probability
 * density for an exponentially-distributed random variable.
 *
 * @brief Natural log of exponential probability density
 *
 * @param value Observed value
 * @return      Natural log of the probability density
 */
double Dist_exp::lnPdf(const RbObject* value) {

    double lambda = ((Real*) getValue("rate"))->getValue();
    double x      = ((Real*) value)->getValue();

    return std::log(lambda) -lambda * x;
}

/**
 * This function calculates the natural log of the probability
 * density ratio for two exponentially-distributed random variables.
 *
 * @brief Natural log of exponential probability density ratio
 *
 * @param newX      Value in numerator
 * @param oldX      Value in denominator
 * @return          Natural log of the probability density ratio
 */
double Dist_exp::lnPriorRatio(const RbObject* newVal, const RbObject* oldVal) {

    double lambda = ((Real*) getValue("rate"))->getValue();
    double newX   = ((Real*) newVal)->getValue();
    double oldX   = ((Real*) oldVal)->getValue();

    return lambda * (oldX - newX);
}

/**
 * This function calculates the probability density
 * for an exponentially-distributed random variable.
 *
 * @brief Exponential probability density
 *
 * @param value Observed value
 * @return      Probability density
 */
double Dist_exp::pdf(const RbObject* value) {

    double lambda = ((Real*) getValue("rate"))->getValue();
    double x      = ((Real*) value)->getValue();

    return lambda * std::exp( -lambda * x );
}

/**
 * This function calculates the quantile for an
 * exponentially-distributed random variable.
 *
 * @brief Quantile of exponential probability density
 *
 * @param p     Cumulative probability of quantile
 * @return      Quantile
 *
 */
double Dist_exp::quantile(const double p) {

    double lambda = ((Real*) getValue("rate"))->getValue();

    return -( 1.0 / lambda ) * std::log( 1.0 - p );
}

/**
 * This function generates an exponentially-distributed
 * random variable.
 *
 * @brief Random draw from exponential distribution
 *
 * @return      Random draw from exponential distribution
 */
Real* Dist_exp::rv(void) {

    double                 lambda = ((Real*) getValue("rate"))->getValue();
    RandomNumberGenerator* rng    = (RandomNumberGenerator*)(getValue("rng"));

    double u = rng->uniform01();

    return new RealPos(-( 1.0 / lambda ) * std::log( u ));
}


