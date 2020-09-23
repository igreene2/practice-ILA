#ifndef MAX_ILA_H__
#define MAX_ILA_H__

#include <ilang/ilang++.h>
#include <string>
#include <ilang/util/log.h>


namespace ilang {
namespace max {


Ila GetMaxIla(const std::string& model_name = "max");


}; // namespace max

}; // namespace ilang
#endif // MAX_TOP_H__