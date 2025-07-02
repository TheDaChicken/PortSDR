//
// Created by TheDaChicken on 7/1/2025.
//

#ifndef PORTSDR_RANGES_H
#define PORTSDR_RANGES_H

#include <string>
#include <utility>

namespace PortSDR
{
    struct Range
    {
        explicit Range(double value = 0)
            : start(value), stop(value), step(0)
        {
        }

        Range(double start, double stop, double step)
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

        MetaRange(double start, double stop, double step)
            : std::vector<Range>(1, Range(start, stop, step))
        {
        }

        /*!
        * Input iterator constructor:
        * Makes boost::assign::list_of work.
        * \param first the beginning iterator
        * \param last the end iterator
        */
        template <typename InputIterator>
        MetaRange(InputIterator first, InputIterator last):
            std::vector<Range>(first, last)
        {
            /* NOP */
        }

        [[nodiscard]] double step() const;
        [[nodiscard]] double min() const;
        [[nodiscard]] double max() const;
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
