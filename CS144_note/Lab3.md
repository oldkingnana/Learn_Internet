### 0 吐槽

* 那么这篇类似于小结的玩意是在完成`Lab3`之后写的
* 和很多前辈一样,我自己在写这个`Lab`的时候也觉得在面向测试编程
* 老实说我觉得`Lab3`会比`Lab2`要更难一些,至少我自己写的时候花了太多时间用来`debug`了
* 如果没有看那个`check3.pdf`文档的话(或者说如果只是草草看了一眼文档的话),会有一些需求其实是不明确的,比方说那个"单字节探测"的玩意,这个东西如果刚开始写的时候不够重视,后期可能会导致需要重构很多代码逻辑
* 至少是长记性了,文档一定要好好啃一遍

* 不过逻辑的复杂程度,我觉得其实反倒没有`Lab2`要高,基本上`debug`都在考虑边界问题和某些特殊情况,也就是所谓"面向测试编程"

### 1 源代码详解

```CPP
// src/tcp_sender.hh

#pragma once

#include "byte_stream.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"

#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <queue>

#include <deque>

#include <iostream>

class TCPSender
{
public:
        /* Construct TCP sender with given default Retransmission Timeout and possible ISN */
        TCPSender( ByteStream&& input, Wrap32 isn, uint64_t initial_RTO_ms )
    : input_( std::move( input ) )
        , isn_( isn )
        , initial_RTO_ms_( initial_RTO_ms )
        , CRT_(0)
        , SeqFNum_(0)
        , next_seq_(isn.unwrap(isn, 0))
        // , RTO_(initial_RTO_ms)
        , RWSize_(0)
        , last_ackno_(0)
        , is_FINed_(false)
        , is_SYNed_(false)
        , should_RST_(false)
        , zero_window_probe_(false)
        , is_zero_window_probeing_(false)
        , zero_window_probe_msg_({}) // 窗口
        , window_({})
        , RT_(initial_RTO_ms_)
        {
                //std::cout << "TCPSender init!" << std::endl;
        }


        // ***** public function *****

        /* Generate an empty TCPSenderMessage */
        TCPSenderMessage make_empty_message() const;

        /* Receive and process a TCPReceiverMessage from the peer's receiver */
        void receive( const TCPReceiverMessage& msg );

        /* Type of the `transmit` function that the push and tick methods can use to send messages */
        using TransmitFunction = std::function<void( const TCPSenderMessage& )>;

        /* Push bytes from the outbound stream */
        void push( const TransmitFunction& transmit );

        /* Time has passed by the given # of milliseconds since the last time the tick() method was called */
        void tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit );

        // Accessors
        uint64_t sequence_numbers_in_flight() const;  // How many sequence numbers are outstanding?
        uint64_t consecutive_retransmissions() const; // How many consecutive *re*transmissions have happened?
    
        Writer& writer() { return input_.writer(); }
    const Writer& writer() const { return input_.writer(); }
  
    // Access input stream reader, but const-only (can't read from outside)
    const Reader& reader() const { return input_.reader(); }

private: 


        // ***** simple mode *****

        void push_simple_mode_( const TransmitFunction& transmit );

        void receive_simple_mode_(const TCPReceiverMessage& msg);

        void tick_simple_mode_(uint64_t ms_since_last_tick, const TransmitFunction& transmit);

        void retransmit_simple_mode_(const TransmitFunction& transmit, TCPSenderMessage TCPSdMsg);


        // ***** zero window probe mode *****

        void push_ZWP_mode_(const TransmitFunction& transmit );

        void receive_ZWP_mode_(const TCPReceiverMessage& msg);

        void tick_ZWP_mode_(uint64_t ms_since_last_tick, const TransmitFunction& transmit);

        void retransmit_ZWP_mode_(const TransmitFunction& transmit, TCPSenderMessage TCPSdMsg);


        // ***** inner class *****

        class RetransmissionTimer
        {
        public:
                RetransmissionTimer(uint64_t initial_RTO_ms_)
                : ms_time_(0)
                , RTO_(initial_RTO_ms_)
                {}

                ~RetransmissionTimer()
                {}

				// 超时判断
                bool is_timeout(); 

				// 时间增长
                void time_plus(const uint64_t& ms_since_last_tick);

				// 重置时间
                void time_reset();

				// 重置超时标准时间
                void RTO_reset(uint64_t initial_RTO_ms_);

				// 翻倍超时标准时间
                void RTO_multi();

				// 获取当前时间
                uint64_t get_time();

				// 获取当前超时标准时间
                uint64_t get_RTO();

        private:
                uint64_t ms_time_; // 时间
                uint64_t RTO_; // 超时标准时间
        };

private:
    // Variables initialized in constructor
    ByteStream input_;
    Wrap32 isn_;
    uint64_t initial_RTO_ms_;

        uint64_t CRT_; // consecutive retransmission times  最近超时次数
        uint64_t SeqFNum_; // seq in flight number  正在传输的字节数

        uint64_t next_seq_; // 下一个包的seq
        uint64_t RWSize_; // 接收方窗口大小
        uint64_t last_ackno_; // 最近一次接收的ACK的绝对位置

        bool is_FINed_; // 是否曾经FIN过
        bool is_SYNed_; // 是否曾经SYN过
        bool should_RST_; // 单次RST标记

        bool zero_window_probe_; // 单字节探测状态标记
        bool is_zero_window_probeing_; // 是否正在做单字节探测
        std::deque<TCPSenderMessage> zero_window_probe_msg_; // 单字节探测专用的容器 

        std::deque<TCPSenderMessage> window_; // 窗口
        RetransmissionTimer RT_; // 重传计时器
};
```


