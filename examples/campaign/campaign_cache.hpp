/* 
 * File:   campaign_cache.hpp
 * Author: vladimir venediktov
 *
 * Created on March 12, 2017, 9:41 PM
 */

#ifndef CAMPAIGN_CACHE_HPP
#define	CAMPAIGN_CACHE_HPP

#include "config.hpp"
#include "core/tagged_tuple.hpp"
#include "common/perf_timer.hpp"
#include "datacache/campaign_entity.hpp"
#include "datacache/entity_cache.hpp"
#include "datacache/memory_types.hpp"
#include <memory>
#include <cstdint>
#include <iostream>

struct CampaignBudget {
    uint32_t campaign_id;
    uint64_t day_budget_limit; //micro dollars
    uint64_t day_budget_spent; //micro dollars
    uint32_t day_show_limit;
    uint32_t day_click_limit;
   
        
    friend std::ostream &operator<<(std::ostream & os, const std::shared_ptr<CampaignBudget> ptr)  {
        os <<  *ptr;
        return os;
    }
    friend std::ostream &operator<<(std::ostream & os, const  CampaignBudget & value)  {
        os << value.campaign_id       << "|" 
           << value.day_budget_limit << "|"
           << value.day_budget_spent << "|"
           << value.day_show_limit   << "|"
           << value.day_click_limit
        ;
        return os;
    }
   
};

template <typename Config = CampaignManagerConfig,
          typename Memory = typename mpclmi::ipc::Shared,
          typename Alloc = typename datacache::entity_cache<Memory, ipc::data::campaign_container>::char_allocator >
class CampaignCache {
        using Cache = datacache::entity_cache<Memory, ipc::data::campaign_container> ; 
        using Keys = vanilla::tagged_tuple< 
            typename ipc::data::campaign_entity<Alloc>::campaign_id_tag,   uint32_t
        >;
        using CampaignTag = typename ipc::data::campaign_entity<Alloc>::campaign_id_tag;
    public:
        using DataCollection = std::vector<std::shared_ptr <CampaignBudget> >;
        CampaignCache(const Config &config):
            config{config}, cache(config.data().ipc_name)
        {}
        
        bool retrieve(DataCollection &data, uint32_t campaign_id) {
            bool result = false;
            auto sp = std::make_shared<std::stringstream>();
            {
                perf_timer<std::stringstream> timer(sp, "campaign_id");
                result = cache.template retrieve<CampaignTag>(data, campaign_id);
            }
            LOG(debug) << sp->str();
            return result;
        }
        bool retrieve(DataCollection &data) {
            bool result = false;
            auto sp = std::make_shared<std::stringstream>();
            {
                perf_timer<std::stringstream> timer(sp, "all_compaign_ids");
                result = cache.template retrieve(data);
            }
            LOG(debug) << sp->str();
            return result;
        }
        bool insert(const CampaignBudget &budget, uint32_t campaign_id) {
            return cache.insert(Keys{ campaign_id }, budget);
        }
        bool update(const CampaignBudget &budget, uint32_t campaign_id) {
            return cache.template update<CampaignTag>(Keys{ campaign_id }, budget, campaign_id);
        }
        bool update(const CampaignBudget &budget, uint32_t old_campaign_id, uint32_t new_campaign_id) {
            return cache.template update<CampaignTag>(Keys{ new_campaign_id }, budget, old_campaign_id);
        }
        bool remove(uint32_t campaign_id) {
            cache.template remove<CampaignTag>(campaign_id);
            return true;
        }
    private:
        const Config &config;
        Cache cache;
};



#endif	/* CAMPAIGN_CACHE_HPP */

