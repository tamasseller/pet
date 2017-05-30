/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Seller Tamás. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/

namespace pet {

namespace detail {

template<class Value, class... Args>
struct ValueExtractor {};

template<class Value>
struct ValueExtractor<Value> {
    static constexpr auto value = Value::value;
};

template<class Value, class Expected, class Comparable, class Current, class... Args>
struct ValueTagChecker {
    static constexpr auto value = ValueExtractor<Value, Args...>::value;
};

template<class Value, class Expected, class Current, class... Args>
struct ValueTagChecker<Value, Expected, Expected, Current, Args...> {
    static constexpr auto value = Current::value;
};

template<class Value, class I, class... Args>
struct ValueExtractor<Value, I, Args...> {
    static constexpr auto value = ValueTagChecker<Value, typename Value::Tag, typename I::Tag, I, Args...>::value;
};

template<class Value, template <Value> class > struct ValueTagger;

template<class Type, class... Args>
struct TypeExtractor {};

template<class Type>
struct TypeExtractor<Type> {
    using type = typename Type::type;
};

template<class Type, class Expected, class Comparable, class Current, class... Args>
struct TypeTagChecker {
    using type = typename TypeExtractor<Type, Args...>::type;
};

template<class Type, class Expected, class Current, class... Args>
struct TypeTagChecker<Type, Expected, Expected, Current, Args...> {
    using type = typename Current::type;
};

template<class Type, class I, class... Args>
struct TypeExtractor<Type, I, Args...> {
    using type = typename TypeTagChecker<Type, typename Type::Tag, typename I::Tag, I, Args...>::type;
};

template<template <class> class > struct TypeTagger;


template<class Template, class... Args>
struct TemplateExtractor {
	template<class... X>
	using typeTemplate = typename Template::template typeTemplate<X...>;
};

template<class Template>
struct TemplateExtractor<Template> {
	template<class... X>
    using typeTemplate = typename Template::template typeTemplate<X...>;
};

template<class Template, class Expected, class Comparable, class Current, class... Args>
struct TemplateTagChecker {
	template<class... X>
    using typeTemplate = typename TemplateExtractor<Template, Args...>::template typeTemplate<X...>;
};

template<class Template, class Expected, class Current, class... Args>
struct TemplateTagChecker<Template, Expected, Expected, Current, Args...> {
	template<class... X>
    using typeTemplate = typename Current::template typeTemplate<X...>;
};

template<class Template, class I, class... Args>
struct TemplateExtractor<Template, I, Args...> {
	template<class... X>
    using typeTemplate = typename TemplateTagChecker<Template, typename Template::Tag, typename I::Tag, I, Args...>::template typeTemplate<X...>;
};

template<template <template <class...> class> class > struct TemplateTagger;


}

template<class InputType, template<InputType> class InputTag, InputType inputValue>
struct ConfigValue {
                template<class, class...> friend struct detail::ValueExtractor;
                template<class, class, class, class...> friend struct detail::ValueTagChecker;
                template<class, class...> friend struct detail::TypeExtractor;
                template<class, class, class, class...> friend struct detail::TypeTagChecker;

                typedef InputType Type;
                static constexpr Type value = inputValue;

                typedef detail::ValueTagger<InputType, InputTag> Tag;


            public:
                template<class... Args>
                using extract = typename detail::ValueExtractor<ConfigValue, Args...>;
        };

template<template<class> class InputTag, class InputType>
class ConfigType {
        template<class, class...> friend struct detail::ValueExtractor;
        template<class, class, class, class...> friend struct detail::ValueTagChecker;
        template<class, class...> friend struct detail::TypeExtractor;
        template<class, class, class, class...> friend struct detail::TypeTagChecker;
        typedef InputType type;

        typedef detail::TypeTagger<InputTag> Tag;
    public:
        template<class... Args>
        using extract = typename detail::TypeExtractor<ConfigType, Args...>;
};

template<template<template<class...> class > class InputTag, template<class...> class InputType>
class ConfigTemplate {
        template<class, class...> friend struct detail::ValueExtractor;
        template<class, class, class, class...> friend struct detail::ValueTagChecker;
        template<class, class...> friend struct detail::TypeExtractor;
        template<class, class, class, class...> friend struct detail::TypeTagChecker;
        template<class, class...> friend struct detail::TemplateExtractor;
        template<class, class, class, class...> friend struct detail::TemplateTagChecker;

		template<class... X>
        using typeTemplate = InputType<X...>;

        typedef detail::TemplateTagger<InputTag> Tag;
    public:
        template<class... Args>
        using extract = typename detail::TemplateExtractor<ConfigTemplate, Args...>;
};
}
