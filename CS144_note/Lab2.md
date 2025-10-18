### 1 吐槽

* 那么,这里就是`Lab2`的内容了,有一说一,因为我是初学老外的课程,所以感觉老外的课程确实严谨很多,配套的东西是真全啊
* 这个`Lab2`,我的体感会比`Lab1`好实现很多,主要是思路清晰,debug也很方便

### 2 测试结果

```shell
oldking@iZwz9b2bj2gor4d8h3rlx0Z:~/CS144-2024-winter-backup$ cmake --build build_check2_2 --target check2
Test project /home/oldking/CS144-2024-winter-backup/build_check2_2
      Start  1: compile with bug-checkers
 1/29 Test  #1: compile with bug-checkers ........   Passed   19.40 sec
      Start  3: byte_stream_basics
 2/29 Test  #3: byte_stream_basics ...............   Passed    0.02 sec
      Start  4: byte_stream_capacity
 3/29 Test  #4: byte_stream_capacity .............   Passed    0.03 sec
      Start  5: byte_stream_one_write
 4/29 Test  #5: byte_stream_one_write ............   Passed    0.02 sec
      Start  6: byte_stream_two_writes
 5/29 Test  #6: byte_stream_two_writes ...........   Passed    0.02 sec
      Start  7: byte_stream_many_writes
 6/29 Test  #7: byte_stream_many_writes ..........   Passed    0.10 sec
      Start  8: byte_stream_stress_test
 7/29 Test  #8: byte_stream_stress_test ..........   Passed    0.05 sec
      Start  9: reassembler_single
 8/29 Test  #9: reassembler_single ...............   Passed    0.05 sec
      Start 10: reassembler_cap
 9/29 Test #10: reassembler_cap ..................   Passed    0.04 sec
      Start 11: reassembler_seq
10/29 Test #11: reassembler_seq ..................   Passed    0.06 sec
      Start 12: reassembler_dup
11/29 Test #12: reassembler_dup ..................   Passed    0.08 sec
      Start 13: reassembler_holes
12/29 Test #13: reassembler_holes ................   Passed    0.02 sec
      Start 14: reassembler_overlapping
13/29 Test #14: reassembler_overlapping ..........   Passed    0.03 sec
      Start 15: reassembler_win
14/29 Test #15: reassembler_win ..................   Passed    1.92 sec
      Start 16: wrapping_integers_cmp
15/29 Test #16: wrapping_integers_cmp ............   Passed    0.04 sec
      Start 17: wrapping_integers_wrap
16/29 Test #17: wrapping_integers_wrap ...........   Passed    0.02 sec
      Start 18: wrapping_integers_unwrap
17/29 Test #18: wrapping_integers_unwrap .........   Passed    0.02 sec
      Start 19: wrapping_integers_roundtrip
18/29 Test #19: wrapping_integers_roundtrip ......   Passed    3.06 sec
      Start 20: wrapping_integers_extra
19/29 Test #20: wrapping_integers_extra ..........   Passed    1.22 sec
      Start 21: recv_connect
20/29 Test #21: recv_connect .....................   Passed    0.07 sec
      Start 22: recv_transmit
21/29 Test #22: recv_transmit ....................   Passed    1.34 sec
      Start 23: recv_window
22/29 Test #23: recv_window ......................   Passed    0.03 sec
      Start 24: recv_reorder
23/29 Test #24: recv_reorder .....................   Passed    0.03 sec
      Start 25: recv_reorder_more
24/29 Test #25: recv_reorder_more ................   Passed    3.21 sec
      Start 26: recv_close
25/29 Test #26: recv_close .......................   Passed    0.04 sec
      Start 27: recv_special
26/29 Test #27: recv_special .....................   Passed    0.07 sec
      Start 37: compile with optimization
27/29 Test #37: compile with optimization ........   Passed    2.65 sec
      Start 38: byte_stream_speed_test
             ByteStream throughput: 2.27 Gbit/s
28/29 Test #38: byte_stream_speed_test ...........   Passed    0.34 sec
      Start 39: reassembler_speed_test
             Reassembler throughput: 0.73 Gbit/s
29/29 Test #39: reassembler_speed_test ...........   Passed    0.68 sec

100% tests passed, 0 tests failed out of 29

Total Test time (real) =  34.71 sec
Built target check2
```

### 3 源代码&注释

