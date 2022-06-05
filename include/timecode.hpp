// Copyright (C) Stefan Olivier
// <https://stefanolivier.com>
// ----------------------------
// Description: Timecode configuration and constants

#pragma once

// Standard headers
#include <algorithm>
#include <array>
#include <cmath>
#include <compare>
#include <concepts>
#include <limits>
#include <ranges>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

// Project headers
#include "macros.hpp"
#include "traits.hpp"
#include "utility.hpp"

#ifndef VTM_TIMECODE_MACROS
#define VTM_TIMECODE_MACROS

#ifndef VTM_TIMECODE_PRECISION_MARGIN
#define VTM_TIMECODE_PRECISION_MARGIN 0.0001
#endif

#ifndef VTM_TIMECODE_FPS_DEFAULT
#define VTM_TIMECODE_FPS_DEFAULT 25
#endif

#define __fps_type() vtm::chrono::internal::__FPSFormat<vtm::chrono::float64_t, vtm::chrono::int64_t>::type

#define __fpsformat_value_to_string(in) get_enum_mapping_1(__fpsformat_string(), in)
#define __fpsformat_string_to_value(in) get_enum_mapping_2(__fpsformat_string(), in)
#define __fpsformat_string() declare_enum_mapping(__fps_type(),                            \
                                                  std::string_view,                        \
                                                  [](){ VTM_WARN("unknown fps format"); }, \
                                                  [](){ VTM_WARN("unknown fps format"); }, \
                                                  __fps_type()::none,                      \
                                                  "NONE",                                  \
                                                  "24 fps",                                \
                                                  "25 fps",                                \
                                                  "30 fps",                                \
                                                  "29.97 fps",                             \
                                                  "29.97 fps drop-frame",                  \
                                                  "60 fps")

#define __fpsformat_value_to_int(in) get_enum_mapping_1(__fpsformat_int(), in)
#define __fpsformat_int_to_value(in) get_enum_mapping_2(__fpsformat_int(), in)
#define __fpsformat_int() declare_enum_mapping(__fps_type(),                            \
                                               vtm::chrono::int64_t,                    \
                                               [](){ VTM_WARN("unknown fps format"); }, \
                                               [](){ VTM_WARN("unknown fps format"); }, \
                                               __fps_type()::none,                      \
                                               0,                                       \
                                               24,                                      \
                                               25,                                      \
                                               30,                                      \
                                               29,                                      \
                                               29,                                      \
                                               60)

#define __fpsformat_value_to_float(in) get_enum_mapping_1(__fpsformat_float(), in)
#define __fpsformat_float_to_value(in) get_enum_mapping_2(__fpsformat_float(), in)
#define __fpsformat_float() declare_enum_mapping(__fps_type(),                            \
                                                 vtm::chrono::float64_t,                  \
                                                 [](){ VTM_WARN("unknown fps format"); }, \
                                                 [](){ VTM_WARN("unknown fps format"); }, \
                                                 __fps_type()::none,                      \
                                                 0.0,                                     \
                                                 24.0,                                    \
                                                 25.0,                                    \
                                                 30.0,                                    \
                                                 29.97,                                   \
                                                 29.97,                                   \
                                                 60.0)

#define __fpsformat_value_to_dropframe(in) get_enum_mapping_1(__fpsformat_dropframe(), in)
#define __fpsformat_dropframe_to_value(in) get_enum_mapping_2(__fpsformat_dropframe(), in)
#define __fpsformat_dropframe() declare_enum_mapping(__fps_type(),                         \
                                                  bool,                                    \
                                                  [](){ VTM_WARN("unknown fps format"); }, \
                                                  [](){ VTM_WARN("unknown fps format"); }, \
                                                  __fps_type()::none,                      \
                                                  false,                                   \
                                                  false,                                   \
                                                  false,                                   \
                                                  false,                                   \
                                                  false,                                   \
                                                  true,                                    \
                                                  false)

#endif

namespace vtm::chrono
{
    // @SECTION: Chrono Types
    using float64_t = long double;
    using int64_t = long long;

