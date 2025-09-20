* 这里我写了两个版本,因为写第一个版本的时候,老实说我不太满意,因为第一个版本对我自己来说确实有点艰难,实际上第一个完成的版本都已经是第二次重构了,前面还有两次废案,所以这个Lab写了整整一周,不过确实,真的积累了不少的`debug`经验,对自己来说也算是收获颇丰了

* 这里是第一个版本,全部通过测试了,这个速度还行,有`2.31 Gbit/s`,但是因为代码规范还差点意思,所以`debug`的时候还是非常恶心的,比第二版多写了大概一天的样子,代码量不多,应该就`200`来行
```shell
oldking@iZwz9b2bj2gor4d8h3rlx0Z:~/CS144-2024-winter-backup$ cmake --build build_check1_3/ --target check1
Test project /home/oldking/CS144-2024-winter-backup/build_check1_3
      Start  1: compile with bug-checkers
 1/17 Test  #1: compile with bug-checkers ........   Passed   13.78 sec
      Start  3: byte_stream_basics
 2/17 Test  #3: byte_stream_basics ...............   Passed    0.05 sec
      Start  4: byte_stream_capacity
 3/17 Test  #4: byte_stream_capacity .............   Passed    0.04 sec
      Start  5: byte_stream_one_write
 4/17 Test  #5: byte_stream_one_write ............   Passed    0.04 sec
      Start  6: byte_stream_two_writes
 5/17 Test  #6: byte_stream_two_writes ...........   Passed    0.04 sec
      Start  7: byte_stream_many_writes
 6/17 Test  #7: byte_stream_many_writes ..........   Passed    0.12 sec
      Start  8: byte_stream_stress_test
 7/17 Test  #8: byte_stream_stress_test ..........   Passed    0.05 sec
      Start  9: reassembler_single
 8/17 Test  #9: reassembler_single ...............   Passed    0.02 sec
      Start 10: reassembler_cap
 9/17 Test #10: reassembler_cap ..................   Passed    0.03 sec
      Start 11: reassembler_seq
10/17 Test #11: reassembler_seq ..................   Passed    0.04 sec
      Start 12: reassembler_dup
11/17 Test #12: reassembler_dup ..................   Passed    0.07 sec
      Start 13: reassembler_holes
12/17 Test #13: reassembler_holes ................   Passed    0.03 sec
      Start 14: reassembler_overlapping
13/17 Test #14: reassembler_overlapping ..........   Passed    0.03 sec
      Start 15: reassembler_win
14/17 Test #15: reassembler_win ..................   Passed    1.94 sec
      Start 37: compile with optimization
15/17 Test #37: compile with optimization ........   Passed    3.22 sec
      Start 38: byte_stream_speed_test
             ByteStream throughput: 2.25 Gbit/s
16/17 Test #38: byte_stream_speed_test ...........   Passed    0.35 sec
      Start 39: reassembler_speed_test
             Reassembler throughput: 2.31 Gbit/s
17/17 Test #39: reassembler_speed_test ...........   Passed    0.60 sec

100% tests passed, 0 tests failed out of 17

Total Test time (real) =  20.49 sec
Built target check1
```