```CPP
// src/wrapping_integers.hh
// 这里不需要做修改

#pragma once

#include <cstdint>
#include <cmath>

/*
 * The Wrap32 type represents a 32-bit unsigned integer that:
 *    - starts at an arbitrary "zero point" (initial value), and
 *    - wraps back to zero when it reaches 2^32 - 1.
 */

class Wrap32
{
public:
  explicit Wrap32( uint32_t raw_value ) : raw_value_( raw_value ) {}

  /* Construct a Wrap32 given an absolute sequence number n and the zero point. */
  static Wrap32 wrap( uint64_t n, Wrap32 zero_point );

  /*
   * The unwrap method returns an absolute sequence number that wraps to this Wrap32, given the zero point
   * and a "checkpoint": another absolute sequence number near the desired answer.
   *
   * There are many possible absolute sequence numbers that all wrap to the same Wrap32.
   * The unwrap method should return the one that is closest to the checkpoint.
   */
  uint64_t unwrap( Wrap32 zero_point, uint64_t checkpoint ) const;

  Wrap32 operator+( uint32_t n ) const { return Wrap32 { raw_value_ + n }; }
  bool operator==( const Wrap32& other ) const { return raw_value_ == other.raw_value_; }

protected:
  uint32_t raw_value_ {};
};
```


```CPP
// src/wrapping_integers.cc

#include "wrapping_integers.hh"
#include <cstdint>
#include <iostream>

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
        return Wrap32(n) + zero_point.raw_value_;
}

// 我在写这个部分的时候,其实最开始想的是暴力解,因为我要找raw_value_的绝对位置,那么首要就是要判断这绝对位置究竟是哪个2^32的绝对位置
// 那么,我们已知了一个checkpoint作为标记点,所以我们需要找离checkpoint最近位置(这里不完全对)的raw_value_的绝对位置
// 我们通过raw_value_搞两个候选位置,一个left,一个right,只要这两个点一个在checkpoint左边,一个在checkpoint右边,那么其中距离最短的就是那个绝对位置
// 于是暴力解思路就是直接无脑加2^32,直到left在checkpoint的左边,right在checkpoint的右边,然后开始比较
// 但是吧,这样总归有些不妥,毕竟时间复杂度还是比较高
// 于是我想到了一个时间复杂度是常数级的办法,也就是下面这个办法
// 思路也不难,我们知道,如果要left和right不断接近checkpoint,需要已知遍历,这很费时间
// 那么如果left和right不动,让checkpoint逼近他俩,还会费时间吗?答案是不会,是完全常数级的
// 解决办法很简单,因为left和right一开始永远在第一个2^32或者第二个2^32里,所以我们完全可以给checkpoint取模,以接近left和right,这里我取模的方法使用的按位与
// 为什么不使用 % pow(left, right)?
// 热知识: pow返回一个double,而一但是浮点数,就会造成精度丢失,所以这里其实不如用按位与
// 此时,我们就能让checkpoint接近left和right了,取模后的checkpoint记作checkpoint_low
// 比较完之后,我们发现一个很恶心的事情,我们得到的最短的那个left/right,他仍然是一个相对位置,怎么让他回到绝对位置?
// 方法也很简单,我们知道,无论对于left还是right还是checkpoint_low,他们距离自己的绝对位置的距离一定是固定的,我们这里知道了checkpoint和checkpoint_low,那么得到"距离"就是轻而易举的事情,只需要让left/right + (checkpoint - checkpoint_low)就行了
// 那么很好,你得到了这个应该返回的绝对位置了,对吗?
// 还没完,这还只是冰山一角,我们需要多关注细节
uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
		// 初始化left和right,因为left和right都用的是相对位置,也就是低32位,所以这里我记作left_low和right_low
        uint64_t left_low = 0;
        uint64_t right_low = 0;

		// 因为存在随机的ISN,所以我们需要作归零
        if(raw_value_ >= zero_point.raw_value_)
        {
				// 这里就是常规的归零操作,即当前为转换的值减去零点位置,另外,right会比left大2^32,也就是一个周期,所以要加上2^32
                left_low = static_cast<uint64_t>(raw_value_) - zero_point.raw_value_;
                right_low = static_cast<uint64_t>(raw_value_) - zero_point.raw_value_ + 0xFFFFFFFFULL + 1ULL;
        }
        else 
        {
				// 有一种情况比较特殊,这个未转换的raw_value_的值有可能会比零点位置小(意味着当前这个点的绝对位置不可能处于零点位置所在的周期),所以我们得往后挪2^32,至于left为什么还要取低32位,理由很简单,就是减完之后left仍然不一定小于2^32,所以我们要取模保证其小于2^32
                left_low = (((static_cast<uint64_t>(raw_value_) + 0xFFFFFFFFULL) + 1ULL) - zero_point.raw_value_) & 0x00000000FFFFFFFFULL;
                right_low = left_low + 0xFFFFFFFFULL + 1ULL;
        }

		// 按位与取模
        uint64_t checkpoint_low = checkpoint & 0x00000000FFFFFFFFULL;

		// 那么,你能看到下面这个语句很奇怪,我似乎定义了一个左窗口和右窗口的东西,为什么要这么定义??因为有案例,在注释里讲起来很麻烦,所以你可以直接翻到对应章节浏览,这里建议直接去看这个章节
		// 这里是个天坑,如果你不使用ULL(Unsigned Long Long)直接做比较,那么0x80000000会默认被当作int32_t处理,但是int32_t是有符号的,于是就始终走不到0ULL那边去,那么养成好习惯,记得加ULL
		uint64_t checkpoint_low_lwindow = (checkpoint_low > 0x80000000ULL) ? (checkpoint_low - 0x80000000ULL) : 0ULL;
        uint64_t checkpoint_low_rwindow = checkpoint_low + 0x80000000ULL;

		// 窗口修正
        if(checkpoint_low_rwindow <= left_low)
        {
                checkpoint_low += 0xFFFFFFFFULL + 1ULL;
                checkpoint_low_lwindow = checkpoint_low - 0x80000000ULL;
                checkpoint_low_rwindow = checkpoint_low + 0x80000000ULL;
        }

        if(checkpoint_low_lwindow <= left_low && left_low < checkpoint_low_rwindow)
                return left_low + (checkpoint > checkpoint_low ? (checkpoint - checkpoint_low) : 0);
        else
                return right_low + (checkpoint > checkpoint_low ? (checkpoint - checkpoint_low) : 0);

        return {};
}
```

