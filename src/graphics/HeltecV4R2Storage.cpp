#include "configuration.h"

#if defined(HELTEC_V4_R2_P04)
#include "graphics/HeltecSdRetry.h"
#include "storage/FatFsStorage.h"
#include <SD.h>
#include <SPI.h>

namespace
{
struct SdDriveAccess : fs::SDFS {
    static uint8_t drive(const fs::SDFS &sd) { return sd.*(&SdDriveAccess::_pdrv); }
};

class HeltecSdBus : public ISdBus
{
  public:
    IStorage::Status mount(bool formatIfNeeded) override
    {
        if (formatIfNeeded || !retry.ready())
            return IStorage::Status::CardError;
        if (SD.begin(SDCARD_CS, SPI, SD_SPI_FREQUENCY, "/sd", 6, false) && SD.cardType() != CARD_NONE) {
            retry.success();
            return IStorage::Status::Ok;
        }
        SD.end();
        retry.failure();
        return IStorage::Status::CardError;
    }

    void unmount() override { SD.end(); }
    uint8_t drive() const override { return SdDriveAccess::drive(SD); }
    fs::FS *arduinoFS() override { return &SD; }

    StorageInfo::CardType cardType() override
    {
        switch (SD.cardType()) {
        case CARD_MMC:
            return StorageInfo::eMMC;
        case CARD_SD:
            return StorageInfo::eSD;
        case CARD_SDHC:
            return SD.cardSize() > 32ULL * 1024 * 1024 * 1024 ? StorageInfo::eSDXC : StorageInfo::eSDHC;
        case CARD_NONE:
            return StorageInfo::eNone;
        default:
            return StorageInfo::eUnknown;
        }
    }

  private:
    HeltecSdRetry retry;
};

class HeltecMapStorage : public FatFsStorage
{
  public:
    HeltecMapStorage() : FatFsStorage(std::unique_ptr<ISdBus>(new HeltecSdBus)) {}
    bool format() override { return false; }
};
} // namespace

std::unique_ptr<IStorage> createStorage()
{
    return std::unique_ptr<IStorage>(new HeltecMapStorage);
}
#endif
