#ifndef DNNLIBRARY_HELPER_H
#define DNNLIBRARY_HELPER_H

#include <common/log_helper.h>
#include <dnnlibrary/NeuralNetworksWrapper.h>
#include <numeric>
#include <vector>

template <typename T>
T Product(const std::vector<T> &v) {
    return static_cast<T>(
        accumulate(v.begin(), v.end(), 1, std::multiplies<T>()));
}

using css = const std::string;

// Make a FOREACH macro
#define FE_1(WHAT, X) WHAT(X)
#define FE_2(WHAT, X, ...) WHAT(X) FE_1(WHAT, __VA_ARGS__)
#define FE_3(WHAT, X, ...) WHAT(X) FE_2(WHAT, __VA_ARGS__)
#define FE_4(WHAT, X, ...) WHAT(X) FE_3(WHAT, __VA_ARGS__)
#define FE_5(WHAT, X, ...) WHAT(X) FE_4(WHAT, __VA_ARGS__)
#define FE_6(WHAT, X, ...) WHAT(X) FE_5(WHAT, __VA_ARGS__)
#define FE_7(WHAT, X, ...) WHAT(X) FE_6(WHAT, __VA_ARGS__)
#define FE_8(WHAT, X, ...) WHAT(X) FE_7(WHAT, __VA_ARGS__)
#define FE_9(WHAT, X, ...) WHAT(X) FE_8(WHAT, __VA_ARGS__)
//... repeat as needed

#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, NAME, ...) NAME
#define FOR_EACH(action, ...)                                              \
    GET_MACRO(__VA_ARGS__, FE_9, FE_8, FE_7, FE_6, FE_5, FE_4, FE_3, FE_2, \
              FE_1)                                                        \
    (action, __VA_ARGS__)

#define FORZS(var, end, step) \
    for (auto var = decltype(end){0}; var < end; var += (step))

#define FORZ(var, end) for (auto var = decltype(end){0}; var < end; var++)

#define FOR(var, start, end) \
    for (auto var = decltype(end){start}; var < end; var++)

#define STR(a) #a
#define XSTR(a) STR(a)

#define PNT_STR(s) << s << " "
#define PNT_VAR(var) << XSTR(var) << " = " << (var) << ", "
#define PNT_TO(stream, ...) stream FOR_EACH(PNT_VAR, __VA_ARGS__);
#define PNT(...) PNT_TO(LOG(INFO), __VA_ARGS__)

#define DNN_ASSERT(condition, ...)                \
    if (!(condition)) {                           \
        std::stringstream ss;                     \
        ss << std::string(XSTR(condition))        \
           << std::string(" is not satisfied! ")  \
                  FOR_EACH(PNT_STR, __VA_ARGS__); \
        LOG(INFO) << ss.str();                    \
        throw std::runtime_error(ss.str());       \
    }

#define DNN_ASSERT_EQ(actual, expected)                                \
    DNN_ASSERT((actual) == (expected), XSTR(actual), "=", actual,      \
               ", the expected value is", XSTR(expected), "(which is", \
               expected, ")")

#define THROW_ON_ERROR(val)                                                   \
    {                                                                         \
        const auto ret = (val);                                               \
        if (ret != ANEURALNETWORKS_NO_ERROR) {                                \
            throw std::invalid_argument(                                      \
                std::string("Error in ") + __FILE__ + std::string(":") +      \
                std::to_string(__LINE__) + std::string(", function name: ") + \
                std::string(__func__) + "error, ret: " + GetErrorCause(ret)); \
        }                                                                     \
    }

#define THROW_ON_ERROR_WITH_NOTE(val, note)                                   \
    {                                                                         \
        const auto ret = (val);                                               \
        if (ret != ANEURALNETWORKS_NO_ERROR) {                                \
            throw std::invalid_argument(                                      \
                std::string("Error in ") + __FILE__ + std::string(":") +      \
                std::to_string(__LINE__) + std::string(", function name: ") + \
                std::string(__func__) + "error, ret: " + GetErrorCause(ret) + \
                std::string(", ") + (note));                                  \
        }                                                                     \
    }

inline std::string GetErrorCause(int errorCode) {
    switch (errorCode) {
        case ANEURALNETWORKS_OUT_OF_MEMORY:
            return "Out of memory";
        case ANEURALNETWORKS_BAD_DATA:
            return "Bad data";
        case ANEURALNETWORKS_BAD_STATE:
            return "Bad state";
        case ANEURALNETWORKS_INCOMPLETE:
            return "Incomplete";
        case ANEURALNETWORKS_UNEXPECTED_NULL:
            return "Unexpected null";
        case ANEURALNETWORKS_OP_FAILED:
            return "Op failed";
        case ANEURALNETWORKS_UNMAPPABLE:
            return "Unmappable";
        case ANEURALNETWORKS_NO_ERROR:
            return "No error";
        default:
            return "Unknown error code";
    }
}

#endif /* DNNLIBRARY_HELPER_H */
