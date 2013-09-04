#ifndef CPPAD_CG_SECTIONED_INDEX_PATTERN_INCLUDED
#define CPPAD_CG_SECTIONED_INDEX_PATTERN_INCLUDED
/* --------------------------------------------------------------------------
 *  CppADCodeGen: C++ Algorithmic Differentiation with Source Code Generation:
 *    Copyright (C) 2013 Ciengis
 *
 *  CppADCodeGen is distributed under multiple licenses:
 *
 *   - Common Public License Version 1.0 (CPL1), and
 *   - GNU General Public License Version 2 (GPL2).
 *
 * CPL1 terms and conditions can be found in the file "epl-v10.txt", while
 * terms and conditions for the GPL2 can be found in the file "gpl2.txt".
 * ----------------------------------------------------------------------------
 * Author: Joao Leal
 */

namespace CppAD {

    /**
     * Several linear patterns
     */
    class SectionedIndexPattern : public IndexPattern {
    protected:
        /**
         * maps the start of the linear section (first x) to the linear pattern
         */
        std::map<size_t, IndexPattern*> sections_;
    public:

        inline SectionedIndexPattern(const std::map<size_t, IndexPattern*>& sections) :
            sections_(sections) {
        }

        inline const std::map<size_t, IndexPattern*>& getLinearSections() const {
            return sections_;
        }

        inline virtual IndexPatternType getType() const {
            return SECTIONED;
        }

        inline virtual ~SectionedIndexPattern() {
            deleteIndexPatterns(sections_);
        }

        /***********************************************************************
         *                        static methods
         **********************************************************************/

        template<class VectorSizeT>
        static inline std::map<size_t, IndexPattern*> detectLinearSections(const Index& index,
                                                                           const VectorSizeT& indexes,
                                                                           size_t maxCount = 0) {
            assert(indexes.size() > 0);

            long dx = 1;
            long xOffset = 0;

            SmartMapValuePointer<size_t, IndexPattern> linearSections;
            size_t xStart = 0;
            while (xStart != indexes.size()) {
                long dy, b;
                size_t lastLinear;
                if (xStart + 1 == indexes.size()) {
                    dy = 0;
                    b = indexes[xStart];
                    lastLinear = xStart + 1;
                } else {
                    dy = long(indexes[xStart + 1]) - indexes[xStart];
                    b = long(indexes[xStart]) - dy * xStart;
                    lastLinear = indexes.size();
                    for (size_t x = xStart + 2; x < indexes.size(); x++) {
                        if (indexes[x] != dy * x + b) {
                            lastLinear = x;
                            break;
                        }
                    }
                }

                linearSections.m[xStart] = new LinearIndexPattern(index, xOffset, dy, dx, b);
                xStart = lastLinear;

                if (linearSections.m.size() == maxCount && xStart != indexes.size()) {
                    // over the limit -> stop
                    return std::map<size_t, IndexPattern*>(); // empty
                }
            }

            return linearSections.release();
        }

        static inline std::map<size_t, IndexPattern*> detectLinearSections(const Index& index,
                                                                           const std::map<size_t, size_t>& x2y,
                                                                           size_t maxCount = 0) {
            SmartMapValuePointer<size_t, IndexPattern> linearSections;

            std::map<size_t, size_t>::const_iterator pStart = x2y.begin();
            while (pStart != x2y.end()) {
                std::map<size_t, size_t>::const_iterator pNextSection = x2y.end();
                std::map<size_t, size_t>::const_iterator p1 = pStart;
                ++p1;

                long xOffset, dy, dx, b;
                if (p1 == x2y.end()) {
                    xOffset = 0;
                    dy = 0;
                    dx = 1;
                    b = pStart->second;
                    pNextSection = p1;

                } else {
                    long x0 = pStart->first;
                    long y0 = pStart->second;

                    dy = long(p1->second) - y0;
                    if (dy != 0) {
                        dx = long(p1->first) - x0;
                        xOffset = x0 % dx;
                        // y = ((x - offset) / dx) * dy + b
                        b = y0 - ((x0 - xOffset) / dx) * dy;
                    } else {
                        dx = 1;
                        xOffset = 0;
                        b = y0;
                    }

                    for (std::map<size_t, size_t>::const_iterator itp = p1; itp != x2y.end(); ++itp) {
                        size_t x = itp->first;
                        size_t y = itp->second;

                        if (y != ((x - xOffset) / dx) * dy + b) {
                            pNextSection = itp;
                            break;
                        }
                    }
                }

                linearSections.m[pStart->first] = new LinearIndexPattern(index, xOffset, dy, dx, b);

                pStart = pNextSection;

                if (linearSections.m.size() == maxCount && pStart != x2y.end()) {
                    // over the limit -> stop
                    return std::map<size_t, IndexPattern*>(); // empty
                }
            }

            return linearSections.release();
        }

    private:

        static inline void deleteIndexPatterns(std::map<size_t, IndexPattern*>& sections) {
            std::map<size_t, IndexPattern*>::const_iterator it;
            for (it = sections.begin(); it != sections.end(); ++it) {
                delete it->second;
            }
            sections.clear();
        }
    };

}

#endif