* 这是第二个版本,这个版本只写了两天左右,`debug`的速度非常快,但代码量却来到了`500`行左右,而且不知道为什么,这里的测试速度很低,只有`0.71 Gbit/s`,效率上远不如上一版
```shell
oldking@iZwz9b2bj2gor4d8h3rlx0Z:~/CS144-2024-winter-backup$ cmake --build build_check1_3_v2 --target check1
Test project /home/oldking/CS144-2024-winter-backup/build_check1_3_v2
      Start  1: compile with bug-checkers
 1/17 Test  #1: compile with bug-checkers ........   Passed    2.02 sec
      Start  3: byte_stream_basics
 2/17 Test  #3: byte_stream_basics ...............   Passed    0.04 sec
      Start  4: byte_stream_capacity
 3/17 Test  #4: byte_stream_capacity .............   Passed    0.04 sec
      Start  5: byte_stream_one_write
 4/17 Test  #5: byte_stream_one_write ............   Passed    0.04 sec
      Start  6: byte_stream_two_writes
 5/17 Test  #6: byte_stream_two_writes ...........   Passed    0.04 sec
      Start  7: byte_stream_many_writes
 6/17 Test  #7: byte_stream_many_writes ..........   Passed    0.13 sec
      Start  8: byte_stream_stress_test
 7/17 Test  #8: byte_stream_stress_test ..........   Passed    0.05 sec
      Start  9: reassembler_single
 8/17 Test  #9: reassembler_single ...............   Passed    0.03 sec
      Start 10: reassembler_cap
 9/17 Test #10: reassembler_cap ..................   Passed    0.02 sec
      Start 11: reassembler_seq
10/17 Test #11: reassembler_seq ..................   Passed    0.04 sec
      Start 12: reassembler_dup
11/17 Test #12: reassembler_dup ..................   Passed    0.08 sec
      Start 13: reassembler_holes
12/17 Test #13: reassembler_holes ................   Passed    0.02 sec
      Start 14: reassembler_overlapping
13/17 Test #14: reassembler_overlapping ..........   Passed    0.03 sec
      Start 15: reassembler_win
14/17 Test #15: reassembler_win ..................   Passed    2.07 sec
      Start 37: compile with optimization
15/17 Test #37: compile with optimization ........   Passed    0.38 sec
      Start 38: byte_stream_speed_test
             ByteStream throughput: 1.77 Gbit/s
16/17 Test #38: byte_stream_speed_test ...........   Passed    0.37 sec
      Start 39: reassembler_speed_test
             Reassembler throughput: 0.71 Gbit/s
17/17 Test #39: reassembler_speed_test ...........   Passed    0.75 sec

100% tests passed, 0 tests failed out of 17
```

* 接下来我们可以看看源码

* 以下是`v1`

* 这是`v1`的大纲
![[Pasted image 20250920202126.png]]

```CPP
// reassembler.hh

#pragma once

#include "byte_stream.hh"
#include <cstdint>
#include <functional>
#include <set>
#include <tuple>
#include <utility>


class Reassembler
{
public:
        // Construct Reassembler to write into given ByteStream.
        explicit Reassembler( ByteStream&& output ) 
                : output_( std::move( output ) )
                , cach_()
                , waiting_index_(0)
                , end_window_index_(output_.writer().available_capacity())
                , size_(0)
                , is_closed_({-1, -1})
        {}

        /*
         * Insert a new substring to be reassembled into a ByteStream.
         *   `first_index`: the index of the first byte of the substring
         *   `data`: the substring itself
         *   `is_last_substring`: this substring represents the end of the stream
         *   `output`: a mutable reference to the Writer
         *
         * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
         * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
         * learns the next byte in the stream, it should write it to the output.
         *
         * If the Reassembler learns about bytes that fit within the stream's available capacity
         * but can't yet be written (because earlier bytes remain unknown), it should store them
         * internally until the gaps are filled in.
         *
         * The Reassembler should discard any bytes that lie beyond the stream's available capacity
         * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
         *
         * The Reassembler should close the stream after writing the last byte.
         */
        void insert( uint64_t first_index, std::string data, bool is_last_substring );

        // How many bytes are stored in the Reassembler itself?
        uint64_t bytes_pending() const;

        // Access output stream reader
        Reader& reader() { return output_.reader(); }
        const Reader& reader() const { return output_.reader(); }

        // Access output stream writer, but const-only (can't write from outside)
        const Writer& writer() const { return output_.writer(); }

private:
        // v1

        class greater
        {
              bool operator()(std::pair<uint64_t, std::string> left, std::pair<uint64_t, std::string> right)
              {
                      return left.first > right.first;
              }
        };

        void cut_cache_in(uint64_t first_index, std::string data);

        ByteStream output_; // the Reassembler writes to this ByteStream
        std::map<uint64_t, std::string> cach_;
        uint64_t waiting_index_;
        uint64_t end_window_index_;
        uint64_t size_;
        std::pair<int64_t, int64_t> is_closed_;
};
```