    namespace internal
    {
        // @SECTION: Default FPS Format Template
        template<typename T>
        concept FpsFormatFactory = std::is_enum_v<typename T::format>
                                && std::is_same_v<typename T::type, typename T::format>
                                && requires (T t, typename T::type type) {
                                       std::floating_point<typename T::float_type>;
                                       std::integral<typename T::int_type>;
                                       { T::from_int(0) } -> std::same_as<typename T::type>;
                                       { T::from_float(0.0) } -> std::same_as<typename T::type>;
                                       { T::from_string(std::string_view("")) } -> std::same_as<typename T::type>;
                                       { T::from_string(std::string("")) } -> std::same_as<typename T::type>;
                                       { T::to_int(type) } -> std::integral;
                                       { T::to_float(type) } -> std::floating_point;
                                       { T::to_string(type) } -> vtm::traits::StringLike;
                                   };
        template<std::floating_point TFloat, std::integral TInt>
        struct __FPSFormat
        {
            enum format : TInt {
                fps_24,
                fps_25,
                fps_30,
                fps_29p97,
                fpsdf_29p97,
                fps_60,
                none
            };

            using type = format;
            using float_type = TFloat;
            using int_type = TInt;

            static constexpr auto default_value() -> type
            {
                return __fpsformat_int_to_value(VTM_TIMECODE_FPS_DEFAULT);
            }

            static constexpr auto is_drop_frame(const type& t) -> bool
            {
                return __fpsformat_value_to_dropframe(t);
            }

            static constexpr auto from_int(std::integral auto const i) -> type
            {
                return __fpsformat_int_to_value(i);
            }

            static constexpr auto from_float(std::floating_point auto const f) -> type
            {
                return __fpsformat_float_to_value(f);
            }

            static constexpr auto from_string(vtm::traits::StringLike auto const& s) -> type
            {
                return __fpsformat_string_to_value(s);
            }

            static constexpr std::integral
            auto to_int(const type& t)
            {
                return __fpsformat_value_to_int(t);
            }

            static constexpr std::floating_point
            auto to_float(const type& t)
            {
                return __fpsformat_value_to_float(t);
            }

            static constexpr vtm::traits::StringLike
            auto to_string(const type& t)
            {
                return __fpsformat_value_to_string(t);
            }
        };
    }

    // @SECTION: Timecode Functions
    inline std::floating_point auto
    fps_to_single_tick(const std::floating_point auto fps)
    {
        VTM_ASSERT(fps > 0.0, "Frame rate must be a non-zero floating point value");
        return 1.0 / fps / 100;
    }

    // TODO: integral version of fps_to_single_tick()

    template<std::floating_point T, std::same_as<T>... Args>
    inline auto chunks_to_total_ticks(const std::tuple<Args...>& time, const T fps)
    {
        VTM_ASSERT(fps > 0.0, "Frame rate must be a non-zero floating point value");
        const auto& [ h, m, s, f ] = time;
        T total_frames = (((h * 60 * 60) + (m * 60) + s) * fps) + f;
        T ticks = fps_to_single_tick(fps) * total_frames;
        return ticks;
    }

    // TODO: integral version of chunks_to_total_ticks()
    template<std::floating_point T>
    inline auto fps_to_ticks_by_chunk(const T fps) -> std::array<T, 4>
    {
        VTM_ASSERT(fps > 0.0, "frame rate must be a non-zero floating point value");
        using float_t = T;
        const float_t ht = fps_to_single_tick(fps) * 60.0 * 60.0 * fps;
        const float_t mt = fps_to_single_tick(fps) * 60.0 * fps;
        const float_t st = fps_to_single_tick(fps) * fps;
        const float_t ft = fps_to_single_tick(fps);
        return std::array{ht, mt, st, ft};
    }

    // TODO: integral version of fps_to_ticks_by_chunk()
//
    template<vtm::traits::StringLike S, std::floating_point F>
    inline auto ticks_to_chunk_string(const F n, const F k, bool round = false) -> S
    {
        using float_t = F;
        using string_t = S;
        string_t str = "00";
        float_t d = (round) ? std::round(n / k) : n / k;    

        if (d > n / k) return str;
        str = std::to_string(d);
        str = str.substr(0, str.find_first_of('.', 0));

        return (str.length() == 1) ? ("0" + str) : str;
    }

    // TODO: integral version of ticks_to_chunk_string()

