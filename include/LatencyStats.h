#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>


class LatencyStats {

private:

    std::vector<uint64_t> latencies;


public:

    // ========================================================
    // RECORD
    // ========================================================

    void record(uint64_t nanoseconds);


    // ========================================================
    // STATISTICS
    // ========================================================

    uint64_t min() const;

    uint64_t max() const;

    double average() const;

    uint64_t percentile(
        double p
    ) const;


    // ========================================================
    // SIZE
    // ========================================================

    std::size_t size() const;
};