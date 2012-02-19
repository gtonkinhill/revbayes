/**
 * @file
 * This file contains the implementation of Dist_exp, which is used to hold
 * parameters and functions related to an exponential distribution.
 *
 * @brief Implementation of Dist_exp
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2009-12-14 12:43:32 +0100 (Mån, 14 Dec 2009) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-08-27, version 1.0
 *
 * $Id: MemberObject.h 194 2009-12-14 11:43:32Z ronquist $
 */

#include "DAGNode.h"
#include "Dist_exp.h"
#include "Move_mscale.h"
#include "RealPos.h"
#include "RandomNumberGenerator.h"
#include "RbUtil.h"
#include "Real.h"
#include "ValueRule.h"
#include "VectorString.h"
#include "Workspace.h"

#include <cmath>
#include <sstream>


// Definition of the static type spec member
const TypeSpec Dist_exp::typeSpec(Dist_exp_name);
const TypeSpec Dist_exp::varTypeSpec(RealPos_name);

/** Default constructor for parser use */
Dist_exp::Dist_exp( void ) : DistributionContinuous( getMemberRules() ), rate( NULL ) {

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
double Dist_exp::cdf( const RbLanguageObject& value ) {

    const double lambda = static_cast<      RealPos&>( rate->getValue() ).getValue();
    const double q      = static_cast<const RealPos&>( value                  ).getValue();

    return 1.0 - std::exp( - lambda * q );
}


/** Clone this object */
Dist_exp* Dist_exp::clone( void ) const {

    return new Dist_exp( *this );
}


/** Get class vector showing type of object */
const VectorString& Dist_exp::getClass( void ) const {

    static VectorString rbClass = VectorString( Dist_exp_name ) + DistributionContinuous::getClass();
    return rbClass;
}

/** Get member variable rules */
const MemberRules& Dist_exp::getMemberRules( void ) const {

    static MemberRules memberRules = MemberRules();
    static bool        rulesSet = false;

    if ( !rulesSet ) {

        memberRules.push_back( new ValueRule( "rate", RealPos_name ) );

        rulesSet = true;
    }

    return memberRules;
}


/** Get the type spec of this class. We return a static class variable because all instances will be exactly from this type. */
const TypeSpec& Dist_exp::getTypeSpec(void) const {
    return typeSpec;
}


/** Get random variable type */
const TypeSpec& Dist_exp::getVariableType( void ) const {

    return varTypeSpec;
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
double Dist_exp::lnPdf( const RbLanguageObject& value ) const {
    
    double lambda = static_cast<const RealPos&>( rate->getValue() ).getValue();
    double x      = static_cast<const RealPos&>( value ).getValue();

    return std::log(lambda) -lambda * x;
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
double Dist_exp::pdf( const RbLanguageObject& value ) const {

    double lambda = static_cast<const RealPos&>( rate->getValue() ).getValue();
    double x      = static_cast<const RealPos&>( value ).getValue();

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
const Real& Dist_exp::quantile(const double p) {

    double lambda = static_cast<RealPos&>( rate->getValue() ).getValue();
    quant.setValue( -( 1.0 / lambda ) * std::log( 1.0 - p ) );
    return quant;
}


/**
 * This function generates an exponentially-distributed
 * random variable.
 *
 * @brief Random draw from exponential distribution
 *
 * @return      Random draw from exponential distribution
 */
const RbLanguageObject& Dist_exp::rv( void ) {

    double lambda = static_cast<RealPos&>( rate->getValue() ).getValue();
    RandomNumberGenerator* rng    = GLOBAL_RNG;

    double u = rng->uniform01();
    randomVariable.setValue( -( 1.0 / lambda ) * std::log( u ) );

    return randomVariable;
}


/** We catch here the setting of the member variables to store our parameters. */
void Dist_exp::setMemberVariable(std::string const &name, Variable *var) {
    
    if ( name == "rate" ) {
        rate = var;
    }
    else {
        DistributionContinuous::setMemberVariable(name, var);
    }
}