```CPP
// src/tcp_receiver.hh

#pragma once

#include "reassembler.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"
#include <cstdint>
#include "wrapping_integers.hh"

class TCPReceiver
{
public:
  // Construct with given Reassembler
  explicit TCPReceiver( Reassembler&& reassembler ) 
          : reassembler_( std::move( reassembler ) ) 
          , zero_point(100) // 这里按理说应该搞一个随机值
          , checkpoint(0)
          , SYN(0)
          , SYN_before(0)
          , FIN(0)
          , need_FIN(0)
        {}

  /*
   * The TCPReceiver receives TCPSenderMessages, inserting their payload into the Reassembler
   * at the correct stream index.
   */
  void receive( TCPSenderMessage message );

  // The TCPReceiver sends TCPReceiverMessages to the peer's TCPSender.
  TCPReceiverMessage send() const;

  // Access the output (only Reader is accessible non-const)
  const Reassembler& reassembler() const { return reassembler_; }
  Reader& reader() { return reassembler_.reader(); }
  const Reader& reader() const { return reassembler_.reader(); }
  const Writer& writer() const { return reassembler_.writer(); }

private:
        Reassembler reassembler_;

        Wrap32 zero_point; // ISN
        uint64_t checkpoint; // stream index, When actually used, it should be used after the self-decrement operation
        uint32_t SYN; // 记录当前是否建立通信
        uint32_t SYN_before; // 记录曾经有没有过通信
        uint32_t FIN; // 记录是否已经结束通信请求
        uint32_t need_FIN; // 记录是否需要结束通信
};

```