```CPP
#include "tcp_sender.hh"
#include "tcp_config.hh"
#include <cstdio>
#include <ostream>

using namespace std;

// =================== TCPSender ======================


// ***** public function *****

// 生成一个空的msg  
TCPSenderMessage TCPSender::make_empty_message() const
{
        TCPSenderMessage TCPSdMsg;
        TCPSdMsg.seqno = Wrap32::wrap(next_seq_, isn_);
		// RST检查
        if(should_RST_)
                TCPSdMsg.RST = true;
        if(input_.reader().has_error())
                TCPSdMsg.RST = true;
        return TCPSdMsg;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
        // 进入单字节探测模式
        if(msg.window_size == 0)
        {
                receive_ZWP_mode_(msg);
        }
        // 进入正常模式
        else if(msg.window_size != 0)
        {
                receive_simple_mode_(msg);
        }
}

void TCPSender::push( const TransmitFunction& transmit )
{
		// 调用单字节探测模式下的push
        if(zero_window_probe_ && !is_zero_window_probeing_)
        {
                push_ZWP_mode_(transmit);
        }
		// 调用普通模式下的push
        else if(!zero_window_probe_)
        {
				// 老实说这里写得还不够优雅,但是懒得改了,就这样吧
		
                // send SYN
                if(!is_SYNed_)
                        push_simple_mode_(transmit);

                while(static_cast<int64_t>(RWSize_ - SeqFNum_) > 0 && !is_FINed_ && input_.reader().bytes_buffered() > 0)
                        push_simple_mode_(transmit);

                // send FIN
                if(!is_FINed_ && RWSize_ - SeqFNum_ > 0 && input_.reader().is_finished() && input_.reader().bytes_buffered() == 0)
                        push_simple_mode_(transmit);
        }
        else if(zero_window_probe_ && is_zero_window_probeing_)
        {}
        else 
        {
                return ;
        }
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
        // 单字节探测模式
        if(zero_window_probe_)
                tick_ZWP_mode_(ms_since_last_tick, transmit);
        // 普通模式
        else 
                tick_simple_mode_(ms_since_last_tick, transmit);
}

uint64_t TCPSender::sequence_numbers_in_flight() const
{
        return SeqFNum_;
}

uint64_t TCPSender::consecutive_retransmissions() const 
{
        return CRT_;
}


// ***** simple mode *****

void TCPSender::push_simple_mode_( const TransmitFunction& transmit )
{
        TCPSenderMessage newTCPSdMsg = make_empty_message();

		// 塞SYN
        if(!is_SYNed_)
        {
                newTCPSdMsg.SYN = true;
                is_SYNed_ = true;
        }

        // 向msg塞入足量的数据
        if(0 < RWSize_ - SeqFNum_ - newTCPSdMsg.sequence_length() && RWSize_ - SeqFNum_ -newTCPSdMsg.sequence_length() <= 1000) 
        {
                newTCPSdMsg.payload = input_.reader().peek().substr(0, RWSize_ - SeqFNum_);
                input_.reader().pop(RWSize_ - SeqFNum_);
        }
        else if(1000 < RWSize_ - SeqFNum_ - newTCPSdMsg.sequence_length()) 
        {
                newTCPSdMsg.payload = input_.reader().peek().substr(0, 1000);
                input_.reader().pop(1000);
        }

		// 塞FIN
        if(!is_FINed_ && RWSize_ - SeqFNum_ - newTCPSdMsg.sequence_length() > 0 && input_.reader().is_finished() && input_.reader().bytes_buffered() == 0)
        {
                newTCPSdMsg.FIN = true;
                is_FINed_ = true;
        }

        if(newTCPSdMsg.sequence_length() == 0)
                return ;

        // 更新公共资源
        next_seq_ += newTCPSdMsg.sequence_length();
        SeqFNum_ += newTCPSdMsg.sequence_length();

        transmit(newTCPSdMsg);
        // 在窗口塞入传输的msg
        window_.push_back(newTCPSdMsg);

		// 如果当前包存在RST,无论如何都把标记位设置为false(标记为不会影响因为字节流导致的RST)
        if(newTCPSdMsg.RST)
                should_RST_ = false;
}


void TCPSender::receive_simple_mode_(const TCPReceiverMessage& msg)
{
		// 如果接收方存在RST,那么一定要把字节流设置成error,因为接收方说数据出错了,那么字节流一定出了问题
        if(msg.RST)
                input_.reader().set_error();

        // 如果上一个包是单字节探测包
        if(zero_window_probe_ == true)
        {
				// 更新相关的公共资源
                RWSize_ = static_cast<uint64_t>(msg.window_size);
                SeqFNum_ -= 1;
                zero_window_probe_ = false;
                is_zero_window_probeing_ = false;
                RT_.time_reset();
                zero_window_probe_msg_.pop_back();
                last_ackno_ = (*msg.ackno).unwrap(isn_, last_ackno_);
        }
        // 如果上一个包不是单字节探测包
        else
        {
                RWSize_ = static_cast<uint64_t>(msg.window_size);
                uint64_t ackno_u64 = (*msg.ackno).unwrap(isn_, last_ackno_);

                // ACK合法性判断
                // 超了--无效
                if(ackno_u64 > next_seq_)
                {}
                // 如果已经有内容被接收了
                else if(ackno_u64 > next_seq_ - SeqFNum_)
                {
                        // 将已经接收的msg从窗口移除
                        for(auto it = window_.begin() ; it != window_.end() ; )
                        {
                                uint64_t seqno_u64 = it->seqno.unwrap(isn_, last_ackno_);

                                if(seqno_u64 + it->sequence_length() <= ackno_u64)
                                        it = window_.erase(it);
                                else 
                                        break;
                        }
                        // 重置公共资源
                        CRT_ = 0;
                        RT_.RTO_reset(initial_RTO_ms_);
                        SeqFNum_ -= ackno_u64 - last_ackno_; 

                        // 重置时间
                        RT_.time_reset();

                        last_ackno_ = (*msg.ackno).unwrap(isn_, last_ackno_);
                }
                // 太少--无效
                else 
                {}
        }
        zero_window_probe_ = false;
}

void TCPSender::tick_simple_mode_(uint64_t ms_since_last_tick, const TransmitFunction& transmit)
{
        if(window_.size() == 0)
                return ;

        // 时间流逝
        RT_.time_plus(ms_since_last_tick);

        // 超时重传
        if(RT_.is_timeout())
                retransmit_simple_mode_(transmit, window_[0]);
}

void TCPSender::retransmit_simple_mode_(const TransmitFunction& transmit, TCPSenderMessage TCPSdMsg)
{
        // 重传
        transmit(TCPSdMsg);
        // 公共资源更新
        CRT_++;
        RT_.RTO_multi();
        RT_.time_reset();
}


// ***** zero window probe mode *****

void TCPSender::push_ZWP_mode_(const TransmitFunction& transmit )
{
        TCPSenderMessage newTCPSdMsg = make_empty_message();

        if(window_.size())
        {
                auto it = window_.begin();

                // 从window获取一个字节的有效数据
                // 别忘记从window删掉被获取的那个数据
                if(it->SYN == true)
                {
                        newTCPSdMsg.SYN = true;
                        it->SYN = false;
                }
                else if(window_[0].payload.length() != 0)
                {
                        newTCPSdMsg.payload = it->payload.substr(0, 1);
                        it->payload = it->payload.substr(0, 1);
                }
                else if(window_[0].FIN == true)
                {
                        newTCPSdMsg.FIN = true;
                        it->FIN = false;
                }
                else
                {
                }

                // window_[0]有效检查
                if(window_[0].sequence_length() == 0)
                        window_.erase(window_.begin());
        }
		// 如果窗口里没有数据,那就从字节流获取一个字节的数据出来
        else // window_.size() == 0
        {
                if(is_SYNed_ == false)
                {
                        newTCPSdMsg.SYN = true;
                        is_SYNed_ = true;
                }
                else if(input_.reader().bytes_buffered())
                {
                        newTCPSdMsg.payload = input_.reader().peek().substr(0, 1);
                        input_.reader().pop(1);
                }
                else if(input_.reader().is_finished() && !is_FINed_)
                {
                        newTCPSdMsg.FIN = true;
                        is_FINed_ = true;
                }
                else 
                {
                }

                if(newTCPSdMsg.sequence_length() != 1)
                        return ;

                next_seq_ += 1;
                SeqFNum_ += 1;
        }
        is_zero_window_probeing_ = true;

        transmit(newTCPSdMsg);
        zero_window_probe_msg_.push_back(newTCPSdMsg);
        if(newTCPSdMsg.RST)
                should_RST_ = false;
        if(zero_window_probe_msg_.size() > 1)
        {
                return ;
        }
}

void TCPSender::receive_ZWP_mode_(const TCPReceiverMessage& msg)
{
        if(msg.RST)
                input_.reader().set_error();

        if(zero_window_probe_)
        {
				// 重置相关的公共资源
                is_zero_window_probeing_ = false;
                SeqFNum_ -= 1;
                RT_.time_reset();
                zero_window_probe_msg_.pop_back();
                last_ackno_ = (*msg.ackno).unwrap(isn_, last_ackno_);
        }
        else 
        {
                RWSize_ = 0;
                zero_window_probe_ = true;

                uint64_t ackno_u64 = (*msg.ackno).unwrap(isn_, last_ackno_);

                // ACK合法性判断
                // 超了--无效
                if(ackno_u64 > next_seq_)
                {}
                // 如果已经有内容被接收了
                else if(ackno_u64 > next_seq_ - SeqFNum_)
                {
                        // 将已经接收的msg从窗口移除
                        for(auto it = window_.begin() ; it != window_.end() ; )
                        {
                                uint64_t seqno_u64 = it->seqno.unwrap(isn_, last_ackno_);

                                if(seqno_u64 + it->sequence_length() <= ackno_u64)
                                        it = window_.erase(it);
                                else 
                                        break;
                        }
                        // 重置公共资源
                        CRT_ = 0;
                        RT_.RTO_reset(initial_RTO_ms_);
                        SeqFNum_ -= ackno_u64 - last_ackno_; 

                        // 重置时间
                        RT_.time_reset();

                        last_ackno_ = (*msg.ackno).unwrap(isn_, last_ackno_);
                }
                // 太少--无效
                else 
                {}
        }
}

void TCPSender::tick_ZWP_mode_(uint64_t ms_since_last_tick, const TransmitFunction& transmit)
{
        if(zero_window_probe_msg_.size() == 0)
                return ;
        // 时间流逝
        RT_.time_plus(ms_since_last_tick);
        if(RT_.is_timeout())
                retransmit_ZWP_mode_(transmit, zero_window_probe_msg_[0]);
}

void TCPSender::retransmit_ZWP_mode_(const TransmitFunction& transmit, TCPSenderMessage TCPSdMsg)
{
        transmit(TCPSdMsg);
        RT_.time_reset();
}


// ============== RetransmissionTimer =================

bool TCPSender::RetransmissionTimer::is_timeout()
{
        return ms_time_ >= RTO_;
}

void TCPSender::RetransmissionTimer::time_plus(const uint64_t& ms_since_last_tick)
{
        ms_time_ += ms_since_last_tick;
}

void TCPSender::RetransmissionTimer::time_reset()
{
        ms_time_ = 0;
}

void TCPSender::RetransmissionTimer::RTO_reset(uint64_t initial_RTO_ms_)
{
        RTO_ = initial_RTO_ms_;
}

void TCPSender::RetransmissionTimer::RTO_multi()
{
        RTO_ *= 2;
}

uint64_t TCPSender::RetransmissionTimer::get_time()
{
        return ms_time_;
}

uint64_t TCPSender::RetransmissionTimer::get_RTO()
{
        return RTO_;
}
```

### 2 坑

#### 2.1 不能过于细化窗口

* 准确来说,窗口里面不能是一个一个的字节,或者说窗口不能仅仅只是一个`std::string`,窗口的移动一定以包为最小单位移动
* 这意味着如果一个包被接收了一半,我们也不能把它分割成已经被接收和没被接收的部分,然后把它已经被接收的部分从窗口中剔除,这是非法的操作!!!
* 当然也更不能把接收了一半的包直接从窗口中剔除
* 有且仅有一种情况可以从窗口移除一个包,即这个包被完整的接收才可以从窗口中移除它
* 如果一个接收了一半的包需要重传,直接重传这个完整的包过去就行

#### 2.2 单字节探测传输的数据仍然是有效数据

* 单字节探测发送的那一个字节的数据仍然是有效数据,所以单字节探测发送的包不仅仅有窗口探测意义,仍然具有数据传输的意义
* 所以当一个单字节探测的包被接收之后,该更新的公共资源,类似于`SeqFNum_`,`next_seq_`,`last_ackno_`等等这种资源一定都要更新
