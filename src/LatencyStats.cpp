#include "LatencyStats.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>


// ============================================================
// RECORD
// ============================================================

void LatencyStats::record(
    uint64_t nanoseconds
)
{
    latencies.push_back(
        nanoseconds
    );
}


// ============================================================
// MIN
// ============================================================

uint64_t LatencyStats::min() const
{
    if (latencies.empty()) {

        return 0;
    }


    return *std::min_element(
        latencies.begin(),
        latencies.end()
    );
}


// ============================================================
// MAX
// ============================================================

uint64_t LatencyStats::max() const
{
    if (latencies.empty()) {

        return 0;
    }


    return *std::max_element(
        latencies.begin(),
        latencies.end()
    );
}


// ============================================================
// AVERAGE
// ============================================================

double LatencyStats::average() const
{
    if (latencies.empty()) {

        return 0.0;
    }


    uint64_t total =
        std::accumulate(
            latencies.begin(),
            latencies.end(),
            uint64_t{0}
        );


    return static_cast<double>(total)
        / latencies.size();
}


// ============================================================
// PERCENTILE
// ============================================================

uint64_t LatencyStats::percentile(
    double p
) const
{
    if (latencies.empty()) {

        return 0;
    }


    std::vector<uint64_t> sorted =
        latencies;


    std::sort(
        sorted.begin(),
        sorted.end()
    );


    std::size_t index =
        static_cast<std::size_t>(
            p * (sorted.size() - 1)
        );


    return sorted[index];
}


// ============================================================
// SIZE
// ============================================================

std::size_t LatencyStats::size() const
{
    return latencies.size();
}