    template<vtm::traits::StringLike S, std::floating_point F>
    auto ticks_to_string(F n, const F fps) -> S
    {
        VTM_ASSERT(fps > 0.0, "frame rate must be a non-zero floating point value");

        using string_t = S;
        using float_t = F;

        const auto& [ht, mt, st, ft] = fps_to_ticks_by_chunk(fps);
        string_t h = ticks_to_chunk_string<string_t>(n, ht); n = std::fmod(n, ht);
        string_t m = ticks_to_chunk_string<string_t>(n, mt); n = std::fmod(n, mt);
        string_t s = ticks_to_chunk_string<string_t>(n, st); n = std::fmod(n, st);
        string_t f = ticks_to_chunk_string<string_t>(n, ft, true);

        return h + ":" + m + ":" + s + ":" + f;
    }

    // TODO: integral version of ticks_to_string()

    template<vtm::traits::StringLike S>
    inline auto is_tcstring_dropframe(const S& tc) -> bool
    {
        for (const auto& c: tc) {
            if (c == ';') return true;
        }

        return false;
    }

    template<vtm::traits::StringLike S>
    inline auto valid_tcstring(const S& tc) -> bool
    {
        namespace rg = std::ranges;
        bool valid = !rg::empty(tc);
        if (!valid) return false;

        std::size_t chunk_count = 0;
        std::array<std::size_t, 4> chunk_sizes{ 0, 2, 2, 2 };
        const std::string_view delim = (is_tcstring_dropframe(tc)) ? ";" : ":";

        for (const auto& ch : rg::split_view(tc, delim)) {
            const std::string_view sv(ch.begin(), ch.end());

            if (chunk_sizes[chunk_count] > 0)
                valid = valid && sv.length() == chunk_sizes[chunk_count];

            for (const auto& d : sv) {
                valid = valid && std::isdigit(d);
                if (!valid) return false;
            }

            ++chunk_count;
        }

        return valid && chunk_count == 4;
    }

    template<vtm::traits::StringLike S, std::floating_point F>
    inline auto tcstring_to_ticks(const S& tc, const F fps) -> F
    {
        namespace rg = std::ranges;
        using string_t = S;
        using float_t = F;

        VTM_ASSERT(valid_tcstring(tc) == true, "invalid timecode string was parsed");

        const std::string_view delim = (is_tcstring_dropframe(tc)) ? ";" : ":";
        const auto coefs = fps_to_ticks_by_chunk(fps);
        float_t ticks = 0.0;

        std::size_t i = 0;
        for (const auto& ch : rg::split_view(tc, delim)) {
            const std::string_view chunk(ch.begin(), ch.end());
            ticks += vtm::string_to_float<float_t>(chunk) * coefs[i++];
        }
        
        return ticks;
    }

    // TODO: integral version of string_to_ticks()
}

namespace vtm::chrono
{
    // @SECTION: Basic Timecode Object
    namespace internal
    {
        template<typename T>
        concept TimecodePrimitive = std::floating_point<T> || std::signed_integral<T>;

        template<vtm::traits::StringLike TString>
        inline constexpr auto basic_timecode_default_display() -> TString
        {
            return "00:00:00:00";
        }

        template<typename T>
        concept BasicTimecodeStringAliases = requires (T t) {
            vtm::traits::StringLike<typename T::string_t>;
            vtm::traits::StringLike<typename T::string_view_t>;
        };

        template<typename T>
        concept BasicTimecodeCompatible = BasicTimecodeStringAliases<T>
                                       && vtm::traits::InterfaceReset<T>
                                       && vtm::traits::InterfaceDisplay<T>
                                       && requires {
                                          typename T::display_t;
                                       };

        template<typename L,  typename R>
        struct paired_members
        {
            template<std::invocable F1, std::invocable F2, std::invocable... Fs>
            static constexpr std::invocable
            auto choose(F1&& f1, F2&& f2, Fs&&... fs)
            {
                if constexpr(std::is_same_v<std::invoke_result_t<F1>, L>
                          && std::is_same_v<std::invoke_result_t<F2>, R>)
                {
                    return [=]() { return std::make_pair(f1(), f2()); };
                }

                else if constexpr(std::is_same_v<std::invoke_result_t<F1>, R>
                               && std::is_same_v<std::invoke_result_t<F2>, L>)
                {
                    return [=]() { return std::make_pair(f2(), f1()); };
                }

                else if constexpr((std::is_same_v<std::invoke_result_t<F1>, L>
                              &&  !std::is_same_v<std::invoke_result_t<F2>, R>)
                              ||  (std::is_same_v<std::invoke_result_t<F1>, R>
                              &&  !std::is_same_v<std::invoke_result_t<F2>, L>))
                {
                    return paired_members<L, R>::choose(std::forward<F1>(f1),
                                                        std::forward<Fs>(fs)...);
                }

                else if constexpr( (!std::is_same_v<std::invoke_result_t<F1>, L>
                                &&   std::is_same_v<std::invoke_result_t<F2>, R>)
                                || (!std::is_same_v<std::invoke_result_t<F1>, R>
                                &&   std::is_same_v<std::invoke_result_t<F2>, L>))
                {
                    return paired_members<L, R>::choose(std::forward<F2>(f2),
                                                        std::forward<Fs>(fs)...);
                }

                else {
                    return paired_members<L, R>::choose(std::forward<Fs>(fs)...);
                }
            }
        };
        
