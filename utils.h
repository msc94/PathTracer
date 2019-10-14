#pragma once

#include <future>

namespace utils {
    float randomFloat(float low = 0.0f, float high = 1.0f);

    template<typename R>
    bool futureReady(std::future<R> const &f) {
        return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }
}
