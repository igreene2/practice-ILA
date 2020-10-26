#ifndef UNINTERPRETED_FUNC_H__
#define UNINTERPRETED_FUNC_H__

#include <ilang/ilang++.h>
#include <vector>

namespace ilang {
namespace max {


static auto signed_type = SortRef::BV(16);
static auto comp1 = SortRef::BV(16);
static auto comp2 = SortRef::BV(16);

static FuncRef Greatest("Greatest", signed_type, comp1, comp2);


} // namespace ilang
} // namespace max

#endif