        template<vtm::traits::StringLike TString, vtm::traits::StringLike TView>
        class BasicTimecode : public vtm::traits::__reset,
                              public vtm::traits::__implicit_string_overloads<TString, TView>,
                              public vtm::traits::__display<typename vtm::traits::__implicit_string_overloads<TString, TView>::string_view_type>
        {
        public:
            using string_t      = typename vtm::traits::__implicit_string_overloads<TString, TView>::string_type;
            using string_view_t = typename vtm::traits::__implicit_string_overloads<TString, TView>::string_view_type;
            using display_t     = string_view_t;

        protected:
            BasicTimecode() = default;
            virtual ~BasicTimecode() noexcept = default;
            BasicTimecode(const BasicTimecode&) = delete;
            BasicTimecode(BasicTimecode&&) noexcept = delete;
            BasicTimecode& operator=(const BasicTimecode&) = delete;
            BasicTimecode& operator=(BasicTimecode&&) = delete;
        };

        template<vtm::traits::StringLike TString,
                 vtm::traits::StringLike TView,
                 std::signed_integral TInt,
                 std::floating_point TFloat,
                 FpsFormatFactory TFps,
                 BasicTimecodeCompatible Interface = BasicTimecode<TString, TView>>
        class __BasicTimecode : public Interface
        {
        public:
            using string_t      = typename Interface::string_t;
            using string_view_t = typename Interface::string_view_t;
            using display_t     = typename Interface::display_t;
            using int_t         = std::remove_cvref_t<TInt>;
            using float_t       = std::remove_cvref_t<TFloat>;
            using fps_factory_t = TFps;
            using fps_t         = typename TFps::type;

            // Modes for union values
            enum class mode : int {
                floating,
                integral
            };

            // Static members
            template<std::integral V>
            static auto from_hmsf(const V h,
                                  const V m,
                                  const V s,
                                  const V f,
                                  const fps_t fps = fps_factory_t::default_value()) -> __BasicTimecode
            {
                VTM_ASSERT(h >= 0, "timecode hours must be greater than or equal to zero");
                VTM_ASSERT(m >= 0, "timecode minutes must be greater than or equal zero");
                VTM_ASSERT(s >= 0, "timecode seconds must be greater than or equal zero");
                VTM_ASSERT(f >= 0, "timecode frames must be greater than or equal zero");

                return __BasicTimecode {
                    chunks_to_total_ticks(std::make_tuple(float_t(h), float_t(m), float_t(s), float_t(f)),
                                          __fpsformat_value_to_float(fps)),
                    fps
                };
            }

            template<vtm::traits::StringConstructible S>
            static auto from_string(const S& tc, const fps_t fps = fps_factory_t::default_value()) -> __BasicTimecode
            {
                const char* msg = "cannot create new timecode object with invalid timecode string";

                if constexpr (std::same_as<string_t, S> || std::same_as<string_view_t, S>){
                    VTM_ASSERT(valid_tcstring(tc), msg);
                    return __BasicTimecode { tcstring_to_ticks(tc, __fpsformat_value_to_float(fps)),
                                             fps };
                }

                else {
                    VTM_ASSERT(valid_tcstring(string_t(tc)), msg);
                    return __BasicTimecode { tcstring_to_ticks(string_t(tc), __fpsformat_value_to_float(fps)), 
                                             fps };
                }
            }

            // Virtual methods
            void reset() noexcept
            {
                this->set_value(float_t(0.0));
            }

            auto display() const noexcept -> display_t { return basic_timecode_default_display<display_t>(); }
            operator string_t() const noexcept { return this->as_string(); }
            operator string_view_t() const noexcept { return this->as_string(); }