```CPP
// src/tcp_receiver.cc

#include "tcp_receiver.hh"
#include "wrapping_integers.hh"
#include <cstdint>
#include <iostream>

using namespace std;

// 接收
void TCPReceiver::receive( TCPSenderMessage message )
{
		// 如果一个包在没有建立连接的时候就发过来,且这个包也没有携带SYN,那么直接设置错误,因为这个包不合法
        if(message.SYN == false && SYN == 0)
                reassembler_.reader().set_error();
		// SYN初始化
        if(message.SYN && SYN == 0)
        {
				// FIN与SYN重置
                if(reassembler_.writer().is_closed() && FIN == 1 && SYN == 1)
                {
                        FIN--;
                        SYN--;
                }
                zero_point = message.seqno;
                SYN++;
        }
		// SYN_before初始化
        if(message.SYN && SYN_before == 0)
                SYN_before++;
        if(SYN)
        {
				// 在这里,因为reassembler的封装问题,我们很难知道这个reassembler到底有没有完全接收所有字节,所以我们只能对比传输前和传输后的字节流状态
                uint64_t old_bytes_pending = reassembler_.writer().bytes_pushed();
//              cout << "first_index" << message.seqno.unwrap(zero_point, checkpoint) << endl;
//              cout << "checkpoint" << checkpoint << endl;
				// 如果SYN和有效数据一起发过来,我们就需要在绝对位置减去一个SYN的宽度,因为SYN不计入实际位置
                if(message.SYN)
                        reassembler_.insert(message.seqno.unwrap(zero_point, checkpoint), message.payload, message.FIN);
                else
                        reassembler_.insert(message.seqno.unwrap(zero_point, checkpoint) - SYN, message.payload, message.FIN);
                uint64_t new_bytes_pending = reassembler_.writer().bytes_pushed();
                checkpoint += new_bytes_pending - old_bytes_pending;
				// 如果这个包已经说明了要结束了,我们就需要为此作准备,让need_FIN++,表示准备结束了,但因为不知道字节流有没有关闭,不知道所有字节是否已经传输完毕,所以我们要等待字节流关闭才能真正结束传输
                if(message.FIN)
                        need_FIN++;
				// 如果已经没有任何缓存的内容了,意味着不需要等待任何包了,也表示所有包已经传输完毕,那么此时就可以关闭连接了
                if(reassembler_.bytes_pending() == 0 && FIN == 0 && need_FIN == 1)
                {
                        FIN++;
                        need_FIN--;
                }
        }
}

// 发送(回复)
TCPReceiverMessage TCPReceiver::send() const
{
		// 初始化回复对象
        TCPReceiverMessage ret_msg;
        // 初始化其窗口大小
		ret_msg.window_size = reassembler_.writer().available_capacity() > UINT16_MAX ? UINT16_MAX : reassembler_.writer().available_capacity() ;
        // 如果原来接收过SYN,那么可以允许返回一个有值的ackno,如果从来没有接收过SYN,那么返回一个空ackno
		if(SYN_before)
                ret_msg.ackno = Wrap32::wrap(checkpoint + SYN + FIN, zero_point);
        else 
                ret_msg.ackno = {};
		// 如果有err,把RST设置成1
        if(reassembler_.reader().has_error())
                ret_msg.RST = 1;
        else 
                ret_msg.RST = {};
		// 返回对象
        return ret_msg;
}
```


###  5`checkpoint`的窗口问题

* 我们先来看一个错误示例,这是我的最初版本,但是过不了测试用例
```CPP
// 输出一些资源方便debug
cout << "checkpoint: " << checkpoint << endl; 
cout << "zero_point: " << zero_point.raw_value_ << endl; 
cout << "raw_value_: " << raw_value_ << endl; 
cout << "left: " << left << endl; 
cout << "right: " << right << endl; 
cout << "checkpoint_low: " << checkpoint_low << endl;

// left等价于left_low,right等价
// 我们知道,left一定是小于2^32的,也就是其值一定在一个周期内
// 同时,checkpoint_low也是一定在一个周期内的
// 如果left直接就大于checkpoint_low,那么我们默认理解为left的绝对位置也在checkpoint的绝对位置的右侧,那么他一定是最近的点(?)
if(checkpoint_low < left) 
	return left + checkpoint_low; 
else 
{ 
	// 比较最近的点
	if(checkpoint_low - left < right - checkpoint_low) 
		return left + (checkpoint - checkpoint_low); 
	else 
		return right + (checkpoint - checkpoint_low); 
}
```

* 接着我们看看报错信息
```shell
checkpoint: 12884901888 
zero_point: 0 
raw_value_: 4294967294 
left: 4294967294 
right: 8589934590 
checkpoint_low: 0 
`Wrap32( UINT32_MAX - 1 ).unwrap( Wrap32( 0 ), 3 * ( 1UL << 32 ) )` should have been `3 * ( 1UL << 32 ) - 2`, but the former is 4294967294 and the latter is 12884901886 (difference of 8589934592) 
(at line 20) 
94% tests passed, 1 tests failed out of 17 
```

* 换成更方便查看的16进制,那么各个资源应该是这样的
```text
checkpoint: 12884901888 == 3 * 2^32
zero_point: 0 == 0
raw_value_: 4294967294 == 2^32 - 2
left: 4294967294 == 2^32 - 2
right: 8589934590 == 2 * 2^32 - 2
checkpoint_low: 0 == 0
```