```CPP
// reassembler.cc

#include "reassembler.hh"
#include "byte_stream.hh"
#include <cstdint>
#include <iterator>
#include <string>
#include <sys/types.h>
#include <iostream>

using namespace std;

// v1
void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
        if(is_closed_.first != -1 && is_closed_.second < static_cast<int64_t>(first_index + data.length()))
                return ;

        if(is_closed_.first != -1 && static_cast<int64_t>(first_index + data.length()) == is_closed_.second && !is_last_substring)
                return ;

        if(is_closed_.first == -1 && is_last_substring)
        {
                is_closed_ = {first_index, first_index + data.length()};
                if(cach_.empty() && first_index == waiting_index_ && data.length() == 0)
                        output_.writer().close();
        }

        //cout << "insert: " << data << " @ index " << first_index << endl;
        // 窗口更新
        end_window_index_ = waiting_index_ + output_.writer().available_capacity();
        //cout << "window update: " << "end_window_index->" << end_window_index_ << endl;
        // 窗口未命中
        if(first_index >= end_window_index_)
                return ;
        if(first_index + data.length() <= waiting_index_)
                return ;

        // 窗口命中
        // 默认判断等待包一定不匹配
        // cut & cache in
        cut_cache_in(first_index, data); 

        // 缓存检查
        while(cach_.begin() != cach_.end() && cach_.begin()->first == waiting_index_)
        {
                // write
                auto cur_first_index = cach_.begin()->first;
                //cout << "cach check!" << endl;
                output_.writer().push(cach_.begin()->second);
                //cout << "write cach begin it to output: " << cach_.begin()->second << endl;
                waiting_index_ = cur_first_index + cach_.begin()->second.length();
                //cout << "change waiting_index to: " << waiting_index_ << endl;
                size_ -= cach_.begin()->second.length();
                cach_.erase(cach_.begin());
                if(is_closed_.first != -1 && is_closed_.second <= static_cast<int64_t>(waiting_index_))
                {
                        cout << "close!" << endl;
                        output_.writer().close();
                        break;
                }
        }
        //if(cach_.empty())
                //cout << "cach_ is empty" << endl;
        //else 
                //cout << "cach_ begin: " << "first_index->" << cach_.begin()->first << " data->" << cach_.begin()->second << endl;

}

uint64_t Reassembler::bytes_pending() const
{
        //cout << "end_window_index->" << end_window_index_ << " waiting_index_->" << waiting_index_ << "size_->" << size_ << endl;
        return size_;
}

void Reassembler::cut_cache_in(uint64_t first_index, std::string data)
{
        //cout << "cut cache in:" << endl;

        if(first_index < waiting_index_)
        {
                //cout << "cut head because of window" << endl;
                data = data.substr(waiting_index_ - first_index);
                first_index = waiting_index_;
        }
        if(first_index + data.length() > end_window_index_)
        {
                //cout << "cut tail because of window" << endl;
                data = data.substr(0, end_window_index_ - first_index);
        }
        for(auto it = cach_.begin(); it != cach_.end();)
        {
                //cout << "string in cache: " << it->second << "@ " << "index " << it->first << endl;
                auto cur_first_index = it->first;
                auto cur_end_index = it->first + it->second.length();

                if(cur_first_index <= first_index && 
                   first_index + data.length() <= cur_end_index)
                        return ;

                if(cur_first_index <= first_index && 
                   first_index < cur_end_index && 
                   cur_end_index <= first_index + data.length())
                {
                        //cout << "merge with " << it->second << " as head" << endl;
                        data = it->second.substr(0, first_index - cur_first_index) + data;
                        first_index = it->first;
                        size_ -= it->second.length();
                        it = cach_.erase(it);
                }
                else if(first_index <= cur_first_index && 
                                cur_first_index < first_index + data.length() && 
                                first_index + data.length() <= cur_end_index) 
                {
                        //cout << "merge with " << it->second << " as tail" << endl;
                        data = data + it->second.substr(it->second.length() - (cur_end_index - (first_index + data.length())));
                        size_ -= it->second.length();
                        it = cach_.erase(it);
                }
                else if(first_index < cur_first_index && cur_end_index < first_index + data.length()) 
                {
                        //cout << "erase " << it->second << endl;
                        size_ -= it->second.length();
                        it = cach_.erase(it);
                }
                else if(first_index == cur_end_index)
                {
                        //cout << "next" << endl;
                        it++;
                        continue;
                }
                else if(first_index + data.length() == cur_first_index) 
                        break;
                else 
                        it++;
        }
        cach_[first_index] = data;
        size_ += data.length();
        //cout << "cache in: " << "first_index->" << first_index << " data->" << data << endl;
}
```

