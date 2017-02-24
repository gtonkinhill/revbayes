//
//  SiteRateModifierFunction.hpp
//  revbayes-branch-proj
//
//  Created by Michael Landis on 2/23/17.
//  Copyright © 2017 Michael Landis. All rights reserved.
//

#ifndef SiteRateModifierFunction_hpp
#define SiteRateModifierFunction_hpp

#include "CharacterHistoryRateModifier.h"
#include "TypedDagNode.h"
#include "TypedFunction.h"

#include <iostream>
#include <vector>

namespace RevBayesCore {
    
    class SiteRateModifierFunction : public TypedFunction<CharacterHistoryRateModifier> {
        
    public:
        SiteRateModifierFunction(const TypedDagNode<RbVector<RbVector<double> > >* rm, const TypedDagNode<RbVector<RbVector<int> > >* ec, const TypedDagNode<RbVector<int> >* sc);
        SiteRateModifierFunction(const SiteRateModifierFunction& m);
        virtual ~SiteRateModifierFunction(void);                                                                                                  //!< Virtual destructor
        
        // public member functions
        SiteRateModifierFunction*                                         clone(void) const;                                                          //!< Create an independent clone
        void                                                              update(void);
        
    protected:
        void                                                              swapParameterInternal(const DagNode *oldP, const DagNode *newP);            //!< Implementation of swaping parameters
        
    private:
        
        // members
        const TypedDagNode<RbVector<RbVector<double> > >*                            rate_multipliers;
        const TypedDagNode<RbVector<RbVector<int> > >*                               event_classes;
        const TypedDagNode<RbVector<int> >*                                          site_classes;
        
    };
}

#endif /* SiteRateModifierFunction_hpp */