* 你发现了一个问题没,所以会有一种情况,对于绝对位置,`left`可能会在`checkpoint`的左边,但因为他俩的不在同一个区间,所以对于他俩的相对位置,`left`反而在`checkpoint`(也就是`checkpoint_low`)的右边,这是因为`checkpoint`转换到`checkpoint_low`的时候,多除了一个2^32

* 在这个报错中,找到`left`对应的绝对位置处于的周期是一件比较困难的事情,因为我们做这些操作的目的其实就是为了找到他的绝对位置,而修正`checkpoint_low`同样也是一件比较困难的事情,因为我们很难把控应该在什么时机修正`checkpoint_low`,因为从相对位置看`left`在`checkpoint_low`的右边,我们无法判断他俩绝对位置的情况

![[Pasted image 20251018180231.png]]

* 换句话说,我们没法仅通过`left`和`checkpoint_low`的左右位置来判断这个`checkpoint_low`是不是一个有效的`checkpoint_low`,我们迫切需要一种方式去判断这个`checkpoint_low`是否合法!!

* 那么这个方法也很简单

* 我们知道,`left`和`right`之间一定相差2^32,所以对于相对位置而言,如果`checkpoint_low`的位置合法,那么`left`和`right`中,一定会有一个点落在`[checkpoint_low - 2^31, checkpoint_low + 2^31)`这个范围内,

* 所以,我们就可以这么写
```CPP
uint64_t checkpoint_low_lwindow = (checkpoint_low > 0x80000000ULL) ? (checkpoint_low - 0x80000000ULL) : 0ULL;
uint64_t checkpoint_low_rwindow = checkpoint_low + 0x80000000ULL;

// 非法修正
if(checkpoint_low_rwindow <= left_low)
{
		checkpoint_low += 0xFFFFFFFFULL + 1ULL;
		checkpoint_low_lwindow = checkpoint_low - 0x80000000ULL;
		checkpoint_low_rwindow = checkpoint_low + 0x80000000ULL;
}

if(checkpoint_low_lwindow <= left_low && left_low < checkpoint_low_rwindow)
		return left_low + (checkpoint > checkpoint_low ? (checkpoint - checkpoint_low) : 0);
else
		return right_low + (checkpoint > checkpoint_low ? (checkpoint - checkpoint_low) : 0);
```

* 那么,我相信你一定会有亿点点问题,你肯定还是不理解为什么这么做就一定是对的??!!!
* 老实说,我们这里的过程其实有一点点不符合逻辑,但是他的结果却正好是对的,下面我会讲三个例子(两个合法`checkpoint_low`一个不合法)来解释

* 第一个例子
* 假设说我们想求这个点的绝对位置

![[Pasted image 20251018180730.png]]

* 那么因为`left`落在了`checkpoint_low`的窗口里,所以这个`checkpoint_low`是合法的,我们可以直接根据`checkpoint`和`checkpoint_low`的差值直接求出`result`位置(也就是绝对位置)

* 第二个例子

![[Pasted image 20251018181817.png]]

* 这里一开始算出来`checkpoint_low`的左窗口可能会越过下限,所以需要做限制
* 因为没有任何一个点落在窗口内,所以我们需要修正窗口
* 然后重复第一个例子的步骤

![[Pasted image 20251018181826.png]]

* 第三个例子最为特殊,也是最令人匪夷所思的例子,也是这个方法最令人疑惑的地方

![[Pasted image 20251018182125.png]]

* 问题: 为什么这个例子中,完全不需要让`left`在`checkpoint_low`的左侧而`right`在`checkpoint_low`的右侧?看上去,这里的`right`似乎什么作用得没用上?!

* 按照第一个例子的步骤,得到的`result`应该在这里
![[Pasted image 20251018182637.png]]
* 那么原因很简单,这个地方虽然使用`left`和这个看似合法的`checkpoint_low`计算,但实际上我们计算的根本就不是他俩,实际上应该计算的是`right`和现在这个真正合法的`checkpoint_low`

* 那为什么使用`left`的结果是对的?原因很简单,我们看个图就明白了
![[Pasted image 20251018183346.png]]

* 那么,`result == left + (checkpoint - checkpoint_low(不右移) ) == right + (checkpoint - checkpoint_low(右移) ) == left + 2^32 + (checkpoint - (checkpoint_low(不右移) + 2^32))`

* 所以说,因为`left`和`right`存在距离且和两个`checkpoint_low`的距离抵消了,所以从结果来看,`left`实际上也是合法的!

