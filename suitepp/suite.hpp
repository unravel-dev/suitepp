#ifndef SUITEPP_HPP
#define SUITEPP_HPP

#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <memory>
#include <egt_hpp/type_traits.hpp>
#include <egt_hpp/utility.hpp>
#include <egt_hpp/concepts.hpp>

#define SUITEPP_STR(s) #s
#define SUITEPP_JOIN(str, num) str##num
#define SUITEPP_GLUE(str, num) SUITEPP_JOIN(str, num)
#define SUITEPP_LINE(str) SUITEPP_GLUE(str, __LINE__)

#ifdef _MSC_VER
#define SUITEPP_DO_PRAGMA(x) __pragma(x)
#define SUITEPP_PRAGMA(compiler, x) SUITEPP_DO_PRAGMA(warning(x))
#else
#define SUITEPP_DO_PRAGMA(x) _Pragma(#x)
#define SUITEPP_PRAGMA(compiler, x) SUITEPP_DO_PRAGMA(compiler diagnostic x)
#endif
#if defined(__clang__)
#define SUITEPP_PUSH_PRAGMA SUITEPP_PRAGMA(clang, push)
#define SUITEPP_DISABLE_WARNING(gcc_unused, clang_option, msvc_unused)                                       \
    SUITEPP_PRAGMA(clang, ignored clang_option)
#define SUITEPP_POP_PRAGMA SUITEPP_PRAGMA(clang, pop)
#elif defined(_MSC_VER)
#define SUITEPP_PUSH_PRAGMA SUITEPP_PRAGMA(msvc, push)
#define SUITEPP_DISABLE_WARNING(gcc_unused, clang_unused, msvc_errorcode)                                    \
    SUITEPP_PRAGMA(msvc, disable : msvc_errorcode)
#define SUITEPP_POP_PRAGMA SUITEPP_PRAGMA(msvc, pop)
#elif defined(__GNUC__)
#define SUITEPP_PUSH_PRAGMA SUITEPP_PRAGMA(GCC, push)
#define SUITEPP_DISABLE_WARNING(gcc_option, clang_unused, msvc_unused) SUITEPP_PRAGMA(GCC, ignored gcc_option)
#define SUITEPP_POP_PRAGMA SUITEPP_PRAGMA(GCC, pop)
#endif

#define SUITEPP_GLOBAL static auto SUITEPP_LINE(sstsuite) =

#define SUITEPP_DECOMPOSE(expr) suite::result(suite::expression_decomposer() << expr)

#define SUITEPP_EXPECT(expr)                                                                                 \
    suite::check(#expr, __FILE__, __LINE__, [&]() {                                                          \
        SUITEPP_PUSH_PRAGMA                                                                                  \
        SUITEPP_DISABLE_WARNING("-Wparentheses", "-Wparentheses", 4554)                                      \
        return SUITEPP_DECOMPOSE(expr);                                                                      \
        SUITEPP_POP_PRAGMA                                                                                   \
    })

