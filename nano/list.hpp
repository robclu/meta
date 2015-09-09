// ----------------------------------------------------------------------------------------------------------
/// @file   list.hpp
/// @brief  Header file for the list metaclass to provide compile time lists
// ----------------------------------------------------------------------------------------------------------

/*
 * ----------------------------------------------------------------------------------------------------------
 * list header file for nano library.
 * Copyright (C) 2015 Rob Clucas
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * ----------------------------------------------------------------------------------------------------------
 */

#ifndef NANO_LIST_HPP
#define NANO_LIST_HPP

#include <nano/functions.hpp>
#include <cassert>

namespace nano {
 
// ----------------------------------------------------------------------------------------------------------
/// @struct     list
/// @brief      Meta class that holds types, and allows functions to be applied to the elements of the list 
///             using the internal apply struct.                                                             \n
///                                                                                                          \n
///             Usage:                                                                                       \n
///                                                                                                          \n
///             using nano::dim;                         \\ To get dimension types                            \n
///             using test_list = list<i, j, k, l>;     \\ A list of dimensions                              \n
///                                                                                                          \n
///             using shifted_list = list::apply<shift>;                                                     \n
///                                                                                                          \n
///             Where shift is some function which operates on a type in the list 
/// @tparam     Ts      The tyes of the elements in the list
// ----------------------------------------------------------------------------------------------------------
template <typename... Ts>
struct list
{
    using type = list<Ts...>;
    
    static constexpr std::size_t size = sizeof...(Ts);                               //!< The size of the list
};

// Define a list with no arguments
using empty_list = list<>;

// ---------------------------------------- Operations on a list(s) -----------------------------------------

namespace detail {
    
// ----------------------------------------------------------------------------------------------------------
/// @struct     get
/// @brief      Meta function to get an element from a list 
/// @tparam     Index       The index of the element in the list to get
/// @tparam     List        The list to get the element from
// ----------------------------------------------------------------------------------------------------------
template <typename Index, typename List>
struct get;

// Recursive case to look through the list
template <std::size_t Index, typename Head, typename... Tail>
struct get<nano::size_t<Index>, list<Head, Tail...>> : public get<nano::size_t<Index - 1>, list<Tail...>> {};

// Base case (when we reach the 0 element)
template <typename Head, typename... Tail>
struct get<nano::size_t<0>, list<Head, Tail...>> : public identify<Head> {};

}

// Wrapper for getting
template <std::size_t Index, typename List>
using get = typename detail::get<nano::size_t<Index>, List>::result;

// ----------------------------------------------------------------------------------------------------------
/// @struct     find_type
/// @brief      Find the index of a specific type in the list (the index of the first occurrence).           \n 
///             If the type is not found then the value 'parameter' will be -1.
/// @tparam     Type    The type to find
/// @tparam     List    The list to find the type in
// ----------------------------------------------------------------------------------------------------------
template <typename Type, typename List>
struct find_type;
    
// Case to recurse
template <typename Type, typename Head, typename... Tail>
struct find_type<Type, list<Head, Tail...>>
{
    static constexpr int next_result = find_type<Type, list<Tail...>>::result;
    
    // 'Move through list'
    static constexpr int result      = next_result >= 0 ? next_result + 1 : -1;
};

// Case for when the type is found
template <typename Type, typename... Tail>
struct find_type<Type, list<Type, Tail...>>
{
    static constexpr int result = 0;
};

// Case for not found (Tail will be empty)
template <typename Type>
struct find_type<Type, list<>>
{
    static constexpr int result = -1;
};

// ----------------------------------------------------------------------------------------------------------
/// @struct     type_not_present 
/// @brief      To check if a type in a list is not found. Where the find_type tries to find a type and get 
///             its index to return, this function simply returns true of the type is not found, and false 
///             if the tpe is found.
/// @tparam     Type        The type to check the presence of
/// @tparam     List        The list to look through
// ----------------------------------------------------------------------------------------------------------
template <typename Type, typename List>
struct type_not_present;

// Recursive case
template <typename Type, typename Head, typename... Tail>
struct type_not_present<Type, list<Head, Tail...>>
{
    // The list must remain the same (the type
    // changes which gives different results)
    static constexpr bool constant_list = true;
    
    static constexpr int next_result = type_not_present<Type, list<Tail...>>::result;
    
    // 'Move through list'
    static constexpr int result      = next_result <= 0 ? 0 : next_result + 1;
};

// Case for when the type is found
template <typename Type, typename... Tail>
struct type_not_present<Type, list<Type, Tail...>>
{
    static constexpr bool constant_list = true;
    
    static constexpr int result = 0;
};

// Case for not found (Tail will be empty)
template <typename Type>
struct type_not_present<Type, list<>>
{
    static constexpr bool constant_list = true;
    
    static constexpr int result = 1;
};

// ----------------------------------------------------------------------------------------------------------
/// @struct     first_not_present
/// @brief      Just check if the first element of the list was found (searching should be done beforehand)
/// @tparam     Type    This is just a placeholder to conform to the Evaluator interface for the filter
///             function 
/// @tparam     List    The list to check if the first element was not found
// ----------------------------------------------------------------------------------------------------------
template <typename Type, typename List>
struct first_not_present;

template <typename Type, typename Head, typename... Tail>
struct first_not_present<Type, list<Head, Tail...>>
{
    // The list must change (to get different results the
    // head of the list must be removed for the next it
    static constexpr bool constant_list = false;
    
    static constexpr bool result = Head::value == -1 ? true : false;
};

// ----------------------------------------------------------------------------------------------------------
/// @struct     join
/// @brief      Joins two lists
/// @tparam     List1       The first list to join
/// @tparam     List2       The second list to join
// ----------------------------------------------------------------------------------------------------------
template <typename List1, typename List2>
struct join;

// Specialization for using list types
template <typename... Ts, typename... Us>
struct join<list<Ts...>, list<Us...>> : public identify<list<Ts..., Us...>> {};

}           // End namespace nano

#endif      // NANO_LIST_HPP
