#include "TradeMarket.h"
#include <algorithm>
#include <cmath>

namespace Voidfall {

static const CommodityInfo s_commodityDatabase[static_cast<size_t>(CommodityID::COUNT)] = {
    { CommodityID::DARK_MATTER_CORE, "Dark Matter Core", "Exotic Tech", 1200, 25, true, "Synthesized microscopic gravitational singularity used in dreadnought warp drives." },
    { CommodityID::HELIUM3_FUEL, "Helium-3 Fuel Cell", "Energy", 180, 10, false, "Refined lunar fusion isotope powering planetary defense reactors and sub-light thrusters." },
    { CommodityID::SUPERCONDUCTING_ALLOY, "Superconducting Alloy", "Industrial", 240, 50, false, "Room-temperature zero-resistance meta-alloy forged in high-gravity star stations." },
    { CommodityID::NEURAL_NANITES, "Neural Nanites", "Cybernetics", 650, 5, false, "Self-assembling carbon-lattice micro-machines for human-mech neural link synchronization." },
    { CommodityID::HYDRO_PURIFIER, "Hydro-Purifier Filter", "Life Support", 95, 15, false, "Catalytic filtration membranes vital for sustaining long-range deep space habitats." },
    { CommodityID::SYNTHETIC_RATIONS, "Synthetic Rations", "Food", 45, 8, false, "High-density nutrient paste fortified with radiation-resistant bio-proteins." },
    { CommodityID::CONTRABAND_STIMULANTS, "Void Stimulant Ampoules", "Contraband", 420, 2, true, "Banned reflex-accelerating synthetic endorphins popular with Syndicate corsairs." },
    { CommodityID::ANTIMATTER_ORDNANCE, "Antimatter Torpedo Warhead", "Military", 980, 40, false, "Magnetically bottled antiproton payload capable of shearing titan shield rings." }
};

const CommodityInfo& StationMarket::GetCommodityInfo(CommodityID id) {
    size_t idx = static_cast<size_t>(id);
    if (idx >= static_cast<size_t>(CommodityID::COUNT)) {
        return s_commodityDatabase[0];
    }
    return s_commodityDatabase[idx];
}

StationMarket::StationMarket(const std::string& stationName, FactionID controllingFaction)
    : m_stationName(stationName), m_faction(controllingFaction)
{
    InitializeInventory();
}

void StationMarket::InitializeInventory() {
    m_entries.clear();
    for (size_t i = 0; i < static_cast<size_t>(CommodityID::COUNT); ++i) {
        const auto& info = s_commodityDatabase[i];
        MarketEntry entry;
        entry.id = info.id;
        entry.quantity = 20 + (static_cast<int>(info.id) * 7) % 35;
        entry.buyPrice = info.basePrice;
        entry.sellPrice = static_cast<int>(info.basePrice * 0.82f);
        entry.priceTrend = 0.0f;
        m_entries.push_back(entry);
    }
    RecalculatePrices();
}

void StationMarket::SetEvent(MarketEvent event, float durationSeconds) {
    m_currentEvent = event;
    m_eventTimer = durationSeconds;
    RecalculatePrices();
}

std::string StationMarket::GetEventName() const {
    switch (m_currentEvent) {
        case MarketEvent::NORMAL: return "Standard Trade Operations";
        case MarketEvent::MINING_BOOM: return "Asteroid Mining Boom";
        case MarketEvent::PIRATE_BLOCKADE: return "Corsair System Blockade";
        case MarketEvent::WAR_MOBILIZATION: return "Fleet War Mobilization";
        case MarketEvent::TECH_BREAKTHROUGH: return "R&D Breakthrough";
        case MarketEvent::FAMINE: return "Life Support Crisis";
    }
    return "Unknown";
}

MarketEntry* StationMarket::GetEntry(CommodityID id) {
    for (auto& entry : m_entries) {
        if (entry.id == id) return &entry;
    }
    return nullptr;
}

void StationMarket::RecalculatePrices() {
    for (auto& entry : m_entries) {
        const auto& info = GetCommodityInfo(entry.id);
        float priceMod = 1.0f;

        // Supply impact (low supply raises price)
        float supplyRatio = static_cast<float>(entry.quantity) / 30.0f;
        priceMod *= std::clamp(1.5f - supplyRatio * 0.5f, 0.6f, 2.2f);

        // Event impacts
        if (m_currentEvent == MarketEvent::MINING_BOOM) {
            if (entry.id == CommodityID::SUPERCONDUCTING_ALLOY) priceMod *= 0.65f;
            if (entry.id == CommodityID::SYNTHETIC_RATIONS || entry.id == CommodityID::HYDRO_PURIFIER) priceMod *= 1.4f;
        } else if (m_currentEvent == MarketEvent::PIRATE_BLOCKADE) {
            priceMod *= 1.6f;
            if (entry.id == CommodityID::CONTRABAND_STIMULANTS) priceMod *= 1.8f;
        } else if (m_currentEvent == MarketEvent::WAR_MOBILIZATION) {
            if (entry.id == CommodityID::ANTIMATTER_ORDNANCE || entry.id == CommodityID::HELIUM3_FUEL) priceMod *= 1.9f;
        } else if (m_currentEvent == MarketEvent::FAMINE) {
            if (entry.id == CommodityID::SYNTHETIC_RATIONS || entry.id == CommodityID::HYDRO_PURIFIER) priceMod *= 2.3f;
        }

        entry.buyPrice = std::max(10, static_cast<int>(info.basePrice * priceMod));
        entry.sellPrice = std::max(8, static_cast<int>(entry.buyPrice * 0.80f));
    }
}

void StationMarket::Update(float dt) {
    if (m_currentEvent != MarketEvent::NORMAL) {
        m_eventTimer -= dt;
        if (m_eventTimer <= 0.0f) {
            SetEvent(MarketEvent::NORMAL);
        }
    }
}

int StationMarket::CalculateBuyPriceForPlayer(CommodityID id, int playerFactionStanding) const {
    const auto* entry = const_cast<StationMarket*>(this)->GetEntry(id);
    if (!entry) return 9999;

    float factor = 1.0f;
    if (playerFactionStanding >= 60) {
        factor = 0.85f; // 15% ally discount
    } else if (playerFactionStanding >= 20) {
        factor = 0.93f; // 7% friendly discount
    } else if (playerFactionStanding <= -20) {
        factor = 1.35f; // 35% tariff markup
    }

    return std::max(5, static_cast<int>(entry->buyPrice * factor));
}

int StationMarket::CalculateSellPriceForPlayer(CommodityID id, int playerFactionStanding) const {
    const auto* entry = const_cast<StationMarket*>(this)->GetEntry(id);
    if (!entry) return 1;

    float factor = 1.0f;
    if (playerFactionStanding >= 60) {
        factor = 1.15f; // 15% ally bonus
    } else if (playerFactionStanding >= 20) {
        factor = 1.07f; // 7% friendly bonus
    } else if (playerFactionStanding <= -20) {
        factor = 0.70f; // 30% penalty
    }

    return std::max(2, static_cast<int>(entry->sellPrice * factor));
}

bool StationMarket::BuyCommodity(
    CommodityID id, int amount, int& inOutPlayerCredits,
    std::vector<CargoItem>& playerCargo, int maxCargoMass, int standing
) {
    if (amount <= 0) return false;
    MarketEntry* entry = GetEntry(id);
    if (!entry || entry->quantity < amount) return false;

    const auto& info = GetCommodityInfo(id);
    int unitCost = CalculateBuyPriceForPlayer(id, standing);
    int totalCost = unitCost * amount;

    if (inOutPlayerCredits < totalCost) return false;

    // Calculate current mass
    int currentMass = 0;
    for (const auto& item : playerCargo) {
        currentMass += GetCommodityInfo(item.id).massKg * item.quantity;
    }
    if (currentMass + info.massKg * amount > maxCargoMass) {
        return false; // Cargo hold full
    }

    inOutPlayerCredits -= totalCost;
    entry->quantity -= amount;

    // Add to player cargo
    bool found = false;
    for (auto& item : playerCargo) {
        if (item.id == id) {
            int prevTotalCost = item.averageBuyPrice * item.quantity;
            item.quantity += amount;
            item.averageBuyPrice = (prevTotalCost + totalCost) / item.quantity;
            found = true;
            break;
        }
    }
    if (!found) {
        playerCargo.push_back({ id, amount, unitCost });
    }

    RecalculatePrices();
    return true;
}

bool StationMarket::SellCommodity(
    CommodityID id, int amount, int& inOutPlayerCredits,
    std::vector<CargoItem>& playerCargo, int standing
) {
    if (amount <= 0) return false;
    MarketEntry* entry = GetEntry(id);
    if (!entry) return false;

    for (auto it = playerCargo.begin(); it != playerCargo.end(); ++it) {
        if (it->id == id) {
            if (it->quantity < amount) return false;

            int unitPrice = CalculateSellPriceForPlayer(id, standing);
            int revenue = unitPrice * amount;

            inOutPlayerCredits += revenue;
            it->quantity -= amount;
            entry->quantity += amount;

            if (it->quantity <= 0) {
                playerCargo.erase(it);
            }

            RecalculatePrices();
            return true;
        }
    }

    return false;
}

EconomyManager::EconomyManager() {
    CreateStationMarket("Aegis Orbital Citadel Alpha", FactionID::AEGIS_DIRECTORATE);
    CreateStationMarket("Tartarus Freehaven Outpost", FactionID::CRIMSON_SYNDICATE);
    CreateStationMarket("Elysium Megamarket 01", FactionID::HYPERION_LOGISTICS);
    CreateStationMarket("Vulcan Deep Smelter", FactionID::SOLAR_FORGE);
    CreateStationMarket("Cygnus Null Sanctum", FactionID::NULL_ORDER);
}

StationMarket* EconomyManager::CreateStationMarket(const std::string& stationName, FactionID faction) {
    m_markets.emplace_back(stationName, faction);
    return &m_markets.back();
}

StationMarket* EconomyManager::GetStationMarket(const std::string& stationName) {
    for (auto& market : m_markets) {
        if (market.GetStationName() == stationName) return &market;
    }
    return nullptr;
}

void EconomyManager::UpdateAllMarkets(float dt) {
    for (auto& market : m_markets) {
        market.Update(dt);
    }
}

} // namespace Voidfall
