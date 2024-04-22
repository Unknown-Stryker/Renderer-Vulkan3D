#ifndef _FE_CORE_REF_TABLE_HXX_
#define _FE_CORE_REF_TABLE_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/allocator.hxx>
#include <deque>




namespace FE
{
	template<typename T>
	class proxy_ptr;

	template<typename T, class Allocator>
	class exclusive_ptr;

	template<typename T>
	class exclusive_ptr_base;
}




BEGIN_NAMESPACE(FE::internal::smart_ptr)



template<typename T>
class ref_table_base 
{
	friend class exclusive_ptr_base<T>;

public:
	using element_type = typename std::remove_extent<T>::type;

	using ref_table_type = std::deque<const exclusive_ptr_base<T>*>;
	using ref_table_const_value_type = typename ref_table_type::value_type;

	using ref_table_key_type = typename ref_table_type::pointer;
	using ref_table_recycler_type = std::deque<ref_table_key_type>;
	FE_STATIC_ASSERT((std::is_same<ref_table_const_value_type, const exclusive_ptr_base<T>*>::value != true), "Static Assertion Failure: ref_table_const_value_type must be const exclusive_ptr_base<T>*");

	static constexpr ref_table_key_type invalid_key_value = nullptr;
	
protected:
	thread_local static ref_table_type tl_s_ref_table;
	thread_local static ref_table_recycler_type tl_s_ref_table_recycler;

public:
	_FORCE_INLINE_ static boolean reset_and_return_result() noexcept
	{
		auto l_table_current_size = tl_s_ref_table.size();
		auto l_table_recycler_current_size = tl_s_ref_table_recycler.size();

		if ((l_table_current_size == l_table_recycler_current_size) && (l_table_current_size != 0))
		{
			tl_s_ref_table.clear();
			tl_s_ref_table_recycler.clear();

			tl_s_ref_table.shrink_to_fit();
			tl_s_ref_table_recycler.shrink_to_fit();
			return _SUCCESSFUL_;
		}

		return _FAILED_;
	}
};

template<typename T>
thread_local typename ref_table_base<T>::ref_table_type ref_table_base<T>::tl_s_ref_table;

template<typename T>
thread_local typename ref_table_base<T>::ref_table_recycler_type ref_table_base<T>::tl_s_ref_table_recycler;



template<typename T, class Allocator>
class ref_table_for_exclusive_ptr : public ref_table_base<T>
{
	friend class exclusive_ptr<T, Allocator>;

public:
	using base_type = ref_table_base<T>;
	using element_type = typename base_type::element_type;

	using ref_table_type = typename base_type::ref_table_type;
	using ref_table_const_value_type = typename base_type::ref_table_const_value_type;

	using ref_table_recycler_type = typename base_type::ref_table_recycler_type;
	using ref_table_key_type = typename ref_table_type::pointer;
	FE_STATIC_ASSERT((std::is_same<ref_table_const_value_type, const exclusive_ptr_base<T>*>::value != true), "Static Assertion Failure: ref_table_const_value_type must be const exclusive_ptr_base<T>*");

private:
	_FORCE_INLINE_ static ref_table_key_type __register_smart_ptr_ref(ref_table_const_value_type value_p) noexcept
	{
	
		if (base_type::tl_s_ref_table_recycler.empty() == false)
		{
			ref_table_key_type l_reused_key = base_type::tl_s_ref_table_recycler.back();
			base_type::tl_s_ref_table_recycler.pop_back();
			*l_reused_key = value_p;
			return l_reused_key;
		}

		return &(base_type::tl_s_ref_table.emplace_back(value_p));
	}

	_FORCE_INLINE_ static void __unregister_smart_ptr_ref(ref_table_key_type key_p) noexcept
	{
		FE_ASSERT(key_p == nullptr, "${%s@0}: ${%s@1} is an invalid key.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(key_p));
		*key_p = nullptr;
		
		//FE_LOG("Unregistered a key value: ${%lu@0}.\n\n\n", &key_p);

		base_type::tl_s_ref_table_recycler.emplace_back(key_p);

//#ifdef _DEBUG_
//
//		auto l_begin = base_type::tl_s_ref_table_recycler.begin();
//		auto l_end = base_type::tl_s_ref_table_recycler.end();
//
//		while(l_begin != l_end)
//		{
//			FE_ASSERT(base_type::tl_s_ref_table[*l_begin] != nullptr, "Assertion Failed: unregistered keys' value must be nullptr.");
//			
//			for (auto it = l_begin + 1; it != l_end; ++it)
//			{
//				FE_ASSERT(*l_begin == *it, "Assertion Failed: Keys are always unique.");
//			}
//			++l_begin;
//		}
//#endif
	}

	_FORCE_INLINE_ static void __update_smart_ptr_ref(ref_table_key_type key_p, ref_table_const_value_type value_p) noexcept
	{
		FE_ASSERT(key_p == nullptr, "${%s@0}: ${%s@1} is an invalid key.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(key_p));
		*key_p = value_p;
	}
};



template<typename T>
class ref_table_for_proxy_ptr : public ref_table_base<T>
{
	friend class proxy_ptr<T>;

public:
	using base_type = ref_table_base<T>;
	using element_type = typename base_type::element_type;

	using ref_table_type = typename base_type::ref_table_type;
	using ref_table_const_value_type = typename base_type::ref_table_const_value_type;

	using ref_table_key_type = typename ref_table_type::pointer;

	FE_STATIC_ASSERT((std::is_same<ref_table_const_value_type, const exclusive_ptr_base<T>*>::value != true), "Static Assertion Failure: ref_table_const_value_type must be const exclusive_ptr_base<T>*");

private:
	_FORCE_INLINE_ static typename exclusive_ptr_base<T>* __get_smart_ptr_ref(typename ref_table_key_type key_p) noexcept
	{
		if (key_p == base_type::invalid_key_value)
		{
			return nullptr;
		}

		return const_cast<exclusive_ptr_base<T>*>(*key_p);
	}
};


END_NAMESPACE
#endif