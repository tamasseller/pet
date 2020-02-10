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

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

/*
 * Meta program for tag based configuration of template modules.
 * =============================================================
 *
 *
 *   Rationale
 *   ---------
 *
 * As highly templated softwares modules grow in size and complexity it is very common
 * that numerous configuration options emerge. Over a certain number of options it is
 * no convenient anymore to use direct template arguments for specifying the parameters.
 *
 * A superior approach is to create a single parameter pack instead, that does not hold
 * the arguments in a specific order, but rather that holds special types, that provide
 * a method to identify not only the value set by the user, but also the configuration
 * option that they belong to.
 *
 *
 *   Features
 *   --------
 *
 * The solution proposed herein enable the author of a software component to present
 * the user with an easily comprehensible and manageable way to fill in configuration
 * data, with support for three kinds of it:
 *
 *  - arbitrarily type values,
 *  - types
 *  - templates with arbitrary number of type arguments.
 *
 * Upon extraction of the configuration values the module must specify a default value that
 * is returned if the arguments in question is unspecified, thus enabling minimal ease of
 * module bring-up for common use cases.
 *
 * NOTE: Templates with non-type arguments can use value wrapper types to pass those arguments.
 *
 *
 *   Usage
 *   -----
 *
 * The intended use of this system is as follows.
 *
 * ### The configurable software module
 *
 * The module specifies a number of option tags that also define the kind (value, type or
 * template) of the configuration parameter they belong to. Value parameters also need to
 * specify the type of the value they expect.
 *
 * This is done by defining user-visible, single-argument template types that specify one
 * of the _ConfigValue_, _ConfigType_ or _ConfigTemplate_ template classes as their base
 * class, with the last argument being its own parameter. The previous argument is the
 * sub-class itself. Type and template tag bases have only these two arguments. For value
 * parameters the type of the value is specified as the first argument. second argument.
 * The sub-class is not supposed to contain any members.
 *
 * ```c++
 * 			template<int x>
 * 			struct Foo: public ConfigValue<int, Foo, x> {};
 *
 * 			template<class T>
 * 			struct Bar: public ConfigType<Bar, T> {};
 *
 * 			template<template<class...> class T>
 * 			struct Baz: public ConfigTemplate<Baz, T> {};
 * ```
 *
 * The body of the module is implemented as a template type which expects a type parameter
 * pack. This pack holds a set of the instances of the above mentioned tags defined by the
 * module. The module can extract the user-supplied values for the individual options by
 * using the _extract_ template member of tag (inherited from the respective base)
 * instantiated with the default value. The _extract_ member needs to be instantiated using
 * the input parameter pack. It contains a member called _value_, _type_ or _typeTemplate_
 * for the value, type and template parameter kinds, respectively. The aforementioned member
 * contains the user-configured value/type/template.
 *
 * ```c++
 * 			template<class... Args>
 * 			class Module {
 * 		    	static constexpr auto userFoo = Foo<42>::extract<Args...>::value;
 *
 *          	using UserBar = typename Bar<void>::extract<Args...>::type;
 *
 *          	template<class... X> using UserBaz =
 *          	typename Baz<std::vector>::extract<Args...>::template typeTemplate<X...>;
 * 			};
 *
 * ```
 *
 * ### The user of the module
 *
 * The user of the module can specify all of the required parameters in a single expression,
 * by instantiating the template body of the module supplying options tags instantiated with
 * the desired values.
 *
 * ```c++
 * 			Module<
 * 				Foo<666>,
 * 				Bar<long double>,
 * 				Baz<std::multimap>
 * 			> myModule;
 * ```
 */