* 以下是`v2`

* 这是`v2`的架构
![[Pasted image 20250920202319.png]]
* 这个图当个参考就好,随便画的请勿当真

```CPP
// reassembler.hh

#pragma once

#include "byte_stream.hh"
#include <cstdint>
#include <functional>
#include <set>
#include <tuple>
#include <utility>


class Reassembler
{
public:
        // Construct Reassembler to write into given ByteStream.
        explicit Reassembler( ByteStream&& output ) 
                : output_( std::move( output ) )
                , waiting_index_(0)
                , tail_index_(output_.writer().available_capacity())
                , EOF_first_index_(-1)
                , EOF_end_index_(-1)
                , size_(0)
                , WF_(waiting_index_, tail_index_)
                , CM_(output_, waiting_index_, tail_index_, size_)
                , EOFM_(output_, size_, EOF_first_index_, EOF_end_index_)
        {}

    // 拷贝构造
    Reassembler(const Reassembler& other)
        : output_(other.output_)
        , waiting_index_(other.waiting_index_)
        , tail_index_(other.tail_index_)
        , EOF_first_index_(other.EOF_first_index_)
        , EOF_end_index_(other.EOF_end_index_)
        , size_(other.size_)
        , WF_(waiting_index_, tail_index_)
        , CM_(output_, waiting_index_, tail_index_, size_)
        , EOFM_(output_, size_, EOF_first_index_, EOF_end_index_) 
        {}

    // 移动构造
    Reassembler(Reassembler&& other) noexcept
        : output_(std::move(other.output_))
        , waiting_index_(other.waiting_index_)
        , tail_index_(other.tail_index_)
        , EOF_first_index_(other.EOF_first_index_)
        , EOF_end_index_(other.EOF_end_index_)
        , size_(other.size_)
        , WF_(waiting_index_, tail_index_)
        , CM_(output_, waiting_index_, tail_index_, size_)
        , EOFM_(output_, size_, EOF_first_index_, EOF_end_index_) 
        {}

	// 值得注意的是,这里一定要写拷贝构造和移动构造,因为reassembler对象初始化之后会被测试框架拷贝或者移动,如果不写拷贝构造和移动构造做深拷贝的话(有点搞笑,因为这里的移动构造根本没用移动语义),那么会造成引用垂悬

        /*
         * Insert a new substring to be reassembled into a ByteStream.
         *   `first_index`: the index of the first byte of the substring
         *   `data`: the substring itself
         *   `is_last_substring`: this substring represents the end of the stream
         *   `output`: a mutable reference to the Writer
         *
         * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
         * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
         * learns the next byte in the stream, it should write it to the output.
         *
         * If the Reassembler learns about bytes that fit within the stream's available capacity
         * but can't yet be written (because earlier bytes remain unknown), it should store them
         * internally until the gaps are filled in.
         *
         * The Reassembler should discard any bytes that lie beyond the stream's available capacity
         * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
         *
         * The Reassembler should close the stream after writing the last byte.
         */
        void insert( uint64_t first_index, std::string data, bool is_last_substring );

        // How many bytes are stored in the Reassembler itself?
        uint64_t bytes_pending() const;

        // Access output stream reader
        Reader& reader() { return output_.reader(); }
        const Reader& reader() const { return output_.reader(); }

        // Access output stream writer, but const-only (can't write from outside)
        const Writer& writer() const { return output_.writer(); }

private:
        // v2

        // class 

		// 窗口过滤器,根据窗口大小对字节流进行过滤
        class WindowFilter
        {
        public:
                WindowFilter(const int64_t& waiting_index, const int64_t& tail_index)
                        : waiting_index_(waiting_index)
                        , tail_index_(tail_index)
                {}

                bool operator()(const uint64_t& first_index, const std::string& data, 
                                        uint64_t& first_index_ret, std::string& data_ret);

        private:
                const int64_t& waiting_index_;
                const int64_t& tail_index_;
        };

		// 末尾管理器,初始化之后,可以按照字节流末尾的包过滤字节流
        class EOFManager
        {
        public:
                EOFManager(ByteStream& output, const int64_t& cache_size, int64_t& EOF_first_index, int64_t& EOF_end_index)
                        : EOF_first_index_(EOF_first_index)
                        , EOF_end_index_(EOF_end_index)
                        , output_(output)
                        , cache_size_(cache_size)
                        , is_init_(false)
                {}

                void init(uint64_t EOF_first_index, uint64_t EOF_end_index);

                bool operator()(const uint64_t& first_index, const std::string& data, 
                                        bool is_last_substring,
                                        uint64_t& first_index_ret, std::string& data_ret);

                const int64_t& get_EOF_first() const ;
                const int64_t& get_EOF_end() const ;
                bool is_EOF() const;

        private:
                int64_t& EOF_first_index_;
                int64_t& EOF_end_index_;
                ByteStream& output_;
                const int64_t& cache_size_;
                bool is_init_;
        };


        class writer_;
        class cache_in_;

		// 缓存管理器,用于管理递达到位了但顺序有问题的包
        class CacheManager
        {
        public:
                CacheManager(ByteStream& output, int64_t& waiting_index, int64_t& tail_index, int64_t& size)
                        : output_(output)
                        , cache_({})
                        , waiting_index_(waiting_index)
                        , tail_index_(tail_index)
                        , size_(size)
                {}

        writer_& writer();

        cache_in_& cachin();

        protected:
                class less
                {
                public:
						// less在容器中都是以const存在的,所以一定给operator()()加const,不过老实说不知道为什么在v1的版本中不用加const,我觉得可能和拷贝有关
                        bool operator()(const std::tuple<uint64_t, uint64_t, std::string>& left, const std::tuple<uint64_t, uint64_t, std::string>& right) const
                        {
                                return std::get<0>(left) < std::get<0>(right);
                        }
                };

                ByteStream& output_;
				// 这里用了元组tuple,这是CPP11新增的结构,可以给多个对象建立映射关系,这里用tuple纯粹是想存一下end_index
                std::set<std::tuple<uint64_t, uint64_t, std::string>, less> cache_; 
                int64_t& waiting_index_;
                const int64_t& tail_index_;
                int64_t& size_;
        };

        // shell class for CacheManager 
		// 内存管理器分为两种模式,我把这两种模式写成了壳对象
		// 这里其实完全可以不用这个方式写这两种模式,这里用壳对象完全是把这个设计复杂化了,这里用这个方式纯粹是想试试Lab0学到的技巧而已
        class writer_ : public CacheManager
        {
        public:
                bool write();
        };

        class cache_in_ : public CacheManager
        {
        public:
                bool cache_in(uint64_t first_index, std::string data);
        };

        // function 


        // members
        ByteStream output_; // the Reassembler writes to this ByteStream
        int64_t waiting_index_;
        int64_t tail_index_;
        int64_t EOF_first_index_;
        int64_t EOF_end_index_;
        int64_t size_;
        WindowFilter WF_;
        CacheManager CM_;
        EOFManager EOFM_;

		// 所有内部类的成员有非常细致的权限划分,因为对于reassembler来说,如果要细化其内部职能,必定会导致各个内部类之间产生耦合
		// 目前来看,让不同的只能共享成员是尽可能保证高内聚的方式,但同时,要在每个内部类中做好对于成员权限的规范,不能改的一定要加const
		// 这里细化粒度有一个好处,就是显著提高`debug`的效率,因为每个组件的职能很清楚,所以出了问题立马就可以找到问题所在,同时新增功能也会非常方便,尽可能减少新增功能对其他组件造成的影响
		// 但相对的,此时就有两个问题,一来是代码量过多,反倒是显得不够精简,另一个是要管理的资源会变多,导致很难管理,在两个组件因为资源而耦合的情况下,就更容易造成问题
		// 然后加之最近也了解了一下设计模式,所以你能很清楚的看到,其实过度设计可能并不是一件好事情
		// 设计模式和重构这两个东西在根本上是站在两个对立面的,设计模式的出现是为了减少重构,提高可维护性,但对可读性而言,这个见仁见智(老实说,可读性这个东西是不是一个伪命题?难道是为了掩盖代码阅读能力不足而创造的"谎言"?)
		// 但设计模式治标不治本,真正好的代码都是重构出来的,当然,我个人不占边,因为确实是各有各的好,具体要怎么做估计要和业务紧密挂钩,甚至有可能要和人挂钩,毕竟领导不让你重构,你再怎么想重构也没办法
		// 你是设计党还是重构党?doge
};
```


