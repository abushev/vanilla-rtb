//
// Created by abushev on 14.10.18.
//

#ifndef VANILLA_RTB_SIMPLE_COUNTER_HPP
#define VANILLA_RTB_SIMPLE_COUNTER_HPP

#include <string>
#include <algorithm>
#include <map>
#include <iostream>
#include <shared_mutex>
#include <mutex>

namespace vanilla {
    template<typename ValueType=uint64_t>
    class SimpleCounter {
    public:
        SimpleCounter(std::string name, std::string comment) :
            name_{std::move(name)},
            comment_{std::move(comment)},
            counter_{}
        {}
        void inc() {
            std::unique_lock <std::shared_mutex> lock(mutex_);

            counter_ += 1;
        }

        void inc(const ValueType &v) {
            std::unique_lock <std::shared_mutex> lock(mutex_);

            counter_ += v;
        }

        friend std::ostream &operator<<(std::ostream &out, const SimpleCounter &c) {
            std::shared_lock <std::shared_mutex> lock(c.mutex_);

            out << "# HELP " << c.name_ << " " << c.comment_ << std::endl;
            out << "# TYPE " << " counter" << std::endl;
            out << c.name_ << " " << c.counter_ << std::endl;

            return out;
        }

    private:
        std::string name_;
        std::string comment_;
        ValueType counter_;

        mutable std::shared_mutex mutex_;
    };
}
#endif //VANILLA_RTB_SIMPLE_COUNTER_HPP
