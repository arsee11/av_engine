///flexible_buffer.h

#ifndef FLEXIBLE_BUFFER_H
#define FLEXIBLE_BUFFER_H

#include <vector>
#include <tuple>

#ifndef NAMESPDEF_H
#include "namespdef.h"
#endif


using std::size_t;

NAMESP_BEGIN


///a flexible buffer.
///capacity can be increase or decrease dynamically.
///not thread safety.
template<typename T>
class FlexibleBuffer
{
	const static size_t factor=1;
	
public:
	FlexibleBuffer(size_t n)
		:_begin(0)
		,_end(0)
		,_buf(n)
	{}
	
	~FlexibleBuffer(){}
	
	typedef T value_type;
	typedef value_type& reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;	
	typedef pointer iterator;
	typedef const iterator const_iterator;
	
public:
	void push( const_pointer  data, size_t len){		
		if( (capacity()-size()) < len )
			increase( (len-(capacity()-size()) ) *factor);
		
		std::copy(data, data+len, end());
		_end += len;
	}
	
	///@param len number of item need.
	///@return 1st pointer to the datas, 2nd number of item obained.
	std::tuple<pointer, size_t> head(size_t len){
		if( size() >= len )
		{
			return std::make_tuple(begin(), len);
		}
		else
		{
			return std::make_tuple(begin(), size());
		}
	}
	
	///@param len the number of item to be consumed.
	///@return if size() >= len return true,\n
	///	else return false.
	bool consume(size_t len){
		if( size() < len )
			return false;
		
		size_t s = size()-len;
		std::copy(begin()+len, end(), _buf.begin() );
		_begin=0;
		_end = _begin+s;
		
		return true;
	}
	
	void clear(){
		_begin=_end=0;
	}
	
	pointer operator[](size_t i){
		if( size() > i)
			return begin()+i;
		
		return nullptr;
	}
	
	iterator begin(){
		return &_buf[_begin];
	}
	
	iterator end(){
		return &_buf[_end];
	}
	
	size_t size(){
		return _end - _begin;
	}
	
	size_t capacity(){ 
		return _buf.capacity(); 
	}
	
	size_t capacity(size_t len){
		size_t n = len - capacity();
		if( n > 0 ){
			increase(n);
		}
		else if(n < 0 ){
			decrease(-n);
		}
		else{
		}
	}
private:

	
	void increase(size_t size){
		_buf.resize( capacity()+size );
	}
	
	void decrease(size_t size){
		_buf.resize( capacity()-size );
	}
	
private:
	size_t _begin, _end;
	std::vector<T> _buf;
	
};

NAMESP_END/*namespace*/

#endif/*FLEXIBLE_BUFFER_H*/
