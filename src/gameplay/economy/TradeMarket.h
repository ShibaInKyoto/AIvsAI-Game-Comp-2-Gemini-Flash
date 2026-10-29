#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "gameplay/factions/FactionSystem.h"

namespace Voidfall {

enum class CommodityID {
    DARK_MATTER_CORE,
    HELIUM3_FUEL,
    SUPERCONDUCTING_ALLOY,
    NEURAL_NANITES,
    HYDRO_PURIFIER,
    SYNTHETIC_RATIONS,
    CONTRABAND_STIMULANTS,
    ANTIMATTER_ORDNANCE,
    COUNT
};

struct CommodityInfo {
    CommodityID id;
    std::string name;
    std::string category;
    int basePrice;
    int massKg;
    bool isIllegal;
    std::string description;
};

enum class MarketEvent {
    NORMAL,
    MINING_BOOM,
    PIRATE_BLOCKADE,
    WAR_MOBILIZATION,
    TECH_BREAKTHROUGH,
    FAMINE
};

struct MarketEntry {
    CommodityID id;
    int quantity;
    int buyPrice;
    int sellPrice;
    float priceTrend; // -1.0 to 1.0
};

struct CargoItem {
    CommodityID id;
    int quantity{0};
    int averageBuyPrice{0};
};

class StationMarket {
public:
    StationMarket(const std::string& stationName, FactionID controllingFaction);

    void InitializeInventory();
    void SetEvent(MarketEvent event, float durationSeconds = 300.0f);
    void Update(float dt);

    const std::string& GetStationName() const noexcept { return m_stationName; }
    FactionID GetFaction() const noexcept { return m_faction; }
    MarketEvent GetCurrentEvent() const noexcept { return m_currentEvent; }
    std::string GetEventName() const;

    const std::vector<MarketEntry>& GetEntries() const noexcept { return m_entries; }
    MarketEntry* GetEntry(CommodityID id);

    int CalculateBuyPriceForPlayer(CommodityID id, int playerFactionStanding) const;
    int CalculateSellPriceForPlayer(CommodityID id, int playerFactionStanding) const;

    bool BuyCommodity(CommodityID id, int amount, int& inOutPlayerCredits, std::vector<CargoItem>& playerCargo, int maxCargoMass, int standing);
    bool SellCommodity(CommodityID id, int amount, int& inOutPlayerCredits, std::vector<CargoItem>& playerCargo, int standing);

    static const CommodityInfo& GetCommodityInfo(CommodityID id);

private:
    void RecalculatePrices();

    std::string m_stationName;
    FactionID m_faction;
    MarketEvent m_currentEvent{MarketEvent::NORMAL};
    float m_eventTimer{0.0f};
    std::vector<MarketEntry> m_entries;
};

class EconomyManager {
public:
    EconomyManager();

    StationMarket* CreateStationMarket(const std::string& stationName, FactionID faction);
    StationMarket* GetStationMarket(const std::string& stationName);
    void UpdateAllMarkets(float dt);

    const std::vector<StationMarket>& GetMarkets() const noexcept { return m_markets; }

private:
    std::vector<StationMarket> m_markets;
};

} // namespace Voidfall
