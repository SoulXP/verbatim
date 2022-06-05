// Stefan Olivier
// Description: Type traits and interfaces

#pragma once

// Standard header
#include <concepts>
#include <cstdio>
#include <iostream>
#include <stdint.h>
#include <string>
#include <type_traits>

// Library header
#include <fmt/core.h>
#include <magic_enum.hpp>

// Project header
#include "locale.hpp"

namespace vtm::traits
{
    // @SECTION CONSTRAINTS
 
    template<typename T>
    concept Regular = std::regular<T>;

    template<typename T>
    concept SemiRegular = std::semiregular<T>;

    template<typename T, typename... Args>
    concept AssignableFrom = requires (T t, Args... a) {
        { ((std::assignable_from<T&, Args>),...) };
    };

    template<typename T, typename... Args>
    concept ConvertibleFrom = requires (T t, Args...  a) {
        { ((std::convertible_to<T, Args>),...) };
    };

    template<typename T>
    concept NumericallyRepresentible = ConvertibleFrom<T, unsigned char, signed char,
                                                          unsigned short, signed short,
                                                          unsigned int, signed int,
                                                          unsigned long, signed long,
                                                          unsigned long long, signed long long>
                                    && ConvertibleFrom<T, float, double, long double>
                                    && requires {
                                        std::floating_point<typename T::float_type>;
                                        std::unsigned_integral<typename T::unsigned_type>;
                                        std::signed_integral<typename T::signed_type>;
                                    };

    template<typename T, typename R, typename... Args>
    concept SymmetricallyInvocable = std::invocable<T, Args...>
                                  && requires (T t, Args... args) {
                                     { t(args...) } -> std::same_as<R>;
                                  };

    template<typename T, typename... Args>
    concept SequenceInvocable = requires (T&& t, Args&&... args) {
        { ((std::invocable<T, Args>),...) };
    };

    template<typename T>
    concept DefaultableEnum = magic_enum::is_unscoped_enum_v<T>
                           && requires (T t) {
                                  T::none;
                              };

    template<typename T, typename O>
    concept SameAsReturn = std::same_as<std::invoke_result_t<T>, O>;

template<typename T>
    concept StdStreamable = requires (T t) {
        { std::cout << t };
        { std::cerr << t };
        { std::clog << t };
    };

    template<typename T>
    concept FmtPrintable = requires (T t) {
        { fmt::print("{}", t) };
    };

    template<typename T>
    concept CStdIOPrintable = requires (T t) { { std::printf("%s", t)         }; }
                           || requires (T t) { { std::printf("%s", t.data())  }; }
                           || requires (T t) { { std::printf("%s", t.c_str()) }; };

    template<typename T>
    concept Printable = StdStreamable<T>
                     && FmtPrintable<T>
                     && CStdIOPrintable<T>;

    template<typename T>
    concept StringViewable = std::convertible_to<T, std::string_view>
                          && std::assignable_from<std::string_view&, T>;


    // TODO: constraints for member type aliases on string interfaces
    template<typename T>
    concept StringLike = Regular<T>
                      && Printable<T>
                      && StringViewable<T>
                      && AssignableFrom<T, const char*, std::string, std::string_view>;

    template<typename T>
    concept StringViewLike = Regular<T>
                          && Printable<T>
                          && StringViewable<T>;

    template<typename T>
    concept StringConstructible = AssignableFrom<T, const char*, std::string, std::string_view>
                               && ConvertibleFrom<T, const char*, std::string, std::string_view>;

    template<typename T>
    concept StdStringInterface = std::same_as<T, std::string>
                              || std::same_as<T, std::wstring>
                              || std::same_as<T, std::u8string>
                              || std::same_as<T, std::u16string>
                              || std::same_as<T, std::u32string>
                              || std::same_as<T, std::pmr::string>
                              || std::same_as<T, std::pmr::wstring>
                              || std::same_as<T, std::pmr::u8string>
                              || std::same_as<T, std::pmr::u16string>
                              || std::same_as<T, std::pmr::u32string>
                              || std::same_as<T, std::string_view>
                              || std::same_as<T, std::wstring_view>
                              || std::same_as<T, std::u8string_view>
                              || std::same_as<T, std::u16string_view>
                              || std::same_as<T, std::u32string_view>;

    // @SECTION Type Traits

    template<typename...>
    struct string_char_type_members : std::false_type
    {};

    template<typename T>
    struct string_char_type_members<T, T> : std::true_type
    {
    private:
        template<StdStringInterface U>
        static constexpr auto fn(U*) -> typename U::value_type;

    public:
        using type = decltype(fn<T>(nullptr));
    };

    template<typename T>
    inline constexpr bool string_char_type_v = string_char_type_members<T, T>::value;

    template<typename T>
    using string_char_type_t = typename string_char_type_members<T, T>::type;

    template<typename...>
    struct identical : std::false_type {};

    template<typename T, typename... Rest>
    struct identical<T, Rest...> : std::bool_constant<((std::is_same_v<T, Rest>) && ...)>
    {};

    template<typename... Ts>
    inline constexpr bool identical_v = identical<Ts...>::value;

    template<typename... Ts>
    using identical_t = typename identical<Ts...>::type;

    template<auto... Args>
    struct arg_size
    {
        static constexpr std::size_t value = sizeof...(Args);
    };

    template<auto... Args>
    inline constexpr std::size_t arg_size_v = arg_size<Args...>::value;

    // @SECTION INTERFACES
    // IMPORTANT
    // All interfaces MUST define the member alias type 'type' to ensure
    // consistency of return types across all virtual interfaces

    struct __clear
    {
        using type = void;
        virtual auto clear() noexcept -> type = 0;
    };

    struct __reset
    {
        using type = void;
        virtual auto reset() noexcept -> type = 0;
    };

    template<StringLike T>
    struct __display
    {
        using type = std::remove_cvref_t<T>;
        virtual auto display() const noexcept -> type = 0;
    };

    template<Regular T>
    struct __implicit_type_overload
    {
        using type = std::conditional_t< std::is_pointer_v<T>,
                                         T,
                                         std::remove_cvref_t<T> >;

        virtual operator type() const noexcept = 0;
    };

    template<Regular Arg, Regular Return>
    struct __op_eq_overload
    {
        using arg_type = std::remove_cvref_t<Arg>;
        using return_type = std::remove_cvref_t<Return>;
        virtual return_type operator=(const arg_type&) const = 0;
    };

    template<typename TString, typename TView>
    struct __implicit_string_overloads : __implicit_type_overload<TString>,
                                         __implicit_type_overload<TView>
    {
        using string_type = typename __implicit_type_overload<TString>::type;
        using string_view_type = typename __implicit_type_overload<TView>::type;
    };

    template<NumericallyRepresentible T>
    struct __numerically_representible
    {
        using type = T;
        using float_type = typename type::float_type;
        using unsigned_type = typename type::unsigned_type;
        using signed_type = typename type::float_type;

        virtual auto as_float() const -> float_type = 0;
        virtual auto as_unsigned() const -> unsigned_type = 0;
        virtual auto as_signed() const -> signed_type = 0;

    protected:
        virtual ~__numerically_representible() = default;
    };

    // @SECTION INTERFACE CONTRAINTS

    template<typename T>
    concept InterfaceClear = requires (T t) {
        { t.clear() } noexcept;
    };

    template<typename T>
    concept InterfaceReset = requires (T t) {
        { t.reset() } noexcept;
    };

    template<typename T>
    concept InterfaceDisplay = requires (T t) {
        { t.display() } noexcept -> std::same_as<typename T::display_t>;
    };
}




