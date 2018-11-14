#ifndef HARDWARE_GOOGLE_PIXEL_POWERSTATS_POWERSTATS_H
#define HARDWARE_GOOGLE_PIXEL_POWERSTATS_POWERSTATS_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <android/hardware/power/stats/1.0/IPowerStats.h>

namespace android {
namespace hardware {

namespace google {
namespace pixel {
namespace powerstats {

using android::hardware::hidl_vec;
using android::hardware::Return;
using android::hardware::Void;
using android::hardware::power::stats::V1_0::EnergyData;
using android::hardware::power::stats::V1_0::IPowerStats;
using android::hardware::power::stats::V1_0::PowerEntityInfo;
using android::hardware::power::stats::V1_0::PowerEntityStateInfo;
using android::hardware::power::stats::V1_0::PowerEntityStateResidencyData;
using android::hardware::power::stats::V1_0::PowerEntityStateResidencyResult;
using android::hardware::power::stats::V1_0::PowerEntityStateSpace;
using android::hardware::power::stats::V1_0::PowerEntityType;
using android::hardware::power::stats::V1_0::RailInfo;
using android::hardware::power::stats::V1_0::Status;

class IRailDataProvider {
  public:
    virtual ~IRailDataProvider() = default;
    virtual Return<void> getRailInfo(IPowerStats::getRailInfo_cb _hidl_cb) = 0;
    virtual Return<void> getEnergyData(const hidl_vec<uint32_t> &railIndices,
                                       IPowerStats::getEnergyData_cb _hidl_cb) = 0;
    virtual Return<void> streamEnergyData(uint32_t timeMs, uint32_t samplingRate,
                                          IPowerStats::streamEnergyData_cb _hidl_cb) = 0;
};

class IStateResidencyDataProvider {
  public:
    virtual ~IStateResidencyDataProvider() = default;
    virtual bool getResults(std::map<uint32_t, PowerEntityStateResidencyResult> &results) = 0;
    virtual std::vector<PowerEntityStateSpace> getStateSpaces() = 0;
};

}  // namespace powerstats
}  // namespace pixel
}  // namespace google

namespace power {
namespace stats {
namespace V1_0 {
namespace implementation {

using android::hardware::google::pixel::powerstats::IRailDataProvider;
using android::hardware::google::pixel::powerstats::IStateResidencyDataProvider;

class PowerStats : public IPowerStats {
  public:
    PowerStats() = default;
    void setRailDataProvider(std::unique_ptr<IRailDataProvider> dataProvider);
    uint32_t addPowerEntity(std::string name, PowerEntityType type);
    void addStateResidencyDataProvider(std::shared_ptr<IStateResidencyDataProvider> p);

    // Methods from ::android::hardware::power::stats::V1_0::IPowerStats follow.
    Return<void> getRailInfo(getRailInfo_cb _hidl_cb) override;
    Return<void> getEnergyData(const hidl_vec<uint32_t> &railIndices,
                               getEnergyData_cb _hidl_cb) override;
    Return<void> streamEnergyData(uint32_t timeMs, uint32_t samplingRate,
                                  streamEnergyData_cb _hidl_cb) override;
    Return<void> getPowerEntityInfo(getPowerEntityInfo_cb _hidl_cb) override;
    Return<void> getPowerEntityStateInfo(const hidl_vec<uint32_t> &powerEntityIds,
                                         getPowerEntityStateInfo_cb _hidl_cb) override;
    Return<void> getPowerEntityStateResidencyData(
        const hidl_vec<uint32_t> &powerEntityIds,
        getPowerEntityStateResidencyData_cb _hidl_cb) override;

  private:
    std::unique_ptr<IRailDataProvider> mRailDataProvider;
    std::vector<PowerEntityInfo> mPowerEntityInfos;
    std::unordered_map<uint32_t, PowerEntityStateSpace> mPowerEntityStateSpaces;
    std::unordered_map<uint32_t, std::shared_ptr<IStateResidencyDataProvider>>
        mStateResidencyDataProviders;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace stats
}  // namespace power
}  // namespace hardware
}  // namespace android

#endif  // HARDWARE_GOOGLE_PIXEL_POWERSTATS_POWERSTATS_H
