// ライセンス: GPL2

//
// 2ch型ノードツリー
//

#ifndef _NODETREE2ch_H
#define _NODETREE2ch_H

#include "nodetree2chcompati.h"
#include <ctime>

namespace DBTREE
{
    class NodeTree2ch : public NodeTree2chCompati
    {
        std::string m_org_url;  // 移転前のオリジナルURL
        time_t m_since_time; // スレが立った時刻
        int m_mode; // 読み込みモード
        int m_res_number_max; // 最大レス数

        std::string m_operate_info;
        
      public:

        NodeTree2ch( const std::string& url, const std::string& org_url,
                     const std::string& date_modified, time_t since_time );
        ~NodeTree2ch();

        int get_res_number_max() override { return m_res_number_max; }

      protected:

        char* process_raw_lines( char* rawlines, size_t& size ) override;

        void create_loaderdata( JDLIB::LOADERDATA& data ) override;

        void parse_extattr( const char* str, const int lng ) override;

      private:

        void receive_finish() override;
    };
}

#endif
