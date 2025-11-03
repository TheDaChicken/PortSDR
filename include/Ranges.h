//
// Created by TheDaChicken on 7/1/2025.
//

#ifndef PORTSDR_RANGES_H
#define PORTSDR_RANGES_H

#include <vector>
#include <string>
#include <utility>

namespace PortSDR
{
    struct Range
    {
        explicit Range(const double value = 0)
            : start(value), stop(value), step(0)
        {
        }

        Range(const double start, const double stop, const double step)
            : start(start), stop(stop), step(step)
        {
        }

        double start, stop, step;
    };

    /*!
     * A meta-range object holds a list of individual ranges.
     */
    class MetaRange : public std::vector<Range>
    {
    public:
        MetaRange() : std::vector<Range>()
        {
        }

        template <typename InputIterator>
        MetaRange(InputIterator first, InputIterator last):
            std::vector<Range>(first, last)
        {

        }

        MetaRange(const double start, const double stop, const double step)
            : std::vector<Range>(1, Range(start, stop, step))
        {
        }

        [[nodiscard]] double Step() const;
        [[nodiscard]] double Min() const;
        [[nodiscard]] double Max() const;
    };

    struct Gain
    {
        Gain(std::string stage, MetaRange range)
            : stage(std::move(stage)), range(std::move(range))
        {
        };

        Gain() = default;

        std::string stage;
        MetaRange range;
    };
}

#endif //PORTSDR_RANGES_H