            __BasicTimecode() = default;
            ~__BasicTimecode() = default;

            __BasicTimecode(const __BasicTimecode& tc)
                : _mode(tc._mode)
                , _fps(tc._fps)
                , _value(tc._value)
            {}

            __BasicTimecode(__BasicTimecode&& tc) noexcept
            {
                this->_mode = tc._mode;
                this->_fps = tc._fps;
                this->_value = tc._value;
                tc._mode = {};
                tc._fps = {};
                tc._value = {};
            }

            __BasicTimecode& operator=(const __BasicTimecode& tc)
            {
                this->_mode = tc._mode;
                this->_fps = tc._fps;
                this->_value = tc._value;
                return *this;
            }

            __BasicTimecode& operator=(__BasicTimecode&& tc) noexcept
            {
                this->_mode = tc._mode;
                this->_fps = tc._fps;
                this->_value = tc._value;
                tc._mode = {};
                tc._fps = {};
                tc._value = {};
                return *this;
            }

            template<TimecodePrimitive V>
            explicit __BasicTimecode(const V value, const fps_t fps = fps_factory_t::default_value()) noexcept
            {
                this->set_value(value);
                this->_fps = fps;
            }

            explicit constexpr __BasicTimecode(const string_view_t& tc)
            {
                
            }

        public:
            // Accessor methods
            // TODO: Frame rate aware set_value()
            void set_value(const __BasicTimecode& value) noexcept
            {
                if (value._mode == mode::floating) {
                    this->_value.f = value._value.f;
                    this->_mode = mode::floating;
                }

                this->_value.i = value._value.i;
                this->_mode = mode::integral;
            }

            template<TimecodePrimitive V>
            void set_value(const V value) noexcept
            {
                if constexpr (std::is_floating_point_v<V>) {
                    this->_value.f = value;
                    this->_mode = mode::floating;
                }

                else {
                    this->_value.i = value;
                    this->_mode = mode::integral;
                }
            }

            void set_fps(const fps_t fps) noexcept
            {
                this->_fps = fps;
                this->_dropframe = fps_factory_t::is_dropframe(fps);
            }

            auto fps() const noexcept -> fps_t
            {
                return this->_fps;
            }

            auto dropframe() const noexcept -> bool
            {
                return this->_dropframe;
            }

            std::signed_integral auto as_signed() const
            {
                int_t tmp = this->_value.i;
                if (this->_mode != mode::integral) {
                    tmp = static_cast<int_t>(std::round(this->_value.f));
                }

                return tmp;
            }

            std::floating_point auto as_float() const
            {
                float_t tmp = this->_value.f;
                if (this->_mode != mode::floating) {
                    tmp = static_cast<float_t>(this->_value.i);
                }

                return tmp;
            }

            auto as_string() const -> string_t
            {
                // TODO: implementation for integral ticks_to_string()
                /* if (this->_mode == mode::integral) { */
                /*     return ticks_to_string<string_t>(this->as_signed(), this->fps()); */
                /* } */

                return ticks_to_string<string_t>(this->as_float(), __fpsformat_value_to_float(this->fps()));
            }

            template<typename L = float_t, typename R = string_t>
            auto as_pair() const -> std::pair<L, R>
            {
                return paired_members<L, R>::choose([this]() { return this->as_signed(); },
                                                    [this]() { return this->as_float();  },
                                                    [this]() { return this->as_string(); },
                                                    [this]() { return this->fps();       })();
            }

            // Assignment & Arithmetic
            // TODO: Frame rate aware assignments and arithmetic
            template<TimecodePrimitive V>
            __BasicTimecode& operator=(const V rhs)
            {
                this->set_value(rhs);
                return *this;
            }

            __BasicTimecode& operator+=(const auto& rhs)
            {
                this->set_value(*this + rhs);
                return *this;
            }

            __BasicTimecode& operator-=(const auto& rhs)
            {
                this->set_value(*this - rhs);
                return *this;
            }

            template<TimecodePrimitive V>
            friend __BasicTimecode operator+(const __BasicTimecode& lhs, const V rhs)
            {
                __BasicTimecode tmp{ lhs };

                if constexpr (std::is_floating_point_v<V>) {
                    tmp.set_value(std::min(std::numeric_limits<float_t>::max(),
                                  tmp._value.f + rhs));
                }

                else {
                    tmp.set_value(std::min(std::numeric_limits<int_t>::max(),
                                  tmp._value.i + rhs));
                }

                return tmp;
            }