#define SUITEPP_EXPECT_THROWS(expr)                                                                          \
    suite::check(#expr, __FILE__, __LINE__, [&]() {                                                          \
        SUITEPP_PUSH_PRAGMA                                                                                  \
        SUITEPP_DISABLE_WARNING("-Wparentheses", "-Wparentheses", 4554)                                      \
        try                                                                                                  \
        {                                                                                                    \
            expr;                                                                                            \
        }                                                                                                    \
        catch(...)                                                                                           \
        {                                                                                                    \
            return suite::result{true, #expr};                                                               \
        }                                                                                                    \
        return suite::result{false, #expr " expected to throw but did not."};                                \
        SUITEPP_POP_PRAGMA                                                                                   \
    })

#define SUITEPP_EXPECT_THROWS_AS(expr, exception_type)                                                       \
    suite::check(#expr, __FILE__, __LINE__, [&]() {                                                          \
        SUITEPP_PUSH_PRAGMA                                                                                  \
        SUITEPP_DISABLE_WARNING("-Wparentheses", "-Wparentheses", 4554)                                      \
        try                                                                                                  \
        {                                                                                                    \
            expr;                                                                                            \
        }                                                                                                    \
        catch(const exception_type&)                                                                         \
        {                                                                                                    \
            return suite::result{true, #expr};                                                               \
        }                                                                                                    \
        catch(const std::exception& e)                                                                       \
        {                                                                                                    \
            return suite::result{false, std::string(#expr " expected to throw " #exception_type              \
                                                          " but threw \"") + e.what() + "\"" };              \
        }                                                                                                    \
        catch(...)                                                                                           \
        {                                                                                                    \
            return suite::result{false, #expr " expected to throw " #exception_type                          \
                                        " but threw something else."};                                       \
        }                                                                                                    \
        return suite::result{false, #expr " expected to throw " #exception_type " but did NOT."};            \
        SUITEPP_POP_PRAGMA                                                                                   \
    })

#define SUITEPP_EXPECT_NOTHROWS(expr)                                                                        \
    suite::check(#expr, __FILE__, __LINE__, [&]() {                                                          \
        SUITEPP_PUSH_PRAGMA                                                                                  \
        SUITEPP_DISABLE_WARNING("-Wparentheses", "-Wparentheses", 4554)                                      \
        try                                                                                                  \
        {                                                                                                    \
            expr;                                                                                            \
        }                                                                                                    \
        catch(const std::exception& e)                                                                       \
        {                                                                                                    \
            return suite::result{false, std::string(#expr " expected NOT to throw but did : ") + e.what()};  \
        }                                                                                                    \
        catch(...)                                                                                           \
        {                                                                                                    \
            return suite::result{false, #expr " expected NOT to throw but did."};                            \
        }                                                                                                    \
        return suite::result{true, #expr};                                                                   \
        SUITEPP_POP_PRAGMA                                                                                   \
    })

#define EXPECT(expr) SUITEPP_EXPECT(expr).run()
#define EXPECT_THROWS(expr) SUITEPP_EXPECT_THROWS(expr).run()
#define EXPECT_THROWS_AS(expr, exception_type) SUITEPP_EXPECT_THROWS_AS(expr, exception_type).run()
#define EXPECT_NOTHROWS(expr) SUITEPP_EXPECT_NOTHROWS(expr).run()
#define TEST_CASE(name) suite::scenario("", name, __FILE__, __LINE__).exit_on_fail(false).mutate() = [&]

// BDD-Style Syntax

#define REQUIRE(expr) SUITEPP_EXPECT(expr).skip_report_on_success(true).exit_on_fail(true).run()
#define REQUIRE_THROWS(expr) SUITEPP_EXPECT_THROWS(expr).skip_report_on_success(true).exit_on_fail(true).run()
#define REQUIRE_THROWS_AS(expr, exception_type) SUITEPP_EXPECT_THROWS_AS(expr, exception_type).skip_report_on_success(true).exit_on_fail(true).run()
#define REQUIRE_NOTHROWS(expr) SUITEPP_EXPECT_NOTHROWS(expr).skip_report_on_success(true).exit_on_fail(true).run()
#define SCENARIO(name) suite::scenario("Scenario: ", name, __FILE__, __LINE__).exit_on_fail(true).mutate() = [&]
#define SCENARIO_CASE(name) suite::scenario_group("Case: ", name, __FILE__, __LINE__).mutate() = [&]
#define TEST_GROUP(name) suite::scenario_group("Group: ", name, __FILE__, __LINE__).mutate() = [&]
#define GIVEN(name) suite::scenario_segment("Given ", name, __FILE__, __LINE__).mutate() = [&]
#define AND_GIVEN(name) suite::scenario_segment("And given ", name, __FILE__, __LINE__).mutate() = [&]
#define WHEN(name) suite::scenario_segment("When ", name, __FILE__, __LINE__).mutate() = [&]
#define AND_WHEN(name) suite::scenario_segment("And when ", name, __FILE__, __LINE__).mutate() = [&]
#define THEN(name) suite::scenario_segment("Then ", name, __FILE__, __LINE__).mutate() = [&]
#define AND_THEN(name) suite::scenario_segment("And then ", name, __FILE__, __LINE__).mutate() = [&]

namespace suite
{

using timer = std::chrono::steady_clock;


// 1 - detecting if std::to_string is valid on T
template <typename T>
using std_to_string_expression = decltype(std::to_string(std::declval<T>()));
template <typename T>
using has_std_to_string = hpp::is_detected<std_to_string_expression, T>;

// 2 - detecting if to_string is valid on T
template <typename T>
using to_string_expression = decltype(to_string(std::declval<T>()));
template <typename T>
using has_to_string = hpp::is_detected<to_string_expression, T>;

// 3 - detecting if T can be sent to an ostringstream
template <typename T>
using ostringstream_expression = decltype(std::declval<std::ostringstream&>() << std::declval<T>());
template <typename T>
using has_ostringstream = hpp::is_detected<ostringstream_expression, T>;


template <typename T, typename std::enable_if<traits::is_range<T>::value, int>::type = 0>
std::string make_string_default(const T& p);

template <typename T, typename std::enable_if<!traits::is_range<T>::value, int>::type = 0>
std::string make_string_default(const T&);

// 1 - std::to_string is valid on T
template <typename T, typename std::enable_if<has_std_to_string<T>::value, int>::type = 0>
inline std::string make_string(const T& t)
{
    return std::to_string(t);
}

// 2 - std::to_string is not valid on T, but to_string is
template <typename T,
          typename std::enable_if<!has_std_to_string<T>::value &&
                                      has_to_string<T>::value, int>::type = 0>
inline std::string make_string(const T& t)
{
    return to_string(t);
}

// 3 - neither std::string nor to_string work on T, let's stream it then
template <typename T, typename std::enable_if<!has_std_to_string<T>::value &&
                                                  !has_to_string<T>::value &&
                                                  has_ostringstream<T>::value,
                                              int>::type = 0>
inline std::string make_string(const T& t)
{
    std::ostringstream oss;
    oss << t;
    return oss.str();
}
// 4 - nothing available
template <typename T, typename std::enable_if<!has_std_to_string<T>::value &&
                                                  !has_to_string<T>::value &&
                                                  !has_ostringstream<T>::value,
                                              int>::type = 0>
inline std::string make_string(const T& t)
{
    return make_string_default(t);
}

template <typename Rep, typename Period>
inline std::string make_string(const std::chrono::duration<Rep, Period>& t)
{
    std::stringstream ss;
    return (ss << std::boolalpha << t.count()) ? ss.str() : std::string("??");
}

inline std::string make_string(bool const& b)
{
    return b ? "true" : "false";
}

inline std::string make_string(std::nullptr_t const&)
{
    return "nullptr";
}

template <typename T>
inline std::string make_string(const std::shared_ptr<T>& p)
{
    return make_string(p.get());
}

template <typename T, typename U>
inline std::string make_string(const std::pair<T, U>& p)
{
    return "{" + make_string(p.first) + ", " + make_string(p.second) + "}";
}

template <typename... Types>
inline std::string make_string(const std::tuple<Types...>& p)
{
    std::string s = "{";
    hpp::for_each(p, [&s](const auto& el) {
        s += make_string(el);
        s += ", ";
    });
    if(s.back() != '{')
    {
        s.pop_back();
        s.pop_back();
    }
    s += "}";
    return s;
}

template <typename T, typename std::enable_if<traits::is_range<T>::value, int>::type>
inline std::string make_string_default(const T& p)
{
    std::string s = "[";
    for(const auto& el : p) {
        s += make_string(el);
        s += ", ";
    };
    if(s.back() != '[')
    {
        s.pop_back();
        s.pop_back();
    }
    s += "]";
    return s;
}

template <typename T, typename std::enable_if<!traits::is_range<T>::value, int>::type>
inline std::string make_string_default(const T&)
{
    return "??";
}

namespace helper
{

template <typename T>
auto to_string(const T& v) -> std::string
{
    return make_string(v);
}
template <typename L, typename R>
auto to_string(const L& lhs, const std::string& op, const R& rhs) -> std::string
{
    std::ostringstream os;
    os << make_string(lhs) << " " << op << " " << make_string(rhs);
    return os.str();
}
}

struct result
{
    result() = default;
    result(bool p, const std::string& dec)
        : passed(p)
        , decomposition(dec)
    {
    }
    bool passed = false;
    std::string decomposition;
};

template <typename L>
struct expression_lhs
{
    static result make_result(bool passed, const std::string& decomp)
    {
        result r;
        r.passed = passed;
        r.decomposition = decomp;
        return r;
    }

    const L lhs;

    expression_lhs(L lh)
        : lhs(lh)
    {
    }

    operator result() const
    {
        return make_result(!!lhs, helper::to_string(lhs));
    }

#define DECOMPOSE_OP(OP)                                                                                     \
    template <typename R>                                                                                    \
    result operator OP(R const& rhs)                                                                         \
    {                                                                                                        \
        return make_result(lhs OP rhs, helper::to_string(lhs, #OP, rhs));                                            \
    }

    DECOMPOSE_OP(<)
    DECOMPOSE_OP(<=)
    DECOMPOSE_OP(>)
    DECOMPOSE_OP(>=)
    DECOMPOSE_OP(!=)
    DECOMPOSE_OP(==)
    DECOMPOSE_OP(&&)
    DECOMPOSE_OP(||)

#undef DECOMPOSE_OP
};

struct expression_decomposer
{
    template <typename L>
    expression_lhs<L const&> operator<<(L const& operand)
    {
        return expression_lhs<L const&>(operand);
    }
};

inline std::string get_file_name(const std::string& s)
{
   char sep = '/';

   size_t i = s.rfind(sep, s.length());
   if (i != std::string::npos)
   {
      return(s.substr(i+1, s.length() - i));
   }

   return s;
}

enum test_status
{
    failed,
    passed,
    scenariono,
    testno
};

inline std::atomic<uint32_t>& get(int i)
{
    static std::atomic<uint32_t> var[testno + 1];
    return var[i];
}

inline size_t& current_indent()
{
    static size_t indent{};
    return indent;
}

inline auto insert_indent( std::ostream& os, const size_t indent, const char* indent_str ) -> std::ostream&
{
    for( size_t i = 0; i < indent; ++i )
    {
        os << indent_str;
    }
    return os;
}

inline auto insert_indent( std::ostream& os, const char* indent ) -> std::ostream&
{
    return insert_indent( os, current_indent(), indent );
}

inline auto insert_indent( std::ostream& os ) -> std::ostream&
{
    return insert_indent( os, "| " );
}

inline auto insert_separator( std::ostream& os, const size_t indent ) -> std::ostream&
{
    return insert_indent( os, indent, "-" );
}

struct summary_reporter
{
    ~summary_reporter()
    {
        uint32_t checks_failed = get(failed);
        uint32_t checks_passed = get(passed);
        uint32_t checks_count = get(testno);
        uint32_t scenarios_count = get(scenariono);
        std::string run = helper::to_string(checks_count);
        std::string res = checks_failed ? "[FAIL]  " : "[ OK ]  ";
        std::string summary;

        if(scenarios_count > 0)
        {
            summary += helper::to_string(get(test_status::scenariono)) + " scenarios run.\n";
        }
        if( checks_failed )
        {
            summary += res + helper::to_string(checks_failed) + '/' + run + " checks failed :(";
        }
        else
        {
            summary += res + helper::to_string(checks_passed) + '/' + run + " checks passed :)";
        }

        std::cout << ss.str();

        const std::string summary_label = " TEST SUMMARY ";
        const auto offset = (summary.size() - summary_label.size()) / 2;
        std::cout << "\n";
        insert_separator(std::cout, summary.size() ) << "\n";
        insert_separator(std::cout, offset) << summary_label;
        insert_separator(std::cout, offset) << "\n";
        insert_separator(std::cout, summary.size()) << "\n" << summary << "\n";
        insert_separator(std::cout, summary.size()) << std::endl;

        if( checks_failed )
        {
            std::exit( int(checks_failed) );
        }
    }

    std::stringstream ss;
};

inline summary_reporter& get_summary()
{
    static summary_reporter reporter;
    return reporter;
}

struct entry
{
    using duration_t = std::chrono::duration<double, std::milli>;

    entry( const std::string& text,
           const char* const file,
           int32_t line )
        : text_( text )
        , file_( file )
        , line_( line )
    {
        get_summary();
        set_label( text_ );
    }

    virtual ~entry() = default;

    entry& set_label( const std::string& name )
    {
        label_ = name;
        return *this;
    }

    entry& repeat( int32_t iterations, bool break_on_fail = false )
    {
        iterations_ = iterations;
        break_on_fail_ = break_on_fail;
        return *this;
    }

    entry& exit_on_fail( bool exit )
    {
        exit_on_fail_ = exit;
        return *this;
    }

    entry& skip_remaining_on_fail( bool skip )
    {
        skip_remaining_on_fail_ = skip;
        return *this;
    }

    entry& skip_report_on_success( bool skip )
    {
        skip_report_on_success_ = skip;
        return *this;
    }


    entry& operator=(const std::function<void()>& func)
    {
        run(func);
        return *this;
    }

    entry& mutate() { return *this; }

    virtual bool run(const std::function<void()>& func = {}) { return true; };

    duration_t duration_;
    std::string text_;
    std::string label_;
    std::string file_;
    int32_t iterations_ = 1;
    int32_t line_ = 0;
    bool break_on_fail_{false};
    bool exit_on_fail_{false};
    bool skip_remaining_on_fail_{false};
    bool skip_report_on_success_{false};
};

inline bool& skip_remaining_cases()
{
    static bool skip{};
    return skip;
}


inline auto& scenario_stack()
{
    static std::vector<std::string> stack {};
    return stack;
}

struct stop_test_exception : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class check : public entry
{
public:
    check( const std::string& text,
           const char* const file,
           int32_t line,
           std::function<result()> f )
        : entry( text, file, line )
        , result_getter_( std::move(f) )
    {}

    bool run(const std::function<void()>& func = {}) override
    {
        if( text_.empty() || result_getter_ == nullptr || skip_remaining_cases() )
        {
            return false;
        }

        std::stringstream output_stream;

        bool failed = false;
        for(int i = 0; i < iterations_; ++i)
        {
            get(test_status::testno)++;

            auto start = timer::now();
            auto result = result_getter_();
            auto end = timer::now();
            duration_ = std::chrono::duration_cast<duration_t>(end - start);

            bool ok = result.passed;

            get(ok ? test_status::passed : test_status::failed)++;

            if( ok )
            {
                insert_indent( output_stream );
            }
            else
            {
                insert_indent( output_stream, "--" );
            }

            output_stream << (ok ? " [ OK ] " : " [FAIL] ");
            if(iterations_ > 1)
            {
                output_stream << label_ << " (" << i << ") ";
            }
            else
            {
                output_stream << label_ << " ";
            }
            output_stream << "(" << helper::to_string(duration_.count()) << "ms) ";

            if(!ok)
            {
                output_stream << "at " << get_file_name(file_) << "(" << line_ << ")\n";
                insert_indent( output_stream, "--" );
                output_stream << "       " << result.decomposition;
            }

            if(!ok)
            {
                failed = true;

                if(skip_remaining_on_fail_)
                {
                    skip_remaining_cases() = true;
                    break;
                }

                if(break_on_fail_)
                {
                    break;
                }
            }
        }

        if( failed || !skip_report_on_success_ )
        {
            get_summary().ss << output_stream.str() << "\n";
        }

        if(failed && exit_on_fail_)
        {
            if(!scenario_stack().empty())
            {
                throw stop_test_exception(label_);
            }

            std::exit(1);
        }

        return true;
    }


private:
    std::function<result()> result_getter_;
    result result_;
};

inline std::string& get_test_label_matcher()
{
    static std::string label;
    return label;
}

inline bool match_label( const std::string& label_ )
{
    const auto& label_matcher = get_test_label_matcher();
    if( label_matcher.empty() )
    {
        return true;
    }

    const auto label_matcher_size = label_matcher.size();
    const auto label_size = label_.size();
    if( label_matcher_size > 2 && label_matcher.front() == '*' &&
        label_matcher.back() == '*' ) // Match it as infix.
    {
        return label_.find(label_matcher.c_str() + 1, 0, label_matcher_size - 2) != std::string::npos;
    }

    if( label_matcher.front() == '*' ) // Match it as suffix.
    {
        return label_size > label_matcher_size &&
               label_.find(label_matcher.c_str() + 1, label_size - label_matcher_size + 1) != std::string::npos;
    }

    if( label_matcher.back() == '*' ) // Match it as prefix.
    {
        return label_.rfind(label_matcher.c_str(), 0, label_matcher_size - 1) == 0;
    }

    // Exact match.
    return label_matcher == label_;
}

struct basic_test : public entry
{    
    basic_test( const std::string& text,
                const char* const file,
                int32_t line )
        : entry( text, file, line )
    {}


    bool run(const std::function<void()>& test) override
    {
        if( !test )
        {
            return true;
        }

        uint32_t fails_before = get(failed);
        test();
        uint32_t fails_after = get(failed);
        return fails_before == fails_after;
    }
};

inline auto get_scenario_label()
{
    std::string label {};
    for( const auto& str : scenario_stack() )
    {
        label.append( str ).append( " " );
    }

    if(!label.empty())
    {
        label.pop_back();
    }

    return label;
}

struct scenario : public basic_test
{
    scenario( const std::string& header,
              const std::string& text,
              const char* const file,
              int32_t line
              )
        : basic_test( std::string(header).append(text), file, line )
    {
        get_summary().ss.str("");
        scenario_stack().emplace_back( text );
    }

    ~scenario() override
    {
        if( !scenario_stack().empty() )
        {
            scenario_stack().pop_back();
        }
    }

    bool run(const std::function<void()>& test) override
    {
        skip_remaining_cases() = {};
        label_ = get_scenario_label();

        if( label_.empty() )
        {
            label_ = "Scenario";
        }

        get(test_status::scenariono)++;
        std::string scenarion_num = " [" + std::to_string(get(test_status::scenariono)) + "]";
        auto label = label_;
        label_ += scenarion_num;
        if( !match_label( label ) )
        {
            return false;
        }

        if( !test )
        {
            return false;
        }

        std::cout << insert_indent;
        insert_separator( std::cout, label_.size() ) << "\n" << insert_indent << label_ << "\n" << insert_indent;
        insert_separator( std::cout, label_.size() ) << std::endl;

        bool result = false;
        try
        {

            result = basic_test::run(test);

            get_summary().ss << insert_indent;
            insert_separator(get_summary().ss, label_.size()) << "\n" << insert_indent << std::endl;
        }
        catch (const stop_test_exception&)
        {
            result = false;

            if(exit_on_fail_)
            {
                std::exit(1);
            }
        }

        if( !skip_report_on_success_ )
        {
            std::cout << get_summary().ss.str() << std::flush;
        }
        get_summary().ss.str("");

        return result;
    }
};

struct scenario_group : public basic_test
{
    scenario_group( const std::string& header,
              const std::string& text,
              const char* const file,
              int32_t line )
        : basic_test( std::string(header).append(text), file, line )
    {
        scenario_stack().emplace_back( text );
    }

    ~scenario_group() override
    {
        if( !scenario_stack().empty() )
        {
            scenario_stack().pop_back();
        }
    }

    bool run(const std::function<void()>& test) override
    {
        bool result = false;
        try
        {
            result = basic_test::run(test);
        }
        catch (const stop_test_exception&/* e*/)
        {
            result = false;
            if(exit_on_fail_)
            {
                std::exit(1);
            }
        }

        return result;
    }
};

struct scenario_segment : public basic_test
{
    scenario_segment( const std::string& header,
                      const std::string& text,
                      const char* const file,
                      int32_t line )
        : basic_test( std::string(header).append(text), file, line )
    {
        ++current_indent();
    }

    bool run(const std::function<void()>& test) override// noexcept(false)
    {
        if( skip_remaining_cases() || !test )
        {
            --current_indent();
            return false;
        }

        if( label_.empty() )
        {
            label_ = "Case";
        }

        match_label( label_ );

        get_summary().ss << insert_indent << label_ << std::endl;
        //std::cout << insert_indent << label_ << std::endl;

        try
        {
            basic_test::run(test);
            --current_indent();
        }
        catch (const stop_test_exception& e)
        {
            --current_indent();

            if(!scenario_stack().empty())
            {
                throw e;
            }
            else
            {
                std::exit(1);
            }
        }

        return true;
    }
};

}

#endif
