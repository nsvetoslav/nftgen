#pragma once
#include "../models/exceptions.hpp"

namespace nftgen
{
namespace typedexceptions
{
class IExceptionProcessor
{
public:
    virtual bool MeetsGenerationConditions() = 0;
    virtual bool ContinueToNextDirectory() const = 0;
    virtual bool ContinueToNextTrait() const = 0;
};
}  // namespace typedexceptions
}  // namespace nftgen
