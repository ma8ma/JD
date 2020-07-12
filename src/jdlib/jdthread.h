// ライセンス: GPL2

// スレッドクラス

#ifndef _JDTHREAD_H
#define _JDTHREAD_H

#include <thread>


typedef void* ( *STARTFUNC )( void * );

enum
{
    DEFAULT_STACKSIZE = 64
};

namespace JDLIB
{
    enum
    {
        DETACH = true,
        NODETACH = false
    };

    class Thread
    {
        std::thread m_thread;

    public:

        Thread() noexcept = default;
        virtual ~Thread();

        bool is_running() const { return m_thread.get_id() != std::thread::id(); }

        // スレッド作成
        bool create( STARTFUNC func , void * arg, const bool detach, const int stack_kbyte = DEFAULT_STACKSIZE );

        bool join();
    };
}

#endif