            template<TimecodePrimitive V>
            friend V operator+(const V lhs, const __BasicTimecode& rhs)
            {
                V tmp;

                if constexpr (std::is_floating_point_v<V>) {
                    tmp = lhs + rhs._value.f;
                }

                else {
                    tmp = lhs + rhs._value.i;
                }

                return tmp;
            }

            template<TimecodePrimitive V>
            friend __BasicTimecode operator-(const __BasicTimecode& lhs, const V rhs)
            {
                __BasicTimecode tmp{ lhs };

                if constexpr (std::is_floating_point_v<V>) {
                    tmp.set_value(std::max(std::numeric_limits<float_t>::min(),
                                  tmp._value.f - rhs));
                }

                else {
                    tmp.set_value(std::max(std::numeric_limits<int_t>::min(),
                                  tmp._value.i - rhs));
                }

                return tmp;
            }

            template<TimecodePrimitive V>
            friend V operator-(const V lhs, const __BasicTimecode& rhs)
            {
                V tmp;

                if constexpr (std::is_floating_point_v<V>) {
                    tmp = lhs - rhs._value.f;
                }

                else {
                    tmp = lhs - rhs._value.i;
                }

                return tmp;
            }

            friend __BasicTimecode operator+(const __BasicTimecode& lhs, const __BasicTimecode& rhs)
            {
                __BasicTimecode tmp { lhs };

                if (tmp._mode == mode::floating) {
                    tmp.set_value(std::min(std::numeric_limits<float_t>::max(),
                                  tmp._value.f + rhs._value.f));
                }

                tmp.set_value(std::min(std::numeric_limits<int_t>::max(),
                              tmp._value.i + rhs._value.i));

                return tmp;
            }

            friend __BasicTimecode operator-(const __BasicTimecode& lhs, const __BasicTimecode& rhs)
            {
                __BasicTimecode tmp { lhs };

                if (tmp._mode == mode::floating) {
                    tmp.set_value(std::max(std::numeric_limits<float_t>::min(),
                                  tmp._value.f - rhs._value.f));
                }

                tmp.set_value(std::max(std::numeric_limits<int_t>::min(),
                              tmp._value.i - rhs._value.i));

                return tmp;
            }

            // Comparison operators
            bool operator==(const __BasicTimecode& rhs) const
            {
                const auto [ tmp_i, tmp_f ] = this->as_pair<int_t, float_t>();
                const auto [ rhs_tmp_i, rhs_tmp_f ] = rhs.as_pair<int_t, float_t>();

                return tmp_i == rhs_tmp_i && tmp_f == rhs_tmp_f;
            }

            auto operator<=>(const __BasicTimecode& rhs) const
            {
                const auto [ tmp_i, tmp_f ] = this->as_pair<int_t, float_t>();
                const auto [ rhs_tmp_i, rhs_tmp_f ] = rhs.as_pair<int_t, float_t>();

                std::strong_ordering int_ordering = tmp_i <=> rhs_tmp_i;
                std::partial_ordering float_ordering = tmp_f <=> rhs_tmp_f;
                if (int_ordering < 0 && float_ordering < 0) return std::strong_ordering::less;
                if (int_ordering > 0 && float_ordering > 0) return std::strong_ordering::greater;
                return std::strong_ordering::equal;
            }

        private:
            // @SECTION __BasicTimecode Data Members
            mode _mode = mode::floating;
            fps_t _fps = fps_factory_t::default_value();
            bool _dropframe = fps_factory_t::is_drop_frame(_fps);
            union {
                float_t f = 0.0;
                int_t i;
            } _value;
        };
    }
    
    // @SECTION: Chrono Library Type Aliases
    using fps = internal::__FPSFormat<float64_t, int64_t>;
    using timecode = internal::__BasicTimecode<std::string,
                                               std::string_view,
                                               int64_t,
                                               float64_t,
                                               fps>;
}

namespace vtm
{
    // @SECTION: VTM timecode object aliases
    using timecode = chrono::timecode;
    using tcfloat_t = typename chrono::timecode::float_t;
    using tcint_t = typename chrono::timecode::int_t;

    // @SECTION: VTM FPS factory object aliases
    using fps = chrono::fps;
    using fpsfloat_t = typename chrono::fps::float_type;
    using fpsint_t = typename chrono::fps::int_type;
}