```CPP
// reassembler.cc

#include "reassembler.hh"
#include "byte_stream.hh"
#include <cstdint>
#include <iterator>
#include <string>
#include <sys/types.h>
#include <iostream>

using namespace std;

// v2
void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
		// 初始化窗口
        tail_index_ = waiting_index_ + output_.writer().available_capacity();
        //cout << "window init: " << "waiting_index->" << waiting_index_ << " tail_index->" << tail_index_ << endl;

		// 这里按理说可以不用返回型参数的,但因为`debug`的时候改成这样了,懒得改回去了,那就这样吧emmm
        uint64_t first_index_buff_1;
        std::string data_buff_1;
		// EOF过滤
        if(!EOFM_(first_index, data, is_last_substring, first_index_buff_1, data_buff_1))
                return ;

        uint64_t first_index_buff_2;
        std::string data_buff_2;
		// 窗口过滤
        if(!WF_(first_index_buff_1, data_buff_1, first_index_buff_2, data_buff_2))
                return ;

		// 缓入
        if(!CM_.cachin().cache_in(first_index_buff_2, data_buff_2))
                return ;

		// 写出
        CM_.writer().write();

		// 保证关闭
        if(EOFM_.is_EOF() && EOF_end_index_ <= waiting_index_)
                output_.writer().close();
}

uint64_t Reassembler::bytes_pending() const
{
        return size_;
}

bool Reassembler::WindowFilter::operator()(const uint64_t& first_index, 
                                                   const std::string& data,
                                                   uint64_t& first_index_ret, 
                                                    std::string& data_ret)
{
        uint64_t end_index = first_index + data.length();
        first_index_ret = first_index;
        data_ret = data;
        //cout << "WindowFilter: " << "first_index->" << first_index << " end_index->" << end_index << endl;
        if(!data_ret.empty() && 
           (tail_index_ <= static_cast<int64_t>(first_index_ret) || 
           static_cast<int64_t>(first_index_ret + data_ret.length()) <= waiting_index_))
        {
                //cout << "out of the window" << endl;
                //cout << "waiting_index->" << waiting_index_ << " tail_index->" << tail_index_ << endl;
                return false;
        }
        else if(data_ret.empty() && 
           (tail_index_ <= static_cast<int64_t>(first_index_ret) || 
           static_cast<int64_t>(first_index_ret + data_ret.length()) < waiting_index_))
        {
                //cout << "out of the window" << endl;
                return false;
        }

        if(waiting_index_ <= static_cast<int64_t>(first_index_ret) &&
           static_cast<int64_t>(end_index) <= tail_index_)
        {
                //cout << "in the window" << endl;
                return true;
        }

        if(static_cast<int64_t>(first_index_ret) < waiting_index_)
        {
                //cout << "cut left" << endl;
                data_ret = data_ret.substr(waiting_index_ - first_index_ret);
                first_index_ret = waiting_index_;
        }

        if(tail_index_ < static_cast<int64_t>(first_index_ret + data_ret.length()))
        {
                //cout << "cut right" << endl;
                data_ret = data_ret.substr(0, tail_index_ - first_index_ret);
        }
        cout << endl;
        return true;
}

void Reassembler::EOFManager::init(uint64_t EOF_first_index, uint64_t EOF_end_index)
{
        is_init_ = true;
        EOF_first_index_ = static_cast<int64_t>(EOF_first_index);
        EOF_end_index_ = static_cast<int64_t>(EOF_end_index);
        //cout << "EOFManager init: " << "EOF_first_index->" << EOF_first_index_ << " EOF_end_index->" << EOF_end_index_ << endl;
}

bool Reassembler::EOFManager::operator()(const uint64_t& first_index,
                                                                                 const std::string& data,
                                                                                 bool is_last_substring,
                                                                                 uint64_t& first_index_ret,
                                                                                 std::string& data_ret)
{
        //uint64_t end_index = first_index + data.length();
        first_index_ret = first_index;
        data_ret = data;

        //cout << "EOFManager: " << "first_index->" << first_index << " end_index->" << end_index << endl;
        if(is_init_)
        {
                //cout << "EOF_first_index->" << EOF_first_index_ << " EOF_end_index->" << EOF_end_index_ << endl;
        }


        if(!is_init_ && !is_last_substring)
        {
                //cout << "not init!" << endl;
                return true;
        }

        if(!is_init_ && is_last_substring)
        {
                //cout << "begin to init!" << endl;
                init(first_index_ret, first_index_ret + data.length());
        }

        if(data.length() == 0 && cache_size_ == 0)
        {
                //cout << "special, data is empty" << endl;
                output_.writer().close();
                return false;
        }

        if(EOF_end_index_ <= static_cast<int64_t>(first_index_ret))
        {
                //cout << "out of EOF" << endl;
                return false;
        }

        if(EOF_end_index_ < static_cast<int64_t>(first_index_ret + data_ret.length()))
        {
                //cout << "cut because of EOF" << endl;
                data_ret = data_ret.substr(0, EOF_end_index_ - first_index_ret);
        }
        cout << endl;
        return true;
}

const int64_t& Reassembler::EOFManager::get_EOF_first() const
{
        return EOF_first_index_;
}
const int64_t& Reassembler::EOFManager::get_EOF_end() const
{
        return EOF_end_index_;
}

bool Reassembler::EOFManager::is_EOF() const
{
        return is_init_;
}

Reassembler::writer_& Reassembler::CacheManager::writer()
{
        return static_cast<writer_&>(*this);
}

Reassembler::cache_in_& Reassembler::CacheManager::cachin()
{
        return static_cast<cache_in_&>(*this);
}

bool Reassembler::writer_::write()
{
        while(!cache_.empty())
        {
                //cout << "begin to write!" << endl;
                if(cache_.empty())
                {
                        //cout << "cache is empty!" << endl;
                        return false;
                }

                if(static_cast<int64_t>(std::get<0>(*cache_.begin())) != waiting_index_)
                {
                        //cout << "the first cache's first index is not waiting_index" << endl;
                        return false;
                }

                //cout << "get " << endl;
                uint64_t cur_data_len = std::get<2>(*cache_.begin()).length();
                std::string push_str = std::get<2>(*cache_.begin()).substr
                        (0, output_.writer().available_capacity() > cur_data_len ? cur_data_len : output_.writer().available_capacity());
                std::string leave_str = std::get<2>(*cache_.begin()).substr
                        (output_.writer().available_capacity() > cur_data_len ? cur_data_len : output_.writer().available_capacity());
                //cout << "push " << endl;
                output_.writer().push(push_str);
                size_ -= std::get<2>(*cache_.begin()).length();
                waiting_index_ += push_str.length();
                cache_.erase(cache_.begin());
                if(!leave_str.empty())
                {
                        //cout << "leave_str.length: " << leave_str.length() << endl;
                        size_ += leave_str.length();
                        cache_.insert(std::tuple<int64_t, int64_t, std::string>(waiting_index_, waiting_index_ + leave_str.length(), leave_str));
                        //cout << "insert back" << endl;

                }
        }
        cout << endl;
        return true;
}

bool Reassembler::cache_in_::cache_in(uint64_t first_index, std::string data)
{
        //cout << "begin to cache in" << endl;
        uint64_t end_index = first_index + data.length();
        //cout << "first_index->" << first_index << " end_index->" << end_index << endl;
        for(auto it = cache_.begin(); it != cache_.end(); )
        {
                uint64_t cur_first_index = std::get<0>(*it);
                uint64_t cur_end_index = std::get<1>(*it);
                //cout << "cur_first_index->" << cur_first_index << " cur_end_index->" << cur_end_index << endl;

                // 新字符串被已有字符串包含
                if(cur_first_index <= first_index && 
                   end_index <= cur_end_index)
                {
                        return false;
                }
                // 新字符串包含已有字符串
                else if(first_index <= cur_first_index && 
                                cur_end_index <= end_index)
                {
                        //cout << "新字符串包含已有字符串" << endl;
                        size_ -= get<2>(*it).length();
                        it = cache_.erase(it);
                }
                // 新字符串在已有字符串左侧且部分重叠
                else if(first_index < cur_first_index && 
                                cur_first_index < end_index && 
                                end_index < cur_end_index)
                {
                        //cout << "新字符串在已有字符串左侧且部分重叠" << endl;
                        data = data + get<2>(*it).substr(end_index - cur_first_index);
                        end_index = first_index + data.length();
                        size_ -= get<2>(*it).length();
                        cache_.erase(it);
                        break;
                }
                // 新字符串在已有字符串右侧且部分重叠
                else if(cur_first_index < first_index && 
                                first_index < cur_end_index &&
                                cur_end_index < end_index)
                {
                        //cout << "新字符串在已有字符串右侧且部分重叠" << endl;
                        data = get<2>(*it) + data.substr(cur_end_index - first_index);
                        first_index = cur_first_index;
                        size_ -= get<2>(*it).length();
                        it = cache_.erase(it);
                }
                // 新字符串在已有字符串左侧且不重叠
                else if(end_index <= cur_first_index)
                {
                        //cout << "新字符串在已有字符串左侧且不重叠" << endl;
                        break;
                }
                // 新字符串在已有字符串右侧且不重叠
                else
                {
                        //cout << "新字符串在已有字符串右侧且不重叠" << endl;
                        it++;
                }
        }
        cache_.insert(std::tuple<int64_t, int64_t, std::string>(first_index, end_index, data));
        size_ += data.length();
        //cout << "insert: " << "first_index->" << first_index << " end_index->" << end_index << endl;
        return true;
}
```

* 当然,我也看到了很多前辈这里用的是`deque`,相比之下,`deque`的效率当然会更高一些,毕竟`deque`可以用下标访问,并且不用像我写的这样,把去重写得这么复杂,在这里,`deque`几乎是最优解,下标访问速度快,对于头插尾插的效率更是顶中顶,对于这种可能先插中间后插两边的场景可是非常合适,同时因为支持下标访问,每个字节都能一一对应到下标,那么去重就会变得非常简单
* 那么为什么我用了`map`和`set`呢?答案是我忘记了有`deque`这个冷门结构了,在`cppreference`里挑了半天,连`priority_queue`都考虑了一下,愣是没想到用`deque`,等到后面万一效率不够再改吧,现在暂时先这样吧QAQ