#include <ifxRadarDeviceCommon/internal/AcquisitionState.hpp>


namespace AcquisitionStateLogic
{
    bool update(Atomic_Acquisition_State_t& state, Acquisition_State_t expectedState, Acquisition_State_t newState)
    {
        return state.compare_exchange_strong(expectedState, newState);
    }

    bool update(Atomic_Acquisition_State_t& state,
                AcquisitionStateLogic::States expectedStates,
                Acquisition_State_t newState)
    {
        bool updated = false;
        for(auto& expectedState : expectedStates) {
            updated = state.compare_exchange_strong(expectedState, newState);
            if(updated) {
                break;
            }
        }
        return updated;
    }
}
