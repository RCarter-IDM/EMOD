
#pragma once

#include "VectorContexts.h"
#include "Individual.h"

#include "IInfection.h"
#include "IContagionPopulation.h"
#include "StrainIdentity.h"

namespace Kernel
{
    // To store randomly selected strains and their weights from various pools an individual is exposed to
    typedef std::pair<StrainIdentity, float> strain_exposure_t;

    class VectorInterventionsContainer;

    class IndividualHumanVector : public IndividualHuman, public IIndividualHumanVectorContext
    {
        friend class SimulationVector;
        IMPLEMENT_DEFAULT_REFERENCE_COUNTING()
        DECLARE_QUERY_INTERFACE()

    public:
        // TODO change double to float
        static IndividualHumanVector *CreateHuman(INodeContext *context, suids::suid _suid, double monte_carlo_weight = 1.0, double initial_age = 0.0, int gender = 0);
        virtual ~IndividualHumanVector();

        virtual void InitializeHuman() override;
        virtual void CreateSusceptibility(float immunity_modifier = 1.0, float risk_modifier = 1.0) override;
        virtual void ExposeToInfectivity(float dt, TransmissionGroupMembership_t transmissionGroupMembership) override;
        virtual void UpdateInfectiousness(float dt) override;
        virtual void Expose( const IContagionPopulation* cp, float dt, TransmissionRoute::Enum tranmsission_route = TransmissionRoute::TRANSMISSIONROUTE_CONTACT ) override;

        virtual void UpdateGroupPopulation(float size_changes) override;

        // IIndividualHumanVectorContext methods
        virtual float GetRelativeBitingRate(void) const override;
        virtual int GetNumInfectiousBites() const override;
        virtual IVectorInterventionsEffects* GetVectorInterventionEffects() const override;

    protected:
        // cumulative exposure by pool stored along with randomly selected strain from each pool + total exposure
        std::vector<strain_exposure_t>  m_strain_exposure; // cumulative strain exposure is sorted for fast weighted random strain selection (although now it is only indoor+outdoor)
        float m_total_exposure;
        int m_num_infectious_bites;
        IVectorSusceptibilityContext * vector_susceptibility;
        VectorInterventionsContainer * vector_interventions; // cache this so we don't have to QI for it all the time. It won't change over time, but careful with malaria sims

        // TODO change double to float
        IndividualHumanVector(suids::suid id = suids::nil_suid(), double monte_carlo_weight = 1.0, double initial_age = 0.0, int gender = 0);
        IndividualHumanVector(INodeContext *context);

        virtual IInfection *createInfection(suids::suid _suid) override;
       
        virtual void setupInterventionsContainer() override;
        virtual bool DidReceiveInfectiousBite();
        virtual void ApplyTotalBitingExposure();
        virtual void AddExposure( const StrainIdentity& rStrainId,
                                  float totalExposure,
                                  TransmissionRoute::Enum transmission_route );

        virtual void PropagateContextToDependents() override;

        static void InitializeStaticsVector( const Configuration* config );

        void ReportInfectionState() override;

        DECLARE_SERIALIZABLE(IndividualHumanVector);
    };

    IArchive& serialize(IArchive&, std::vector<strain_exposure_t>&);

    struct compare_strain_exposure_float_less
    {
        bool operator() (const strain_exposure_t &lhs, const float rhs)
        {
            return lhs.second < rhs;
        }
    };
}