namespace pet {

namespace detail {

/// Unique type for a value option tag (common to instances without regard to the parameter).
template<class Value, template <Value> class > struct ValueTagger;

/**
 * Main part of value extractor.
 *
 * Its purpose is to extract the tags of the _Value_ parameter and the first element of the pack.
 * The former is the tag instance, whose _Tag_ member is used as the key for searching (and it
 * also holds the default value). The later is the first one of the configuration input types.
 * It supplies these to the _ValueTagChecker_ defined later, that handles matching the tags and
 * either terminates or proceeds with the recursive consumption of the input data.
 */
template<class Value, class... Args> struct ValueExtractor {};

/**
 * Recursion stopper.
 *
 * If there are no more input data return default value.
 */
template<class Value>
struct ValueExtractor<Value> {
    static constexpr decltype(Value::value) value = Value::value;
};

template<class Value>
constexpr decltype(Value::value) ValueExtractor<Value>::value;


/**
 * Generic definition of the tag matching worker.
 *
 * Parameters:
 *
 *  - _Value_ is reference tag instance.
 *  - _Expected_ is the unique _ValueTagger_ instance belonging to _Value_.
 *  - _Comparable_ is the unique _ValueTagger_ instance belonging to first input element.
 *  - _Current_ is the first input element.
 *  - _Args_ is the rest of the input elements.
 *
 * This definition and the specialization below basically implement a meta-programming if
 * statement where the condition to be check is that whether the _Expected_ and
 * _Comparable_ type arguments hold the same type. If these two are not the same, then
 * it goes on with the recursion by 'calling' back to the ValueExtractor with the first
 * element removed.
 */
template<class Value, class Expected, class Comparable, class Current, class... Args>
struct ValueTagChecker {
    static constexpr auto value = ValueExtractor<Value, Args...>::value;
};

/**
 * Specialization of the tag matching worker, for matching elements.
 *
 * This is the "then" branch of the above mentioned if statement, if the two
 * _ValueTagger_ instances match then return the value contained in the first
 * input element.
 */
template<class Value, class Expected, class Current, class... Args>
struct ValueTagChecker<Value, Expected, Expected, Current, Args...> {
    static constexpr auto value = Current::value;
};

/**
 * Extraction starter.
 *
 * This is the entry point of the recursive search algorithm outlined above.
 */
template<class Value, class I, class... Args>
struct ValueExtractor<Value, I, Args...> {
    static constexpr auto value = ValueTagChecker<Value, typename Value::Tag, typename I::Tag, I, Args...>::value;
};


/// Unique type for a type option tag (common to instances without regard to the parameter).
template<template <class> class > struct TypeTagger;

/// @see ValueExtractor
template<class Type, class... Args>
struct TypeExtractor {};

/// @see ValueExtractor
template<class Type>
struct TypeExtractor<Type> {
    using type = typename Type::type;
};

/// @see ValueTagChecker
template<class Type, class Expected, class Comparable, class Current, class... Args>
struct TypeTagChecker {
    using type = typename TypeExtractor<Type, Args...>::type;
};

/// @see ValueTagChecker
template<class Type, class Expected, class Current, class... Args>
struct TypeTagChecker<Type, Expected, Expected, Current, Args...> {
    using type = typename Current::type;
};

/// @see ValueExtractor
template<class Type, class I, class... Args>
struct TypeExtractor<Type, I, Args...> {
    using type = typename TypeTagChecker<Type, typename Type::Tag, typename I::Tag, I, Args...>::type;
};

/// Unique type for a template option tag (common to instances without regard to the parameter).
template<template <template <class...> class> class > struct TemplateTagger;

/// @see ValueExtractor
template<class Template, class... Args>
struct TemplateExtractor {
	template<class... X>
	using typeTemplate = typename Template::template typeTemplate<X...>;
};

/// @see ValueExtractor
template<class Template>
struct TemplateExtractor<Template> {
	template<class... X>
    using typeTemplate = typename Template::template typeTemplate<X...>;
};

/// @see ValueTagChecker
template<class Template, class Expected, class Comparable, class Current, class... Args>
struct TemplateTagChecker {
	template<class... X>
    using typeTemplate = typename TemplateExtractor<Template, Args...>::template typeTemplate<X...>;
};

/// @see ValueTagChecker
template<class Template, class Expected, class Current, class... Args>
struct TemplateTagChecker<Template, Expected, Expected, Current, Args...> {
	template<class... X>
    using typeTemplate = typename Current::template typeTemplate<X...>;
};

/// @see ValueExtractor
template<class Template, class I, class... Args>
struct TemplateExtractor<Template, I, Args...> {
	template<class... X>
    using typeTemplate = typename TemplateTagChecker<Template, typename Template::Tag, typename I::Tag, I, Args...>::template typeTemplate<X...>;
};

}



/**
 * Value parameter tag.
 *
 * Contains a _Tag_ member typedef that is specific to the tag template itself
 * but not the instance holding a specific setting. This is used as a key for
 * the recursive search during extraction. Also holds the specified setting value.
 * Includes a using for the adequate extractor for simplified syntax.
 */
template<class InputType, template<InputType> class InputTag, InputType inputValue>
struct ConfigValue {
		template<class, class...> friend struct detail::ValueExtractor;
		template<class, class, class, class, class...> friend struct detail::ValueTagChecker;
		template<class, class...> friend struct detail::TypeExtractor;
		template<class, class, class, class, class...> friend struct detail::TypeTagChecker;
		template<class, class...> friend struct detail::TemplateExtractor;
		template<class, class, class, class, class...> friend struct detail::TemplateTagChecker;


		typedef InputType Type;
		static constexpr Type value = inputValue;

		typedef detail::ValueTagger<InputType, InputTag> Tag;


	public:
		template<class... Args>
		using extract = typename detail::ValueExtractor<ConfigValue, Args...>;
};

/**
 * Type parameter tag.
 *
 * Contains a _Tag_ member typedef that is specific to the tag template itself
 * but not the instance holding a specific setting. This is used as a key for
 * the recursive search during extraction. Also holds the specified setting value.
 * Includes a using for the adequate extractor for simplified syntax.
 */
template<template<class> class InputTag, class InputType>
class ConfigType {
        template<class, class...> friend struct detail::ValueExtractor;
        template<class, class, class, class, class...> friend struct detail::ValueTagChecker;
        template<class, class...> friend struct detail::TypeExtractor;
        template<class, class, class, class, class...> friend struct detail::TypeTagChecker;
        template<class, class...> friend struct detail::TemplateExtractor;
        template<class, class, class, class, class...> friend struct detail::TemplateTagChecker;

        typedef InputType type;

        typedef detail::TypeTagger<InputTag> Tag;
    public:
        template<class... Args>
        using extract = typename detail::TypeExtractor<ConfigType, Args...>;
};

/**
 * Template parameter tag.
 *
 * Contains a _Tag_ member typedef that is specific to the tag template itself
 * but not the instance holding a specific setting. This is used as a key for
 * the recursive search during extraction. Also holds the specified setting value.
 * Includes a using for the adequate extractor for simplified syntax.
 */
template<template<template<class...> class > class InputTag, template<class...> class InputType>
class ConfigTemplate {
        template<class, class...> friend struct detail::ValueExtractor;
        template<class, class, class, class, class...> friend struct detail::ValueTagChecker;
        template<class, class...> friend struct detail::TypeExtractor;
        template<class, class, class, class, class...> friend struct detail::TypeTagChecker;
        template<class, class...> friend struct detail::TemplateExtractor;
        template<class, class, class, class, class...> friend struct detail::TemplateTagChecker;

		template<class... X>
        using typeTemplate = InputType<X...>;

        typedef detail::TemplateTagger<InputTag> Tag;
    public:
        template<class... Args>
        using extract = typename detail::TemplateExtractor<ConfigTemplate, Args...>;
};
}

/**
 * Helper macros
 */
#define PET_CONFIG_VALUE(name, type) \
	template<type x> struct name: pet::ConfigValue<type, name, x> {}

#define PET_CONFIG_TYPE(name) \
	template<class x> struct name: pet::ConfigType<name, x> {}

#define PET_CONFIG_TEMPLATE(name) \
	template<template<class...> class x> struct name: pet::ConfigTemplate<name, x> {}

#define PET_EXTRACT_VALUE(name, config, defaultValue, arguments) \
	static constexpr auto name = config<defaultValue>::template extract<arguments...>::value

#define PET_EXTRACT_TYPE(name, config, defaultValue, arguments) \
	using name = typename config<defaultValue>::template extract<arguments...>::type

#define PET_EXTRACT_TEMPLATE(name, config, defaultValue, arguments) \
	template<class... X> using name = typename config<defaultValue>::template extract<arguments...>::template typeTemplate<X...>

#endif /* CONFIGURATION_H_ */
